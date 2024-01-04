#include "Database.h"

void Table::addColumn(const std::string& columnName, DataType type) {
    std::lock_guard<std::mutex> lock(tableMutex);
    columns.push_back({columnName, type});
}

bool Table::addRow(const Row& row) {
    std::lock_guard<std::mutex> lock(tableMutex);
    // VALidacia inj?
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
//podla zadanych podmienok (vratia PK)
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

//vsetky alebo podla zadanych podmienok (vratia PK)
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

//podm. - iba ak ma user prava
void Database::createTable(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    tables.emplace(std::piecewise_construct,
                   std::forward_as_tuple(tableName),
                   std::forward_as_tuple(tableName));
}
//funkcia na sort
//vsetky alebo podla zadanych podmienok
std::optional<Table*> Database::getTable(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return &(it->second);
    }
    return std::nullopt;
}

