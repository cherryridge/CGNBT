#include <chrono>
#include <iostream>
#include <boost/unordered/unordered_flat_map.hpp>
#include <physfs.h>
#ifdef _WIN32
    #include <Windows.h>
#endif // _WIN32

#include <nbt/nbt.hpp>

int main() {
    using std::cout, std::endl, std::string, boost::unordered_flat_map, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds;

    cout << "========NBT========" << endl;

    unordered_flat_map<string, NBT::Tag> result;
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif // _WIN32
    PHYSFS_init(nullptr);
    PHYSFS_mount("E:/", "/", 1);

    auto start = steady_clock::now();
    if (NBT::read("a.cgb", result)) {
        auto end = steady_clock::now();
        cout << "Parse took " << duration_cast<microseconds>(end - start).count() << "us" << endl;
        auto str = NBT::serialize(result);
        cout << str << endl;
        cout << "==========Test Completed==========" << endl;
    }
    else {
        cout << "Parse failed! Errors:" << endl;
        auto errors = NBT::getErrors();
        for(const auto& error : errors) cout << error << endl;
    }
    return 0;
}