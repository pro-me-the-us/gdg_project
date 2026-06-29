#!/bin/bash

set -e

echo "=== ArenaShooter Setup ==="

# --- Install dependencies ---
echo ""
echo "[1/2] Installing dependencies..."
sudo apt update -y
sudo apt install -y \
    build-essential \
    cmake \
    libglfw3-dev \
    libglm-dev \
    libenet-dev \
    libgl1-mesa-dev \
    pkg-config \
    libxinerama-dev \
    libxcursor-dev \
    libxrandr-dev \
    libxi-dev \
    libxkbcommon-dev

# --- Build ---
echo ""
echo "[2/2] Building..."
rm -rf build
cmake -B build
cmake --build build -j$(nproc)

echo ""
echo "=== Done! Run with: ./build/client ==="