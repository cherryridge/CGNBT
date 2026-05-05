#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "types.hpp"

namespace NBT::Helpers {
    using namespace NBT::Type;
    using boost::unordered_flat_map;

    template <Types T>
    struct TagOf {
        using type = void;
    };

    #define LINK_TYPE_TO_TAG(type_, tag, member) \
    template <> \
    struct TagOf<type_> { \
        using type = tag; \
        static constexpr auto field = &Tag::member; \
    };

    LINK_TYPE_TO_TAG(Types::Object, TagObject, tagObject)
    LINK_TYPE_TO_TAG(Types::IVarInt, TagIVarInt, tagIVarInt)
    LINK_TYPE_TO_TAG(Types::UVarInt, TagUVarInt, tagUVarInt)
    LINK_TYPE_TO_TAG(Types::Bool, TagBool, tagBool)
    LINK_TYPE_TO_TAG(Types::Hex, TagHex, tagHex)
    LINK_TYPE_TO_TAG(Types::Float, TagFloat, tagFloat)
    LINK_TYPE_TO_TAG(Types::Double, TagDouble, tagDouble)
    LINK_TYPE_TO_TAG(Types::Array, TagArray, tagArray)
    LINK_TYPE_TO_TAG(Types::String, TagString, tagString)
    LINK_TYPE_TO_TAG(Types::Raw, TagRaw, tagRaw)
    LINK_TYPE_TO_TAG(Types::ArrayBool, TagArrayBool, tagArrayBool)
    LINK_TYPE_TO_TAG(Types::ArrayHex, TagArrayHex, tagArrayHex)
    LINK_TYPE_TO_TAG(Types::ArrayFloat, TagArrayFloat, tagArrayFloat)
    LINK_TYPE_TO_TAG(Types::ArrayDouble, TagArrayDouble, tagArrayDouble)
    LINK_TYPE_TO_TAG(Types::ArrayRaw, TagArrayRaw, tagArrayRaw)

    #undef LINK_TYPE_TO_TAG

    template <Types T>
    [[nodiscard]] inline decltype(TagOf<T>::type::payload) valueOr(const Tag& tag, decltype(TagOf<T>::type::payload) defaultValue = {}) noexcept {
        if (tag.type != T) return defaultValue;
        return tag.*(TagOf<T>::field).payload;
    }

    template <Types T>
    [[nodiscard]] inline decltype(TagOf<T>::type::payload) memberOr(const unordered_flat_map<string, Tag>& members, const string& name, decltype(TagOf<T>::type::payload) defaultValue = {}) noexcept {
        const auto it = members.find(name);
        if (it == members.end()) return defaultValue;
        return valueOr<T>(it->second, defaultValue);
    }
}