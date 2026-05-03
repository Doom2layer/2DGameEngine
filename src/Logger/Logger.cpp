#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>

std::vector<LogEntry> Logger::LogEntries;

void Logger::Log(const std::string& Message)
{
    LogEntry Entry{LOG_INFO, Message};
    Print(std::cout, Message, WHITE, "INFO");
    LogEntries.push_back(Entry);
}

void Logger::Warning(const std::string& Message)
{
    LogEntry Entry{LOG_WARNING, Message};
    Print(std::cout, Message, YELLOW, "WARNING");
    LogEntries.push_back(Entry);   
}

void Logger::Error(const std::string& Message)
{
    LogEntry Entry{LOG_ERROR, Message};
    Print(std::cerr, Message, RED, "ERROR");
    LogEntries.push_back(Entry);  
}

std::string Logger::GetDateAndTime()
{
    std::time_t Now = std::time(nullptr);
    
    std::tm LocalTime{};
    localtime_s(&LocalTime, &Now);
    
    char Buffer[32];
    std::strftime(Buffer, sizeof(Buffer), "%Y-%m-%d %H:%M:%S", &LocalTime);
    return Buffer;   
}

void Logger::Print(std::ostream& Stream, const std::string& Message, const char* LabelColor, const std::string& Label)
{
    Stream << LabelColor << "[" << GetDateAndTime() << "] [" << Label << "] " << Message << RESET << "\n";
}

