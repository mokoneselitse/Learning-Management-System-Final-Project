#!/usr/bin/env bash
# package-windows.sh - builds the release zip that the school technician installs.
# Run it in the MSYS2 UCRT64 terminal, from the project folder:
#     bash tools/package-windows.sh
# Result: dist/LMS-<version>-windows.zip containing lms.exe, every DLL it needs,
# the installation guide, the user manuals and the release notes.
set -euo pipefail

EXE="${EXE:-lms.exe}"                       # program file name
DLL_PATTERN="${DLL_PATTERN:-/ucrt64/}"      # DLLs that come from MSYS2 (not from Windows itself)
VERSION=$(sed -n 's/^project(ModeratedLMS VERSION \([0-9.]*\).*/\1/p' CMakeLists.txt)
NAME="LMS-${VERSION}-windows"
OUT="dist/${NAME}"

echo "== Building release ${VERSION}"
rm -rf dist build-release
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DLMS_BUILD_TESTS=OFF > /dev/null
cmake --build build-release

echo "== Collecting files"
mkdir -p "${OUT}"
cp "build-release/${EXE}" "${OUT}/"
# Every DLL lms.exe loads that is not part of Windows must travel with it.
ldd "build-release/${EXE}" | awk -v pat="${DLL_PATTERN}" '$3 ~ pat {print $3}' | sort -u |
    while read -r dll; do
        cp "${dll}" "${OUT}/"
        echo "   ${dll##*/}"
    done
cp docs/INSTALLATION_GUIDE.md docs/RELEASE_NOTES.md "${OUT}/"
cp -r docs/user-manuals "${OUT}/user-manuals"

echo "== Checking the program starts with only the packaged files"
# PATH is cut down to Windows' own folders, so a DLL that was not copied makes this fail,
# exactly as it would on the school computer.
env PATH="/c/Windows/System32:/c/Windows" "${OUT}/${EXE}" --version

echo "== Creating zip"
(cd dist && cmake -E tar cf "${NAME}.zip" --format=zip "${NAME}")
echo "Done: dist/${NAME}.zip"
