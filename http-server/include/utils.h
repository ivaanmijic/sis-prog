#pragma once

#include <string>

[[noreturn]] void fatal(const std::string &msg);

std::string validateArgs(int argc, char *argv[]);
