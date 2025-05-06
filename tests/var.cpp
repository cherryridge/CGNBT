#include <iostream>

#include <nbt/read.hpp>

using std::cout, std::endl, std::string;

int main3r244() {
	cout << "========VarText========" << endl;

	//VarText test
	{
		string str("Cherr\0\0yGrove", 13);
		//Should have complete string
		NBT::VarTextNS::VarText t1(str);
		//Should only have `Cherr`
		NBT::VarTextNS::VarText t2(str.c_str());
		//Should have complete string
		NBT::VarTextNS::VarText t3(str.c_str(), str.size());

		auto a = "CherryGrove Named Binary Tag"_vartext;
		auto a1 = "CherryGrove\0Named\0Binary\0\0Tag"_vartext;
		auto b = ""_vartext;
		cout << a << endl;
		cout << b << endl;
		//NBT::VarTextNS::VarText c = NBT::VarTextNS::fromRaw(a.start);
		//string d = NBT::VarTextNS::fromRaw(a.start);
		//string e = string(NBT::VarTextNS::fromRaw(b.start));
		string f = a;
		//auto g = c;

		string nullstr1("\0", 1);
		NBT::VarTextNS::VarText nullvartext1(nullstr1);
		auto nullvartext2 = "\0"_vartext;
		//Should be ""
		//string nullstr2 = NBT::VarTextNS::fromRaw(nullvartext1.start);

		auto add1 = "Cherry"_vartext, add2 = "Grove1234567"_vartext;
		cout << (add1 + add2) << endl;
	}

	cout << "\n========VarInt========" << endl;

	//VarInt test
	{
		//20250421 = 1001 1010011 1111110 0110101
		//Should be 00110101 01111110 01010011 10001001
		//35 7E 53 89
		//53 126 83 9(137)
		NBT::VarIntNS::UVarInt a(20250421u);
		NBT::VarIntNS::UVarInt a2 = 7u;
		NBT::VarIntNS::UVarInt b(0u);
		NBT::VarIntNS::UVarInt c(1u);
		NBT::VarIntNS::IVarInt sfa = 2313124124;
		auto d = 0xFFF48F4564F564F5_uvarint; //18443523902198342901
		auto e = a.get();
		uint64_t f = ((NBT::VarIntNS::UVarInt)((NBT::VarIntNS::UVarInt)((NBT::VarIntNS::UVarInt)((NBT::VarIntNS::UVarInt)(4 + c) - 3)) * 6) / 4) % 2; //1
		cout << b << " " << f << " " << d << endl;
		NBT::VarIntNS::UVarInt gew = 312u;
		gew += 12;
		gew = a * gew;
		NBT::VarIntNS::IVarInt asd = 132;
		asd = -asd;
		asd *= 4;
		asd--;
		cout << asd << " " << asd.isNegative() << " " << (-asd).isNegative() << endl;
		cout << "666";
	}
	return 0;
}