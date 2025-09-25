# CherryGrove Named Binary Tag

The official implementation of CGNBT. Licensed under LGPL-2.1.

## Usage

This library is a header-only library.

In common use cases, only `nbt/nbt.hpp` is needed to be imported.

### Example

```cpp
#include <iostream>
#include <string>
#include <boost/unordered/unordered_flat_map.hpp>
#include <nbt/nbt.hpp>

int main() {
    using ufm = boost::unordered_flat_map;
    ufm<std::string, NBT::Tag> result;
    //Read from file. This function takes a reference, try to put result in that, and return success boolean.
    if(NBT::read("test.cgb", result)) {
        //Serialize to pseudo-json string.
        std::cout << NBT::serialize(result) << std::endl;
    }
    ufm<std::string, NBT::tag> data;
    data.emplace("test1", TagUVarInt(12914));
    data.emplace("test2", TagString("支持 UTF-8 编码！ཝ་ཡེ། འཛམ་གླིང་།！"));
    data.emplace("test3", TagArrayBool(vector<u8>({
        1,0,1,1,0,0,1,0,
        1,1,1,0,0,1,0,1,
        1,0,0,1,0,0,1,0,
        0,0,0,1,0,0,0,1,
    })));
    if(NBT::write("out.cgb", data, true, true)) {
        std::cout << "NBT data written successfully!" << std::endl;
    }
    return 0;
}
```

## Dependency

### `Boost`

This implementation uses `Boost.assert`, `Boost.config`, `Boost.container_hash`, `Boost.core`, `Boost.describe`, `Boost.mp11`, `Boost.predef`, `Boost.static_assert`, `Boost.throw_exception`, and `Boost.unordered`.

If the host is already using Boost, you may set `CGNBT_HAS_BOOST` to `TRUE` and set `CGNBT_BOOST_DIRECTORY` to the according path to stop this implementation from importing its own `Boost.*` dependency.

### `PhysFS`

This implementation uses `physfs` to process compressed CGNBT files.

If the host already has `physfs` as a dependency, set `CGNBT_HAS_PHYSFS` to `TRUE` and set `CGNBT_PHYSFS_DIRECTORY` to the according path to stop this implementation from importing its own `physfs` dependency.

#### Note: Currently we don't support using `std::filesystem::path` as the file path input. We will work on it soon.

### `zstd`

This implementation uses `zstd` to process compressed CGNBT files.

If the host already has `zstd` as a dependency, set `CGNBT_HAS_ZSTD` to `TRUE` and set `CGNBT_ZSTD_DIRECTORY` to the according path to stop this implementation from importing its own `zstd` dependency.

## Documentation

See CGNBT's specification: https://docs.cherrygrove.dev/cg/references/format/cgnbt

### Implementation-Specific

1. This implementation is NOT thread-safe intrinsically.
2. This implementation has internal Array types for fast read and write, namely `ArrayBool`, `ArrayHex`, `ArrayFloat`, `ArrayDouble`, `ArrayUtf8`, and `ArrayRaw`.
3. For `VarText`, this implementation converts `"\0"` to `""` implicitly.