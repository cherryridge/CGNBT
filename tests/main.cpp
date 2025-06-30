#include <iostream>
#include <vector>
#include <chrono>
#include <boost/unordered/unordered_flat_map.hpp>

#include <nbt/nbt.hpp>

int main() {
    using std::cout, std::endl, std::vector, std::string, std::get_if, boost::unordered_flat_map, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds;

    cout << "========NBT========" << endl;

    auto start = steady_clock::now();
    auto asdf = NBT::read("E:/a.cgb");
    auto end = steady_clock::now();
    cout << "Parse took " << duration_cast<microseconds>(end - start).count() << "us" << endl;

    if (const auto* tagp = get_if<unordered_flat_map<string, NBT::Tag>>(&asdf)) {
        NBT::inspect(*tagp);
        cout << "==========Test Completed==========" << endl; 
    }
    else {
        cout << "Parse failed!" << endl;
        const auto& errors = *get_if<vector<string>>(&asdf);
        for (const auto& str : errors) cout << str << endl;
    }
    
    return 0;
}