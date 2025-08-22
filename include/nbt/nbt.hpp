#pragma once 

#include "read.hpp" // IWYU pragma: export
#include "write.hpp"  // IWYU pragma: export
#include "serialize.hpp"  // IWYU pragma: export

namespace NBT {
    //IO APIs
    using NBT::IO::read, NBT::IO::write, NBT::IO::serialize, NBT::IO::getFileInfo, NBT::IO::getErrors;

    //Data Structures
    using NBT::IO::NBTFileInfo;

    //Tags
    using NBT::TypeNS::Tag;
    using NBT::TypeNS::TagObject, NBT::TypeNS::TagIVarInt, NBT::TypeNS::TagUVarInt, NBT::TypeNS::TagBool, NBT::TypeNS::TagHex, NBT::TypeNS::TagFloat, NBT::TypeNS::TagDouble, NBT::TypeNS::TagArray, NBT::TypeNS::TagString, NBT::TypeNS::TagRaw;
    using NBT::TypeNS::TagArrayBool, NBT::TypeNS::TagArrayHex, NBT::TypeNS::TagArrayFloat, NBT::TypeNS::TagArrayDouble, NBT::TypeNS::TagArrayRaw;
}