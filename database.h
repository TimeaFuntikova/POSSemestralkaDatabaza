#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <optional>

enum class DataType { INT, DOUBLE, STRING, BOOL, DATE };

struct Column {
    std::string name;
    DataType type;
};

struct Row {
    std::map<std::string, std::string> data;
};

class Table {
public:
    Table(std::string name) : name(name) {}

    void addColumn(const std::string& columnName, DataType type);
    bool addRow(const Row& row);
    std::optional<Row> getRow(const std::string& primaryKey);
    bool updateRow(const std::string& primaryKey, const Row& newRow);
    bool deleteRow(const std::string& primaryKey);

private:
    std::string name;
    std::vector<Column> columns;
    std::vector<Row> rows;
    std::mutex tableMutex;
};

class Database {
public:
    void createTable(const std::string& tableName);
    std::optional<Table*> getTable(const std::string& tableName);


private:
    std::map<std::string, Table> tables;
    std::mutex dbMutex; // Mutex for thread-safe operations
};

#endif // DATABASE_H
