#pragma once
#include <vector>
#include <unordered_map>
#include <array>
#include <cassert>
#include <string>
#include <type_traits>

#include "VarText.hpp"
#include "VarInt.hpp"

namespace NBT::TypeNS {
	typedef uint8_t u8;
	typedef char8_t c8;
	typedef uint64_t u64;
	using std::vector, std::array, std::unordered_map, std::string, std::u8string, std::move, std::forward, std::enable_if_t, std::decay_t, std::is_same_v, VarTextNS::VarText, VarIntNS::UVarInt, VarIntNS::IVarInt;

	template<typename T, typename U>
	concept equal = is_same_v<decay_t<T>, U>;

	enum struct Types : u8 {
		ObjectEnd = 0, Object, IVarInt, UVarInt, Bool, Hex, Float, Double, Array, Utf8, Raw, ArrayBool, ArrayHex, ArrayFloat, ArrayDouble, ArrayUtf8, ArrayRaw, Count
	};

	//No `ObjectEnd`.
	enum struct TypesN : u8 {
		Object = 1, IVarInt, UVarInt, Bool, Hex, Float, Double, Array, Utf8, Raw, ArrayBool, ArrayHex, ArrayFloat, ArrayDouble, ArrayUtf8, ArrayRaw, Count
	};

	inline constexpr Types toTypes(TypesN type) noexcept { return static_cast<Types>(type); }
	//It is not guaranteed to success.
	inline constexpr TypesN toTypesN(Types type) noexcept {
		assert(type != Types::ObjectEnd);
		return static_cast<TypesN>(type);
	}

	inline constexpr const char* getTypeStr(Types type) noexcept {
		switch (type) {
			case Types::ObjectEnd: return "ObjectEnd";
			case Types::Object: return "Object";
			case Types::IVarInt: return "IVarInt";
			case Types::UVarInt: return "UVarInt";
			case Types::Bool: return "Bool";
			case Types::Hex: return "Hexadecimal";
			case Types::Float: return "Float";
			case Types::Double: return "Double";
			case Types::Array: return "Array";
			case Types::Utf8: return "UTF8";
			case Types::Raw: return "Raw";
			case Types::ArrayBool: return "Array(Bool";
			case Types::ArrayHex: return "Array(Hexadecimal";
			case Types::ArrayFloat: return "Array(Float";
			case Types::ArrayDouble: return "Array(Double";
			case Types::ArrayUtf8: return "Array(UTF8";
			case Types::ArrayRaw: return "Array(Raw";
			default: return "Invalid";
		}
	}

	struct Tag;

	struct TagObject {
		unordered_map<string, Tag> payload;

		TagObject& operator=(const TagObject& copy) noexcept = default;
		TagObject& operator=(TagObject&& move) noexcept = default;
		TagObject(const TagObject& copy) noexcept = default;
		TagObject(TagObject&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, unordered_map<string, Tag>>>>
		TagObject(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagObject(int) noexcept {}
		~TagObject() = default;
	};

	struct TagIVarInt {
		IVarInt payload;

		TagIVarInt& operator=(const TagIVarInt& copy) noexcept = default;
		TagIVarInt& operator=(TagIVarInt&& move) noexcept = default;
		TagIVarInt(const TagIVarInt& copy) noexcept = default;
		TagIVarInt(TagIVarInt&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, IVarInt>>>
		TagIVarInt(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagIVarInt(int) noexcept : payload(true) {}
		~TagIVarInt() = default;
	};

	struct TagUVarInt {
		UVarInt payload;

		TagUVarInt& operator=(const TagUVarInt& copy) noexcept = default;
		TagUVarInt& operator=(TagUVarInt&& move) noexcept = default;
		TagUVarInt(const TagUVarInt& copy) noexcept = default;
		TagUVarInt(TagUVarInt&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, UVarInt>>>
		TagUVarInt(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagUVarInt(int) noexcept : payload(true) {}
		~TagUVarInt() = default;
	};

	struct TagBool {
		bool payload;

		TagBool& operator=(const TagBool& copy) noexcept = default;
		TagBool& operator=(TagBool&& move) noexcept = default;
		TagBool(const TagBool& copy) noexcept = default;
		TagBool(TagBool&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, bool>>>
		TagBool(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagBool(int) noexcept {}
		~TagBool() = default;
	};

	struct TagHex {
		u8 payload;

		TagHex& operator=(const TagHex& copy) noexcept = default;
		TagHex& operator=(TagHex&& move) noexcept = default;
		TagHex(const TagHex& copy) noexcept = default;
		TagHex(TagHex&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, u8>>>
		TagHex(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagHex(int) noexcept {}
		~TagHex() = default;
	};

	struct TagFloat {
		float payload;

		TagFloat& operator=(const TagFloat& copy) noexcept = default;
		TagFloat& operator=(TagFloat&& move) noexcept = default;
		TagFloat(const TagFloat& copy) noexcept = default;
		TagFloat(TagFloat&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, float>>>
		TagFloat(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagFloat(int) noexcept {}
		~TagFloat() = default;
	};

	struct TagDouble {
		double payload;

		TagDouble& operator=(const TagDouble& copy) noexcept = default;
		TagDouble& operator=(TagDouble&& move) noexcept = default;
		TagDouble(const TagDouble& copy) noexcept = default;
		TagDouble(TagDouble&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, double>>>
		TagDouble(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagDouble(int) noexcept {}
		~TagDouble() = default;
	};

	//This struct is used for arrays of `Object`s, `Array`s, `IVarInt`s and `UVarInt`s.
	//There is a plan to separate `IVarInt` and `UVarInt` as well.
	struct TagArray {
		//`count` is encoded into the vector.
		//`type` is also encoded into the entries of vector. In fact it's not possible to not store them into union structs because we need a proper destructor.
		vector<Tag> payload;

		TagArray& operator=(const TagArray& copy) noexcept = default;
		TagArray& operator=(TagArray&& move) noexcept = default;
		TagArray(const TagArray& copy) noexcept = default;
		TagArray(TagArray&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, vector<Tag>>>>
		TagArray(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagArray(int) noexcept {}
		~TagArray() = default;
	};

	struct TagUtf8 {
		array<c8, 4> payload;
		//To optimize things
		u8 length;

		TagUtf8& operator=(const TagUtf8& copy) noexcept = default;
		TagUtf8& operator=(TagUtf8&& move) noexcept = default;
		TagUtf8(const TagUtf8& copy) noexcept = default;
		TagUtf8(TagUtf8&& move) noexcept = default;
		template<typename T, typename U, typename = enable_if_t<equal<T, array<c8, 4>> && equal<U, u8>>>
		TagUtf8(T&& payload, U&& length) noexcept : payload(forward<T>(payload)), length(forward<T>(length)) {}
		#pragma warning(suppress: 26495)
		TagUtf8(int) noexcept {}
		~TagUtf8() = default;
	};

	struct TagRaw {
		u8 payload;

		TagRaw& operator=(const TagRaw& copy) noexcept = default;
		TagRaw& operator=(TagRaw&& move) noexcept = default;
		TagRaw(const TagRaw& copy) noexcept = default;
		TagRaw(TagRaw&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, u8>>>
		TagRaw(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagRaw(int) noexcept {}
		~TagRaw() = default;
	};

	//Important: `vector<bool>` uses 1-bit packed storage, which is not viable for bulk memory operations. 7 bit compensation for each entry is acceptable.
	//And, implicit conversion from any integer to `bool` is well-defined in the C++ standard aligning with CGNBT's specifications, so users can just use the value as they are `bool`s.
	struct TagArrayBool {
		//`count` is encoded into the vector.
		vector<u8> payload;

		TagArrayBool& operator=(const TagArrayBool& copy) noexcept = default;
		TagArrayBool& operator=(TagArrayBool&& move) noexcept = default;
		TagArrayBool(const TagArrayBool& copy) noexcept = default;
		TagArrayBool(TagArrayBool&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, vector<u8>>>>
		TagArrayBool(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagArrayBool(int) noexcept {}
		~TagArrayBool() = default;
	};

	struct TagArrayHex {
		//`count` is encoded into the vector.
		vector<u8> payload;

		TagArrayHex& operator=(const TagArrayHex& copy) noexcept = default;
		TagArrayHex& operator=(TagArrayHex&& move) noexcept = default;
		TagArrayHex(const TagArrayHex& copy) noexcept = default;
		TagArrayHex(TagArrayHex&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, vector<u8>>>>
		TagArrayHex(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagArrayHex(int) noexcept {}
		~TagArrayHex() = default;
	};

	struct TagArrayFloat {
		//`count` is encoded into the vector.
		vector<float> payload;

		TagArrayFloat& operator=(const TagArrayFloat& copy) noexcept = default;
		TagArrayFloat& operator=(TagArrayFloat&& move) noexcept = default;
		TagArrayFloat(const TagArrayFloat& copy) noexcept = default;
		TagArrayFloat(TagArrayFloat&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, vector<float>>>>
		TagArrayFloat(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagArrayFloat(int) noexcept {}
		~TagArrayFloat() = default;
	};

	struct TagArrayDouble {
		//`count` is encoded into the vector.
		vector<double> payload;

		TagArrayDouble& operator=(const TagArrayDouble& copy) noexcept = default;
		TagArrayDouble& operator=(TagArrayDouble&& move) noexcept = default;
		TagArrayDouble(const TagArrayDouble& copy) noexcept = default;
		TagArrayDouble(TagArrayDouble&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, vector<double>>>>
		TagArrayDouble(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagArrayDouble(int) noexcept {}
		~TagArrayDouble() = default;
	};

	struct TagArrayUtf8 {
		vector<c8> payload;
		//Codepoint Count
		u64 count;

		TagArrayUtf8& operator=(const TagArrayUtf8& copy) noexcept = default;
		TagArrayUtf8& operator=(TagArrayUtf8&& move) noexcept = default;
		TagArrayUtf8(const TagArrayUtf8& copy) noexcept = default;
		TagArrayUtf8(TagArrayUtf8&& move) noexcept = default;
		template<typename T, typename U, typename = enable_if_t<equal<T, vector<c8>> && equal<U, u64>>>
		TagArrayUtf8(T&& payload, U&& count) noexcept : payload(forward<T>(payload)), count(forward<U>(count)) {}
		#pragma warning(suppress: 26495)
		TagArrayUtf8(int) noexcept {}
		~TagArrayUtf8() = default;
	};

	struct TagArrayRaw {
		//`count` is encoded into the vector.
		vector<u8> payload;

		TagArrayRaw& operator=(const TagArrayRaw& copy) noexcept = default;
		TagArrayRaw& operator=(TagArrayRaw&& move) noexcept = default;
		TagArrayRaw(const TagArrayRaw& copy) noexcept = default;
		TagArrayRaw(TagArrayRaw&& move) noexcept = default;
		template<typename T, typename = enable_if_t<equal<T, vector<u8>>>>
		TagArrayRaw(T&& payload) noexcept : payload(forward<T>(payload)) {}
		#pragma warning(suppress: 26495)
		TagArrayRaw(int) noexcept {}
		~TagArrayRaw() = default;
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
			TagUtf8        tagUtf8;
			TagRaw         tagRaw;
			TagArrayBool   tagArrayBool;
			TagArrayHex    tagArrayHex;
			TagArrayFloat  tagArrayFloat;
			TagArrayDouble tagArrayDouble;
			TagArrayUtf8   tagArrayUtf8;
			TagArrayRaw    tagArrayRaw;
		};

		TypesN type;

		#pragma warning(suppress: 26495)
		Tag() noexcept : type(TypesN::Count) {}
		Tag(const TagObject& copy)      noexcept : type(TypesN::Object),      tagObject(copy)      {}
		Tag(const TagIVarInt& copy)     noexcept : type(TypesN::IVarInt),     tagIVarInt(copy)     {}
		Tag(const TagUVarInt& copy)     noexcept : type(TypesN::UVarInt),     tagUVarInt(copy)     {}
		Tag(const TagBool& copy)        noexcept : type(TypesN::Bool),        tagBool(copy)        {}
		Tag(const TagHex& copy)         noexcept : type(TypesN::Hex),         tagHex(copy)         {}
		Tag(const TagFloat& copy)       noexcept : type(TypesN::Float),       tagFloat(copy)       {}
		Tag(const TagDouble& copy)      noexcept : type(TypesN::Double),      tagDouble(copy)      {}
		Tag(const TagArray& copy)       noexcept : type(TypesN::Array),       tagArray(copy)       {}
		Tag(const TagUtf8& copy)        noexcept : type(TypesN::Utf8),        tagUtf8(copy)        {}
		Tag(const TagRaw& copy)         noexcept : type(TypesN::Raw),         tagRaw(copy)         {}
		Tag(const TagArrayBool& copy)   noexcept : type(TypesN::ArrayBool),   tagArrayBool(copy)   {}
		Tag(const TagArrayHex& copy)    noexcept : type(TypesN::ArrayHex),    tagArrayHex(copy)    {}
		Tag(const TagArrayFloat& copy)  noexcept : type(TypesN::ArrayFloat),  tagArrayFloat(copy)  {}
		Tag(const TagArrayDouble& copy) noexcept : type(TypesN::ArrayDouble), tagArrayDouble(copy) {}
		Tag(const TagArrayUtf8& copy)   noexcept : type(TypesN::ArrayUtf8),   tagArrayUtf8(copy)   {}
		Tag(const TagArrayRaw& copy)    noexcept : type(TypesN::ArrayRaw),    tagArrayRaw(copy)    {}
		Tag(TagObject&& _move)      noexcept : type(TypesN::Object),      tagObject(move(_move))      {}
		Tag(TagIVarInt&& _move)     noexcept : type(TypesN::IVarInt),     tagIVarInt(move(_move))     {}
		Tag(TagUVarInt&& _move)     noexcept : type(TypesN::UVarInt),     tagUVarInt(move(_move))     {}
		Tag(TagBool&& _move)        noexcept : type(TypesN::Bool),        tagBool(move(_move))        {}
		Tag(TagHex&& _move)         noexcept : type(TypesN::Hex),         tagHex(move(_move))         {}
		Tag(TagFloat&& _move)       noexcept : type(TypesN::Float),       tagFloat(move(_move))       {}
		Tag(TagDouble&& _move)      noexcept : type(TypesN::Double),      tagDouble(move(_move))      {}
		Tag(TagArray&& _move)       noexcept : type(TypesN::Array),       tagArray(move(_move))       {}
		Tag(TagUtf8&& _move)        noexcept : type(TypesN::Utf8),        tagUtf8(move(_move))        {}
		Tag(TagRaw&& _move)         noexcept : type(TypesN::Raw),         tagRaw(move(_move))         {}
		Tag(TagArrayBool&& _move)   noexcept : type(TypesN::ArrayBool),   tagArrayBool(move(_move))   {}
		Tag(TagArrayHex&& _move)    noexcept : type(TypesN::ArrayHex),    tagArrayHex(move(_move))    {}
		Tag(TagArrayFloat&& _move)  noexcept : type(TypesN::ArrayFloat),  tagArrayFloat(move(_move))  {}
		Tag(TagArrayDouble&& _move) noexcept : type(TypesN::ArrayDouble), tagArrayDouble(move(_move)) {}
		Tag(TagArrayUtf8&& _move)   noexcept : type(TypesN::ArrayUtf8),   tagArrayUtf8(move(_move))   {}
		Tag(TagArrayRaw&& _move)    noexcept : type(TypesN::ArrayRaw),    tagArrayRaw(move(_move))    {}

		Tag& operator=(const Tag& copy) noexcept {
			if (this == &copy) goto same;
			switch (type) {
				case TypesN::Object:      tagObject.~TagObject();           break;
				case TypesN::IVarInt:     tagIVarInt.~TagIVarInt();         break;
				case TypesN::UVarInt:     tagUVarInt.~TagUVarInt();         break;
				case TypesN::Bool:        tagBool.~TagBool();               break;
				case TypesN::Hex:         tagHex.~TagHex();                 break;
				case TypesN::Float:       tagFloat.~TagFloat();             break;
				case TypesN::Double:      tagDouble.~TagDouble();           break;
				case TypesN::Array:       tagArray.~TagArray();             break;
				case TypesN::Utf8:        tagUtf8.~TagUtf8();               break;
				case TypesN::Raw:         tagRaw.~TagRaw();                 break;
				case TypesN::ArrayBool:   tagArrayBool.~TagArrayBool();     break;
				case TypesN::ArrayHex: 	  tagArrayHex.~TagArrayHex();       break;
				case TypesN::ArrayFloat:  tagArrayFloat.~TagArrayFloat();   break;
				case TypesN::ArrayDouble: tagArrayDouble.~TagArrayDouble(); break;
				case TypesN::ArrayUtf8:   tagArrayUtf8.~TagArrayUtf8();     break;
				case TypesN::ArrayRaw: 	  tagArrayRaw.~TagArrayRaw();       break;
				default:                                                    break;
			}
			type = copy.type;
			switch (type) {
				case TypesN::Object:      new(&tagObject)      TagObject(copy.tagObject);           break;
				case TypesN::IVarInt:     new(&tagIVarInt)     TagIVarInt(copy.tagIVarInt);         break;
				case TypesN::UVarInt:     new(&tagUVarInt)     TagUVarInt(copy.tagUVarInt);         break;
				case TypesN::Bool:        new(&tagBool)        TagBool(copy.tagBool);               break;
				case TypesN::Hex:         new(&tagHex)         TagHex(copy.tagHex);                 break;
				case TypesN::Float:       new(&tagFloat)       TagFloat(copy.tagFloat);             break;
				case TypesN::Double:      new(&tagDouble)      TagDouble(copy.tagDouble);           break;
				case TypesN::Array:       new(&tagArray)       TagArray(copy.tagArray);             break;
				case TypesN::Utf8:        new(&tagUtf8)        TagUtf8(copy.tagUtf8);               break;
				case TypesN::Raw:         new(&tagRaw)         TagRaw(copy.tagRaw);                 break;
				case TypesN::ArrayBool:   new(&tagArrayBool)   TagArrayBool(copy.tagArrayBool);     break;
				case TypesN::ArrayHex: 	  new(&tagArrayHex)    TagArrayHex(copy.tagArrayHex);       break;
				case TypesN::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(copy.tagArrayFloat);   break;
				case TypesN::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(copy.tagArrayDouble); break;
				case TypesN::ArrayUtf8:   new(&tagArrayUtf8)   TagArrayUtf8(copy.tagArrayUtf8);     break;
				case TypesN::ArrayRaw: 	  new(&tagArrayRaw)    TagArrayRaw(copy.tagArrayRaw);       break;
				default:                                                                            break;
			}
			same: return *this;
		}

		Tag& operator=(Tag&& _move) noexcept {
			if (this == &_move) goto same;
			switch (type) {
				case TypesN::Object:      tagObject.~TagObject();           break;
				case TypesN::IVarInt:     tagIVarInt.~TagIVarInt();         break;
				case TypesN::UVarInt:     tagUVarInt.~TagUVarInt();         break;
				case TypesN::Bool:        tagBool.~TagBool();               break;
				case TypesN::Hex:         tagHex.~TagHex();                 break;
				case TypesN::Float:       tagFloat.~TagFloat();             break;
				case TypesN::Double:      tagDouble.~TagDouble();           break;
				case TypesN::Array:       tagArray.~TagArray();             break;
				case TypesN::Utf8:        tagUtf8.~TagUtf8();               break;
				case TypesN::Raw:         tagRaw.~TagRaw();                 break;
				case TypesN::ArrayBool:   tagArrayBool.~TagArrayBool();     break;
				case TypesN::ArrayHex: 	  tagArrayHex.~TagArrayHex();       break;
				case TypesN::ArrayFloat:  tagArrayFloat.~TagArrayFloat();   break;
				case TypesN::ArrayDouble: tagArrayDouble.~TagArrayDouble(); break;
				case TypesN::ArrayUtf8:   tagArrayUtf8.~TagArrayUtf8();     break;
				case TypesN::ArrayRaw: 	  tagArrayRaw.~TagArrayRaw();       break;
				default:                                                    break;
			}
			type = _move.type;
			switch (type) {
				case TypesN::Object:      new(&tagObject)      TagObject(move(_move.tagObject));           break;
				case TypesN::IVarInt:     new(&tagIVarInt)     TagIVarInt(move(_move.tagIVarInt));         break;
				case TypesN::UVarInt:     new(&tagUVarInt)     TagUVarInt(move(_move.tagUVarInt));         break;
				case TypesN::Bool:        new(&tagBool)        TagBool(move(_move.tagBool));               break;
				case TypesN::Hex:         new(&tagHex)         TagHex(move(_move.tagHex));                 break;
				case TypesN::Float:       new(&tagFloat)       TagFloat(move(_move.tagFloat));             break;
				case TypesN::Double:      new(&tagDouble)      TagDouble(move(_move.tagDouble));           break;
				case TypesN::Array:       new(&tagArray)       TagArray(move(_move.tagArray));             break;
				case TypesN::Utf8:        new(&tagUtf8)        TagUtf8(move(_move.tagUtf8));               break;
				case TypesN::Raw:         new(&tagRaw)         TagRaw(move(_move.tagRaw));                 break;
				case TypesN::ArrayBool:   new(&tagArrayBool)   TagArrayBool(move(_move).tagArrayBool);     break;
				case TypesN::ArrayHex: 	  new(&tagArrayHex)    TagArrayHex(move(_move).tagArrayHex);       break;
				case TypesN::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(move(_move).tagArrayFloat);   break;
				case TypesN::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(move(_move).tagArrayDouble); break;
				case TypesN::ArrayUtf8:   new(&tagArrayUtf8)   TagArrayUtf8(move(_move).tagArrayUtf8);     break;
				case TypesN::ArrayRaw: 	  new(&tagArrayRaw)    TagArrayRaw(move(_move).tagArrayRaw);       break;
				default:                                                                                   break;
			}
			same: return *this;
		}

		//We can't combine ctor/mtor with copy/move assignment operator when the struct has unions.
		//We can't determine whether the union is initialized if so, and we will be destructing garbage.
		Tag(const Tag& copy) noexcept {
			type = copy.type;
			switch (type) {
				case TypesN::Object:      new(&tagObject)      TagObject(copy.tagObject);           break;
				case TypesN::IVarInt:     new(&tagIVarInt)     TagIVarInt(copy.tagIVarInt);         break;
				case TypesN::UVarInt:     new(&tagUVarInt)     TagUVarInt(copy.tagUVarInt);         break;
				case TypesN::Bool:        new(&tagBool)        TagBool(copy.tagBool);               break;
				case TypesN::Hex:         new(&tagHex)         TagHex(copy.tagHex);                 break;
				case TypesN::Float:       new(&tagFloat)       TagFloat(copy.tagFloat);             break;
				case TypesN::Double:      new(&tagDouble)      TagDouble(copy.tagDouble);           break;
				case TypesN::Array:       new(&tagArray)       TagArray(copy.tagArray);             break;
				case TypesN::Utf8:        new(&tagUtf8)        TagUtf8(copy.tagUtf8);               break;
				case TypesN::Raw:         new(&tagRaw)         TagRaw(copy.tagRaw);                 break;
				case TypesN::ArrayBool:   new(&tagArrayBool)   TagArrayBool(copy.tagArrayBool);     break;
				case TypesN::ArrayHex: 	  new(&tagArrayHex)    TagArrayHex(copy.tagArrayHex);       break;
				case TypesN::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(copy.tagArrayFloat);   break;
				case TypesN::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(copy.tagArrayDouble); break;
				case TypesN::ArrayUtf8:   new(&tagArrayUtf8)   TagArrayUtf8(copy.tagArrayUtf8);     break;
				case TypesN::ArrayRaw: 	  new(&tagArrayRaw)    TagArrayRaw(copy.tagArrayRaw);       break;
				default:                                                                            break;
			}
		}

		Tag(Tag&& _move) noexcept {
			type = _move.type;
			switch (type) {
				case TypesN::Object:      new(&tagObject)      TagObject(move(_move.tagObject));           break;
				case TypesN::IVarInt:     new(&tagIVarInt)     TagIVarInt(move(_move.tagIVarInt));         break;
				case TypesN::UVarInt:     new(&tagUVarInt)     TagUVarInt(move(_move.tagUVarInt));         break;
				case TypesN::Bool:        new(&tagBool)        TagBool(move(_move.tagBool));               break;
				case TypesN::Hex:         new(&tagHex)         TagHex(move(_move.tagHex));                 break;
				case TypesN::Float:       new(&tagFloat)       TagFloat(move(_move.tagFloat));             break;
				case TypesN::Double:      new(&tagDouble)      TagDouble(move(_move.tagDouble));           break;
				case TypesN::Array:       new(&tagArray)       TagArray(move(_move.tagArray));             break;
				case TypesN::Utf8:        new(&tagUtf8)        TagUtf8(move(_move.tagUtf8));               break;
				case TypesN::Raw:         new(&tagRaw)         TagRaw(move(_move.tagRaw));                 break;
				case TypesN::ArrayBool:   new(&tagArrayBool)   TagArrayBool(move(_move).tagArrayBool);     break;
				case TypesN::ArrayHex: 	  new(&tagArrayHex)    TagArrayHex(move(_move).tagArrayHex);       break;
				case TypesN::ArrayFloat:  new(&tagArrayFloat)  TagArrayFloat(move(_move).tagArrayFloat);   break;
				case TypesN::ArrayDouble: new(&tagArrayDouble) TagArrayDouble(move(_move).tagArrayDouble); break;
				case TypesN::ArrayUtf8:   new(&tagArrayUtf8)   TagArrayUtf8(move(_move).tagArrayUtf8);     break;
				case TypesN::ArrayRaw: 	  new(&tagArrayRaw)    TagArrayRaw(move(_move).tagArrayRaw);       break;
				default:                                                                                   break;
			}
		}
		
		~Tag() {
			switch (type) {
				case TypesN::Object:      tagObject.~TagObject();           break;
				case TypesN::IVarInt:     tagIVarInt.~TagIVarInt();         break;
				case TypesN::UVarInt:     tagUVarInt.~TagUVarInt();         break;
				case TypesN::Bool:        tagBool.~TagBool();               break;
				case TypesN::Hex:         tagHex.~TagHex();                 break;
				case TypesN::Float:       tagFloat.~TagFloat();             break;
				case TypesN::Double:      tagDouble.~TagDouble();           break;
				case TypesN::Array:       tagArray.~TagArray();             break;
				case TypesN::Utf8:        tagUtf8.~TagUtf8();               break;
				case TypesN::Raw:         tagRaw.~TagRaw();                 break;
				case TypesN::ArrayBool:   tagArrayBool.~TagArrayBool();     break;
				case TypesN::ArrayHex: 	  tagArrayHex.~TagArrayHex();       break;
				case TypesN::ArrayFloat:  tagArrayFloat.~TagArrayFloat();   break;
				case TypesN::ArrayDouble: tagArrayDouble.~TagArrayDouble(); break;
				case TypesN::ArrayUtf8:   tagArrayUtf8.~TagArrayUtf8();     break;
				case TypesN::ArrayRaw: 	  tagArrayRaw.~TagArrayRaw();       break;
				default:                                                    break;
			}
		}
	};
}