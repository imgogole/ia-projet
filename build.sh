#!/usr/bin/env bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$SCRIPT_DIR"
BUILD_DIR="$SCRIPT_DIR/build"

if [ -d "$BUILD_DIR" ]; then
  echo "→ Nettoyage de $BUILD_DIR"
  rm -rf "$BUILD_DIR"
fi
echo "→ Création de $BUILD_DIR"
mkdir -p "$BUILD_DIR"
echo "→ Configuration CMake"
cmake -S "$SRC_DIR" -B "$BUILD_DIR"
echo "→ Compilation"
cmake --build "$BUILD_DIR" -- -j"$(nproc)"
echo "→ Lancement du jeu"
"$BUILD_DIR/game"