# CherryGrove Named Binary Tag

The official implementation of CGNBT. Licensed under LGPL-2.1.

## Usage

### Import

This library is a header-only library.

In common use cases, only `nbt/nbt.hpp` is needed to be imported.

### Data Source

The library support increment-only byte stream and whole data chunks. You can use the library's built-in file reader and writer (including Zstandard compressed file reader and writer), or implement your own data source by the `NBT::IO::Readable` and the `NBT::IO::Writable` concepts.

The library doesn't enforce a map container for object tags or top-level result, and you need to specify the map container type by using the `CGNBT_USE_MAP_CONTAINER` macro before you can use the library.

The syntax is `CGNBT_USE_MAP_CONTAINER(mapTemplate, mapTypeOutput, policyOutput)`, where `mapTemplate` is the template of the map container you want to use, `mapTypeOutput` is the type name of the map container type that will be used for object tags and you receiving the parse result, and `policyOutput` is the name of the policy struct that will be used in explicit construction of tags.

### Example

```cpp
#include <iostream>
#include <string>
#include <boost/unordered/unordered_flat_map.hpp>
#include <nbt/nbt.hpp>

CGNBT_USE_MAP_CONTAINER(boost::unordered_flat_map, ufm, ufmPolicy)

int main() {
    //1. Use `mapTypeOutput` directly as the type of the parse result.
    ufm result;
    //2. Use `std::ifstream` as the data source, and use `NBT::readStream` to parse the data. `NBT::readStream` returns `true` if the parsing is successful, and the parse result will be stored in the second parameter.
    std::ifstream inStream("a.cgb", std::ios::binary);
    if(NBT::readStream<ufmPolicy>(inStream, result)) {
        //Serialize to pseudo-json string.
        std::cout << NBT::serialize<ufmPolicy>(result) << std::endl;
    }

    //3. Construct a NBT data and write it to a file.
    ufm data;
    data.emplace("test1", TagUVarInt(12914));
    data.emplace("test2", TagString("支持 UTF-8 编码！ཝ་ཡེ། འཛམ་གླིང་།！"));
    data.emplace("test3", TagArrayBool(vector<u8>({
        1,0,1,1,0,0,1,0,
        1,1,1,0,0,1,0,1,
        1,0,0,1,0,0,1,0,
        0,0,0,1,0,0,0,1,
    })));
    //4. Use `std::ofstream` as the data sink, and use `NBT::writeStream` to write the data.
    std::ofstream outStream("out.cgb", std::ios::binary | std::ios::trunc);
    //5. The third parameter is whether to use Zstandard compression, and the fourth parameter is the zstd compression level. `NBT::writeStream` returns `true` if the writing is successful and vice versa.
    if(NBT::writeStream<ufmPolicy>(outStream, data, true, 10)) {
        std::cout << "NBT data written successfully!" << std::endl;
    }
    return 0;
}
```

## Dependency

### `zstd`

This implementation uses `zstd` to process compressed CGNBT files.

If the host already has `zstd` as a dependency, set `CGNBT_HAS_ZSTD` to `TRUE` and set `CGNBT_ZSTD_DIRECTORY` to the according path to stop this implementation from importing its own `zstd` dependency.

## Documentation

See CGNBT's specification: https://docs.cherrygrove.dev/cg/references/format/cgnbt

### Implementation-Specific

1. This implementation is NOT thread-safe intrinsically.
2. This implementation has internal Array types for fast read and write, namely `ArrayBool`, `ArrayHex`, `ArrayFloat`, `ArrayDouble`, `ArrayUtf8`, and `ArrayRaw`.
3. For `VarText`, this implementation converts `"\0"` to `""` implicitly.