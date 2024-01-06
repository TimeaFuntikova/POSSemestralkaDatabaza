#include <fstream>
#include <iostream>
#include "sstream"
#include "DatabaseLogic.h"
#include "string"


using namespace std;




bool DatabaseLogic::isDefined()
{
    return false;
}

bool DatabaseLogic::containsColumn(string columnName)
{
    return false;
}

bool DatabaseLogic::hasRights()
{
    return false;
}

string DatabaseLogic::addRights(string userName)
{
    return string();
}

string DatabaseLogic::addColumn(string columnName, DataType type)
{
    return string();
}

string DatabaseLogic::loginUser(string userName, string password) {
    string fileName = "./Users_table.csv";
   // ifstream file(fileName);
    if (fileExists(fileName)) {
        writeCsvFile(fileName, "id,username,password");
        writeCsvFile(fileName, "0,admin,admin");
    }
    if (findUserinCsvFile(fileName, userName, password) == userName) {
        cout<<userName;
        return userName;
    }

    return "";
}

void DatabaseLogic::createTable(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    tables.emplace(std::piecewise_construct,
                   std::forward_as_tuple(tableName),
                   std::forward_as_tuple(tableName));
}

string DatabaseLogic::registerUser(string userName,string password)
{
    string fileName = "/database/Users_table.csv";
    ifstream file(fileName);
    if (!fileExists(fileName)) {
        writeCsvFile(fileName, "id,username,password");
        writeCsvFile(fileName, "0,admin,admin");
    }
    int count = getCount(fileName);
    std::stringstream dataStream;
    dataStream << count - 1;
    std::string data;
    dataStream >> data;
    data += "," + userName + "," + password;
    if (writeCsvFile(fileName, data)) {
        return "User successfully created you can now log in";
    }
    else {
        return "User cannot be created error occured";
    }
}

bool DatabaseLogic::fileExists(string& fileName)
{
    return static_cast<bool>(std::ifstream(fileName));
}

bool DatabaseLogic::writeCsvFile(std::string fileName, string data)
{
    lock_guard<std::mutex> csvLock(logMutex);
    ofstream file;
    stringstream ss(data);
    string column;
    file.open(fileName); //std::ios::out | std::ios::app);
    if (file) {
        while (getline(ss,column , ',')) {
            file << "\"" + column + "\"";
        }
        file << std::endl;
        return true;
    }
    else {
        return false;
    }
}

string DatabaseLogic::findUserinCsvFile(std::string fileName, std::string username,std::string password)
{
    std::ifstream ip(fileName);
    std::string line;
    while (std::getline(ip, line))
    {
        std::istringstream iss(line);
        string id, usernameDt, passwordDt;
        if (std::getline(iss, id, ',') && std::getline(iss, usernameDt, ',') && std::getline(iss, passwordDt)) {
            if (usernameDt == username && password == passwordDt) {
                return usernameDt;
            }
        }
    }
}

int DatabaseLogic::getCount(std::string fileName)
{
    std::ifstream ip(fileName);
    std::string line;
    int count = 0;
    while (std::getline(ip, line)) {
        count++;
    }
    return count - 1;
}


