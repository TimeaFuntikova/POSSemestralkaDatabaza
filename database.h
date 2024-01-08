#ifndef DATABASE_H
#define DATABASE_H

#include <string>
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

    bool addRow(const Row& row);
    std::optional<Row> getRow(const std::string& primaryKey);
    bool updateRow(const std::string& primaryKey, const Row& newRow);
    bool deleteRow(const std::string& primaryKey);

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

    static bool isUserRegistered(const std::string& username);
    static void registerUser(const std::string& username, const std::string& password);
    static bool validateUserPassword(const std::string& username, const std::string& password);

    void savePermissions();
    void loadPermissions();

    void saveTableCreatorInfo();
    void loadTableCreatorInfo();

    bool deleteTable(const std::string& username, const std::string& tableName);

    bool hasPermission(const std::string& username, const std::string& tableName);
    void grantAccess(const std::string& username, const std::string& tableName);

    void loadFromFile();

private:
    std::map<std::string, std::unique_ptr<Table>> tables;
    std::mutex dbMutex;
    std::map<std::string, User> users;


};
#endif // DATABASE_H
