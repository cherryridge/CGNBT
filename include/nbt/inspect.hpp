#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <limits>
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include "type/type.hpp"

namespace NBT {
	typedef uint8_t u8;
	typedef int32_t i32;
	typedef uint64_t u64;
	using std::unordered_map, std::string, std::cout, std::endl, std::setprecision, std::numeric_limits;

	inline void inspectObject(const TagObject& data) noexcept;
	inline void inspectIVarInt(const TagIVarInt& data) noexcept;
	inline void inspectUVarInt(const TagUVarInt& data) noexcept;
	inline void inspectBool(const TagBool& data) noexcept;
	inline void inspectHex(const TagHex& data) noexcept;
	inline void inspectFloat(const TagFloat& data) noexcept;
	inline void inspectDouble(const TagDouble& data) noexcept;
	inline void inspectArray(const TagArray& data) noexcept;
	inline void inspectUtf8(const TagUtf8& data) noexcept;
	inline void inspectRaw(const TagRaw& data) noexcept;
	inline void inspectArrayBool(const TagArrayBool& data) noexcept;
	inline void inspectArrayHex(const TagArrayHex& data) noexcept;
	inline void inspectArrayFloat(const TagArrayFloat& data) noexcept;
	inline void inspectArrayDouble(const TagArrayDouble& data) noexcept;
	inline void inspectArrayUtf8(const TagArrayUtf8& data) noexcept;
	inline void inspectArrayRaw(const TagArrayRaw& data) noexcept;

	inline constexpr u64 depth = 0;

	inline void inspect(const unordered_map<string, Tag>& data) noexcept {
		#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
		#endif // _WIN32
		inspectObject(TagObject(data));
	}

	inline static void coutHex(const u8& data) noexcept {
		switch (data) {
			case 0:  cout << "0"; break;
			case 1:  cout << "1"; break;
			case 2:  cout << "2"; break;
			case 3:  cout << "3"; break;
			case 4:  cout << "4"; break;
			case 5:  cout << "5"; break;
			case 6:  cout << "6"; break;
			case 7:  cout << "7"; break;
			case 8:  cout << "8"; break;
			case 9:  cout << "9"; break;
			case 10: cout << "A"; break;
			case 11: cout << "B"; break;
			case 12: cout << "C"; break;
			case 13: cout << "D"; break;
			case 14: cout << "E"; break;
			case 15: cout << "F"; break;
			default: cout << "(" << data << "?)"; break;
		}
	}

	inline void inspectObject(const TagObject& data) noexcept {
		for (const auto& [name, member] : data.payload) {
			cout << getTypeStr(toTypes(member.type));
			switch (member.type) {
				case TypesN::Object:
					cout << "(" << name << "):" << endl;
					inspectObject(member.tagObject);
					cout << "EndObject(" << name << ")." << endl;
					break;
				case TypesN::IVarInt:
					cout << "(" << name << ") = ";
					inspectIVarInt(member.tagIVarInt);
					cout << endl;
					break;
				case TypesN::UVarInt:
					cout << "(" << name << ") = ";
					inspectUVarInt(member.tagUVarInt);
					cout << endl;
					break;
				case TypesN::Bool:
					cout << "(" << name << ") = ";
					inspectBool(member.tagBool);
					cout << endl;
					break;
				case TypesN::Hex:
					cout << "(" << name << ") = ";
					inspectHex(member.tagHex);
					cout << endl;
					break;
				case TypesN::Float:
					cout << "(" << name << ") = ";
					inspectFloat(member.tagFloat);
					cout << endl;
					break;
				case TypesN::Double:
					cout << "(" << name << ") = ";
					inspectDouble(member.tagDouble);
					cout << endl;
					break;
				case TypesN::Array:
					cout << "(" << getTypeStr(toTypes(member.tagArray.payload[0].type)) << "[" << member.tagArray.payload.size() << "])(" << name << ")";
					inspectArray(member.tagArray);
					cout << endl;
					break;
				case TypesN::Utf8:
					cout << "(" << name << ") = ";
					inspectUtf8(member.tagUtf8);
					cout << endl;
					break;
				case TypesN::Raw:
					cout << "(" << name << ") = ";
					inspectRaw(member.tagRaw);
					cout << endl;
					break;
				case TypesN::ArrayBool:
					cout << "[" << member.tagArrayBool.payload.size() << "])(" << name << ") = ";
					inspectArrayBool(member.tagArrayBool);
					cout << endl;
					break;
				case TypesN::ArrayHex:
					cout << "[" << member.tagArrayHex.payload.size() << "])(" << name << ") = ";
					inspectArrayHex(member.tagArrayHex);
					cout << endl;
					break;
				case TypesN::ArrayFloat:
					cout << "[" << member.tagArrayFloat.payload.size() << "])(" << name << ") = ";
					inspectArrayFloat(member.tagArrayFloat);
					cout << endl;
					break;
				case TypesN::ArrayDouble:
					cout << "[" << member.tagArrayDouble.payload.size() << "])(" << name << ") = ";
					inspectArrayDouble(member.tagArrayDouble);
					cout << endl;
					break;
				case TypesN::ArrayUtf8:
					cout << "[" << member.tagArrayUtf8.count << "])(" << name << ") = ";
					inspectArrayUtf8(member.tagArrayUtf8);
					cout << endl;
					break;
				case TypesN::ArrayRaw:
					cout << "[" << member.tagArrayRaw.payload.size() << "])(" << name << ") = ";
					inspectArrayRaw(member.tagArrayRaw);
					cout << endl;
					break;
				default:
					cout << " (invalid): " << static_cast<i32>(member.type) << endl;
					break;
			}
		}
	}

	inline void inspectIVarInt(const TagIVarInt& data) noexcept { cout << data.payload; }

	inline void inspectUVarInt(const TagUVarInt& data) noexcept { cout << data.payload; }

	inline void inspectBool(const TagBool& data) noexcept { cout << (data.payload ? "true" : "false"); }

	inline void inspectHex(const TagHex& data) noexcept { coutHex(data.payload); }

	inline void inspectFloat(const TagFloat& data) noexcept { cout << setprecision(numeric_limits<float>::digits10) << data.payload; }

	inline void inspectDouble(const TagDouble& data) noexcept { cout << setprecision(numeric_limits<double>::digits10) << data.payload; }

	inline void inspectArray(const TagArray& data) noexcept {
		auto type = data.payload[0].type;
		cout << endl;
		for (u64 i = 0; i < data.payload.size(); i++) {
			cout << "[" << i << "] ";
			switch (type) {
				case NBT::TypesN::Object:
					cout << "Object:" << endl;
					inspectObject(data.payload[i].tagObject);
					cout << "EndObject.";
					break;
				case TypesN::IVarInt:
					inspectIVarInt(data.payload[i].tagIVarInt);
					break;
				case TypesN::UVarInt:
					inspectUVarInt(data.payload[i].tagUVarInt);
					break;
				case TypesN::Bool:
					inspectBool(data.payload[i].tagBool);
					break;
				case TypesN::Hex:
					inspectHex(data.payload[i].tagHex);
					break;
				case TypesN::Float:
					inspectFloat(data.payload[i].tagFloat);
					break;
				case TypesN::Double:
					inspectDouble(data.payload[i].tagDouble);
					break;
				case TypesN::Array:
					cout << "Array[" <<  data.payload[i].tagArray.payload.size() << "]" << endl;
					inspectArray(data.payload[i].tagArray);
					break;
				case TypesN::Utf8:
					inspectUtf8(data.payload[i].tagUtf8);
					break;
				case TypesN::Raw:
					inspectRaw(data.payload[i].tagRaw);
					break;
				case TypesN::ArrayBool:
					cout << "Array[" << data.payload[i].tagArrayBool.payload.size() << "]" << endl;
					inspectArrayBool(data.payload[i].tagArrayBool);
					break;
				case TypesN::ArrayHex:
					cout << "Array[" << data.payload[i].tagArrayHex.payload.size() << "]" << endl;
					inspectArrayHex(data.payload[i].tagArrayHex);
					break;
				case TypesN::ArrayFloat:
					cout << "Array[" << data.payload[i].tagArrayFloat.payload.size() << "]" << endl;
					inspectArrayFloat(data.payload[i].tagArrayFloat);
					break;
				case TypesN::ArrayDouble:
					cout << "Array[" << data.payload[i].tagArrayDouble.payload.size() << "]" << endl;
					inspectArrayDouble(data.payload[i].tagArrayDouble);
					break;
				case TypesN::ArrayUtf8:
					cout << "Array[" << data.payload[i].tagArrayUtf8.payload.size() << "]" << endl;
					inspectArrayUtf8(data.payload[i].tagArrayUtf8);
					break;
				case TypesN::ArrayRaw:
					cout << "Array[" << data.payload[i].tagArrayRaw.payload.size() << "]" << endl;
					inspectArrayRaw(data.payload[i].tagArrayRaw);
					break;
				default:
					cout << "Invalid Type: " << static_cast<i32>(type);
					break;
			}
			if(i < data.payload.size() - 1) cout << endl;
		}
	}

	inline void inspectUtf8(const TagUtf8& data) noexcept {
		u8string temp;
		temp.append(data.payload.data(), data.length);
		cout << reinterpret_cast<const char*>(temp.c_str());
	}

	inline void inspectRaw(const TagRaw& data) noexcept {
		coutHex((data.payload & 0xF0) >> 4);
		coutHex(data.payload & 0x0F);
	}

	inline void inspectArrayBool(const TagArrayBool& data) noexcept {
		for (u64 i = 0; i < data.payload.size(); i++) {
			cout << "[" << i << "] " << (data.payload[i] ? "true" : "false");
			if (i < data.payload.size() - 1) cout << endl;
		}
	}

	inline void inspectArrayHex(const TagArrayHex& data) noexcept {
		for (u64 i = 0; i < data.payload.size(); i++) {
			coutHex(data.payload[i]);
			if (i % 8 != 7) cout << " ";
			else if (i < data.payload.size() - 1) cout << endl;
		}
	}

	inline void inspectArrayFloat(const TagArrayFloat& data) noexcept {
		for (u64 i = 0; i < data.payload.size(); i++) {
			cout << "[" << i << "] " << setprecision(numeric_limits<float>::digits10) << data.payload[i];
			if (i < data.payload.size() - 1) cout << endl;
		}
	}

	inline void inspectArrayDouble(const TagArrayDouble& data) noexcept {
		for (u64 i = 0; i < data.payload.size(); i++) {
			cout << "[" << i << "] " << setprecision(numeric_limits<double>::digits10) << data.payload[i];
			if (i < data.payload.size() - 1) cout << endl;
		}
	}

	inline void inspectArrayUtf8(const TagArrayUtf8& data) noexcept {
		u8string temp;
		temp.append(data.payload.data(), data.payload.size());
		cout << reinterpret_cast<const char*>(temp.c_str());
	}

	inline void inspectArrayRaw(const TagArrayRaw& data) noexcept {
		for (u64 i = 0; i < data.payload.size(); i++) {
			coutHex((data.payload[i] & 0xF0) >> 4);
			coutHex(data.payload[i] & 0x0F);
			if (i % 8 != 7) cout << " ";
			else if (i < data.payload.size() - 1) cout << "\n";
		}
	}
}