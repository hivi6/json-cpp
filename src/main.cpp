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
    Json json = {
        JsonPair{"number", 123},
        JsonPair{"number2", -1.2e44},
        JsonPair{"array", {1, 2, 3, "str1", false, true, nullptr}},
    };
    cout << "JsonFile: ";
    cout << json;
    return 0;
}