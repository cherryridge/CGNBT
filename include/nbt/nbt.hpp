#pragma once 

#include "read.hpp" // IWYU pragma: export
#include "readRaw.hpp" //IWYU pragma: export
#include "write.hpp"  // IWYU pragma: export
#include "serialize.hpp"  // IWYU pragma: export
#include "error.hpp" // IWYU pragma: export

namespace NBT {
    //IO APIs
    using NBT::IO::read, NBT::IO::readRawData, NBT::IO::write, NBT::IO::writeRawData, NBT::IO::serialize, NBT::IO::getFileInfo, NBT::Error::getErrors;

    using NBT::IO::NBTFileInfo, NBT::IO::RawDataStatus;

    //Tags
    using NBT::Type::Tag;
    using NBT::Type::TagObject, NBT::Type::TagIVarInt, NBT::Type::TagUVarInt, NBT::Type::TagBool, NBT::Type::TagHex, NBT::Type::TagFloat, NBT::Type::TagDouble, NBT::Type::TagArray, NBT::Type::TagString, NBT::Type::TagRaw;
    using NBT::Type::TagArrayBool, NBT::Type::TagArrayHex, NBT::Type::TagArrayFloat, NBT::Type::TagArrayDouble, NBT::Type::TagArrayRaw;
}