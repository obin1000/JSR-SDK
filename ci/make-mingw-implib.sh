#!/usr/bin/env bash
# Creates a MinGW import library (libJSR-SDK.dll.a) from JSR-SDK.dll, so
# MinGW-w64 GCC and MinGW clang can link the C API.
#
# Usage: ci/make-mingw-implib.sh <path/to/JSR-SDK.dll> <x64|Win32>
# Writes libJSR-SDK.dll.a next to the DLL. Needs gendef and dlltool.
set -euo pipefail

dll=$1
arch=$2
dir=$(dirname "$dll")
name=$(basename "$dll")
def="$dir/${name%.dll}.def"

case "$arch" in
  x64) flags=(-m i386:x86-64) ;;
  # The C API is __cdecl, so x86 names get the usual leading underscore.
  Win32) flags=(-m i386 --as-flags=--32) ;;
  *) echo "unknown architecture: $arch" >&2; exit 1 ;;
esac

gendef - "$dll" > "$def"
dlltool "${flags[@]}" -d "$def" -D "$name" -l "$dir/lib${name}.a"
rm -f "$def"
echo "Wrote $dir/lib${name}.a"
