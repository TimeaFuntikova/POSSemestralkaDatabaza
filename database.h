#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <cstring>
#include <utility>
#include <vector>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <memory>

enum class DataType { INT, DOUBLE, STRING, BOOL, DATE };

struct Column {
    std::string name;
    DataType type;
    bool isNullable;
};

struct User {
    std::string username;
    std::set<std::string> permissions;
};

class Table {
public:
    explicit Table(std::string name, std::string creator = "") : name(std::move(name)), creator(std::move(creator)) {}

    [[nodiscard]] const std::string& getName() const { return name; }
    [[nodiscard]] const std::vector<Column>& getColumns() const { return columns; }
    [[nodiscard]] const std::string& getCreator() const { return creator; }

    void addColumn(const std::string& columnName, DataType type, bool isNullable) {
        columns.push_back({columnName, type, isNullable});
    }

    void setCreator(const std::string& creatorUsername) {
        this->creator = creatorUsername;
    }

private:
    std::string name;
    std::string creator;
    std::vector<Column> columns;
};

class Database {
public:

    void createTable(const std::string& tableName, const std::vector<Column>& columns, const std::string& creatorUsername);
    static void saveTableStructure(const Table& table);
    std::vector<std::string> listTablesCreatedByUser(const std::string& username);
    bool deleteTable(const std::string& username, const std::string& tableName);

   std::string getRow(std::string primaryKey, std::string user, std::string tableName);
   std::string getRows(std::string user, std::string tableName);
   std::string getCertainCount(std::string fileName, std::string primaryKey);
   bool deleteRow(std::string primaryKey,std::string user,std::string tableName);

    bool hasRights(std::string user, std::string neededRights, std::string tableName);
    bool grantAccess(const std::string& username, const std::string& tableName, std::string& right);

    std::string loginUser(std::string userName, std::string password);
    static std::string findUserinCsvFile(std::string fileName, std::string username);

    bool registerUser(const std::string& userName, std::string password);
    bool fileExists(std::string& fileName);

    void saveTableCreatorInfo();
    void loadTableCreatorInfo();

    bool insert(std::string data, std::string tableName, std::string user);
    bool update(std::string primaryKey, std::string data, std::string tableName, std::string user);
    bool addColumn(std::string columnName, std::string user, std::string tableName);

    static bool validateUserPassword(const std::string& username, const std::string& password);
    static std::string getHeader(std::string fileName);

private:
    std::map<std::string, std::unique_ptr<Table>> tables;
    std::map<std::string, User> users;
    static std::mutex dbMutex;
    int getCount(std::string fileName);


};
#endif // DATABASE_H
