#include "DatabaseManager.h"
#include <locale>
#include <codecvt>



std::wstring DatabaseManager::sqlite3_column_wstring(sqlite3_stmt* stmt, int column_index)
{
    const char* text = (const char*)(sqlite3_column_text(stmt, column_index));
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(text);
}

std::string DatabaseManager::sqlite3_column_string(sqlite3_stmt* stmt, int column_index)
{
    const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, column_index));
    return std::string(text);
}

std::wstring DatabaseManager::getNameByTypeId(int id)
{
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    std::string sql = "SELECT typeName_zh FROM invtypes WHERE typeID = " + std::to_string(id) + ";";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        return sqlite3_column_wstring(stmt, 0);
    }
    return std::wstring();
}

void testQueryAndWriteToFile(int typeID) {
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    std::string sql = "SELECT * FROM invtypes WHERE typeID = " + std::to_string(typeID) + ";";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::ofstream outFile("result.txt");
    if (!outFile) {
        std::cerr << "Failed to open result.txt for writing." << std::endl;
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        outFile << "typeID: " << sqlite3_column_int(stmt, 0) << std::endl;
        outFile << "typeName: " << (const char*)sqlite3_column_text(stmt, 1) << std::endl;
        outFile << "typeName_fr: " << (const char*)sqlite3_column_text(stmt, 2) << std::endl;
        outFile << "typeName_ja: " << (const char*)sqlite3_column_text(stmt, 3) << std::endl;
        outFile << "typeName_ko: " << (const char*)sqlite3_column_text(stmt, 4) << std::endl;
        outFile << "typeName_ru: " << (const char*)sqlite3_column_text(stmt, 5) << std::endl;
        outFile << "typeName_zh: " << (const char*)sqlite3_column_text(stmt, 6) << std::endl;
        outFile << "typeDescription: " << (const char*)sqlite3_column_text(stmt, 7) << std::endl;
        outFile << "typeDescription_fr: " << (const char*)sqlite3_column_text(stmt, 8) << std::endl;
        outFile << "typeDescription_ja: " << (const char*)sqlite3_column_text(stmt, 9) << std::endl;
        outFile << "typeDescription_ko: " << (const char*)sqlite3_column_text(stmt, 10) << std::endl;
        outFile << "typeDescription_ru: " << (const char*)sqlite3_column_text(stmt, 11) << std::endl;
        outFile << "typeDescription_zh: " << (const char*)sqlite3_column_text(stmt, 12) << std::endl;
        outFile << "raceID: " << sqlite3_column_int(stmt, 13) << std::endl;
        outFile << "factionID: " << sqlite3_column_int(stmt, 14) << std::endl;
        outFile << "published: " << (sqlite3_column_int(stmt, 15) ? "true" : "false") << std::endl;
        outFile << "marketGroupID: " << sqlite3_column_int(stmt, 16) << std::endl;
        outFile << "iconID: " << sqlite3_column_int(stmt, 17) << std::endl;
        outFile << "graphicID: " << sqlite3_column_int(stmt, 18) << std::endl;
        outFile << "groupID: " << sqlite3_column_int(stmt, 19) << std::endl;
        outFile << "metaLevel: " << sqlite3_column_int(stmt, 20) << std::endl;
        outFile << "metaGroupID: " << sqlite3_column_int(stmt, 21) << std::endl;
        outFile << "variationParentTypeID: " << sqlite3_column_int(stmt, 22) << std::endl;
        const char* replacementsText = (const char*)sqlite3_column_text(stmt, 23);
        outFile << "replacements: ";
        if (replacementsText != nullptr) {
            outFile << replacementsText << std::endl;
        }
        else {
            outFile << "NULL" << std::endl;
        }

        const char* reqskillsText = (const char*)sqlite3_column_text(stmt, 24);
        outFile << "reqskills: ";
        if (reqskillsText != nullptr) {
            outFile << reqskillsText << std::endl;
        }
        else {
            outFile << "NULL" << std::endl;
        }

        const char* requiredforText = (const char*)sqlite3_column_text(stmt, 25);
        outFile << "requiredfor: ";
        if (requiredforText != nullptr) {
            outFile << requiredforText << std::endl;
        }
        else {
            outFile << "NULL" << std::endl;
        }
        outFile << std::endl;
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    outFile.close();
}