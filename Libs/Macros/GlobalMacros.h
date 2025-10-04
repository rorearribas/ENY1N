#pragma once
#include <windows.h>
#include <iostream>

// GLOBAL MACROS
#define TO_STRING(x) #x
#define UNUSED_VAR(x) ((void)(x))
#define UNUSED_VARS(...) UNUSED_VAR(__VA_ARGS__)

// LOGS
#define LOG(x) do { \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); \
    std::cout << x << std::endl; \
} while(0)

#define SUCCESS_LOG(x) do { \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); \
    std::cout << x << std::endl; \
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); \
} while(0)

#define WARNING_LOG(x) do { \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); \
    std::cout << x << std::endl; \
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); \
} while(0)

#define ERROR_LOG(x) do { \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); \
    std::cout << x << std::endl; \
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); \
} while(0)

// MODES
#define DEBUG_MODE
