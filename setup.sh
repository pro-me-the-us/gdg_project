#!/bin/bash

set -e

echo "=== ArenaShooter Setup ==="

# --- Install dependencies ---
echo ""
echo "[1/3] Installing dependencies..."
sudo apt update -y
sudo apt install -y \
    build-essential \
    cmake \
    git \
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

# --- Setup ImGui ---

IMGUI_DIR="third_party/imgui"

if [ ! -f "$IMGUI_DIR/imgui.cpp" ]; then
    mkdir -p "$IMGUI_DIR"

    git clone --depth 1 https://github.com/ocornut/imgui.git imgui_temp

    cp imgui_temp/imgui*.cpp "$IMGUI_DIR/"
    cp imgui_temp/imgui*.h "$IMGUI_DIR/"
    cp imgui_temp/imconfig.h "$IMGUI_DIR/"
    cp imgui_temp/imstb_*.h "$IMGUI_DIR/"

    cp imgui_temp/backends/imgui_impl_glfw.cpp "$IMGUI_DIR/"
    cp imgui_temp/backends/imgui_impl_glfw.h "$IMGUI_DIR/"
    cp imgui_temp/backends/imgui_impl_opengl3.cpp "$IMGUI_DIR/"
    cp imgui_temp/backends/imgui_impl_opengl3.h "$IMGUI_DIR/"
    cp imgui_temp/backends/imgui_impl_opengl3_loader.h "$IMGUI_DIR/"
    
    # Clean up the temporary clone
    rm -rf imgui_temp

# --- Build ---
echo ""
echo "[3/3] Building..."
rm -rf build
cmake -B build
cmake --build build -j$(nproc)

echo ""
echo "=== Done! Run with: ./build/client ==="