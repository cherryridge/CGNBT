#pragma once 

#include "error.hpp"     // IWYU pragma: export
#include "helpers.hpp"   // IWYU pragma: export
#include "read.hpp"      // IWYU pragma: export
#include "serialize.hpp" // IWYU pragma: export
#include "types.hpp"     // IWYU pragma: export
#include "write.hpp"     // IWYU pragma: export

namespace NBT {
    //IO APIs
    using NBT::IO::readStream, NBT::IO::readData, NBT::IO::writeStream, NBT::IO::writeData, NBT::IO::serialize, NBT::IO::getFileInfo, NBT::IO::NBTFileInfo;
    
    //Errors
    using NBT::Error::getLastError, NBT::Error::getErrors;

    //Tags
    using NBT::Type::Tag;
    using NBT::Type::TagObject, NBT::Type::TagIVarInt, NBT::Type::TagUVarInt, NBT::Type::TagBool, NBT::Type::TagHex, NBT::Type::TagFloat, NBT::Type::TagDouble, NBT::Type::TagArray, NBT::Type::TagString, NBT::Type::TagRaw;
    using NBT::Type::TagArrayBool, NBT::Type::TagArrayHex, NBT::Type::TagArrayFloat, NBT::Type::TagArrayDouble, NBT::Type::TagArrayRaw;
    using NBT::Type::Types;

    //Helpers
    using NBT::Helpers::TagOf, NBT::Helpers::valueOr, NBT::Helpers::memberOr;
    #define CGNBT_USE_MAP_CONTAINER(mapTemplate, mapTypeOutput, policyOutput) \
    struct policyOutput { \
        template <typename K, typename V> \
        using map = mapTemplate<K, V>; \
    }; \
    using mapTypeOutput = policyOutput::map<std::string, NBT::Tag<policyOutput>>;
}