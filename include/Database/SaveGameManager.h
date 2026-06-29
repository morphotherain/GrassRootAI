#pragma once

#include "DatabaseManager.h"
#include <string>
#include <vector>

// 存档槽信息结构体，对应主库中的 saveSlots 表
struct SaveSlotInfo
{
    int         slotID = 0;
    std::string fileName;       // 例如 "save_0001.save"
    std::string displayName;    // 玩家看到的名称
    long long   createTime = 0; // Unix 时间戳（秒）
    long long   lastPlayedTime = 0;
    std::string gameVersion;    // 可选：当前游戏版本号
    bool        isDeleted = false;
};

// 负责管理存档数据库的附加 / 分离，以及按 slotID 选择当前存档。
// 约定：所有操作基于 DatabaseManager 打开的主库连接（EVEdata.sqlite）。
// 约定：附加时使用的别名固定为 "dyn"，以配合主库中的视图方案。
class SaveGameManager : public SingletonBase<SaveGameManager>
{
private:
    sqlite3*    m_db = nullptr;       // 指向主库 EVEdata.sqlite 的连接（由 DatabaseManager 持有）
    std::string m_currentAlias;       // 当前附加存档库使用的别名，一般为 "dyn"
    int         m_currentSlotID = -1; // 当前激活的存档槽 ID，-1 表示尚未加载任何存档

public:
    SaveGameManager();
    ~SaveGameManager();

    // 返回当前主库连接，方便必要时直接访问
    sqlite3* getDatabase() const { return m_db; }

    // 当前是否已经附加了存档库
    bool hasAttachedSave() const { return !m_currentAlias.empty(); }

    int  getCurrentSlotID() const { return m_currentSlotID; }

    // 按磁盘路径附加一个存档库，别名默认使用 "dyn"
    // 注意：filePath 为存档数据库（例如 save_xxx.save）的完整或相对路径
    bool attachSaveDatabase(const std::string& filePath, const std::string& alias = "dyn");

    // 分离当前附加的存档库（如果有）
    bool detachCurrentSaveDatabase();

    // 按 slotID 从主库 saveSlots 表中查找对应的存档文件，并附加为 "dyn"
    // 要求：EVEdata.sqlite 中已存在 saveSlots 表，且保存了 fileName 等信息
    bool loadSaveBySlotID(int slotID);

    // 仅列出存档槽信息（来自主库 saveSlots 表）
    // includeDeleted 为 false 时忽略 isDeleted = 1 的记录
    std::vector<SaveSlotInfo> listSaveSlots(bool includeDeleted = false);

    // 按 slotID 查询单条存档槽信息（不含已软删记录）
    bool getSaveSlotInfo(int slotID, SaveSlotInfo& outInfo);

    // 更新指定槽位的 lastPlayedTime 为当前时间
    bool updateLastPlayedTime(int slotID);

    // 软删除存档槽（isDeleted = 1，不删除磁盘上的 .save 文件）
    bool deleteSaveSlot(int slotID);

    // 仅在主库 saveSlots 中新增一条记录，不负责实际在磁盘上创建 / 初始化存档数据库文件。
    // 一般做法是：外部先创建好存档数据库文件（包含所有 dyn* 表），然后调用本函数登记。
    bool registerExistingSave(const std::string& fileName,
                              const std::string& displayName,
                              const std::string& gameVersion,
                              int&               outSlotID);

    // 从给定的模板存档文件（例如 "save/initial"）复制出一个新的独立存档文件，
    // 文件名会自动带时间戳（例如 "save/save_20251116_235959.save"），
    // 然后在 saveSlots 中登记，并通过 outSlotID 返回对应的槽位 ID。
    bool createNewSaveFromTemplate(const std::string& templatePath,
                                   const std::string& displayName,
                                   const std::string& gameVersion,
                                   int&               outSlotID);

private:
    // 将路径中的单引号转义，便于安全地拼接到 SQL 字符串里
    static std::string escapeSingleQuotes(const std::string& input);
};


