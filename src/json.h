/*
    use JsonNull() for null values
    use JsonArray() for empty array
    using JsonPair{"key", value} is better than "key" >> Json(value)
*/

#ifndef JSON_H
#define JSON_H

// #define JSON_DEBUG
// #define JSON_CONSTRUCTOR_DEBUG
// #define JSON_DESTRUCTOR_DEBUG
// #define JSON_ASSIGNMENT_DEBUG
// #define JSON_DEALLOCATEVALUE_DEBUG
// #define JSON_SETTYPE_DEBUG
// #define JSON_COPYVALUE_DEBUG
// #define JSON_ACCESS_DEBUG
#define TODO() cout << "TODO: " << __PRETTY_FUNCTION__ << endl
#define INSIDE() cout << "Inside " << __PRETTY_FUNCTION__ << endl

#define JsonPair pair<const string, Json>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <initializer_list>
#include <sstream>
#include <fstream>
using namespace std;

// convert escape characters
string convertEscape(const string &s)
{
    string res;
    for (auto ch : s)
    {
        switch (ch)
        {
        case '\"':
            res += "\\\"";
            break;
        case '\\':
            res += "\\\\";
            break;
        case '\b':
            res += "\\b";
            break;
        case '\f':
            res += "\\f";
            break;
        case '\n':
            res += "\\n";
            break;
        case '\r':
            res += "\\r";
            break;
        case '\t':
            res += "\\t";
            break;
        default:
            res.push_back(ch);
            break;
        }
    }
    return res;
}

class Json
{
public:
    enum class JsonType
    {
        JT_NULL,
        JT_BOOL,
        JT_INT,
        JT_DOUBLE,
        JT_STRING,
        JT_ARRAY,
        JT_OBJECT,
    };

private:
    union JsonValue
    {
        bool bValue;               // boolean value
        int iValue;                // integer value
        double dValue;             // double value
        string *sValue;            // string value
        vector<Json> *aValue;      // array value
        map<string, Json> *oValue; // object value
    };

    // ========== helper functions

    // print the JsonType name
    string printType(JsonType t)
    {
        switch (t)
        {
        case JsonType::JT_NULL:
            return "NULL";
        case JsonType::JT_BOOL:
            return "BOOL";
        case JsonType::JT_INT:
            return "INT";
        case JsonType::JT_DOUBLE:
            return "DOUBLE";
        case JsonType::JT_STRING:
            return "STRING";
        case JsonType::JT_ARRAY:
            return "ARRAY";
        case JsonType::JT_OBJECT:
            return "OBJECT";
        }
        return "NOT-POSSIBLE";
    }

    // deallocating the pointer values if any
    void deallocateValue()
    {
#ifdef JSON_DEALLOCATEVALUE_DEBUG
        INSIDE();
        cout << "type: " << printType(type) << endl;
#endif
        // free the pointers
        if (isString())
            delete value.sValue;
        if (isArray())
            delete value.aValue;
        if (isObject())
            delete value.oValue;

        // and set type to NULL
        type = JsonType::JT_NULL;
    }

public:
    // ========== Constructor

    // default constructor
    Json()
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
#endif
        type = JsonType::JT_NULL;
    }

    // copy constructor
    Json(const Json &json)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "copy type: " << printType(json.type) << endl;
        cout << "json value: " << json.dump() << endl;
#endif
        type = json.type;
        switch (json.type)
        {
        case JsonType::JT_STRING:
            value.sValue = new string(json.value.sValue->begin(), json.value.sValue->end());
            break;
        case JsonType::JT_ARRAY:
            value.aValue = new vector<Json>(json.value.aValue->begin(), json.value.aValue->end());
            break;
        case JsonType::JT_OBJECT:
            value.oValue = new map<string, Json>(json.value.oValue->begin(), json.value.oValue->end());
            break;
        default:
            value = json.value;
            break;
        }
    }

    // move constructor
    Json(Json &&json)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "move type: " << printType(json.type) << endl;
        cout << "json type: " << json.dump() << endl;
#endif
        type = json.type;
        value = json.value;
        json.type = JsonType::JT_NULL;
    }

    // nullptr constructor
    Json(nullptr_t)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
#endif
        type = JsonType::JT_NULL;
    }

    // boolean constructor
    Json(bool b)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "set boolean value to: " << b << endl;
#endif
        type = JsonType::JT_BOOL;
        value.bValue = b;
    }

    // integer constructor
    Json(int n)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "set integer value to: " << n << endl;
#endif
        type = JsonType::JT_INT;
        value.iValue = n;
    }

    // double constructor
    Json(double f)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "set double value to: " << f << endl;
#endif
        type = JsonType::JT_DOUBLE;
        value.dValue = f;
    }

    // string constructor
    Json(string s)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "set string value to: " << s << endl;
#endif
        type = JsonType::JT_STRING;
        value.sValue = new string(s.begin(), s.end());
    }

    // const char* constructor
    Json(const char *s)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "set string(const char*) value to: " << s << endl;
#endif
        type = JsonType::JT_STRING;
        value.sValue = new string(s);
    }

    // For Json array type
    Json(initializer_list<Json> jsonArray)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "json array size: " << jsonArray.size() << endl;
        cout << "array element types: ";
        for (const auto &elem : jsonArray)
            cout << printType(elem.type) << " ";
        cout << endl;
        cout << "elements: ";
        for (const auto &elem : jsonArray)
            cout << elem.dump() << " ";
        cout << endl;
#endif
        type = JsonType::JT_ARRAY;
        value.aValue = new vector<Json>(jsonArray.begin(), jsonArray.end());
    }

    // For Json object type
    Json(initializer_list<pair<const string, Json>> jsonMap)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "json array size: " << jsonMap.size() << endl;
        cout << "array element types: ";
        for (const auto &elem : jsonMap)
            cout << printType(elem.type) << " ";
        cout << endl;
        cout << "elements: ";
        for (const auto &elem : jsonMap)
            cout << elem.dump() << " ";
        cout << endl;
#endif
        type = JsonType::JT_OBJECT;
        value.oValue = new map<string, Json>(jsonMap.begin(), jsonMap.end());
    }

    // For array type using vector<Json>
    Json(vector<Json> jsonArray)
    {
#ifdef JSON_CONSTRUCTOR_DEBUG
        INSIDE();
        cout << "json array size: " << jsonArray.size() << endl;
        cout << "array element types: ";
        for (const auto &elem : jsonArray)
            cout << printType(elem.type) << " ";
        cout << endl;
        cout << "elements: ";
        for (const auto &elem : jsonArray)
            cout << elem.dump() << " ";
        cout << endl;
#endif
        type = JsonType::JT_ARRAY;
        value.aValue = new vector<Json>(jsonArray.begin(), jsonArray.end());
    }

    // ========== Desctructor

    ~Json()
    {
#ifdef JSON_DESTRUCTOR_DEBUG
        INSIDE();
#endif
        setType(JsonType::JT_NULL);
    }

    // ========== Assignment Operator

    // Copy assignment
    Json &operator=(const Json &json)
    {
#ifdef JSON_ASSIGNMENT_DEBUG
        INSIDE();
        cout << "copy assignment type: " << printType(json.type) << endl;
        cout << "json value: " << json.dump() << endl;
#endif
        copyValue(json.type, json.value);
        return *this;
    }

    // move assignment
    Json &operator=(Json &&json)
    {
#ifdef JSON_ASSIGNMENT_DEBUG
        INSIDE();
        cout << "move assignment type: " << printType(json.type) << endl;
        cout << "json value: " << json.dump() << endl;
#endif
        deallocateValue();
        type = json.type;
        value = json.value;
        json.type = JsonType::JT_NULL;
        return *this;
    }

    // ========== access operators

    const Json &operator[](size_t index) const // for constant array access
    {
#ifdef JSON_ACCESS_DEBUG
        INSIDE();
        cout << "index: " << index << endl;
#endif
        if (!isArray())
            throw "Error: Not an json array!";
        if (index >= value.aValue->size())
            throw "Error: out-of-bound error!";
        return value.aValue->operator[](index);
    }

    Json &operator[](size_t index) // for non constant array access
    {
#ifdef JSON_ACCESS_DEBUG
        INSIDE();
        cout << "index: " << index << endl;
#endif
        setType(JsonType::JT_ARRAY);
        if (index >= value.aValue->size())
            value.aValue->resize(index + 1);
        return value.aValue->operator[](index);
    }

    const Json &operator[](string key) const // for const object access
    {
#ifdef JSON_ACCESS_DEBUG
        INSIDE();
        cout << "key: " << key << endl;
#endif
        if (!isObject())
            throw "Error: Not an json object!";
        return value.oValue->operator[](key);
    }

    Json &operator[](string key) // for object access
    {
#ifdef JSON_ACCESS_DEBUG
        INSIDE();
        cout << "key: " << key << endl;
#endif
        setType(JsonType::JT_OBJECT);
        return value.oValue->operator[](key);
    }
    // ========== utility functions

    // setTypes and Values

    // set type to a given type
    void setType(JsonType t)
    {
#ifdef JSON_SETTYPE_DEBUG
        INSIDE();
        cout << "current type: " << printType(type) << endl;
        cout << "change type: " << printType(t) << endl;
#endif
        // no change required if type is same
        if (t == type)
            return;

        deallocateValue();

        type = t;
        switch (t)
        {
        case JsonType::JT_BOOL:
            value.bValue = false;
            break;
        case JsonType::JT_INT:
            value.iValue = 0;
            break;
        case JsonType::JT_DOUBLE:
            value.dValue = 0.0;
            break;
        case JsonType::JT_STRING:
            value.sValue = new string();
            break;
        case JsonType::JT_ARRAY:
            value.aValue = new vector<Json>();
            break;
        case JsonType::JT_OBJECT:
            value.oValue = new map<string, Json>();
            break;
        default:
            break;
        }
    }

    // set value to a given type
    void copyValue(JsonType t, JsonValue v)
    {
#ifdef JSON_COPYVALUE_DEBUG
        INSIDE();
        cout << "current type: " << printType(type) << endl;
        cout << "change type: " << printType(t) << endl;
#endif
        deallocateValue();

        type = t;
        switch (t)
        {
        case JsonType::JT_BOOL:
            value.bValue = v.bValue;
            break;
        case JsonType::JT_INT:
            value.iValue = v.iValue;
            break;
        case JsonType::JT_DOUBLE:
            value.dValue = v.dValue;
            break;
        case JsonType::JT_STRING:
            value.sValue = new string(v.sValue->begin(), v.sValue->end());
            break;
        case JsonType::JT_ARRAY:
            value.aValue = new vector<Json>(v.aValue->begin(), v.aValue->end());
            break;
        case JsonType::JT_OBJECT:
            value.oValue = new map<string, Json>(v.oValue->begin(), v.oValue->end());
            break;
        default:
            break;
        }
    }

    // at function

    const Json &at(size_t index) const { return operator[](index); }
    Json &at(size_t index) { return operator[](index); }
    const Json &at(string key) const { return operator[](key); }
    Json &at(string key) { return operator[](key); }

    // array functions

    void push_back(Json arg)
    {
        setType(JsonType::JT_ARRAY);
        value.aValue->emplace_back(arg);
    }

    void pop_back()
    {
        if (!isArray() || value.aValue->size() == 0)
            return;
        value.aValue->pop_back();
    }

    // size function

    size_t size() const
    {
        if (isArray())
            return value.aValue->size();
        if (isObject())
            return value.oValue->size();
        return -1;
    }

    // Type functions
    JsonType getType() const { return type; }
    bool isNull() const { return type == JsonType::JT_NULL; }
    bool isBool() const { return type == JsonType::JT_BOOL; }
    bool isInt() const { return type == JsonType::JT_INT; }
    bool isDouble() const { return type == JsonType::JT_DOUBLE; }
    bool isString() const { return type == JsonType::JT_STRING; }
    bool isArray() const { return type == JsonType::JT_ARRAY; }
    bool isObject() const { return type == JsonType::JT_OBJECT; }

    // get values
    const bool &getBool() const
    {
        if (!isBool())
            throw "Error: not-bool!";
        return value.bValue;
    }
    bool &getBool()
    {
        if (!isBool())
            throw "Error: not-bool!";
        return value.bValue;
    }
    const int &getInt() const
    {
        if (!isInt())
            throw "Error: not-Int!";
        return value.iValue;
    }
    int &getInt()
    {
        if (!isInt())
            throw "Error: not-Int!";
        return value.iValue;
    }
    const double &getDouble() const
    {
        if (!isDouble())
            throw "Error: not-Double!";
        return value.dValue;
    }
    double &getDouble()
    {
        if (!isDouble())
            throw "Error: not-Double!";
        return value.dValue;
    }
    const string &getString() const
    {
        if (!isString())
            throw "Error: not-String!";
        return *value.sValue;
    }
    string &getString()
    {
        if (!isString())
            throw "Error: not-String!";
        return *value.sValue;
    }
    const vector<Json> &getArray() const
    {
        if (!isArray())
            throw "Error: not-Array!";
        return *value.aValue;
    }
    vector<Json> &getArray()
    {
        if (!isArray())
            throw "Error: not-Array!";
        return *value.aValue;
    }
    const map<string, Json> &getObject() const
    {
        if (!isObject())
            throw "Error: not-Object!";
        return *value.oValue;
    }
    map<string, Json> &getObject()
    {
        if (!isObject())
            throw "Error: not-Object!";
        return *value.oValue;
    }

    // printing json

    string dump(int depth = 1, string tabStyle = "    ") const
    {
        if (isNull())
            return "null";
        if (isBool())
            return (value.bValue ? "true" : "false");
        if (isInt())
            return to_string(value.iValue);
        if (isDouble())
        {
            std::ostringstream strs;
            strs << value.dValue;
            return strs.str();
        }
        if (isString())
            return "\"" + convertEscape(*value.sValue) + "\"";

        // tab creation
        string normalTab = "", exitTab = "";
        for (int i = 0; i < depth; i++)
            normalTab += tabStyle;
        for (int i = 0; i < depth - 1; i++)
            exitTab += tabStyle;

        if (isArray())
        {
            if (size() == 0)
                return "[]";
            string res = "";
            for (size_t i = 0; i < value.aValue->size(); i++)
            {
                if (i != 0)
                    res += ",\n";
                res += normalTab + value.aValue->at(i).dump(depth + 1);
            }
            return "[\n" + res + "\n" + exitTab + "]";
        }

        if (size() == 0)
            return "{}";
        string res = "";
        bool skip = true;
        for (auto p : *value.oValue)
        {
            if (!skip)
                res += ",\n";
            res += normalTab + "\"" + p.first + "\": " + p.second.dump(depth + 1);
            skip = false;
        }
        return "{\n" + res + "\n" + exitTab + "}";
    }

    // friend functions

    friend ostream &operator<<(ostream &out, const Json &json);

private:
    JsonType type;
    JsonValue value;
};

// For Json Pair
pair<const string, Json> operator>>(string key, Json value)
{
    return JsonPair{key, value};
}

// print Json
ostream &operator<<(ostream &out, const Json &json)
{
    out << json.dump() << endl;
    return out;
}

// Json Null value
inline Json JsonNull()
{
    return Json();
}

// Json Bool
inline Json JsonBool(bool b)
{
    return Json(b);
}

// Json Int
inline Json JsonInt(int n)
{
    return Json(n);
}

// Json double
inline Json JsonDouble(double d)
{
    return Json(d);
}

// Json string
inline Json JsonString(string s)
{
    return Json(s);
}

// Json array type
void makeArray(Json &json, const Json &arg)
{
    json.push_back(arg);
}
template <class... U>
void makeArray(Json &json, const Json &arg1, U... args)
{
    json.push_back(arg1);
    makeArray(json, args...);
}
Json JsonArray()
{
    Json json;
    json.setType(Json::JsonType::JT_ARRAY);
    return json;
}
template <class... T>
Json JsonArray(T... args)
{
    Json json = JsonArray();
    makeArray(json, args...);
    return json;
}

// Json Object Type
void makeObject(Json &json, const pair<const string, Json> &arg)
{
    json.getObject().insert(arg);
}
template <class... U>
void makeObject(Json &json, const pair<const string, Json> &arg, U... args)
{
    json.getObject().insert(arg);
    makeObject(json, args...);
}
Json JsonObject()
{
    Json json;
    json.setType(Json::JsonType::JT_OBJECT);
    return json;
}
template <class... T>
Json JsonObject(T... args)
{
    Json json = JsonObject();
    makeObject(json, args...);
    return json;
}

// Json parsing

class ParseResult
{
private:
    bool is_err = false;
    Json res;
    string err;

public:
    bool isError() { return is_err; }
    ParseResult setError(string e)
    {
        is_err = true;
        err = e;
        return *this;
    }
    ParseResult setJson(Json r)
    {
        res = r;
        return *this;
    }
    string getError() const
    {
        return err;
    }
    Json getJson() const
    {
        return res;
    }
};

struct
{
private:
    string src;
    int curIndex;

    bool isEnd(size_t offset = 0)
    {
        return offset + curIndex >= src.size();
    }

    char peek(size_t offset = 0)
    {
        if (isEnd(offset))
            return 0;
        return src[curIndex + offset];
    }

    char advance(size_t step = 1)
    {
        if (isEnd())
            return 0;
        auto ch = src[curIndex];
        curIndex += step;
        return ch;
    }

    void ignoreWhitespace()
    {
        while (!isEnd() && isspace(peek()))
            advance();
    }

    template <class T>
    ParseResult parseConstant(string name, T value)
    {
        if (src.substr(curIndex, name.size()) != name ||
            (!isEnd(name.size()) && (isalnum(peek(name.size())) || peek(name.size()) == '_')))
            return ParseResult().setError("Unexpected Constant.");
        advance(name.size());
        return ParseResult().setJson(value);
    }

    ParseResult parseString()
    {
        advance(); // "
        string val;
        while (!isEnd() && peek() != '"')
        {
            if (peek() == '\\') // for escape characters
            {
                advance(); // '\'
                if (peek() == '"')
                    val += "\"";
                else if (peek() == '\\')
                    val += "\\";
                else if (peek() == 't')
                    val += "\t";
                else if (peek() == 'n')
                    val += "\n";
                else if (peek() == 'f')
                    val += "\f";
                else if (peek() == 'b')
                    val += "\b";
                else if (peek() == 'r')
                    val += "\r";
                // TODO: for any hex escape character
                // else if (peek() == 'u')
                // {
                //    TODO();
                // }
                else // for invalid escape character
                    val += "\\" + string(1, peek());
                advance();
            }
            else
                val += advance();
        }
        advance(); // "
        return ParseResult().setJson(val);
    }

    ParseResult parseNumber()
    {
        int startIndex = curIndex;
        advance(); // - or + or some digit
        bool isInt = true;
        // check for integer
        while (!isEnd() && isdigit(peek()))
            advance();

        // check if fraction
        if (peek() == '.')
        {
            isInt = false;
            advance(); // '.'
            while (!isEnd() && isdigit(peek()))
                advance();
        }

        // check for exponents
        if (peek() == 'e' || peek() == 'E')
        {
            isInt = false;
            advance(); // e
            while (!isEnd() && isdigit(peek()))
                advance();
        }

        std::istringstream sin(src.substr(startIndex, curIndex - startIndex));
        if (isInt)
        {
            int d;
            sin >> d;
            return ParseResult().setJson(d);
        }
        double d;
        sin >> d;
        return ParseResult().setJson(d);
    }

    ParseResult parseArray()
    {
        advance(); // [
        ignoreWhitespace();
        vector<Json> val;
        if (peek() == ']')
        {
            advance(); // ]
            return ParseResult().setJson(JsonArray());
        }
        auto elem = parseValue();
        if (elem.isError())
            return elem;
        val.push_back(elem.getJson());
        ignoreWhitespace();
        while (!isEnd() && peek() == ',')
        {
            advance(); // ,
            ignoreWhitespace();
            if (peek() == ']')
            {
                advance(); // ]
                return ParseResult().setJson(val);
            }
            elem = parseValue();
            if (elem.isError())
                return elem;
            val.push_back(elem.getJson());
            ignoreWhitespace();
        }
        if (peek() != ']')
            return ParseResult().setError("Expected ']'.");
        advance(); // ]
        return ParseResult().setJson(val);
    }

    ParseResult parseObject()
    {
        advance(); // {
        ignoreWhitespace();
        if (peek() == '}')
        {
            advance(); // }
            return ParseResult().setJson(JsonObject());
        }
        Json obj;
        auto key = parseString(); // key
        if (key.isError())
            return key;
        ignoreWhitespace();
        advance(); // ':'
        ignoreWhitespace();
        auto value = parseValue(); // value
        if (value.isError())
            return value;
        obj[key.getJson().getString()] = value.getJson();
        ignoreWhitespace();
        while (!isEnd() && peek() == ',')
        {
            advance(); // ,
            ignoreWhitespace();
            if (peek() == '}')
            {
                advance(); // }
                return ParseResult().setJson(obj);
            }
            auto key = parseString(); // key
            if (key.isError())
                return key;
            ignoreWhitespace();
            advance(); // ':'
            ignoreWhitespace();
            auto value = parseValue(); // value
            if (value.isError())
                return value;
            obj[key.getJson().getString()] = value.getJson();
            ignoreWhitespace();
        }
        if (peek() != '}')
            return ParseResult().setError("Expected ']'.");
        advance(); // }
        return ParseResult().setJson(obj);
    }

    ParseResult parseValue()
    {
        ignoreWhitespace();
        if (peek() == 'n')
            return parseConstant("null", nullptr);
        if (peek() == 't')
            return parseConstant("true", true);
        if (peek() == 'f')
            return parseConstant("false", false);
        if (peek() == '+' || peek() == '-' || isdigit(peek()))
            return parseNumber();
        if (peek() == '"')
            return parseString();
        if (peek() == '[')
            return parseArray();
        if (peek() == '{')
            return parseObject();
        return ParseResult().setError("Undefined token '" + string(1, peek()) + "'.");
    }

public:
    ParseResult operator()(const string &s)
    {
        src = s;
        curIndex = 0;

        ParseResult res = parseValue();
        if (res.isError())
            return res;
        ignoreWhitespace();
        if (!isEnd())
            return res.setError("Something went wrong!");
        return res;
    }
} parseJson;

ParseResult readJson(string filepath)
{
    string s;
    ifstream fin(filepath);
    while (fin) {
        string temp;
        fin >> temp;
        s += " " + temp;
    }
    return parseJson(s);
}

#endif // JSON_H