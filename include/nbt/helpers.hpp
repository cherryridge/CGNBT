#pragma once

#include "mapLike.hpp"
#include "types.hpp"

namespace NBT::Helpers {
    using namespace NBT::Type;
    using NBT::MapLike::MapLike;

    template <Types T, typename P> requires MapLike<P>
    struct TagOf {
        using type = void;
    };

    #define LINK_TYPE_TO_TAG(type_, tag, member) \
    template <typename P> requires MapLike<P> \
    struct TagOf<type_, P> { \
        using type = tag; \
        static constexpr auto field = &Tag<P>::member; \
    };

    LINK_TYPE_TO_TAG(Types::Object, TagObject<P>, tagObject)
    LINK_TYPE_TO_TAG(Types::IVarInt, TagIVarInt, tagIVarInt)
    LINK_TYPE_TO_TAG(Types::UVarInt, TagUVarInt, tagUVarInt)
    LINK_TYPE_TO_TAG(Types::Bool, TagBool, tagBool)
    LINK_TYPE_TO_TAG(Types::Hex, TagHex, tagHex)
    LINK_TYPE_TO_TAG(Types::Float, TagFloat, tagFloat)
    LINK_TYPE_TO_TAG(Types::Double, TagDouble, tagDouble)
    LINK_TYPE_TO_TAG(Types::Array, TagArray<P>, tagArray)
    LINK_TYPE_TO_TAG(Types::String, TagString, tagString)
    LINK_TYPE_TO_TAG(Types::Raw, TagRaw, tagRaw)
    LINK_TYPE_TO_TAG(Types::ArrayBool, TagArrayBool, tagArrayBool)
    LINK_TYPE_TO_TAG(Types::ArrayHex, TagArrayHex, tagArrayHex)
    LINK_TYPE_TO_TAG(Types::ArrayFloat, TagArrayFloat, tagArrayFloat)
    LINK_TYPE_TO_TAG(Types::ArrayDouble, TagArrayDouble, tagArrayDouble)
    LINK_TYPE_TO_TAG(Types::ArrayRaw, TagArrayRaw, tagArrayRaw)

    #undef LINK_TYPE_TO_TAG

    template <Types T, typename P> requires MapLike<P>
    [[nodiscard]] inline decltype(TagOf<T, P>::type::payload) valueOr(const Tag<P>& tag, decltype(TagOf<T, P>::type::payload) defaultValue = {}) noexcept {
        if (tag.type != T) return defaultValue;
        return (tag.*(TagOf<T, P>::field)).payload;
    }

    template <Types T, typename P> requires MapLike<P>
    [[nodiscard]] inline decltype(TagOf<T, P>::type::payload) memberOr(const typename P::template map<string, Tag<P>>& members, const string& name, decltype(TagOf<T, P>::type::payload) defaultValue = {}) noexcept {
        const auto it = members.find(name);
        if (it == members.end()) return defaultValue;
        return valueOr<T, P>(it->second, defaultValue);
    }
}