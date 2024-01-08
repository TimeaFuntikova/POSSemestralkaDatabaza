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

struct Row {
    std::map<std::string, std::string> data;
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
    [[nodiscard]] const std::vector<Row>& getRows() const { return rows; }
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
    std::vector<Row> rows;
    std::mutex tableMutex;
};

class Database {
public:
    void createTable(const std::string& tableName, const std::vector<Column>& columns, const std::string& creatorUsername);
    std::optional<Table*> getTable(const std::string& tableName);
    static void saveTableStructure(const Table& table);
    std::vector<std::string> listTablesCreatedByUser(const std::string& username);
    bool deleteTable(const std::string& username, const std::string& tableName);

    bool hasRights(std::string user, std::string neededRights, std::string tableName);
    bool grantAccess(const std::string& username, const std::string& tableName, std::string& right);

    static bool isUserRegistered(const std::string& username);
    static void registerUser(const std::string& username, const std::string& password);

    void saveTableCreatorInfo();
    void loadTableCreatorInfo();

    bool insert(std::string data, std::string tableName, std::string user);
    bool update(std::string primaryKey, std::string data, std::string tableName, std::string user)

    static bool validateUserPassword(const std::string& username, const std::string& password);
    static std::string getHeader(std::string fileName);

private:
    std::map<std::string, std::unique_ptr<Table>> tables;
    std::mutex dbMutex;
    std::map<std::string, User> users;

    int getCount(std::string fileName);


};
#endif // DATABASE_H
