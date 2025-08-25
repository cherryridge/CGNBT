#pragma once
#include <string>
#include <boost/unordered_map.hpp>

#include "type.hpp"

namespace NBT::IO {
    using std::string, boost::unordered_flat_map, NBT::Type::Tag;

    inline string serialize(const unordered_flat_map<string, Tag>& data) noexcept {
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

    inline string serialize(const Tag& singleTag) noexcept { return singleTag.toString(); }
}