#pragma once
#include <string>
#include <vector>

enum LogType : uint8_t
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

struct LogEntry
{
    LogType Type;
    std::string Message;
};

class Logger
{
public:
    static void Log(const std::string& Message);
    static void Warning(const std::string& Message);
    static void Error(const std::string& Message);
    
    static std::vector<LogEntry> LogEntries;
    
private:
    static std::string GetDateAndTime();
    static void Print(std::ostream& Stream, const std::string& Message, const char* LabelColor, const std::string& Label);
    
    // ANSI Color Codes
    static constexpr const char* RESET   = "\033[0m";
    static constexpr const char* WHITE   = "\033[37m";
    static constexpr const char* GREEN   = "\033[32m";
    static constexpr const char* YELLOW  = "\033[33m";
    static constexpr const char* RED     = "\033[31m";
    static constexpr const char* GRAY    = "\033[90m";
};
