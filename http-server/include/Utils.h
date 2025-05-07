#pragma once

#include <string>
#include <vector>

[[noreturn]] void fatal(const std::string &prefix, const std::string &msg);

std::string validateArgs(int argc, char *argv[]);

std::string execute(const std::vector<const char *> &argv);
