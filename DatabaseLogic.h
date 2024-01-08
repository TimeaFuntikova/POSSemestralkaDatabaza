#pragma once
#include <string>
#include "Tables.h"
#ifndef DATABASELOGIC_H
#define DATABASELOGIC_H


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

    const std::vector<Column>& getColumns() const { return columns; }
    const std::vector<Row>& getRows() const { return rows; }

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

//struct TableDef {
//    int columnIndex;
//    std::string columnName;
//    DataType type;
//};
struct TableRights {
    std::string tableName;
    std::string user;
};
struct DatabaseUsers {
    std::string userName;
    std::string password;
};

class DatabaseLogic {
public:
    DatabaseLogic() = default;
    DatabaseLogic(std::string& tableName,std::string& loggedUser) : tableName(tableName),loggedUser(loggedUser){}
    DatabaseLogic(std::string& loggedUser) : loggedUser(loggedUser) {}
    bool isDefined();
    bool containsColumn(std::string columnName);
    bool hasRights(std::string neededRights);
    std::string insert(std::string data);
    std::string update(std::string primaryKey, std::string data);
    std::string deleteRow(std::string primaryKey);
    std::string getRow(std::string primaryKey);
    std::string getRows();
    std::string addRights(std::string userName, std::string rights);
    std::string DeleteTable();
    std::string addColumn(std::string columnName, DataType type);
    std::string loginUser(std::string userName, std::string password);
    std::string createTable(std::string columns);
    std::string registerUser(std::string userName, std::string password);

private:
    std::mutex logMutex;
    bool fileExists(std::string& fileName);
    bool writeCsvFile(std::string fileName, std::string data);
    std::string findUserinCsvFile(std::string fileName, std::string username, std::string password);
    std::string getCertainCount(std::string fileName, std::string primaryKey);
    int getCount(std::string fileName);
    std::string tableName;
    std::string loggedUser;
    std::map<std::string, Table> tables;
    std::mutex dbMutex;

};
#endif
