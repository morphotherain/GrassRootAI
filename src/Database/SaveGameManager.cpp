#include "SaveGameManager.h"
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>

SaveGameManager::SaveGameManager()
{
    // 复用 DatabaseManager 已经打开的主库连接
    m_db = DatabaseManager::getInstance()->getDatabase();
}

SaveGameManager::~SaveGameManager()
{
    // 退出时尝试分离当前存档库（如果有）
    detachCurrentSaveDatabase();
}

// 在主库上创建一组视图，把无前缀的 dyn* 表名转发到当前附加的 dyn 库。
// 这些视图只需要创建一次；如果已经存在则不会报错。
static void CreateDynViewsIfNotExists(sqlite3* db)
{
    if (!db)
    {
        return;
    }

    // 注意：这里视图引用的是 dyn.<表名>，要求外部先通过 ATTACH DATABASE ... AS dyn; 挂载存档库。
    const char* sql =
        "CREATE VIEW IF NOT EXISTS dynPilotSkill AS SELECT * FROM dyn.dynPilotSkill;"
        "CREATE VIEW IF NOT EXISTS dynSkillQueue AS SELECT * FROM dyn.dynSkillQueue;"
        "CREATE VIEW IF NOT EXISTS dynPilots AS SELECT * FROM dyn.dynPilots;"
        "CREATE VIEW IF NOT EXISTS dynGameObjects AS SELECT * FROM dyn.dynGameObjects;"
        "CREATE VIEW IF NOT EXISTS dynContainers AS SELECT * FROM dyn.dynContainers;"
        "CREATE VIEW IF NOT EXISTS dynObjectAttributes AS SELECT * FROM dyn.dynObjectAttributes;"
        "CREATE VIEW IF NOT EXISTS dynMarketOrders AS SELECT * FROM dyn.dynMarketOrders;";

    char* errMsg = nullptr;
    int   rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "CreateDynViewsIfNotExists failed: " << (errMsg ? errMsg : sqlite3_errmsg(db)) << std::endl;
        if (errMsg)
        {
            sqlite3_free(errMsg);
        }
    }
}

std::string SaveGameManager::escapeSingleQuotes(const std::string& input)
{
    std::string result;
    result.reserve(input.size());
    for (char c : input)
    {
        result.push_back(c);
        if (c == '\'')
        {
            // SQLite 中字符串常量内的单引号需要写成两个单引号
            result.push_back('\'');
        }
    }
    return result;
}

bool SaveGameManager::attachSaveDatabase(const std::string& filePath, const std::string& alias)
{
    if (!m_db)
    {
        std::cerr << "SaveGameManager::attachSaveDatabase: database handle is null." << std::endl;
        return false;
    }

    // 如果已经有附加的存档库，先尝试分离
    if (hasAttachedSave())
    {
        if (!detachCurrentSaveDatabase())
        {
            std::cerr << "SaveGameManager::attachSaveDatabase: failed to detach previous save database." << std::endl;
            return false;
        }
    }

    // 手动转义路径中的单引号
    std::string escapedPath = escapeSingleQuotes(filePath);
    std::string sql = "ATTACH DATABASE '" + escapedPath + "' AS " + alias + ";";

    char* errMsg = nullptr;
    int   rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to ATTACH save database: " << (errMsg ? errMsg : sqlite3_errmsg(m_db)) << std::endl;
        if (errMsg)
        {
            sqlite3_free(errMsg);
        }
        return false;
    }

    if (errMsg)
    {
        sqlite3_free(errMsg);
    }

    m_currentAlias = alias;
    // 通过路径附加时，不改变 m_currentSlotID，由调用者根据需要设置
    return true;
}

bool SaveGameManager::detachCurrentSaveDatabase()
{
    if (!m_db || m_currentAlias.empty())
    {
        return true;
    }

    std::string sql = "DETACH DATABASE " + m_currentAlias + ";";
    char*       errMsg = nullptr;
    int         rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to DETACH save database: " << (errMsg ? errMsg : sqlite3_errmsg(m_db)) << std::endl;
        if (errMsg)
        {
            sqlite3_free(errMsg);
        }
        return false;
    }

    if (errMsg)
    {
        sqlite3_free(errMsg);
    }

    m_currentAlias.clear();
    m_currentSlotID = -1;
    return true;
}

bool SaveGameManager::loadSaveBySlotID(int slotID)
{
    if (!m_db)
    {
        std::cerr << "SaveGameManager::loadSaveBySlotID: database handle is null." << std::endl;
        return false;
    }

    const char* sql =
        "SELECT fileName "
        "FROM saveSlots "
        "WHERE slotID = ? AND isDeleted = 0;";

    sqlite3_stmt* stmt = nullptr;
    int           rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SaveGameManager::loadSaveBySlotID: prepare failed: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, slotID);

    std::string fileName;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text)
        {
            fileName = reinterpret_cast<const char*>(text);
        }
    }
    sqlite3_finalize(stmt);

    if (fileName.empty())
    {
        std::cerr << "SaveGameManager::loadSaveBySlotID: no active save slot found for slotID=" << slotID << std::endl;
        return false;
    }

    // 这里仅使用 fileName，本身是相对路径还是绝对路径由外部约定。
    // 例如可以约定 fileName 为 "saves/save_0001.save"。
    if (!attachSaveDatabase(fileName, "dyn"))
    {
        std::cerr << "SaveGameManager::loadSaveBySlotID: failed to attach file " << fileName << std::endl;
        return false;
    }

    // ATTACH 成功后，确保主库上存在一组视图，把无前缀 dyn* 表名映射到 dyn.<table>
    CreateDynViewsIfNotExists(m_db);

    m_currentSlotID = slotID;
    return true;
}

std::vector<SaveSlotInfo> SaveGameManager::listSaveSlots(bool includeDeleted)
{
    std::vector<SaveSlotInfo> result;

    if (!m_db)
    {
        std::cerr << "SaveGameManager::listSaveSlots: database handle is null." << std::endl;
        return result;
    }

    std::string sql =
        "SELECT slotID, fileName, displayName, createTime, lastPlayedTime, gameVersion, isDeleted "
        "FROM saveSlots ";

    if (!includeDeleted)
    {
        sql += "WHERE isDeleted = 0 ";
    }
    sql += "ORDER BY slotID ASC;";

    sqlite3_stmt* stmt = nullptr;
    int           rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SaveGameManager::listSaveSlots: prepare failed: " << sqlite3_errmsg(m_db) << std::endl;
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        SaveSlotInfo info;
        info.slotID = sqlite3_column_int(stmt, 0);

        const unsigned char* fileNameText = sqlite3_column_text(stmt, 1);
        if (fileNameText)
        {
            info.fileName = reinterpret_cast<const char*>(fileNameText);
        }

        const unsigned char* displayNameText = sqlite3_column_text(stmt, 2);
        if (displayNameText)
        {
            info.displayName = reinterpret_cast<const char*>(displayNameText);
        }

        info.createTime = static_cast<long long>(sqlite3_column_int64(stmt, 3));
        info.lastPlayedTime = static_cast<long long>(sqlite3_column_int64(stmt, 4));

        const unsigned char* gameVersionText = sqlite3_column_text(stmt, 5);
        if (gameVersionText)
        {
            info.gameVersion = reinterpret_cast<const char*>(gameVersionText);
        }

        info.isDeleted = (sqlite3_column_int(stmt, 6) != 0);

        result.push_back(std::move(info));
    }

    sqlite3_finalize(stmt);
    return result;
}

bool SaveGameManager::registerExistingSave(const std::string& fileName,
                                           const std::string& displayName,
                                           const std::string& gameVersion,
                                           int&               outSlotID)
{
    outSlotID = -1;

    if (!m_db)
    {
        std::cerr << "SaveGameManager::registerExistingSave: database handle is null." << std::endl;
        return false;
    }

    const char* sql =
        "INSERT INTO saveSlots (fileName, displayName, createTime, lastPlayedTime, gameVersion, isDeleted) "
        "VALUES (?, ?, strftime('%s','now'), strftime('%s','now'), ?, 0);";

    sqlite3_stmt* stmt = nullptr;
    int           rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SaveGameManager::registerExistingSave: prepare failed: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, fileName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, displayName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, gameVersion.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "SaveGameManager::registerExistingSave: insert failed: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    // 获取刚插入记录的 slotID
    outSlotID = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    return true;
}

bool SaveGameManager::createNewSaveFromTemplate(const std::string& templatePath,
                                                const std::string& displayName,
                                                const std::string& gameVersion,
                                                int&               outSlotID)
{
    outSlotID = -1;

    namespace fs = std::filesystem;

    try
    {
        fs::path src(templatePath);
        if (!fs::exists(src))
        {
            std::cerr << "SaveGameManager::createNewSaveFromTemplate: template file not found: "
                      << templatePath << std::endl;
            return false;
        }

        fs::path dir = src.parent_path();
        if (dir.empty())
        {
            dir = fs::path("save");
        }

        // 根据当前时间生成一个较为唯一的文件名
        auto     now = std::chrono::system_clock::now();
        auto     tt = std::chrono::system_clock::to_time_t(now);
        std::tm  tmLocal{};
#if defined(_WIN32)
        localtime_s(&tmLocal, &tt);
#else
        localtime_r(&tt, &tmLocal);
#endif

        auto makeFileName = [&tmLocal](int suffix) {
            std::ostringstream oss;
            oss << "save_" << std::put_time(&tmLocal, "%Y%m%d_%H%M%S");
            if (suffix > 0)
            {
                oss << "_" << suffix;
            }
            oss << ".save";
            return oss.str();
        };

        fs::path dst;
        int      suffix = 0;
        do
        {
            dst = dir / makeFileName(suffix);
            ++suffix;
        } while (fs::exists(dst));

        fs::copy_file(src, dst, fs::copy_options::none);

        // 在 saveSlots 中登记这个新存档
        if (!registerExistingSave(dst.string(), displayName, gameVersion, outSlotID))
        {
            std::cerr << "SaveGameManager::createNewSaveFromTemplate: registerExistingSave failed."
                      << std::endl;
            return false;
        }

        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "SaveGameManager::createNewSaveFromTemplate exception: " << e.what()
                  << std::endl;
        return false;
    }
}


