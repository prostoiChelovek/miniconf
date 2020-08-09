/*
 * miniconf.h
 *
 * A minimalist C++ configuration manager
 *
 * Author: Tsz-Ho Yu (tszhoyu@gmail.com)
 *
 */

/*
 * changelog:
 *
 * Version 1.0
 *     First workable version, polishing / documentation pending
 * Version 1.1
 *     Basic JSON export/import
 * Version 1.2
 *     Basic CSV export/import
 * Version 1.3 
 *     Code clean up
 * Version 1.4
 *     Support nested JSON
 * Version 1.5
 *     JSON-less version
 *
 */

#ifndef __MINICONF_H__
#define __MINICONF_H__

/* Comment the line below to disable JSON support */
#define MINICONF_JSON_SUPPORT

#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>

#ifdef MINICONF_JSON_SUPPORT
#include "picojson.h"
#endif

namespace miniconf
{

    /* A flexible container for multiple data type
     *
     * miniconf::Value is a flexible container for int, double, bool and char array. The 
     * actual value is stored in a buffer declared during assignment. An extra "unknown"
     * type is also defined for empty, or invalid value. 
     */
    class Value
    {
        public:

            // DataType specifies the datatype the instance is storing
            enum class DataType {
                UNKNOWN,
                INT,
                NUMBER,
                BOOL,
                STRING
            };

            /* Default constructors and assignments for Value, "unknown" type is assigned
             * to the instnace. It is suggested to use other constructor to assign valid
             * value when the instance is instantiated.
             */
            Value();

            // Copy assignment constructor
            Value(const Value& other);
            
            // Move assignment constructor 
            Value(Value&& other);

            // Assignment operator
            Value& operator=(const Value& other);

            // Move assignment operator
            Value& operator=(Value&& other);

            // Default destructor, releases buffer used to hold the value
            ~Value();

            // Constructs a Value instance from an integer 
            explicit Value(const int& other);
            
            // Constructs a Value instance from a floating point
            explicit Value(const double& other);
            
            // Constructs a Value instance from a boolean
            explicit Value(const bool& other);

            // Constructs a Value instance from a char array
            explicit Value(const char* other);
            
            // Constructs a Value instance from a std::string
            explicit Value(const std::string& other);
           
            // Assigns an integer to a Value instance
            Value& operator=(const int& other);
            
            // Assigns a floating point to a Value instance
            Value& operator=(const double& other);
            
            // Assigns a boolean to a Value instance
            Value& operator=(const bool& other);
            
            // Assigns a char array to a Value instance
            Value& operator=(const char* other);
            
            // Assigns a std::string to a Value instance
            Value& operator=(const std::string& other);
           
            // Casts a Value to an integer
            explicit operator int() const;

            // Casts a Value to a floating point number
            explicit operator double() const;

            // Casts a Value to a boolean
            explicit operator bool() const;

            // Casts a Value to a char array
            explicit operator char*() const;

            // Casts a Value to a std::string
            explicit operator std::string() const;
           
            // Explicitly gets an integer from a Value instance
            int getInt() const;
            
            // Explicitly gets a floating point number from a Value instance
            double getNumber() const;
            
            // Explicitly gets a boolean from a Value instance
            bool getBoolean() const;

            // Explicitly gets a char array from a Value instance
            char* getCharArray() const;

            // Explicitly gets a std::string from a Value instance
            std::string getString() const;

            // Serializes the value to a string
            std::string print();

            // Serializes the data type of the current value to a string, mainly for debugging purpose
            std::string printType();

            // Gets the data type of the current value
            DataType type();

            // Checks if the value is empty (unknown)
            bool isEmpty();

            // Generates an unknown (empty) Value object
            static Value unknown();

        private:

            // Moves value data from a pointer
            Value& moveData(char*& src, const size_t size, const DataType type);

            // Copies value data from a pointer
            Value& copyData(const char* src, const size_t size, const DataType& type);

            // Clears allocated value data
            void clearData();

            // It stores the data type of the current value
            DataType _type;

            // Number of bytes allocated to the buffer
            size_t _size;

            // The pointer to the value buffer
            char* _data;
    };

    /*
     * A Config object describes the configuration settings of an 
     * application. It contains a list of options which can be parsed from 
     * program arguments or from a configuration file (e.g. JSON).
     *
     * Configuration options are defined via the Config class's API. The
     * data type of each option is determined during compile time. User
     * can modify the values of the configuration options during run-time.
     * An option is invoked by a option flag, e.g. "--option" or "-o" in 
     * the command line arguments, or their corresponding attribute values
     * in a configuration file. Configuration file support can also be 
     * disabled manually.
     *
     * Basic sanity checks, such as default values and format checks are 
     * also performed automatically. User can display warnings and error
     * messages generated from the parser. 
     *
     * Help and usage messages can also be generated automatically from 
     * the application config settings (This feature can also be
     * disabled manually). 
     *
     */
    class Config
    {
        public:

            /* Level of error / log messages
             * 
             * The LogLevel enums defines the level of log / error messages generated
             * by the config / argument parser. 
             * * INFO - Normal log message
             * * WARNING = There are something thta need to be fixed, but not fatal
             * * ERROR  = Fatal error which  will cause undefined behavior
             * * NONE = No message will be generated, all logging is suppressed.
             */
            enum class LogLevel {
                INFO,
                WARNING,
                ERROR,
                NONE
            };

            /* File format for serialization 
             *
             * User can either serialize the current configuration, or 
             * write one config file manually using external editors.
             */
#ifdef MINICONF_JSON_SUPPORT
            enum class ExportFormat {
                JSON,
                CSV
            };
#else
            enum class ExportFormat {
                CSV
            };
#endif

            /* Option member class which describe the properties of a configuration option.
             * 
             * A complete configuration setting is composed of multiple options, 
             * each option is responsible for one property. Each option can be 
             * invoked by its flag (e.g. --flag) or shortened flag (e.g. -f). 
             * Some options are required to be set manually, while some options
             * have default values such that user input is not always required.
             * 
             * The data type of an option is determined from its defaultValue.
             */
            class Option;

            // Default constructor, no option is defined except the default "help" and "config"
            Config();

            // Default destructor
            ~Config();

            /* The main parsing function for input command line
             * @argc The number of input arguments
             * @argv An array of char array which contains the arguments, the first argument
             * should be the name of the executable
             * @return True when parsing is successful, False when parsing error(s) occur
             */
            bool parse(int argc, char** argv);
            
            // Creates a new configuration option, which is uniquely identified by its flag
            Config::Option& option(const std::string& flag);

            // Removes an option
            bool remove(const std::string& flag);

            // Checks if the option value is defined in the current configuration
            bool contains(const std::string& flag);
            
            // Sets a short description of the current application.
            void description(const std::string& desc);

            // Prints the current configuration settings
            void print(FILE* fd = stdout);

            // Prints the current log messages
            void log(FILE* fd = stdout);

            // Sets the logging level, NO
            void log(const LogLevel logType);

            // display the log message
            void verbose(bool value);
            
            /*Checks config format design and reports errors if necessary
             *
             * This function will validate the configuration format set by the user agsinst
             * invalid settings (e.g. optional arguments without a default value). This will
             * be called during the parse() process.
             *  
             * @return The most severe level of issue detected by the process
             */
            LogLevel checkFormat();

            /* Validates user input after parsing / loading config file
             * 
             * This validates the configuration after user input, e.g. all option values are 
             * defined and not of the UNKNOWN type. This will be called automatically 
             * after the option values have been parsed in the parse() function. 
             *
             * @return The most severe level of issue detected by the process
             */
            LogLevel validate();

            /* Accesses the configuration value
             *
             * If the configuration value does not exist, an empty Value object is returned. 
             */
            Value& operator[](const std::string& flag);

            /* Load the configuration settings via a config file
             * 
             * This function loads a config file, if the config file has been specified in 
             * command line arguments, this will be called automatically in "parse()" function.
             *
             * @configPath the input configuration file path
             */
            void config(const std::string& configPath);

            /* Serializes the current configuration
             *
             * Currently JSON and CSV are supported.
             */
#ifdef MINICONF_JSON_SUPPORT
            std::string serialize(const std::string& serializeFilePath = "", ExportFormat format = ExportFormat::JSON, bool pretty = true);
#else
            std::string serialize(const std::string& serializeFilePath = "", ExportFormat format = ExportFormat::CSV, bool pretty = true);
#endif

            // Enables automatically generated help message (--help/-h)
            void enableHelp(bool enabled = true);

            // Ennables setting via external config file (--config/-cfg)
            void enableConfig(bool enabled = true);

            // Prints usage of this program's configuration options
            void usage(FILE* fd = stdout);

            // Prints a automatically generated help message 
            void help(FILE* fd = stdout);

        private:

            /* Types of command line arguments
             *
             * "UNKNOWN" indicates a parsing error
             * A "FLAG" is command line flag which starts with "--"
             * A "SHORTFLAG" is shortened flag which starts with "-"
             * "VALUE" represent an argument value to be parsed
             */
            enum class TokenType {
                UNKNOWN,    // parsing error
                FLAG,       // a long flag, e.g. "--setting"
                SHORTFLAG,  // a short flag, e.g. "-s"
                VALUE       // value, e.g. 123, "hello", true
            };

            // Sets default values to option values, used in parse() for initialization
            void setDefaultValues();

            // get current token type
            TokenType getTokenType(const char* token);

            // since long flag is the key for the option directory,
            // this function transltes short flag to long flag
            std::string translateShortflag(const std::string& shortflag);

            // search for options using token
            Option* getOption(const char* token, Config::TokenType tokenType);

            // determine is a flag is defined in the config
           bool findOption(const std::string& flag);

            // parse a token into Value
            Value parseValue(const char* token, Value::DataType dataType);

#ifdef MINICONF_JSON_SUPPORT
            // load json config string
            bool loadJSON(const std::string& JSONStr);

            // parse a json value
            bool parseJSON(const picojson::value *v, const std::string& flag); 

            // load a value from json
            bool getJSONValue(const picojson::value *v, const std::string& flag); 
#endif

            // load csv config string
            bool loadCSV(const std::string& CSVStr);

            // internal function for adding log messages
            void log(LogLevel logType, const std::string& token, const std::string& msg);

            // this map stores configuration format design, e.g. flag, default values.
            std::map<std::string, Option> _options;

            // this map stores the values parsed form user input
            std::map<std::string, Value> _optionValues;

            // this is a stack of log messages
            std::vector<std::string> _log;

            // switch for verbose
            bool _verbose;  
            
            // log level setting
            LogLevel _logLevel; 
            
            // program name stripped from command line
            std::string _exeName; 
            
            // the program's description
            std::string _description; 
            
            // switch for auto help mesage
            bool _autoHelp; 
            
            // switch for enable loading configuration
            bool _loadConfig; 

    };

    /*
     * Describe one configuration options
     * A complete configuration setting is composed of multiple options, 
     * each option is responsible for one property. Each option can be 
     * invoked by its flag (e.g. --flag) or shortened flag (e.g. -f). 
     * Some options are required to be set manually, while some options
     * have default values such that user input is not always required.
     * 
     * The data type of an option is determined from its defaultValue.
     */
    class Config::Option
    {
        public:

            /* Default constructor that creates a empty option
             *
             * Use should use Config::option(flag) to create an new option
             */
            Option();

            // Default destructor
            ~Option();

            // Sets the flag of an option
            Config::Option& flag(const std::string& flag);

            // Sets the short flag of an option
            Config::Option& shortflag(const std::string& shortflag);

            // Sets the description of an option
            Config::Option& description(const std::string& description);

            // Sets the default value of an option from a Value object
            Config::Option& defaultValue(const Value& defaultValue);

            // Sets the default value of an option from an integer
            Config::Option& defaultValue(const int& defaultValue);
            
            // Sets the default value of an option from a floating point
            Config::Option& defaultValue(const double& defaultValue);
            
            // Sets the default value of an option from a boolean
            Config::Option& defaultValue(const bool& defaultValue);
            
            // Sets the default value of an option from a char array
            Config::Option& defaultValue(const char* defaultValue);
            
            // Sets the default value of an option from a string
            Config::Option& defaultValue(const std::string& defaultValue);

            // Makes an option to be required or optional
            Config::Option& required(const bool required);

            /* Sets an option to be hidden, so it will be erased after parsing
             *
             * This will be used for the default "help" and "config" options. 
             */
            Config::Option& hidden(const bool hidden);

            // Prints the flag of an option as a string
            std::string flag();

            // Prints the shortflag of an option as a string
            std::string shortflag();

            // Prints the description of an option
            std::string description();

            // Returns the default value of an option
            Value defaultValue();

            // Checks if the option is required or optional
            bool required();

            // Gets the data type 
            Value::DataType type();

            // Checks if an option is hidden
            bool hidden();

        private:

            // Flag of the option
            std::string     _flag;          
            
            // Shortened flag of the option
            std::string     _shortflag;     
            
            // Description of the option
            std::string     _description;   
            
            // Default value of the option
            Value           _defaultValue;
            
            // Option is required to be specified
            bool            _required;
            
            // Option is hidden
            bool            _hidden;

    };

}

// TODO: Stray arguments
// TODO: Support choice (value must be chosen form a list)
// TODO: Beautiful print, in help() and usage(), instead of printf()
// TODO: Support array
// TODO: Switch to JSON backend?

#endif // __MINICONF_H__
