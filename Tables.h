#ifndef TABLES_H
#define TABLES_H
#pragma once
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <mutex>
#include "DatabaseLogic.h"

//class Table {
//public:
//    Table(std::string name, DatabaseLogic logic) : name(name),logic(logic) { }
//    std::string addColumn(std::string columnName, DataType type);
//    bool addRow(const Row& row);
//    std::optional<Row> getRow(const std::string& primaryKey);
//    bool updateRow(const std::string& primaryKey, const Row& newRow);
//    bool deleteRow(const std::string& primaryKey);
//
//private:
//    DatabaseLogic logic;
//    std::string name;
//    std::vector<Column> columns;
//    std::vector<Row> rows;
//    std::mutex tableMutex;
//};
#endif