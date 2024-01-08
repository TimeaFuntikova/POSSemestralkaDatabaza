#include "database.h"
#include <fstream>
#include <iostream>
#import <sstream>
#include <memory>

void Database::saveTableCreatorInfo() {
    std::ofstream file("table_creators.csv");
    if (!file.is_open()) {
        std::cerr << "Unable to open table_creators.csv for writing." << std::endl;
        return;
    }

    for (const auto& pair : tables) {
        file << pair.first << "," << pair.second->getCreator() << "\n";
    }
}

void Database::loadTableCreatorInfo() {
    std::ifstream file("table_creators.csv");
    if (!file.is_open()) {
        std::cerr << "Unable to open table_creators.csv for reading." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string tableName, creator;
        std::getline(ss, tableName, ',');
        std::getline(ss, creator);

        if (!tableName.empty() && !creator.empty()) {
            auto table = std::make_unique<Table>(tableName);
            table->setCreator(creator);
            tables[tableName] = std::move(table);
        }
    }
}


std::vector<std::string> Database::listTablesCreatedByUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);

    loadTableCreatorInfo();

    std::vector<std::string> createdTables;
    for (const auto& pair : tables) {
        if (pair.second->getCreator() == username) {
            createdTables.push_back(pair.first);
        }
    }
    return createdTables;
}

bool Database::deleteTable(const std::string& username, const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);

    auto it = tables.find(tableName);
    if (it != tables.end() && it->second->getCreator() == username) {
        std::string filePath = tableName + ".csv";
        if (remove(filePath.c_str()) != 0) {
            std::cerr << "Error deleting file: " << filePath << std::endl;
        } else {
            std::cout << "File deleted successfully: " << filePath << std::endl;
        }

        tables.erase(it);
        saveTableCreatorInfo();

        return true;
    } else {
        std::cerr << "Table " << tableName << " not found or permission denied." << std::endl;
        return false;
    }
}

void Database::loadPermissions() {
    std::ifstream file("permissions.csv");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string username, tableName;
        std::getline(ss, username, ',');
        User user{username, {}};
        while (std::getline(ss, tableName, ',')) {
            user.permissions.insert(tableName);
        }
        users[username] = user;
    }
}

void Database::savePermissions() {
    std::ofstream file("permissions.csv");
    for (const auto& [username, user] : users) {
        file << username;
        for (const auto& tableName : user.permissions) {
            file << "," << tableName;
        }
        file << std::endl;
    }
}

bool Table::addRow(const Row& row) {
    std::lock_guard<std::mutex> lock(tableMutex);
    //todo: VALidac inj?ections?
    rows.push_back(row);
    return true;
}

std::optional<Row> Table::getRow(const std::string& primaryKey) {
    std::lock_guard<std::mutex> lock(tableMutex);
    for (const auto& row : rows) {
        if (row.data.find(columns[0].name)->second == primaryKey) {
            return row;
        }
    }
    return std::nullopt;
}
//todo: podla zadanych podmienok (vratia PK)
bool Table::updateRow(const std::string& primaryKey, const Row& newRow) {
    std::lock_guard<std::mutex> lock(tableMutex);
    for (auto& row : rows) {
        if (row.data.find(columns[0].name)->second == primaryKey) {
            row = newRow;
            return true;
        }
    }
    return false;
}

//todo:vsetky alebo podla zadanych podmienok (vratia PK)
bool Table::deleteRow(const std::string& primaryKey) {
    std::lock_guard<std::mutex> lock(tableMutex);
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        if (it->data.find(columns[0].name)->second == primaryKey) {
            rows.erase(it);
            return true;
        }
    }
    return false;
}

std::string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::INT: return "INT";
        case DataType::DOUBLE: return "DOUBLE";
        case DataType::STRING: return "STRING";
        case DataType::BOOL: return "BOOL";
        case DataType::DATE: return "DATE";
        default: return "UNKNOWN";
    }
}

//https://raymii.org/s/snippets/Cpp_create_and_write_to_a_csv_file.html
std::mutex logMutex;

bool fileExists(std::string& fileName) {
    return static_cast<bool>(std::ifstream(fileName));
}

template <typename filename, typename T1, typename T2, typename T3>
bool writeCsvFile(filename &fileName, T1 column1, T2 column2, T3 column3) {
    std::lock_guard<std::mutex> csvLock(logMutex);
    std::fstream file;
    file.open (fileName, std::ios::out | std::ios::app);
    if (file) {
        file << "\"" << column1 << "\",";
        file << "\"" << column2 << "\",";
        file << "\"" << column3 << "\"";
        file <<  std::endl;
        return true;
    } else {
        return false;
    }
}

void Database::saveTableStructure(const Table& table) {
    std::string csvFile = table.getName() + ".csv";
    if(!fileExists(csvFile))
    {
        writeCsvFile(csvFile, "columnName", "columnType", "isNullable");
    }

    for (const auto& column : table.getColumns()) {
        std::cout << "Writing column to file: " << column.name << " of type " << dataTypeToString(column.type) << " nullable: " << (column.isNullable ? "true" : "false") << std::endl;
        if(!writeCsvFile(csvFile, column.name, dataTypeToString(column.type), (column.isNullable ? "true" : "false") )) {
            std::cerr << "Failed to write to file for table " << csvFile << std::endl;
        }
    }
        std::cout << "Table structure written successfully for table " << table.getName() << std::endl;
}

void Database::createTable(const std::string& tableName, const std::vector<Column>& columns, const std::string& creatorUsername) {
    std::lock_guard<std::mutex> lock(dbMutex);
    if (tables.find(tableName) == tables.end()) {
        auto newTable = std::make_unique<Table>(tableName);
        newTable->setCreator(creatorUsername);
        for (const auto& column : columns) {
            newTable->addColumn(column.name, column.type, column.isNullable);
        }
        tables[tableName] = std::move(newTable);

        saveTableStructure(*tables[tableName]);
        saveTableCreatorInfo();
    } else {
        std::cout << "Table " << tableName << " already exists." << std::endl;
    }
}


bool Database::hasPermission(const std::string& username, const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto userIt = users.find(username);
    if (userIt != users.end()) {
        return userIt->second.permissions.find(tableName) != userIt->second.permissions.end();
    }
    return false;
}

/**
 * Funkcia na update práv, ako keď  sa pridá nový používateľ alebo keď
 * už existujúci udelí práva inému používateľovi
 * @param username
 * @param tableName
 */
void Database::grantAccess(const std::string& username, const std::string& tableName) {
    users[username].permissions.insert(tableName);
    savePermissions(); // Save the updated permissions to file
}

//todo: funkcia na sort
//todo: vsetky alebo podla zadanych podmienok

std::optional<Table*> Database::getTable(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second.get();
    }
    return std::nullopt;
}

void Database::loadFromFile() {
    std::lock_guard<std::mutex> lock(dbMutex);
    std::ifstream tablesFile("tables.csv");
    std::string line;
    while (std::getline(tablesFile, line)) {
        std::istringstream ss(line);
        std::string tableName, creator;
        std::getline(ss, tableName, ',');
        std::getline(ss, creator);
        auto table = std::make_unique<Table>(tableName);
        table->setCreator(creator);
        tables[tableName] = std::move(table);
    }
    tablesFile.close();
}

bool Database::isUserRegistered(const std::string& username) {
    std::ifstream file("users.csv");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string existingUsername;
        std::getline(ss, existingUsername, ',');
        if (existingUsername == username) {
            return true;
        }
    }
    return false;
}

/**
 * Funkcia na registráciu používateľa otvorí súbor users.csv a na jeho koniec bez prepísania
 * už exitujúcich dát pridá nového používateľa s novým zadaným heslom vo formáte CSV.
 * @param username
 * @param password
 */

//todo: make bool to make this as a condition:
void Database::registerUser(const std::string& username, const std::string& password) {
    std::ofstream file("users.csv", std::ios::app);
    if (file) {
        file << username << "," << password << std::endl;
        file.flush();
        std::cout << "Successfully registered user: " << username << std::endl;
    } else {
        std::cerr << "Unable to open users.csv for writing." << std::endl;
    }
    file.close();
}

bool Database::validateUserPassword(const std::string& username, const std::string& password) {
    std::ifstream file("users.csv");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string existingUsername, existingPassword;
        std::getline(ss, existingUsername, ',');
        std::getline(ss, existingPassword);
        if (existingUsername == username && existingPassword == password) {
            return true;
        }
    }
    return false;
}


