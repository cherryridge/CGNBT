#pragma once
#include <array>
#include <format>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>

#include "auxiliary.hpp"
#include "utils.hpp"

namespace NBT::Type {
    typedef int8_t i8;
    typedef uint8_t u8;
    typedef char8_t c8;
    typedef int16_t i16;
    typedef uint16_t u16;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::vector, std::array, std::string, std::u8string, std::numeric_limits, std::to_string, std::format, std::move, std::forward, std::enable_if_t, std::decay_t, std::is_same_v, boost::unordered_flat_map, Utils::hexToString;

    template<typename T, typename U>
    using equal = enable_if_t<is_same_v<decay_t<T>, U>>;

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

    struct Tag;

    struct TagObject {
        unordered_flat_map<string, Tag> payload;

        [[nodiscard]] TagObject() noexcept = default;
        template<typename T, typename = equal<T, unordered_flat_map<string, Tag>>>
        [[nodiscard]] TagObject(T&& payload) noexcept : payload(forward<T>(payload)) {}

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
    struct TagArray {
        //`count` is encoded into the vector.
        //`type` is also encoded into the entries of vector. In fact it's not possible to not store them into union structs because we need a proper destructor.
        vector<Tag> payload;

        [[nodiscard]] TagArray() noexcept = default;
        template<typename T, typename = equal<T, vector<Tag>>>
        [[nodiscard]] TagArray(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept;
    };

    struct TagString {
        u8string payload;

        [[nodiscard]] TagString() noexcept = default;
        [[nodiscard]] TagString(const char* constStr) noexcept : payload(reinterpret_cast<const c8*>(constStr)) {}
        [[nodiscard]] TagString(const string& str) noexcept : payload(reinterpret_cast<const c8*>(str.c_str()), str.size()) {}
        template<typename T, typename = equal<T, u8string>>
        [[nodiscard]] TagString(T&& payload) noexcept : payload(forward<T>(payload)) {}

        [[nodiscard]] string toString() const noexcept { return "\"" + string(reinterpret_cast<const char*>(payload.c_str())) + "\""; }
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
        template<typename T, typename = equal<T, vector<u8>>>
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
        template<typename T, typename = equal<T, vector<u8>>>
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
        template<typename T, typename = equal<T, vector<float>>>
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
        template<typename T, typename = equal<T, vector<double>>>
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
        template<typename T, typename = equal<T, vector<u8>>>
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

    struct Tag {
        union {
            TagObject      tagObject;
            TagIVarInt     tagIVarInt;
            TagUVarInt     tagUVarInt;
            TagBool        tagBool;
            TagHex         tagHex;
            TagFloat       tagFloat;
            TagDouble      tagDouble;
            TagArray       tagArray;
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
        [[nodiscard]] Tag(const TagObject& copy)      noexcept : tagObject(copy),      type(Types::Object)      {}
        [[nodiscard]] Tag(const TagIVarInt& copy)     noexcept : tagIVarInt(copy),     type(Types::IVarInt)     {}
        [[nodiscard]] Tag(const TagUVarInt& copy)     noexcept : tagUVarInt(copy),     type(Types::UVarInt)     {}
        [[nodiscard]] Tag(const TagBool& copy)        noexcept : tagBool(copy),        type(Types::Bool)        {}
        [[nodiscard]] Tag(const TagHex& copy)         noexcept : tagHex(copy),         type(Types::Hex)         {}
        [[nodiscard]] Tag(const TagFloat& copy)       noexcept : tagFloat(copy),       type(Types::Float)       {}
        [[nodiscard]] Tag(const TagDouble& copy)      noexcept : tagDouble(copy),      type(Types::Double)      {}
        [[nodiscard]] Tag(const TagArray& copy)       noexcept : tagArray(copy),       type(Types::Array)       {}
        [[nodiscard]] Tag(const TagString& copy)      noexcept : tagString(copy),      type(Types::String)      {}
        [[nodiscard]] Tag(const TagRaw& copy)         noexcept : tagRaw(copy),         type(Types::Raw)         {}
        [[nodiscard]] Tag(const TagArrayBool& copy)   noexcept : tagArrayBool(copy),   type(Types::ArrayBool)   {}
        [[nodiscard]] Tag(const TagArrayHex& copy)    noexcept : tagArrayHex(copy),    type(Types::ArrayHex)    {}
        [[nodiscard]] Tag(const TagArrayFloat& copy)  noexcept : tagArrayFloat(copy),  type(Types::ArrayFloat)  {}
        [[nodiscard]] Tag(const TagArrayDouble& copy) noexcept : tagArrayDouble(copy), type(Types::ArrayDouble) {}
        [[nodiscard]] Tag(const TagArrayRaw& copy)    noexcept : tagArrayRaw(copy),    type(Types::ArrayRaw)    {}
        [[nodiscard]] Tag(TagObject&& _move)      noexcept : tagObject(move(_move)),      type(Types::Object)      {}
        [[nodiscard]] Tag(TagIVarInt&& _move)     noexcept : tagIVarInt(move(_move)),     type(Types::IVarInt)     {}
        [[nodiscard]] Tag(TagUVarInt&& _move)     noexcept : tagUVarInt(move(_move)),     type(Types::UVarInt)     {}
        [[nodiscard]] Tag(TagBool&& _move)        noexcept : tagBool(move(_move)),        type(Types::Bool)        {}
        [[nodiscard]] Tag(TagHex&& _move)         noexcept : tagHex(move(_move)),         type(Types::Hex)         {}
        [[nodiscard]] Tag(TagFloat&& _move)       noexcept : tagFloat(move(_move)),       type(Types::Float)       {}
        [[nodiscard]] Tag(TagDouble&& _move)      noexcept : tagDouble(move(_move)),      type(Types::Double)      {}
        [[nodiscard]] Tag(TagArray&& _move)       noexcept : tagArray(move(_move)),       type(Types::Array)       {}
        [[nodiscard]] Tag(TagString&& _move)      noexcept : tagString(move(_move)),      type(Types::String)      {}
        [[nodiscard]] Tag(TagRaw&& _move)         noexcept : tagRaw(move(_move)),         type(Types::Raw)         {}
        [[nodiscard]] Tag(TagArrayBool&& _move)   noexcept : tagArrayBool(move(_move)),   type(Types::ArrayBool)   {}
        [[nodiscard]] Tag(TagArrayHex&& _move)    noexcept : tagArrayHex(move(_move)),    type(Types::ArrayHex)    {}
        [[nodiscard]] Tag(TagArrayFloat&& _move)  noexcept : tagArrayFloat(move(_move)),  type(Types::ArrayFloat)  {}
        [[nodiscard]] Tag(TagArrayDouble&& _move) noexcept : tagArrayDouble(move(_move)), type(Types::ArrayDouble) {}
        [[nodiscard]] Tag(TagArrayRaw&& _move)    noexcept : tagArrayRaw(move(_move)),    type(Types::ArrayRaw)    {}

        Tag& operator=(const Tag& copy) noexcept {
            if (this == &copy) goto same;
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
            type = copy.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(copy.tagObject);           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(copy.tagIVarInt);         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(copy.tagUVarInt);         break;
                case Types::Bool:        new(&tagBool)        TagBool(copy.tagBool);               break;
                case Types::Hex:         new(&tagHex)         TagHex(copy.tagHex);                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(copy.tagFloat);             break;
                case Types::Double:      new(&tagDouble)      TagDouble(copy.tagDouble);           break;
                case Types::Array:       new(&tagArray)       TagArray(copy.tagArray);             break;
                case Types::String:      new(&tagString)      TagString(copy.tagString);           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(copy.tagRaw);                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(copy.tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(copy.tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(copy.tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(copy.tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(copy.tagArrayRaw);       break;
                default:                                                                           break;
            }
            same: return *this;
        }

        Tag& operator=(Tag&& _move) noexcept {
            if (this == &_move) goto same;
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
            type = _move.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(move(_move.tagObject));           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(move(_move.tagIVarInt));         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(move(_move.tagUVarInt));         break;
                case Types::Bool:        new(&tagBool)        TagBool(move(_move.tagBool));               break;
                case Types::Hex:         new(&tagHex)         TagHex(move(_move.tagHex));                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(move(_move.tagFloat));             break;
                case Types::Double:      new(&tagDouble)      TagDouble(move(_move.tagDouble));           break;
                case Types::Array:       new(&tagArray)       TagArray(move(_move.tagArray));             break;
                case Types::String:      new(&tagString)      TagString(move(_move.tagString));           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(move(_move.tagRaw));                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(move(_move).tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(move(_move).tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(move(_move).tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(move(_move).tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(move(_move).tagArrayRaw);       break;
                default:                                                                                  break;
            }
            same: return *this;
        }

        //We can't combine ctor/mtor with copy/move assignment operator when the struct has unions.
        //We can't determine whether the union is initialized if so, and we will be destructing garbage.
        [[nodiscard]] Tag(const Tag& copy) noexcept {
            type = copy.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(copy.tagObject);           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(copy.tagIVarInt);         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(copy.tagUVarInt);         break;
                case Types::Bool:        new(&tagBool)        TagBool(copy.tagBool);               break;
                case Types::Hex:         new(&tagHex)         TagHex(copy.tagHex);                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(copy.tagFloat);             break;
                case Types::Double:      new(&tagDouble)      TagDouble(copy.tagDouble);           break;
                case Types::Array:       new(&tagArray)       TagArray(copy.tagArray);             break;
                case Types::String:      new(&tagString)      TagString(copy.tagString);           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(copy.tagRaw);                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(copy.tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(copy.tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(copy.tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(copy.tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(copy.tagArrayRaw);       break;
                default:                                                                           break;
            }
        }

        [[nodiscard]] Tag(Tag&& _move) noexcept {
            type = _move.type;
            switch (type) {
                case Types::Object:      new(&tagObject)      TagObject(move(_move.tagObject));           break;
                case Types::IVarInt:     new(&tagIVarInt)     TagIVarInt(move(_move.tagIVarInt));         break;
                case Types::UVarInt:     new(&tagUVarInt)     TagUVarInt(move(_move.tagUVarInt));         break;
                case Types::Bool:        new(&tagBool)        TagBool(move(_move.tagBool));               break;
                case Types::Hex:         new(&tagHex)         TagHex(move(_move.tagHex));                 break;
                case Types::Float:       new(&tagFloat)       TagFloat(move(_move.tagFloat));             break;
                case Types::Double:      new(&tagDouble)      TagDouble(move(_move.tagDouble));           break;
                case Types::Array:       new(&tagArray)       TagArray(move(_move.tagArray));             break;
                case Types::String:      new(&tagString)      TagString(move(_move.tagString));           break;
                case Types::Raw:         new(&tagRaw)         TagRaw(move(_move.tagRaw));                 break;
                case Types::ArrayBool:   new(&tagArrayBool)   TagArrayBool(move(_move).tagArrayBool);     break;
                case Types::ArrayHex:    new(&tagArrayHex)    TagArrayHex(move(_move).tagArrayHex);       break;
                case Types::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(move(_move).tagArrayFloat);   break;
                case Types::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(move(_move).tagArrayDouble); break;
                case Types::ArrayRaw:    new(&tagArrayRaw)    TagArrayRaw(move(_move).tagArrayRaw);       break;
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
                default: return "";
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

    [[nodiscard]] inline string TagObject::toString() const noexcept {
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

    [[nodiscard]] inline string TagArray::toString() const noexcept {
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