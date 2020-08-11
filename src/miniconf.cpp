/*
 * miniconf.cpp
 *
 * A minimalist C++ configuration manager
 *
 * Author: Tsz-Ho Yu (tszhoyu@gmail.com)
 *
 */

#include "miniconf.h"

namespace miniconf {

    // Value
    Value::Value() : _type(DataType::UNKNOWN), _size(0), _data(nullptr)
    {}

    Value::Value(const Value& other) : Value()
    {
        copyData(other._data, other._size, other._type);
    }

    Value::Value(Value&& other) : Value()
    {
        moveData(other._data, other._size, other._type);
    }


    Value& Value::operator=(const Value& other)
    {
        clearData();
        return copyData(other._data, other._size, other._type);
    }

    Value& Value::operator=(Value&& other)
    {
        return moveData(other._data, other._size, other._type);
    }

    Value::~Value()
    {
        clearData();
    }

    //  int
    Value::Value(const int& other) : Value()
    {
        copyData(reinterpret_cast<const char*>(&other), sizeof(int), DataType::INT);
    }

    Value& Value::operator=(const int& other)
    {
        clearData();
        return copyData(reinterpret_cast<const char*>(&other), sizeof(int), DataType::INT);
    }

    Value::operator int() const
    {
        return *reinterpret_cast<int*>(_data);
    }

    int Value::getInt() const
    {
        return *reinterpret_cast<int*>(_data);
    }

    //  number (floating point)
    Value::Value(const double& other) : Value()
    {
        copyData(reinterpret_cast<const char*>(&other), sizeof(double), DataType::NUMBER);
    }

    Value& Value::operator=(const double& other)
    {
        clearData();
        return copyData(reinterpret_cast<const char*>(&other), sizeof(double), DataType::NUMBER);
    }

    Value::operator double() const
    {
        return *reinterpret_cast<double*>(_data);
    }

    double Value::getNumber() const
    {
        return *reinterpret_cast<double*>(_data);
    }


    //  bool
    Value::Value(const bool& other) : Value()
    {
        copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::BOOL);
    }

    Value& Value::operator=(const bool& other)
    {
        clearData();
        return copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::BOOL);
    }

    Value::operator bool() const
    {
        return *reinterpret_cast<bool*>(_data);
    }

    bool Value::getBoolean() const
    {
        return *reinterpret_cast<bool*>(_data);
    }

    //  char array
    Value::Value(const char* other) : Value()
    {
        copyData(other, strlen(other) + 1, DataType::STRING);
    }

    Value& Value::operator=(const char* other)
    {
        clearData();
        return copyData(other, strlen(other) + 1, DataType::STRING);
    }

    Value::operator char*() const
    {
        return reinterpret_cast<char*>(_data);
    }

    char* Value::getCharArray() const
    {
        return reinterpret_cast<char*>(_data);
    }

    //  std::string
    Value::Value(const std::string& other) : Value()
    {
        copyData(other.c_str(), other.size() + 1, DataType::STRING);
    }

    Value& Value::operator=(const std::string& other)
    {
        clearData();
        return copyData(other.c_str(), other.size() + 1, DataType::STRING);
    }

    Value::operator std::string() const
    {
        return std::string(reinterpret_cast<char*>(_data));
    }

    std::string Value::getString() const
    {
        return std::string(reinterpret_cast<char*>(_data));
    }

    // print function
    std::string Value::print()
    {
        const int slen = 31;
        char tempStr[slen + 1];
        std::string outStr;
        switch (_type) {
            case DataType::UNKNOWN:
                snprintf(tempStr, slen, "null");
                break;
            case DataType::INT:
                snprintf(tempStr, slen, "%d", getInt());
                outStr = std::string(tempStr);
                break;
            case DataType::NUMBER:
                snprintf(tempStr, slen, "%f", getNumber());
                outStr = std::string(tempStr);
                break;
            case DataType::BOOL:
                snprintf(tempStr, slen, "%s", getBoolean() ? "true" : "false");
                outStr = std::string(tempStr);
                break;
            case DataType::STRING:
                outStr = "\"" + std::string(getCharArray()) + "\"";
                break;
            default:
                break;
        }
        return outStr;
    }

    // return data type
    Value::DataType Value::type()
    {
        return _type;
    }

    // check empty
    bool Value::isEmpty()
    {
        return (_data == nullptr || _type == DataType::UNKNOWN);
    }

    // generate unknown value
    Value Value::unknown()
    {
        return Value();
    }

    // print value data type
    std::string Value::printType()
    {
        std::string outStr;
        const int slen = 15;
        char tempStr[slen + 1];
        switch (_type) {
            case DataType::UNKNOWN:
                snprintf(tempStr, slen, "UNKNOWN");
                break;
            case DataType::INT:
                snprintf(tempStr, slen, "INT");
                break;
            case DataType::NUMBER:
                snprintf(tempStr, slen, "NUMBER");
                break;
            case DataType::BOOL:
                snprintf(tempStr, slen, "BOOLEAN");
                break;
            case DataType::STRING:
                snprintf(tempStr, slen, "STRING");
                break;
            default:
                break;
        }
        return std::string(tempStr);
    }

    // move and copy function
    Value& Value::moveData(char*& src, const size_t size, const DataType type)
    {
        _type = type;
        _size = size;
        _data = src;
        src = nullptr;
        return *this;
    }

    // internal use
    Value& Value::copyData(const char* src, const size_t size, const DataType& type)
    {
        char* newData = new char[size];
        memcpy(newData, src, size);
        return moveData(newData, size, type);
    }

    // internal use
    void Value::clearData()
    {
        if (_size != 0 && _data != nullptr) {
            delete _data;
            _data = nullptr;
        }
    }

    // Option
    Config::Option::Option() : _flag(), _shortflag(), _description(), _defaultValue(Value::unknown()), _required(false), _hidden(false)
    {}

    Config::Option::~Option()
    {}

    Config::Option& Config::Option::flag(const std::string& flag)
    {
        _flag = flag;
        return *this;
    }

    Config::Option& Config::Option::shortflag(const std::string& shortflag)
    {
        _shortflag = shortflag;
        return *this;
    }

    Config::Option& Config::Option::description(const std::string& description)
    {
        _description = description;
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const Value& defaultValue)
    {
        _defaultValue = defaultValue;
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const int& defaultValue)
    {
        _defaultValue = static_cast<int>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const double& defaultValue)
    {
        _defaultValue = static_cast<double>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const bool& defaultValue)
    {
        _defaultValue = static_cast<bool>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const char* defaultValue)
    {
        _defaultValue = defaultValue;
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const std::string& defaultValue)
    {
        _defaultValue = static_cast<std::string>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::required(const bool required)
    {
        _required = required;
        return *this;
    }

    Config::Option& Config::Option::hidden(const bool hidden)
    {
        _hidden = hidden;
        return *this;
    }

    std::string Config::Option::flag()
    {
        return _flag;
    }

    std::string Config::Option::shortflag()
    {
        return _shortflag;
    }

    std::string Config::Option::description()
    {
        return _description;
    }

    Value Config::Option::defaultValue()
    {
        return _defaultValue;
    }

    bool Config::Option::required()
    {
        return _required;
    }

    bool Config::Option::hidden()
    {
        return _hidden;
    }

    Value::DataType Config::Option::type()
    {
        return _defaultValue.type();
    }

    Config::Config() :
            _verbose(false),
            _logLevel(Config::LogLevel::WARNING),
            _exeName(""),
            _description(""),
            _autoHelp(true),
            _loadConfig(true)
    {
        enableHelp(true); // set auto help to true
        enableConfig(true); // set auto config to true
    }

    Config::~Config()
    {
        _options.clear();
        _optionValues.clear();
        _log.clear();
    }

    Config::Option& Config::option(const std::string& flag)
    {
        _options.insert(std::make_pair(flag, Config::Option().flag(flag)));
        return _options[flag];
    }

    bool Config::remove(const std::string& flag)
    {
        if (findOption(flag)){
            _options.erase(flag);
            return true;
        }
        return false;
    }

    void Config::setDefaultValues()
    {
        for (auto && o : _options) {
            _optionValues[o.first] = o.second.defaultValue();
        }
    }

    void Config::log(const LogLevel logType)
    {
        _logLevel = logType;
    }

    void Config::log(FILE* fd)
    {
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "PARSE LOG");
        for (auto && logline : _log) {
            fprintf(fd, "%s\n", logline.c_str());
        }
    }

    void Config::log(Config::LogLevel logType, const std::string& token, const std::string& msg)
    {
        // do don't anything if log level is low
        if (logType < _logLevel) {
            return;
        }
        const int tagWidth = 16;
        char tag[tagWidth + 1];
        char format[tagWidth + 1];
        std::string logString = "";
        snprintf(format, tagWidth, "<<<%%%ds>>>", tagWidth - 7);
        switch (logType) {
            case Config::LogLevel::INFO:
                snprintf(tag, tagWidth, format, "INFO");
                break;
            case Config::LogLevel::WARNING:
                snprintf(tag, tagWidth, format, "WARNING");
                break;
            case Config::LogLevel::ERROR:
                snprintf(tag, tagWidth, format, "ERROR");
                break;
            default:
                break;
        }
        logString = std::string(tag) + " Input \"" + token + "\" : " + msg;
        _log.emplace_back(logString);
        if (_verbose) {
            fprintf(stdout, "%s\n", logString.c_str());
        }
    }

    Config::TokenType Config::getTokenType(const char* token)
    {
        // get token type:
        // starts with "--" - flag
        // starts with "-" - shortflag/value
        // otherwise value
        size_t tlen = strlen(token);
        if (tlen == 0) return TokenType::UNKNOWN;
        if (token[0] == '-') {
            // check if it can be converted to a number
            char* endptr = nullptr;
            std::strtod(token, &endptr);
            if (*endptr == '\0' && endptr != token) {
                return TokenType::VALUE;
            }
            return (token[1] == '-') ? TokenType::FLAG : TokenType::SHORTFLAG;
        }
        return TokenType::VALUE;
    }

    std::string Config::translateShortflag(const std::string& shortflag)
    {
        auto found = _options.begin();
        auto end = _options.end();
        for (; found != end; ++found) {
            if (found->second.shortflag() == shortflag) {
                break;
            }
        }
        if (found != end) {
            return found->first;
        }
        return shortflag;
    }

    bool Config::findOption(const std::string& flag)
    {
        return (_options.find(flag) != _options.end());
    }

    Config::Option* Config::getOption(const char* token, Config::TokenType tokenType)
    {
        std::string flag = "";
        if (tokenType == TokenType::FLAG) {
            flag = std::string(token + 2);
        } else if (tokenType == TokenType::SHORTFLAG) {
            flag = translateShortflag(std::string(token + 1));
        }
        if (flag.empty()) {
            return nullptr;
        }
        if (findOption(flag)) {
            return &(_options[flag]);
        }
        return nullptr;
    }

    Value Config::parseValue(const char* token, Value::DataType dataType)
    {
        if (dataType == Value::DataType::INT) {
            int v;
            int success = sscanf(token, "%d", &v);
            return (success == 1) ? Value(v) : Value::unknown();
        }
        if (dataType == Value::DataType::NUMBER) {
            double v;
            int success = sscanf(token, "%lf", &v);
            return (success == 1) ? Value(v) : Value();

        }
        if (dataType == Value::DataType::BOOL) {
            std::string v(token);
            if (v == "false" || v == "False" || v == "FALSE" || v == "F" || v == "f") {
                return Value(false);
            }
            return Value(true);

        }
        if (dataType == Value::DataType::STRING) {
            return Value(token);
        }
        return Value::unknown(); // fool-proof, return an unknown
    }

    static Config::LogLevel worseLevel(const Config::LogLevel& a, const Config::LogLevel& b)
    {
        return ((a) < (b)) ? (b) : (a);
    }

    Config::LogLevel Config::checkFormat()
    {
        LogLevel errorLv = LogLevel::INFO;
        for (auto && opt : _options) {
            Option& o = opt.second;
            // check for error
            if (!o.required() && o.defaultValue().isEmpty()) {
                log(LogLevel::ERROR, o.flag(), "default value is not defined");
                errorLv = worseLevel(errorLv, LogLevel::ERROR);
            }
            for (auto& opt2 : _options) {
                Option& o2 = opt2.second;
                if ((o.flag() != o2.flag()) && (o.shortflag() == o2.shortflag()) && !(o.shortflag().empty())) {
                    log(LogLevel::ERROR, o.flag(), "duplicate short flags (" + o2.shortflag() + ")");
                    errorLv = worseLevel(errorLv, LogLevel::ERROR);
                }
            }
            // check for warnings
            if (o.description().empty()) {
                log(LogLevel::WARNING, o.flag(), "no description text for argument");
                errorLv = worseLevel(errorLv, LogLevel::WARNING);
            }
            if (o.shortflag().empty()) {
                log(LogLevel::WARNING, o.flag(), "no short flag is provided");
                errorLv = worseLevel(errorLv, LogLevel::WARNING);
            }
        }
        if (_description.empty()) {
            log(LogLevel::WARNING, "", "No program description text is provided");
            errorLv = worseLevel(errorLv, LogLevel::WARNING);
        }
        return errorLv;
    }

    // Validate User Input
    Config::LogLevel Config::validate()
    {
        LogLevel errorLv = LogLevel::INFO;

        // remove all the hidden values
        for (auto&& opt: _options){
            if (opt.second.hidden() && contains(opt.first)){
                _optionValues.erase(opt.first);
            }
        }

        // scan for all option vlaues
        for (auto && val : _optionValues) {
            if (val.second.isEmpty()) {
                log(LogLevel::ERROR, val.first, "option contains invalid value");
                errorLv = worseLevel(errorLv, LogLevel::ERROR);
            }
        }

        // scan for all remaining options are defined
        for (auto && opt : _options) {
            if (!contains(opt.first) && !opt.second.hidden()) {
                log(LogLevel::ERROR, opt.first, "option is undefined");
                errorLv = worseLevel(errorLv, LogLevel::ERROR);
            }
        }
        return errorLv;
    }

    bool Config::parse(int argc, char **argv)
    {
        // Extract executable name
        _exeName = std::string(argv[0]);
        size_t lastslash = _exeName.find_last_of("\\/");
        if (lastslash != std::string::npos) {
            _exeName = _exeName.substr(lastslash + 1);
        }

        // check format of the option parser
        // if fatal error occurs and log level is not "NONE" (NONE = ignore errors)
        LogLevel checkFormatResult = checkFormat();
        if (checkFormatResult >= LogLevel::ERROR && _logLevel <= LogLevel::ERROR) {
            log();
            printf("\nFatal Error: Option format validation failed, abort.\n\n");
            return false;
        }

        // define a wildcard option to capture "stray" option values (values without a flag)
        // string argument by default
        Config::Option wildcard;
        wildcard.defaultValue("");

        // Value Precedence:
        // (1) Default Value
        // (2) Config File Settings (overwrites default values)
        // (3) Command Line Arguments (overwrites default values and config file)

        // * Set Default Values
        setDefaultValues();

        // * Load Config File before scanning for other arguments
        // case 1: only config file is defined, flag is not necessary
        // case 2: check if config flag has been defined
        if (_loadConfig) {
            for (int i = 1; i < argc - 1; ++i) {
                std::string flag = std::string(argv[i]);
                std::string value = std::string(argv[i + 1]);
                TokenType ttype = getTokenType(argv[i + 1]);
                if (( flag == "--config" || flag == "-cfg" ) && (ttype == TokenType::VALUE)) {
                    config(value);
                }
            }
        }

        // start normal parsing
        Option* currentOption = nullptr;
        for (int i = 1; i < argc; ++i) {
            TokenType currentTokenType = getTokenType(argv[i]);
            if (currentTokenType == TokenType::UNKNOWN) {
                log(LogLevel::ERROR, std::string(argv[i]), "unknown input");
            } else if (currentTokenType == TokenType::FLAG || currentTokenType == TokenType::SHORTFLAG) {
                currentOption = getOption(argv[i], currentTokenType);
                if (!currentOption) {
                    log(LogLevel::WARNING, std::string(argv[i]), "unrecognized flag");
                    if (currentTokenType == TokenType::FLAG) {
                        wildcard.flag(std::string(argv[i] + 2));
                        currentOption = &wildcard;
                    }
                }
                // special case - if the option type is bool, set to true by default
                if (currentOption && currentOption->type() == Value::DataType::BOOL) {
                    _optionValues[currentOption->flag()] = true;
                }
            } else if (currentTokenType == TokenType::VALUE) {
                if (currentOption) {
                    // parse the value according to default data type
                    Value newValue = parseValue(argv[i], currentOption->type());
                    // if value cannot be parsed
                    if (newValue.isEmpty()) {
                        log(LogLevel::WARNING, std::string(argv[i]), "unvalid value type is provided");
                    } else {
                        // assign parsed values
                        _optionValues[currentOption->flag()] = parseValue(argv[i], currentOption->type());
                        log(LogLevel::INFO, std::string(argv[i]), "value parsed successfully");
                    }
                    // reset current option flag -> ready for a new flag
                    currentOption = nullptr;
                } else {
                    // stray arguments, ignore
                    log(LogLevel::WARNING, std::string(argv[i]), "unassociated argument is not stored");
                }
            }
        }

        // if contains help and auto-help is enabled, display help message
        if (contains("help") && _optionValues["help"].getBoolean() && _autoHelp) {
            help();
        }

        // validate user inputs
        // remove all hidden options
        // reserved words should not be displayed as normal config values

        // if fatal error occurs and log level is not "NONE" (NONE = ignore errors)
        LogLevel validateResult = validate();
        if (validateResult >= LogLevel::ERROR && _logLevel <= LogLevel::ERROR) {
            log();
            printf("\nFatal Error: Option format validation failed, abort.\n\n");
            return false;
        }

        return true;
    }

    void Config::help(FILE* fd)
    {
        // print program description
        if (!_description.empty()) {
            fprintf(fd, "\n");
            if (!_exeName.empty()) {
                fprintf(fd, "[[[  %s  ]]]\n\n    ", _exeName.c_str());
            }
            fprintf(fd, "%s\n\n", _description.c_str());
        }

        // print usage
        usage();
        // print help
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "HELP");
        for (auto && opt : _options) {
            Option& o = opt.second;
            // print short
            fprintf(fd, "    ");
            if (!o.shortflag().empty()) {
                fprintf(fd, "-%s, ", o.shortflag().c_str());
            }
            // print long
            fprintf(fd, "--%s ", o.flag().c_str());
            // required
            if (o.required()) {
                fprintf(fd, "<REQUIRED>");
            }
            fprintf(fd, "\n");
            // print description
            fprintf(fd, "        ");
            if (!o.description().empty()) {
                fprintf(fd, "%s ", o.description().c_str());
            }
            // default value
            if (!o.defaultValue().isEmpty() && !o.hidden()) {
                fprintf(fd, " ( ");
                fprintf(fd, "DEFAULT = %s", o.defaultValue().print().c_str());
                fprintf(fd, " ) ");
            }
            fprintf(fd, "\n\n");
        }
    }

    void Config::usage(FILE* fd)
    {
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "USAGE");
        char exeTag[256];
        snprintf(exeTag, 256 - 1, "    %s ", (_exeName.empty()) ? ("<executable>") : (_exeName.c_str()));
        fprintf(fd, "%s", exeTag);
        int lineWidth = 0;
        for (auto && opt : _options) {
            Option& o = opt.second;
            char argTag[512];
            snprintf(argTag, 512 - 1, "%s%s%s <%s>%s",
                     o.required() ? "" : "[",
                     o.shortflag().empty() ? "--" : "-",
                     o.shortflag().empty() ? o.flag().c_str() : o.shortflag().c_str(),
                     o.defaultValue().printType().c_str(),
                     o.required() ? "" : "]");
            if (lineWidth + strlen(argTag) >= 80 - 1 - strlen(exeTag)) {
                fprintf(fd, "\n%*s", static_cast<int>(strlen(exeTag)), " ");
                lineWidth = 0;
            }
            fprintf(fd, "%s ", argTag);
            lineWidth += strlen(argTag);
        }
        fprintf(fd, "\n\n");
    }

    void Config::description(const std::string& desc)
    {
        _description = desc;
    }

    void Config::enableConfig(bool enabled)
    {
        _loadConfig = enabled;
        if (_autoHelp && !findOption("config")) {
            option("config").
                    shortflag("cfg").
                    defaultValue("").
                    description("Input configuration file (JSON/CSV)").
                    required(false).hidden(true);
        } else {
            if (findOption("config")) {
                _options.erase("config");
            }
        }

    }

    void Config::enableHelp(bool enabled)
    {
        _autoHelp = enabled;
        if (_autoHelp && !findOption("help")) {
            option("help").
                    shortflag("h").
                    defaultValue(false).
                    description("Display the help message").
                    required(false).hidden(true);
        } else {
            if (findOption("help")) {
                _options.erase("help");
            }
        }
    }

    void Config::verbose(bool value)
    {
        _verbose = value;
    }

    bool Config::contains(const std::string& flag)
    {
        return _optionValues.find(flag) != _optionValues.end();
    }

    Value& Config::operator[](const std::string& flag)
    {
        return _optionValues[flag];
    }

    Value const &Config::operator[](const std::string &flag) const {
        return _optionValues.at(flag);
    }

    void Config::print(FILE* fd)
    {
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "CONFIGURATION");

        printf("|-------------------------|------------|--------------------------------------------------|\n");
        printf("|           NAME          |    TYPE    |                     VALUE                        |\n");
        printf("|-------------------------|------------|--------------------------------------------------|\n");
        for (auto && v : _optionValues) {
            if (_options.find(v.first.c_str()) != _options.end()) {
                fprintf(fd, "| %-23s | %-10s | %-48s |\n", v.first.c_str(), v.second.printType().c_str(), v.second.print().c_str());
            } else {
                fprintf(fd, "| %-23s | %-10s | %-48s |\n", v.first.c_str(), (v.second.printType() + "*").c_str(), v.second.print().c_str());
            }
        }
        printf("|-------------------------|------------|--------------------------------------------------|\n");
        printf("\n");
    }

    std::string Config::serialize(const std::string& serializeFilePath, ExportFormat format, bool pretty)
    {
        std::stringstream ss;
        std::string outStr;

        // extract extension
        std::string extension = "";
        size_t lastDot = serializeFilePath.find_last_of(".");
        if (lastDot != std::string::npos) {
            extension = serializeFilePath.substr(lastDot + 1);
        }
#ifdef MINICONF_JSON_SUPPORT
        if (extension == "json" || extension == "JSON"){
            format = ExportFormat::JSON;
        } else if (extension == "csv" || extension == "CSV"){
            format = ExportFormat::CSV;
        } else {
            format = ExportFormat::CSV;
        }
#else
        format = ExportFormat::CSV;
#endif

        // serialize JSON
#ifdef MINICONF_JSON_SUPPORT
        if (format == ExportFormat::JSON) {
            picojson::value outObj = picojson::value(picojson::object());
            for (auto v: _optionValues){
                std::vector<std::string> flagTokens;
                std::stringstream ss(v.first);
                // tokenize
                while (ss.good()){
                    std::string tempToken;
                    std::getline(ss, tempToken, '.');
                    flagTokens.emplace_back(tempToken);
                }
                // parse
                if (flagTokens.size() > 1){
                    picojson::value* thisObj = &outObj;
                    for (size_t i = 0; i < flagTokens.size(); ++i){
                        if (i != flagTokens.size() - 1){
                            if (thisObj->get<picojson::object>().find(flagTokens[i]) == thisObj->get<picojson::object>().end()){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(picojson::object());
                            }
                            thisObj = &(thisObj->get<picojson::object>()[flagTokens[i]]);
                        } else {
                            if (v.second.type() == Value::DataType::INT){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(static_cast<double>(v.second.getInt()));
                            } else if (v.second.type() == Value::DataType::NUMBER){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(v.second.getNumber());
                            } else if (v.second.type() == Value::DataType::BOOL){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(v.second.getBoolean());
                            } else if (v.second.type() == Value::DataType::STRING){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(v.second.getString());
                            }

                        }
                    }
                }else{
                    if (outObj.get<picojson::object>().find(flagTokens[0]) == outObj.get<picojson::object>().end()){
                        if (v.second.type() == Value::DataType::INT){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(static_cast<double>(v.second.getInt()));
                        } else if (v.second.type() == Value::DataType::NUMBER){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(v.second.getNumber());
                        } else if (v.second.type() == Value::DataType::BOOL){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(v.second.getBoolean());
                        }  else if (v.second.type() == Value::DataType::STRING){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(v.second.getString());
                        }
                    }
                }
            }
            outStr = outObj.serialize(true);
        }
#endif

        // serialize CSV
        if (format == ExportFormat::CSV) {
            for (auto opt = std::begin(_optionValues); opt != std::end(_optionValues); ++opt) {
                std::string flag = opt->first;
                std::string val = opt->second.print();
                if (opt->second.type() == Value::DataType::STRING){
                    // remove "" from string
                    if (val.size() >= 2){
                        val = val.substr(1, val.size()-2);
                    }
                }
                ss << flag << "," << val << std::endl;
            }
            outStr = ss.str();
        }

        // write out file
        if (!serializeFilePath.empty()) {
            std::ofstream ofd(serializeFilePath);
            if (ofd.good()) {
                ofd << outStr;
                ofd.close();
            }
        }
        return outStr;
    }

    bool Config::config(const std::string& configPath)
    {
        // read content of the file
        std::ifstream ifd(configPath, std::ios::in | std::ios::binary);
        std::string configContent = "";
        if (ifd) {
            configContent = std::string((std::istreambuf_iterator<char>(ifd)), std::istreambuf_iterator<char>());
        }

        // extract extension
        std::string extension = "";
        size_t lastDot = configPath.find_last_of(".");
        if (lastDot != std::string::npos) {
            extension = configPath.substr(lastDot + 1);
        }

        // load config according to extension
        // default is json
#ifdef MINICONF_JSON_SUPPORT
        if (extension == "json" || extension == "JSON") {
            return loadJSON(configContent);
        } else if (extension == "csv" || extension == "CSV") {
            return loadCSV(configContent);
        } else {
            return loadJSON(configContent);
        }
#else
        return loadCSV(configContent);
#endif

        return false;
    }

    bool Config::loadCSV(const std::string& CSVStr)
    {
        std::stringstream ss(CSVStr);
        bool success = true;
        while (ss.good()){
            std::string templine;
            std::getline(ss, templine);
            std::stringstream lss(templine);
            if (templine.empty()){
                continue;
            }
            while (lss.good()){
                std::string sflag;
                std::string svalue;
                std::getline(lss, sflag, ',');
                std::getline(lss, svalue, ',');
                if (svalue.empty()){
                    continue;
                    success = false;
                }
                // check if options exists
                if (findOption(sflag)){
                    // parse the default data type
                    _optionValues[sflag] = parseValue(svalue.c_str(), _options[sflag].type());
                    log(LogLevel::INFO, std::string(sflag), "value is loaded from config");
                } else {
                    // parse string when the flag does not exist in the original configuration
                    _optionValues[sflag] = parseValue(svalue.c_str(), Value::DataType::STRING);
                    log(LogLevel::INFO, std::string(sflag), "value is not defined in config, parsed as a string value");
                }
            }
        }
        return success;
    }

#ifdef MINICONF_JSON_SUPPORT
    bool Config::getJSONValue(const picojson::value *v, const std::string& flag){
        bool success = true;
        if (findOption(flag)){
            Config::Option opt = _options[flag];
            if (opt.type() == Value::DataType::INT && v->is<double>()){
                _optionValues[flag] = static_cast<int>(v->get<double>());
            } else if (opt.type() == Value::DataType::NUMBER && v->is<double>()) {
                _optionValues[flag] = v->get<double>();
            } else if (opt.type() == Value::DataType::BOOL && v->is<bool>()) {
                _optionValues[flag] = v->get<bool>();
            } else if (opt.type() == Value::DataType::STRING && v->is<std::string>()) {
                _optionValues[flag] = v->get<std::string>();
            } else {
                log(LogLevel::WARNING, flag, "Unable to parse the option from config file, flag = " + flag);
                success = false;
            }
        }
            // stray options
        else
        {
            if (v->is<double>()){
                _optionValues[flag] = v->get<double>();
            }
            else if (v->is<bool>()) {
                _optionValues[flag] = v->get<bool>();
            } else if (v->is<std::string>()) {
                _optionValues[flag] = v->get<std::string>();
            } else {
                log(LogLevel::WARNING, flag, "Unable to parse the option from config file.");
                success = false;
            }
        }
        return success;
    }

    bool Config::parseJSON(const picojson::value *v, const std::string& flag){
        if (v->is<double>() || v->is<bool>() || v->is<std::string>()){
            return getJSONValue(v, flag);
        } else if (v->is<picojson::object>()){
            bool success = true;
            for (auto objItem: v->get<picojson::object>()){
                success = success && parseJSON(&(objItem.second), flag + (flag.empty() ? "" : ".") + objItem.first);
            }
            return success;
        } else if (v->is<picojson::array>()){
            for (auto arrayItem : v->get<picojson::array>()){
                // print array, not supported yet
            }
        } else {
            log(LogLevel::WARNING, flag, "Unable to parse JSON, abort, flag = " + flag);
            return false;
        }
        return false;
    }

    bool Config::loadJSON(const std::string& JSONStr)
    {
        picojson::value json;
        picojson::parse(json, JSONStr);
        return parseJSON(&json, "");
    }
#endif

}
