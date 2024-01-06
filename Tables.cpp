#include "Tables.h"
#include "DatabaseLogic.h"
#include <iostream>
std::string Table::addColumn(std::string columnName, DataType type) {
    try {
        if (logic.containsColumn(columnName)) {
            throw("Column " + columnName + "in table" + name +" allready exists");
        }
        else {
            std::lock_guard<std::mutex> lock(tableMutex);
            std::string created = logic.addColumn(columnName, type);
            if (created == "Error occured while creating column") {
                throw("Column cannot be added to table "+name+ ": " + created);
            }
            else {
                return "Database successfully created new collumn to table " + name;
            }
        }
        throw("Adding collumn to table " + name + " has encountered unexpected error");
    }
    catch(std::string Error) {
        std::cout << Error;
        return Error;
    }
}
