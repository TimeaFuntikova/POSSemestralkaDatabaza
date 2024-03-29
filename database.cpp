#include "database.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <filesystem>
#include <utility>

namespace fs = std::filesystem;


std::mutex logMutex;
std::mutex Database::dbMutex;

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

//rights: O as owner, IUDSV - insert update delete, I - only inserting, U - only updating, D - only deleting, S -selecting only
bool Database::hasRights(std::string user, std::string neededRights, std::string tableName)
{
    std::fstream table("/tmp/semestralka/" + tableName + "_rights.csv");
    std::string line;
    while (getline(table, line)) {
        std::istringstream iss(line);
        std::string username, rights;
        if (std::getline(iss, username, ',') && std::getline(iss, rights)) {
            if (username == user && neededRights == rights) {
                return true;
            }
        }
    }
    return false;
}

bool Database::deleteTable(const std::string& username, const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    {
        if (hasRights(username, "O", tableName)) {

            std::string filePath = "/tmp/semestralka/" + tableName + ".csv";

            if (remove(filePath.c_str()) != 0) {
                std::cerr << "Error deleting file: " << filePath << std::endl;
            } else {
                std::cout << "File deleted successfully: " << filePath << std::endl;
            }
            std::string fileRightsPath = "/tmp/semestralka/" + tableName + "_rights.csv";

            if (remove(fileRightsPath.c_str()) != 0) {
                std::cerr << "Error deleting file: " << fileRightsPath << std::endl;
            } else {
                std::cout << "File deleted successfully: " << fileRightsPath << std::endl;
            }
        } else {
            std::cerr << "Table " << tableName << " not found or permission denied." << std::endl;
            return false;
        }
        return true;
    }
}


void Database::saveTableStructure(const Table& table) {
    std::lock_guard<std::mutex> guard(dbMutex);

    {
        //VYTVORENIE TABULKY
        std::string csvFile = "/tmp/semestralka/" + table.getName() + ".csv";

        std::string header = "id,";
        for (const auto &column: table.getColumns()) {
            header += column.name;
        }

        std::string trimmedHeader = header.substr(0, header.size() - 1);
        std::fstream file;
        if (file) {
            file.open(csvFile, std::ios::out | std::ios::app);
            if (file) {
                file << trimmedHeader;
                file << std::endl;
            }
            std::cout << "Created header for the table " << table.getName() << std::endl;
            file.close();
        }

        //VYTVORENIE PRAV PRE TABULKU
        std::string rightsFile = "/tmp/semestralka/" + table.getName() + "_rights.csv";

        std::lock_guard<std::mutex> csvLock(logMutex);
        if (file) {
            file.open(rightsFile, std::ios::out | std::ios::app);
            if (file) {
                file << table.getCreator() << "," << "O";
                file << std::endl;
            }
            std::cout << "Assigned rights for the table " << table.getName() << std::endl;
            file.close();
        }

        fs::permissions(csvFile,
                        fs::perms::all);


        fs::permissions(rightsFile,
                        fs::perms::all);
    }
    std::cout << "Table structure written successfully for table " << table.getName() << std::endl;
}

/**
 * Funkcia na vytvorenie tabuľky, spracuje stĺpce a zavolá funkciu na vytovrenie csv súboru
 * z výslednej štruktury.
 * @param tableName
 * @param columns
 * @param creatorUsername
 */
void Database::createTable(const std::string& tableName, const std::vector<Column>& columns, const std::string& creatorUsername) {

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

/**
 * Funkcia na update práv, ako keď  sa pridá nový používateľ alebo keď
 * už existujúci udelí práva inému používateľovi. Uzamkne kritickú sekciu pri zápise do súboru.
 * @param username
 * @param tableName
 */
bool Database::grantAccess(const std::string& username, const std::string& tableName, std::string& right) {
    std::string rightsFile = "/tmp/semestralka/" + tableName + "_rights.csv";

    std::lock_guard<std::mutex> csvLock(logMutex);
    {
        std::fstream file;
        file.open(rightsFile, std::ios::out | std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << rightsFile << std::endl;
            return false;
        }

        file << username << "," << right << "\n";
        if (!file.good()) {
            std::cerr << "Failed to write to file: " << rightsFile << std::endl;
            file.close();
            return false;
        }

        std::cout << "Assigned rights for the table " << tableName << " to user: " << username << std::endl;
        file.close();
        return true;
    }
}


bool Database::insert(std::string data, std::string tableName, std::string user) {
    if (!hasRights(user, "IUDS", tableName) &&
        !hasRights(user, "I", tableName) &&
        !hasRights(user, "O", tableName)) {
        return false;
    }

    std::string filePath = "/tmp/semestralka/" + tableName + ".csv";
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::app);


    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> csvLock(logMutex);
    {
        int nextId = getCount(filePath);
        std::string lineToInsert = std::to_string(nextId) + "," + data + "\n";

        file.seekp(0, std::ios::end);
        file << lineToInsert;

        std::cout << "User " + user + " has inserted data " + data + " into " + tableName << std::endl;
        file.close();
        return true;
    }
}


std::string Database::getHeader(std::string fileName) {
    std::ifstream ip(fileName);
    std::string line;
    std::getline(ip, line);
    return line;
}

int Database::getCount(std::string fileName)
{
    std::fstream ip(fileName);
    std::string line;
    int count = 0;
    while (std::getline(ip, line)) {
        count++;
    }
    return count - 1;
}

bool Database::update(std::string primaryKey, std::string data, std::string tableName, std::string user) {
    if (!hasRights(user, "IUDS", tableName) &&
        !hasRights(user, "U", tableName) &&
        !hasRights(user, "O", tableName)) {
        return false;
    }

    std::string filePath = "/tmp/semestralka/" + tableName + ".csv";
    std::fstream file(filePath);
    std::string line;
    std::string header;
    std::stringstream updatedContent;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> csvLock(logMutex);
    {
        //skipping header
        if (!getline(file, header)) {
            std::cerr << "Failed to read header from file: " << filePath << std::endl;
            return false;
        }
        updatedContent << header << "\n";

        bool updateSuccess = false;
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string id;
            getline(iss, id, ',');
            if (id == primaryKey) {
                updatedContent << id << "," << data << "\n";
                updateSuccess = true;
            } else {
                updatedContent << line << "\n";
            }
        }
        file.close();

        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return false;
        }
        outFile << updatedContent.str();
        outFile.close();

        return updateSuccess;
    }
}

bool Database::addColumn(std::string columnName, std::string user, std::string tableName)
{
    if(hasRights(user, "O", tableName)) {
        std::string tblName = "/tmp/semestralka/" + tableName + ".csv";
        std::ifstream t("/tmp/semestralka/" + tableName + ".csv");
        std::stringstream buffer;
        std::string line;
        std::string header;
        std::string trimmedHeader;
        int count = 0;

        std::lock_guard<std::mutex> csvLock(logMutex);
        {
            while (getline(t, line)) {
                if (count == 0) {
                    header = line;
                    trimmedHeader = header.substr(0, header.size() - 2);
                    trimmedHeader += "," + columnName + "\n";
                    count++;
                } else {
                    trimmedHeader += line;
                }
            }
            std::fstream file;
            if (file) {
                file.open(tblName);
                if (file) {
                    file << trimmedHeader;

                }

                std::cout << "User " + user + " has added column " + columnName + " to table " + tableName << std::endl;
                file.close();
                return true;
            }
        }
        return false;
    }
}

std::string Database::findUserinCsvFile(std::string fileName, std::string username)
{
    std::fstream ip(fileName);
    std::string line;
    std::string fail;

    std::lock_guard<std::mutex> csvLock(logMutex);
    {
        while (std::getline(ip, line)) {
            std::istringstream iss(line);
            std::string id, usernameDt, passwordDt;
            if (std::getline(iss, id, ',') && std::getline(iss, usernameDt, ',')) {
                if (usernameDt == username) {
                    return usernameDt;
                } else {
                    fail = "notfound";
                }
            }
        }
        return fail;
    }
}

bool Database::fileExists(std::string& fileName)
{
    return static_cast<bool>(std::ifstream(fileName));
}

std::string Database::loginUser(std::string userName, std::string password) {
    const char *fileName = "/tmp/semestralka/Users_table.csv";
    std::string flName = "/tmp/semestralka/Users_table.csv";

    std::lock_guard<std::mutex> csvLock(logMutex);
    {
        if (!fileExists(flName)) {
            std::fstream file;
            if (file) {
                file.open(flName);
                if (file) {
                    file << "id,username,password\n";
                    file << "0,admin,admin\n";
                }
                file.close();
            }
        }
        if (findUserinCsvFile(fileName, userName) == userName) {
            std::cout<<userName;
            return userName;
        }
        return "";
    }
}
bool Database::registerUser(const std::string& userName, std::string password)
{
    std::string fileName = "/tmp/semestralka/Users_table.txt";

        if (!fileExists(fileName)) {
            std::fstream file;
            if (file) {
                file.open(fileName);
                if (file) {
                    file << "id,username,password\n";
                    file << "0,admin,admin\n";
                }
                file.close();
            }
        }
        int count = getCount(fileName);
        std::stringstream dataStream;
        dataStream << count;
        std::string data;
        dataStream >> data;
        data += "," + userName + "," + password +"\n";
        try {
            std::fstream file;
            if (file) {
                file.open(fileName);
                if (file) {
                    file << data ;
                }
                file.close();
            }
            return findUserinCsvFile(fileName, userName) == userName;
        } catch (std::string Err) {
            return false;
        }
}

std::string Database::getCertainCount(std::string fileName, std::string primaryKey)
{
    std::lock_guard<std::mutex> csvLock(logMutex);
    {
        std::fstream ip(fileName);
        std::string line;
        int count = 0;
        while (std::getline(ip, line)) {
            count++;
            if (count - 1 == atoi(reinterpret_cast<const char *>(primaryKey.c_str()))) {
                return line;
            }
        }
        return "Not found";
    }
}
std::string Database::getRow(std::string primaryKey, std::string user, std::string tableName)
{
    if (hasRights(user,"IUDS",tableName) == true || hasRights(user,"S",tableName) == true || hasRights(user, "O",tableName) == true) {
        std::string data = getCertainCount("/tmp/semestralka/" + tableName + ".csv", primaryKey);
        return data;
    }
    else {
        return "User " + user + " has no right to select items from " + tableName;
    }
}
std::string Database::getRows(std::string user, std::string tableName)
{
    if (hasRights(user, "IUDS", tableName)
        || hasRights(user, "S", tableName)
        || hasRights(user, "O", tableName)) {
        std::ifstream file("/tmp/semestralka/" + tableName + ".csv");
        std::string line;
        std::string result;

        std::lock_guard<std::mutex> csvLock(logMutex);
        {
            while (getline(file, line)) {
                result += "|" + line +  "|\n";
            }
            return result;
        }
    }
    else {
        return "User " + user + " has no right to select items from " + tableName;
    }
}

bool Database::deleteRow(std::string primaryKey, std::string user, std::string tableName) {
    if (!hasRights(user, "IUDS", tableName) &&
        !hasRights(user, "D", tableName) &&
        !hasRights(user, "O", tableName)) {
        return false;
    }

    std::string filePath = "/tmp/semestralka/" + tableName + ".csv";
    std::fstream file(filePath);
    std::string line, header;
    std::stringstream updatedContent;


    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> csvLock(logMutex);
    {
        // skip header
        if (!getline(file, header)) {
            std::cerr << "Failed to read header from file: " << filePath << std::endl;
            return false;
        }
        updatedContent << header << "\n";

        bool deleteSuccess = false;
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string id;
            getline(iss, id, ',');
            if (id != primaryKey) {
                updatedContent << line << "\n";
            } else {
                deleteSuccess = true;
            }
        }
        file.close();

        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return false;
        }
        outFile << updatedContent.str();
        outFile.close();

        return deleteSuccess;
    }
}

