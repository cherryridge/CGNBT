#pragma once
#include <string>

#include "type.hpp"

namespace NBT::IO {
    using std::string, NBT::TypeNS::Tag, NBT::TypeNS::SupportedContainers;

    template<SupportedContainers T>
    inline string serialize(const T& data) noexcept {
        //Reason why we are not making a TagObject out of it and why is here even a `serialize.hpp`: NO DATA COPYING PLEASE!
        string result("{");
        bool first = true;
        for (const auto& [key, value] : data) {
            if (first) {
                first = false;
                result += "\"";
            }
            else result += ", \"";
            result += key;
            result += "\": ";
            result += value.toString();
        }
        result += "}";
        return result;
    }
}