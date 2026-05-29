#pragma once
#include <array>
#include <concepts>
#include <format>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "mapLike.hpp"
#include "utils.hpp"

namespace NBT::Type {
    typedef int8_t i8;
    typedef uint8_t u8;
    typedef int16_t i16;
    typedef uint16_t u16;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::vector, std::array, std::string, std::numeric_limits, std::to_string, std::format, std::move, std::forward, std::same_as, std::pair, std::enable_if_t, std::decay_t, std::is_same_v, Utils::hexToString, NBT::MapLike::MapLike;

    template<typename T, typename U>
    concept equal = is_same_v<decay_t<T>, U>;

    template<typename T>
    concept Uint = is_same_v<T, u8> || is_same_v<T, u16> || is_same_v<T, u32> || is_same_v<T, u64>;
    template<typename T>
    concept Sint = is_same_v<T, i8> || is_same_v<T, i16> || is_same_v<T, i32> || is_same_v<T, i64>;
    template<typename T>
    concept Int = Sint<T> || Uint<T>;

    enum struct Types : u8 {
        ObjectEnd = 0, Object, IVarInt, UVarInt, Bool, Hex, Float, Double, Array, String, Raw,
        ArrayBool, ArrayHex, ArrayFloat, ArrayDouble, ArrayRaw,
        Count
    };

    [[nodiscard]] inline constexpr Types getType(u8 head) noexcept {
        if ((head & 0xF0) == static_cast<u8>(Types::Array) << 4) switch (static_cast<Types>(head & 0x0F)) {
            case Types::Bool:   return Types::ArrayBool;
            case Types::Hex:    return Types::ArrayHex;
            case Types::Float:  return Types::ArrayFloat;
            case Types::Double: return Types::ArrayDouble;
            case Types::Raw:    return Types::ArrayRaw;
            default:            return Types::Array;
        }
        return static_cast<Types>((head & 0xF0) >> 4);
    }

    [[nodiscard]] inline constexpr Types getSecondType(u8 head) noexcept { return static_cast<Types>(head & 0x0F); }

    [[nodiscard]] inline constexpr Types getOriginalType(Types type) noexcept {
        switch (type) {
            case Types::ArrayBool:
            case Types::ArrayHex:
            case Types::ArrayFloat:
            case Types::ArrayDouble:
            case Types::ArrayRaw:
                return Types::Array;
            default:
                return type;
        }
    }

    template <typename P> requires MapLike<P>
    struct Tag;

    template <typename P> requires MapLike<P>
    struct TagObject {
        static_assert(same_as<typename P::template map<string, Tag<P>>::value_type, pair<const string, Tag<P>>>, "TagObject's template parameter must be a MapLike Policy with string keys and Tag values!");
        typename P::template map<string, Tag<P>> payload;

        [[nodiscard]] TagObject() noexcept = default;
        template<typename T1> requires equal<T1, typename P::template map<string, Tag<P>>>
        [[nodiscard]] TagObject(T1&& payload) noexcept : payload(forward<T1>(payload)) {}

        [[nodiscard]] string toString() const noexcept;
    };

    struct TagIVarInt {
        i64 payload;

        [[nodiscard]] string toString() const noexcept { return to_string(payload); }
    };

    struct TagUVarInt {
        u64 payload;

        [[nodiscard]] string toString() const noexcept { return to_string(payload) + "u"; }
    };

    struct TagBool {
        bool payload;

        [[nodiscard]] string toString() const noexcept { return payload ? "true" : "false"; }
    };

    struct TagHex {
        u8 payload;

        [[nodiscard]] string toString() const noexcept { return hexToString(payload); }
    };

    struct TagFloat {
        float payload;

        [[nodiscard]] string toString() const noexcept { return format("{:.{}g}", payload, numeric_limits<float>::max_digits10); }
    };

    struct TagDouble {
        double payload;

        [[nodiscard]] string toString() const noexcept { return format("{:.{}g}", payload, numeric_limits<double>::max_digits10); }
    };

    //This struct is used for arrays of `Object`s, `IVarInt`s, `UVarInt`s, `Array`s and `String`s.
    //There is a plan to separate `IVarInt` and `UVarInt` as well.
    template <typename P> requires MapLike<P>
    struct TagArray {
        //`count` is encoded into the vector.
        //`type` is also encoded into the entries of vector. In fact it's not possible to not store them into union structs because we need a proper destructor.
        vector<Tag<P>> payload;

        [[nodiscard]] TagArray() noexcept = default;
        template<typename T1> requires equal<T1, vector<Tag<P>>>
        [[nodiscard]] TagArray(T1&& payload) noexcept : payload(forward<T1>(payload)) {}

        [[nodiscard]] string toString() const noexcept;
    };

    struct TagString {
        string payload;

        [[nodiscard]] TagString() noexcept = default;
        [[nodiscard]] TagString(const char* constStr) noexcept : payload(constStr) {}
        [[nodiscard]] TagString(const string& str) noexcept : payload(str) {}
        template<typename T> requires equal<T, string>
        [[nodiscard]] TagString(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept { return "\"" + payload + "\""; }
    };

    struct TagRaw {
        u8 payload;

        [[nodiscard]] string toString() const noexcept { return string(hexToString(payload >> 4)) + hexToString(payload & 0x0F); }
    };

    //Important: `vector<bool>` uses 1-bit packed storage, which is not viable for bulk memory operations. 7 bit compensation for each entry is acceptable.
    //And, implicit conversion from any integer to `bool` is well-defined in the C++ standard aligning with CGNBT's specifications, so users can just use the value as they are `bool`s.
    struct TagArrayBool {
        //`count` is encoded into the vector.
        vector<u8> payload;

        [[nodiscard]] TagArrayBool() noexcept = default;
        template<typename T> requires equal<T, vector<u8>>
        [[nodiscard]] TagArrayBool(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept {
            string result("[");
            bool first = true;
            for (const auto& value : payload) {
                if (first) first = false;
                else result += ", ";
                result += value ? "true" : "false";
            }
            result += "]";
            return result;
        }
    };

    struct TagArrayHex {
        //`count` is encoded into the vector.
        vector<u8> payload;

        [[nodiscard]] TagArrayHex() noexcept = default;
        template<typename T> requires equal<T, vector<u8>>
        [[nodiscard]] TagArrayHex(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept {
            string result("[");
            bool first = true;
            for (const auto& value : payload) {
                if (first) first = false;
                else result += ", ";
                result += hexToString(value);
            }
            result += "]";
            return result;
        }
    };

    struct TagArrayFloat {
        //`count` is encoded into the vector.
        vector<float> payload;

        [[nodiscard]] TagArrayFloat() noexcept = default;
        template<typename T> requires equal<T, vector<float>>
        [[nodiscard]] TagArrayFloat(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept {
            string result("[");
            bool first = true;
            for (const auto& value : payload) {
                if (first) first = false;
                else result += ", ";
                result += format("{:.{}g}", value, numeric_limits<float>::max_digits10);
            }
            result += "]";
            return result;
        }
    };

    struct TagArrayDouble {
        //`count` is encoded into the vector.
        vector<double> payload;

        [[nodiscard]] TagArrayDouble() noexcept = default;
        template<typename T> requires equal<T, vector<double>>
        [[nodiscard]] TagArrayDouble(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept {
            string result("[");
            bool first = true;
            for (const auto& value : payload) {
                if (first) first = false;
                else result += ", ";
                result += format("{:.{}g}", value, numeric_limits<double>::max_digits10);
            }
            result += "]";
            return result;
        }
    };

    struct TagArrayRaw {
        //`count` is encoded into the vector.
        vector<u8> payload;

        [[nodiscard]] TagArrayRaw() noexcept = default;
        template<typename T> requires equal<T, vector<u8>>
        [[nodiscard]] TagArrayRaw(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept {
            string result("[");
            bool first = true;
            for (const auto& value : payload) {
                if (first) first = false;
                else result += ", ";
                result += string(hexToString(value >> 4)) + hexToString(value & 15);
            }
            result += "]";
            return result;
        }
    };

    template <typename P> requires MapLike<P>
    struct Tag {
        union {
            TagObject<P> tagObject;
            TagIVarInt     tagIVarInt;
            TagUVarInt     tagUVarInt;
            TagBool        tagBool;
            TagHex         tagHex;
            TagFloat       tagFloat;
            TagDouble      tagDouble;
            TagArray<P>  tagArray;
            TagString      tagString;
            TagRaw         tagRaw;
            TagArrayBool   tagArrayBool;
            TagArrayHex    tagArrayHex;
            TagArrayFloat  tagArrayFloat;
            TagArrayDouble tagArrayDouble;
            TagArrayRaw    tagArrayRaw;
        };
        Types type;

        #pragma warning(suppress: 26495)
        [[nodiscard]] Tag() noexcept : type(Types::Count) {}
        [[nodiscard]] Tag(const TagObject<P>& other) noexcept : tagObject(other),      type(Types::Object)      {}
        [[nodiscard]] Tag(const TagIVarInt& other)     noexcept : tagIVarInt(other),     type(Types::IVarInt)     {}
        [[nodiscard]] Tag(const TagUVarInt& other)     noexcept : tagUVarInt(other),     type(Types::UVarInt)     {}
        [[nodiscard]] Tag(const TagBool& other)        noexcept : tagBool(other),        type(Types::Bool)        {}
        [[nodiscard]] Tag(const TagHex& other)         noexcept : tagHex(other),         type(Types::Hex)         {}
        [[nodiscard]] Tag(const TagFloat& other)       noexcept : tagFloat(other),       type(Types::Float)       {}
        [[nodiscard]] Tag(const TagDouble& other)      noexcept : tagDouble(other),      type(Types::Double)      {}
        [[nodiscard]] Tag(const TagArray<P>& other)  noexcept : tagArray(other),       type(Types::Array)       {}
        [[nodiscard]] Tag(const TagString& other)      noexcept : tagString(other),      type(Types::String)      {}
        [[nodiscard]] Tag(const TagRaw& other)         noexcept : tagRaw(other),         type(Types::Raw)         {}
        [[nodiscard]] Tag(const TagArrayBool& other)   noexcept : tagArrayBool(other),   type(Types::ArrayBool)   {}
        [[nodiscard]] Tag(const TagArrayHex& other)    noexcept : tagArrayHex(other),    type(Types::ArrayHex)    {}
        [[nodiscard]] Tag(const TagArrayFloat& other)  noexcept : tagArrayFloat(other),  type(Types::ArrayFloat)  {}
        [[nodiscard]] Tag(const TagArrayDouble& other) noexcept : tagArrayDouble(other), type(Types::ArrayDouble) {}
        [[nodiscard]] Tag(const TagArrayRaw& other)    noexcept : tagArrayRaw(other),    type(Types::ArrayRaw)    {}
        [[nodiscard]] Tag(TagObject<P>&& other) noexcept : tagObject(move(other)),      type(Types::Object)      {}
        [[nodiscard]] Tag(TagIVarInt&& other)     noexcept : tagIVarInt(move(other)),     type(Types::IVarInt)     {}
        [[nodiscard]] Tag(TagUVarInt&& other)     noexcept : tagUVarInt(move(other)),     type(Types::UVarInt)     {}
        [[nodiscard]] Tag(TagBool&& other)        noexcept : tagBool(move(other)),        type(Types::Bool)        {}
        [[nodiscard]] Tag(TagHex&& other)         noexcept : tagHex(move(other)),         type(Types::Hex)         {}
        [[nodiscard]] Tag(TagFloat&& other)       noexcept : tagFloat(move(other)),       type(Types::Float)       {}
        [[nodiscard]] Tag(TagDouble&& other)      noexcept : tagDouble(move(other)),      type(Types::Double)      {}
        [[nodiscard]] Tag(TagArray<P>&& other)  noexcept : tagArray(move(other)),       type(Types::Array)       {}
        [[nodiscard]] Tag(TagString&& other)      noexcept : tagString(move(other)),      type(Types::String)      {}
        [[nodiscard]] Tag(TagRaw&& other)         noexcept : tagRaw(move(other)),         type(Types::Raw)         {}
        [[nodiscard]] Tag(TagArrayBool&& other)   noexcept : tagArrayBool(move(other)),   type(Types::ArrayBool)   {}
        [[nodiscard]] Tag(TagArrayHex&& other)    noexcept : tagArrayHex(move(other)),    type(Types::ArrayHex)    {}
        [[nodiscard]] Tag(TagArrayFloat&& other)  noexcept : tagArrayFloat(move(other)),  type(Types::ArrayFloat)  {}
        [[nodiscard]] Tag(TagArrayDouble&& other) noexcept : tagArrayDouble(move(other)), type(Types::ArrayDouble) {}
        [[nodiscard]] Tag(TagArrayRaw&& other)    noexcept : tagArrayRaw(move(other)),    type(Types::ArrayRaw)    {}

        Tag& operator=(const Tag& other) noexcept {
            if (this == &other) goto same;
            switch (type) {
                case Types::Object:      tagObject.~TagObject();           break;
                case Types::IVarInt:     tagIVarInt.~TagIVarInt();         break;
                case Types::UVarInt:     tagUVarInt.~TagUVarInt();         break;
                case Types::Bool:        tagBool.~TagBool();               break;
                case Types::Hex:         tagHex.~TagHex();                 break;
                case Types::Float:       tagFloat.~TagFloat();             break;
                case Types::Double:      tagDouble.~TagDouble();           break;
                case Types::Array:       tagArray.~TagArray();             break;
                case Types::String:      tagString.~TagString();           break;
                case Types::Raw:         tagRaw.~TagRaw();                 break;
                case Types::ArrayBool:   tagArrayBool.~TagArrayBool();     break;
                case Types::ArrayHex:    tagArrayHex.~TagArrayHex();       break;
                case Types::ArrayFloat:  tagArrayFloat.~TagArrayFloat();   break;
                case Types::ArrayDouble: tagArrayDouble.~TagArrayDouble(); break;
                case Types::ArrayRaw:    tagArrayRaw.~TagArrayRaw();       break;
                default:                                                   break;
            }
            type = other.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(other.tagObject);           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(other.tagIVarInt);         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(other.tagUVarInt);         break;
                case Types::Bool:        new(&tagBool)        TagBool(other.tagBool);               break;
                case Types::Hex:         new(&tagHex)         TagHex(other.tagHex);                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(other.tagFloat);             break;
                case Types::Double:      new(&tagDouble)      TagDouble(other.tagDouble);           break;
                case Types::Array:       new(&tagArray)       TagArray(other.tagArray);             break;
                case Types::String:      new(&tagString)      TagString(other.tagString);           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(other.tagRaw);                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(other.tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(other.tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(other.tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(other.tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(other.tagArrayRaw);       break;
                default:                                                                            break;
            }
            same: return *this;
        }

        Tag& operator=(Tag&& other) noexcept {
            if (this == &other) goto same;
            switch (type) {
                case Types::Object:      tagObject.~TagObject();           break;
                case Types::IVarInt:     tagIVarInt.~TagIVarInt();         break;
                case Types::UVarInt:     tagUVarInt.~TagUVarInt();         break;
                case Types::Bool:        tagBool.~TagBool();               break;
                case Types::Hex:         tagHex.~TagHex();                 break;
                case Types::Float:       tagFloat.~TagFloat();             break;
                case Types::Double:      tagDouble.~TagDouble();           break;
                case Types::Array:       tagArray.~TagArray();             break;
                case Types::String:      tagString.~TagString();           break;
                case Types::Raw:         tagRaw.~TagRaw();                 break;
                case Types::ArrayBool:   tagArrayBool.~TagArrayBool();     break;
                case Types::ArrayHex:    tagArrayHex.~TagArrayHex();       break;
                case Types::ArrayFloat:  tagArrayFloat.~TagArrayFloat();   break;
                case Types::ArrayDouble: tagArrayDouble.~TagArrayDouble(); break;
                case Types::ArrayRaw:    tagArrayRaw.~TagArrayRaw();       break;
                default:                                                   break;
            }
            type = other.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(move(other.tagObject));           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(move(other.tagIVarInt));         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(move(other.tagUVarInt));         break;
                case Types::Bool:        new(&tagBool)        TagBool(move(other.tagBool));               break;
                case Types::Hex:         new(&tagHex)         TagHex(move(other.tagHex));                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(move(other.tagFloat));             break;
                case Types::Double:      new(&tagDouble)      TagDouble(move(other.tagDouble));           break;
                case Types::Array:       new(&tagArray)       TagArray(move(other.tagArray));             break;
                case Types::String:      new(&tagString)      TagString(move(other.tagString));           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(move(other.tagRaw));                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(move(other).tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(move(other).tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(move(other).tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(move(other).tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(move(other).tagArrayRaw);       break;
                default:                                                                                  break;
            }
            same: return *this;
        }

        //We can't combine ctor/mtor with copy/move assignment operator when the struct has unions.
        //We can't determine whether the union is initialized if so, and we will be destructing garbage.
        [[nodiscard]] Tag(const Tag& other) noexcept {
            type = other.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(other.tagObject);           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(other.tagIVarInt);         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(other.tagUVarInt);         break;
                case Types::Bool:        new(&tagBool)        TagBool(other.tagBool);               break;
                case Types::Hex:         new(&tagHex)         TagHex(other.tagHex);                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(other.tagFloat);             break;
                case Types::Double:      new(&tagDouble)      TagDouble(other.tagDouble);           break;
                case Types::Array:       new(&tagArray)       TagArray(other.tagArray);             break;
                case Types::String:      new(&tagString)      TagString(other.tagString);           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(other.tagRaw);                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(other.tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(other.tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(other.tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(other.tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(other.tagArrayRaw);       break;
                default:                                                                            break;
            }
        }

        [[nodiscard]] Tag(Tag&& other) noexcept {
            type = other.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(move(other.tagObject));           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(move(other.tagIVarInt));         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(move(other.tagUVarInt));         break;
                case Types::Bool:        new(&tagBool)        TagBool(move(other.tagBool));               break;
                case Types::Hex:         new(&tagHex)         TagHex(move(other.tagHex));                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(move(other.tagFloat));             break;
                case Types::Double:      new(&tagDouble)      TagDouble(move(other.tagDouble));           break;
                case Types::Array:       new(&tagArray)       TagArray(move(other.tagArray));             break;
                case Types::String:      new(&tagString)      TagString(move(other.tagString));           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(move(other.tagRaw));                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(move(other).tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(move(other).tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(move(other).tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(move(other).tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(move(other).tagArrayRaw);       break;
                default:                                                                                  break;
            }
        }

        [[nodiscard]] string toString() const noexcept {
            switch (type) {
                case Types::Object:      return tagObject.toString();
                case Types::IVarInt:     return tagIVarInt.toString();
                case Types::UVarInt:     return tagUVarInt.toString();
                case Types::Bool:        return tagBool.toString();
                case Types::Hex:         return tagHex.toString();
                case Types::Float:       return tagFloat.toString();
                case Types::Double:      return tagDouble.toString();
                case Types::Array:       return tagArray.toString();
                case Types::String:      return tagString.toString();
                case Types::Raw:         return tagRaw.toString();
                case Types::ArrayBool:   return tagArrayBool.toString();
                case Types::ArrayHex:    return tagArrayHex.toString();
                case Types::ArrayFloat:  return tagArrayFloat.toString();
                case Types::ArrayDouble: return tagArrayDouble.toString();
                case Types::ArrayRaw:    return tagArrayRaw.toString();
                default: return "<invalid type>";
            }
        }
        
        ~Tag() {
            switch (type) {
                case Types::Object:      tagObject.~TagObject();           break;
                case Types::IVarInt:     tagIVarInt.~TagIVarInt();         break;
                case Types::UVarInt:     tagUVarInt.~TagUVarInt();         break;
                case Types::Bool:        tagBool.~TagBool();               break;
                case Types::Hex:         tagHex.~TagHex();                 break;
                case Types::Float:       tagFloat.~TagFloat();             break;
                case Types::Double:      tagDouble.~TagDouble();           break;
                case Types::Array:       tagArray.~TagArray();             break;
                case Types::String:      tagString.~TagString();           break;
                case Types::Raw:         tagRaw.~TagRaw();                 break;
                case Types::ArrayBool:   tagArrayBool.~TagArrayBool();     break;
                case Types::ArrayHex:    tagArrayHex.~TagArrayHex();       break;
                case Types::ArrayFloat:  tagArrayFloat.~TagArrayFloat();   break;
                case Types::ArrayDouble: tagArrayDouble.~TagArrayDouble(); break;
                case Types::ArrayRaw:    tagArrayRaw.~TagArrayRaw();       break;
                default:                                                   break;
            }
        }
    };

    template <typename P> requires MapLike<P>
    [[nodiscard]] inline string TagObject<P>::toString() const noexcept {
        string result("{");
        bool first = true;
        for (const auto& [key, value] : payload) {
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
    [[nodiscard]] inline string TagArray<P>::toString() const noexcept {
        string result("[");
        bool first = true;
        for (const auto& value : payload) {
            if (first) first = false;
            else result += ", ";
            result += value.toString();
        }
        result += "]";
        return result;
    }
}