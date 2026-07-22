#pragma once

#include <filesystem>

class Globals
{
public:
    static Globals &instance();

    std::filesystem::path local_dir;
    std::filesystem::path global_dir;

private:
    Globals();
};
