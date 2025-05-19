#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$SCRIPT_DIR"
BUILD_DIR="$SCRIPT_DIR/build"

# Nettoyage
if [ -d "$BUILD_DIR" ]; then
  echo "→ Nettoyage de $BUILD_DIR"
  rm -rf "$BUILD_DIR"
fi

# Création du dossier de build
echo "→ Création de $BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Configuration CMake (Debug + AddressSanitizer)
echo "→ Configuration CMake (Debug + ASAN)"
cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_FLAGS="-g -O1 -fsanitize=address -fno-omit-frame-pointer" \
      -DCMAKE_CXX_FLAGS="-g -O1 -fsanitize=address -fno-omit-frame-pointer"

# Compilation
echo "→ Compilation"
cmake --build "$BUILD_DIR" -- -j"$(nproc)"

# Lancement avec AddressSanitizer
echo "→ Lancement du jeu (avec ASan)"
"$BUILD_DIR/game"
