#pragma once

#include "types.hpp"

namespace NBT::Helpers {
    using namespace NBT::Type;

    template <Types T>
    struct TagOf {
        using value = void;
    };

    #define LINK_TYPE_TO_TAG(type, tag, member) \
    template <> \
    struct TagOf<type> { \
        using value = tag; \
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
    [[nodiscard]] inline decltype(TagOf<T>::value::payload) valueOr(const Tag& tag, decltype(TagOf<T>::value::payload) defaultValue = {}) noexcept {
        if (tag.type != T) return defaultValue;
        return tag.*(TagOf<T>::field).payload;
    }
}