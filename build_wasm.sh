#!/usr/bin/env bash

# Change directory to the location of the script
cd "$(dirname "$0")"

DIRECTORY="build_wasm"
if [ -d "$DIRECTORY" ]; then
  rm -rf "$DIRECTORY"
fi
mkdir "$DIRECTORY"
cd "$DIRECTORY"
emcc -o index.html -s USE_SDL=2 ../source/*.cpp