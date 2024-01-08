#include <fstream>
#include <iostream>
#include "sstream"
#include "DatabaseLogic.h"
#include "string"
#include "WriteIntoFile.h"
using namespace std;




bool DatabaseLogic::isDefined()
{
    return false;
}

bool DatabaseLogic::containsColumn(string columnName)
{
    return false;
}
//rights: O as owner, IUDSV - insert update delete, I - only inserting, U - only updating, D - only deleting, S -selecting only
bool DatabaseLogic::hasRights(std::string neededRights)
{
    std::ifstream table(tableName + "_rights.txt");
    std::string line;
    while (getline(table, line)) {
        std::istringstream iss(line);
        std::string id,username, rights;
        if (std::getline(iss, id, ',') && std::getline(iss, username, ',') && std::getline(iss, rights)) {
            if (username == loggedUser && neededRights == rights) {
                return true;
            }
        }
    }
    return false;
}
std::string DatabaseLogic::insert(std::string data)
{
    if (hasRights("IUDS") == true || hasRights("I") == true || hasRights("O") == true) {
        int count = getCount(tableName + ".txt");
        std::stringstream dataStream;
        dataStream << count;
        std::string datas;
        dataStream >> datas;
        datas += "," + data + "\n";
        std::string tblName = tableName + ".txt";
        const char* fileName = reinterpret_cast <const char*>(tblName.c_str());
        WriteIntoFile* zapis = new WriteIntoFile(fileName);
        zapis->Write(reinterpret_cast<const unsigned char*>(data.c_str()));
        cout << "User " + loggedUser + " has inserted data " + data + " into " + tableName;
        return "Insert successfull";
    }
    else {
        return "User " + loggedUser + " has no rights to insert into table " + tableName;
    }
    return std::string();
}
std::string DatabaseLogic::update(std::string primaryKey, std::string data)
{
    if (hasRights("IUDS") == true || hasRights("U") == true || hasRights("O") == true) {
        std::string tblName = tableName + ".txt";
        std::ifstream file(tableName + ".txt");
        std::string line;
        std::string dataToUpdate;
        while (getline(file, line)) {
            std::string id;
            std::istringstream iss(line);
            std::getline(iss, id, ',');
            if (id == primaryKey) {
                dataToUpdate = id + "," + data + "\n";
            }
            else {
                dataToUpdate += line;
            }
        }
        const char* fileName = reinterpret_cast <const char*>(tblName.c_str());
        WriteIntoFile* zapis = new WriteIntoFile(fileName);
        zapis->reWrite(reinterpret_cast<const unsigned char*>(data.c_str()));
        cout << "Updated data in table " + tableName + " by user " + loggedUser;
        return "Update was successfull";
    }
    else {
        return "User " + loggedUser + " has no rights to updete in table " + tableName;
    }
    return std::string();
}
std::string DatabaseLogic::deleteRow(std::string primaryKey)
{
    if (hasRights("IUDS") == true || hasRights("D") == true || hasRights("O") == true) {
        std::string tblName = tableName + ".txt";
        std::ifstream file(tableName + ".txt");
        std::string line;
        std::string data;
        while (getline(file, line)) {
            std::string id;
            std::istringstream iss(line);
            std::getline(iss, id, ',');
            if (id != primaryKey) {
                data += line;
            }
        }
        const char* fileName = reinterpret_cast <const char*>(tblName.c_str());
        WriteIntoFile* zapis = new WriteIntoFile(fileName);
        zapis->reWrite(reinterpret_cast<const unsigned char*>(data.c_str()));
        cout << "Deleted data from " + tableName + " by user " + loggedUser;
        return "Delete was successfull";
    }
    else {
        return "User " + loggedUser + " has no rights to delete from " + tableName;
    }

}
std::string DatabaseLogic::getRow(std::string primaryKey)
{
    if (hasRights("IUDS") == true || hasRights("S") == true || hasRights("O") == true) {
        std::string data = getCertainCount(tableName + ".txt", primaryKey);
        return data;
    }
    else {
        return "User " + loggedUser + " has no right to select items from " + tableName;
    }
}
std::string DatabaseLogic::getRows()
{
    if (hasRights("IUDS") == true || hasRights("S") == true || hasRights("O") == true) {
        ifstream file(tableName + ".txt");
        std::string line;
        std::string result;
        while (getline(file, line)) {
            result += "|" + line +  "|\n";
        }
        return result;
    }
    else {
        return "User " + loggedUser + " has no right to select items from " + tableName;
    }
}
string DatabaseLogic::addRights(string userName,string rights)
{
    if (hasRights("O") == true) {
        std::string rightsName = tableName + "_rights.txt";
        const char* fileName = reinterpret_cast <const char*>(rightsName.c_str());
        int count = getCount(tableName + "_rights.txt");
        std::string data;
        std::stringstream bufferData;
        bufferData << count;
        bufferData >> data;
        data += "," + userName + "," + rights;
        WriteIntoFile* zapis = new WriteIntoFile(fileName);
        zapis->Write(reinterpret_cast<const unsigned char*>(data.c_str()));
        cout << "Rights granted to " + userName + " on table " + tableName + " by user " + loggedUser;
        return "Rights granted";
    }
    else {
        return "Logged user doesnt have rights to add rights to table " + tableName;
    }
}

string DatabaseLogic::DeleteTable() {
    if (hasRights("O")) {
        std::string tblFileName = tableName + ".txt";
        std::string rightsFileName = tableName + "_rights.txt";
        std::remove(tblFileName.c_str());
        std::remove(rightsFileName.c_str());
        cout << "User " + loggedUser + " has removed table named " + tableName;
        return "Drop table successfull";
    }
    else {
        return "User " + loggedUser + " has no right to drop table " + tableName;
    }
}
string DatabaseLogic::addColumn(string columnName, DataType type)
{
    if(hasRights("O") == true) {
        std::string tblName = tableName + ".txt";
        std::ifstream t(tableName+".txt");
        std::stringstream buffer;
        std::string line;
        std::string header;
        std::string trimmedHeader;
        int count = 0;
        while (getline(t, line)) {
            if (count == 0) {
                header = line;
                trimmedHeader = header.substr(0, header.size() - 2);
                trimmedHeader += "," + columnName + "\n";
                count++;
            }
            else {
                trimmedHeader += line;
            }
        }
        const char* fileName = reinterpret_cast <const char*>(tblName.c_str());
        WriteIntoFile* zapis = new WriteIntoFile(fileName);
        zapis->reWrite(reinterpret_cast<const unsigned char*>(trimmedHeader.c_str()));
        return "Column " + columnName + " was successfully added to table " + tableName;
    }
    else {
        return "You dont have rights to add columns";
    }
}

string DatabaseLogic::loginUser(string userName, string password) {
    const char *fileName = "Users_table.txt";
    std::string flName = "Users_table.txt";
    if (fileExists(flName)==false) {
        WriteIntoFile* zapis = new WriteIntoFile(fileName);
        zapis->Write(reinterpret_cast<const unsigned char *>("id,username,password\n"));
        zapis->Write(reinterpret_cast<const unsigned char *>("0,admin,admin\n"));
    }
    if (findUserinCsvFile(fileName, userName, password) == userName) {
        cout<<userName;
        return userName;
    }

    return "";
}

std::string DatabaseLogic::createTable(std::string columns) {
    std::string tblName = tableName + ".txt";
    const char* fileName = reinterpret_cast <const char*>(tblName.c_str());
    std::string tblRights = tableName + "_rights.txt";
    const char* fileNameRights = reinterpret_cast <const char*>(tblRights.c_str());
    if (fileExists(tblName) == false) {
        WriteIntoFile* zapis = new WriteIntoFile(fileName);
        zapis->Write(reinterpret_cast<const unsigned char*>(columns.c_str()));
        WriteIntoFile* zapis_rights = new WriteIntoFile(fileNameRights);
        std::string Owner = "0" + loggedUser + "O";
        zapis_rights->Write(reinterpret_cast<const unsigned char*>(Owner.c_str()));
        cout << "Table with name " + tableName + " and rights for owner " + loggedUser + " were created in db ";
        return "Table created";
    }
    else {
        return "Table allready exists";
    }
}

string DatabaseLogic::registerUser(string userName,string password)
{
    const char* fileName = "Users_table.txt";
    std::string flName = "Users_table.txt";
    WriteIntoFile* zapis = new WriteIntoFile(fileName);
    if (fileExists(flName) == false) {
        zapis->Write(reinterpret_cast<const unsigned char*>("id,username,password\n"));
        zapis->Write(reinterpret_cast<const unsigned char*>("0,admin,admin\n"));
    }
    int count = getCount(fileName);
    std::stringstream dataStream;
    dataStream << count;
    std::string data;
    dataStream >> data;
    data += "," + userName + "," + password+"\n";
    try {
        zapis->Write(reinterpret_cast<const unsigned char*>(data.c_str()));
        if (findUserinCsvFile(fileName, userName, password) == userName) {
            return "User succesfully created";
        }
        else {
            throw("User cannot be created error occured");
        }
    }catch (string Err) {
        return Err;
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
    std::string fail;
    while (std::getline(ip, line))
    {
        std::istringstream iss(line);
        string id, usernameDt, passwordDt;
        if (std::getline(iss, id, ',') && std::getline(iss, usernameDt, ',') && std::getline(iss, passwordDt)) {
            if (usernameDt == username && password == passwordDt) {
                return usernameDt;
            }
            else {
                fail = "notfound";
            }
        }
    }
    return fail;
}
std::string DatabaseLogic::getCertainCount(std::string fileName, std::string primaryKey){
    std::ifstream ip(fileName);
    std::string line;
    int count = 0;
    while (std::getline(ip, line)) {
        count++;
        if (count - 1 == atoi(reinterpret_cast<const char*>(primaryKey.c_str()))) {
            return line;
        }
    }
    return "Not found";
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


