#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#ifdef _WIN32
    #include <Windows.h>
#endif // _WIN32

#include <nbt/nbt.hpp>

typedef uint8_t u8;
using namespace NBT;
using std::cout, std::endl, std::string, std::ifstream, std::ofstream, std::ios, std::vector, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds, std::unordered_map;
CGNBT_USE_MAP_CONTAINER(unordered_map, Map, Policy)

int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

{
    cout << "========Reading NBT From File========" << endl;
    Map result;
    ifstream f("../../../tests/a.cgb", ios::binary);
    auto start = steady_clock::now();
    if (readStream<Policy>(f,result)) {
        auto end = steady_clock::now();
        cout << "Parse took " << duration_cast<microseconds>(end - start).count() << "us" << endl;
        auto str = serialize<Policy>(result);
        cout << str << endl;
        cout << "========Test Completed========" << endl;
    }
    else {
        cout << "Parse failed! Errors:" << endl;
        auto errors = getErrors();
        for (const auto& error : errors) cout << error << endl;
    }
}

{
    cout << "========Writing NBT to File========" << endl;
    Map writeTest;
    writeTest.emplace("uint1-1 :)", TagUVarInt(12914));
    writeTest.emplace("int-1~!@#$%^&*()`[];',./{}|:\"<>? ___super_____long__________________________________________________________________________________________________________________________________________________________________________________________________________________", TagIVarInt(180613137));
    writeTest.emplace("std::string", TagString("标准库：：字符串？"));
    writeTest.emplace("A boolean. Which state do you think it's currently in, `true` or `false`?", TagBool(false));
    writeTest.emplace("TagArrayBool+-*/=0123456789", TagArrayBool(vector<u8>({ 1,0,1,1,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1 })));
    writeTest.emplace("Some floats about the date.", TagFloat(8.252025));
    writeTest.emplace("\\\\dbl", TagDouble(12347.538297));
    writeTest.emplace("A hex value", TagHex(0x0C));
    writeTest.emplace("wHy WoUlD aNyOnE uSe ThIs!", TagRaw(0xC7));

    Map tempMap, tempMap2, tempMap3;
    tempMap.emplace("Array of ArrayHex, frightening, huh?", TagArray<Policy>(vector<Tag<Policy>>({
        TagArrayHex(vector<u8>({ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 })),
        TagArrayHex(vector<u8>({ 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x08 })),
        TagArrayHex(vector<u8>({ 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x08, 0x00 })),
        TagArrayHex(vector<u8>({ 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x08, 0x00, 0x06 })),
        TagArrayHex(vector<u8>({ 0x01, 0x02, 0x09, 0x01, 0x04, 0x08, 0x08, 0x00, 0x06, 0x01 })),
        TagArrayHex(vector<u8>({ 0x08, 0x00, 0x06, 0x01, 0x03, 0x07, 0x08, 0x00, 0x06, 0x01, 0x03, 0x07 })),
    })));
    tempMap2.emplace("An array of doubles", TagArrayDouble(vector<double>({ 3.141592653589793, 2.718281828459045, 1.618033988749895, 0.577215664901532, 1.4142135623730951 })));
    tempMap2.emplace("You know what's coming :D", TagArrayFloat(vector<float>({ 3.1415927f, 2.7182818f, 1.6180339f, 0.5772157f, 1.4142136f })));
    tempMap3.emplace("I give up on naming these things", tempMap2);
    tempMap3.emplace("coordinates", TagArrayDouble(vector<double>({ 321312.521, 5236.734, -539128124.46 })));
    tempMap3.emplace("raws", TagArray<Policy>(vector<Tag<Policy>>({
        TagArrayRaw(vector<u8>({ 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't', '.' })),
        TagArrayRaw(vector<u8>({ 'c', 'G', 'n', 'b', 'T', '!' }))
    })));
    tempMap3.emplace("descriptions", TagArray<Policy>(vector<Tag<Policy>>({
        TagString("This is a test."),
        TagString("这是一个测试。"),
        TagString("C'est un test."),
        TagString("Dies ist ein Test."),
        TagString("Este es una prueba."),
        TagString("Dit is een test."),
        TagString("Questo è un test."),
        TagString("Это тест."),
        TagString("هذا اختبار."),
        TagString("זהו מבחן."),
        TagString("यह एक परीक्षण है।"),
        TagString("これはテストです。"),
        TagString("이것은 테스트입니다."),
        TagString("Bu bir testtir.")
    })));
    tempMap.emplace("???", tempMap3);
    writeTest.emplace("the_most_thorough_embedding_test_ever", TagObject<Policy>(tempMap));

    {
        ofstream f("../../../tests/test.cgb", ios::binary | ios::trunc);
        cout << writeStream<Policy>(f,writeTest, true) << endl;
    }

    Map result2;
    ifstream f("../../../tests/test.cgb", ios::binary);
    auto start = steady_clock::now();
    if (readStream<Policy>(f,result2)) {
        auto end = steady_clock::now();
        cout << "Parse took " << duration_cast<microseconds>(end - start).count() << "us" << endl;
        auto str = serialize<Policy>(result2);
        cout << str << endl;
        cout << "========Test Completed========" << endl;
    }
    else {
        cout << "Parse failed! Errors:" << endl;
        auto errors = getErrors();
        for(const auto& error : errors) cout << error << endl;
    }
}

{
    cout << "========Empty Files========" << endl;
    Map emptyTest1, result1;
    {
        ofstream f("../../../tests/empty1.cgb", ios::binary | ios::trunc);
        cout << writeStream<Policy>(f,emptyTest1, true) << endl;
    }
    {
        ifstream f("../../../tests/empty1.cgb", ios::binary);
        if (readStream<Policy>(f,result1)) {
            auto str = serialize<Policy>(result1);
            cout << str << endl;
            cout << "========Test Completed========" << endl;
        }
        else {
            cout << "Parse failed! Errors:" << endl;
            auto errors = getErrors();
            for (const auto& error : errors) cout << error << endl;
        }
    }

    Map emptyTest2, result2;
    {
        ofstream f("../../../tests/empty2.cgb", ios::binary | ios::trunc);
        cout << writeStream<Policy>(f,emptyTest2) << endl;
    }
    {
        ifstream f("../../../tests/empty2.cgb", ios::binary);
        if (readStream<Policy>(f,result2)) {
            auto str = serialize<Policy>(result2);
            cout << str << endl;
        }
        else {
            cout << "Parse failed! Errors:" << endl;
            auto errors = getErrors();
            for (const auto& error : errors) cout << error << endl;
        }
    }
    cout << "========Test Completed========" << endl;
}

    return 0;
}