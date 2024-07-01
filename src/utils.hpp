#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>

std::string YELLOW = "\e[36m";
std::string RED = "\e[31m";
std::string END = "\e[0m";

struct Options {
    static bool verbose;
    static bool debug;
};

inline void warning(const std::string& str) { std::cerr << YELLOW << "[WRN]: " << str << END << std::endl; }
inline void error(const std::string& str) { std::cerr << RED << "[ERR]: " << str << END << std::endl; exit(2); }

#endif // UTILS_HPP
