# Json-Cpp

A Json type parser for C++.

## Usage

Just include the json.h file, and start using it.
\
To parse a file, use `readJson()` function.
\
Make an json object like this,
```cpp
Json json = {
    JsonPair{"number", 123},
    JsonPair{"number2", -1.2e44},
    JsonPair{"array", {1, 2, 3, "str1", false, true, nullptr}}
};
```

output:
```json
{
    "array": [
        1,
        2,
        3,
        "str1",
        false,
        true,
        null
    ],
    "number": 123,
    "number2": -1.2e+44
}
```