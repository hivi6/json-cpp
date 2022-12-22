#include <iostream>
#include <vector>
#include <fstream>
#include "json.h"

using namespace std;

void typeTable(const Json &json)
{
    cout << "N B I D S A O\n";
    cout << json.isNull() << " "
         << json.isBool() << " "
         << json.isInt() << " "
         << json.isDouble() << " "
         << json.isString() << " "
         << json.isArray() << " "
         << json.isObject() << endl;
}

int main(int argc, char **argv)
{
    auto res = readJson("test/test2.json");
    cout << (res.isError() ? res.getError() : res.getJson()["web-app"]["servlet"][1]["servlet-name"]) << endl;
    return 0;
}