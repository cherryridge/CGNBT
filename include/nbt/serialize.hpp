#pragma once
#include <string>

#include "mapLike.hpp"
#include "types.hpp"

namespace NBT::IO {
    using std::string, NBT::Type::Tag, NBT::MapLike::MapLike;

    template <typename P> requires MapLike<P>
    inline string serialize(const typename P::template map<string, Tag<P>>& data) noexcept {
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

    template <typename P> requires MapLike<P>
    inline string serialize(const Tag<P>& singleTag) noexcept { return singleTag.toString(); }
}