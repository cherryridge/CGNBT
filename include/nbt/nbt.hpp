#pragma once 

#include "read.hpp"
#include "write.hpp"
#include "inspect.hpp"

namespace NBT {
    using NBT::IO::read, NBT::IO::write, NBT::Debug::inspect;
    using NBT::IO::NBT_WRITE_GZIP_DATA;
    using NBT::TypeNS::Tag, NBT::TypeNS::TagArray, NBT::TypeNS::TagArrayBool, NBT::TypeNS::TagArrayDouble, NBT::TypeNS::TagArrayFloat, NBT::TypeNS::TagArrayHex, NBT::TypeNS::TagArrayRaw, NBT::TypeNS::TagArrayUtf8, NBT::TypeNS::TagBool, NBT::TypeNS::TagDouble, NBT::TypeNS::TagFloat, NBT::TypeNS::TagHex, NBT::TypeNS::TagIVarInt, NBT::TypeNS::TagObject, NBT::TypeNS::TagRaw, NBT::TypeNS::TagUtf8, NBT::TypeNS::TagUVarInt;
}