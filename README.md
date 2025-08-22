# CherryGrove Named Binary Tag

The official implementation of CGNBT. Licensed under LGPL-2.1.

## Dependency

This library is a header-only library, but it requires `zlib-ng` to decompress/compress data.

If the host software already has `zlib-ng` as dependency, set `ZLIBNG_ROOT` option to the according path in the CMakeLists file.

## Documentation

See CGNBT's specification: https://docs.cherrygrove.dev/cg/references/format/cgnbt

### Implementation Specific

1. This implementation is NOT thread-safe intrinsically.
2. This implementation has internal Array types for fast read and write, namely `ArrayBool`, `ArrayHex`, `ArrayFloat`, `ArrayDouble`, `ArrayUtf8`, and `ArrayRaw`.
3. For `VarText`, this implementation converts `"\0"` to `""` implicitly.