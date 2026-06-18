# 🎯 Shooter's Arena

A fast-paced, 2D top-down multiplayer arena shooter built from scratch in C++. 

**Shooter's Arena** is an educational project designed to explore low-level multiplayer game development, client-server architecture, real-time game state synchronization, and manual memory management.

---

## ✨ Features

*   **Fast-paced Multiplayer:** 4 to 6 player free-for-all deathmatches.
*   **Procedural Arenas:** A uniquely generated 200x200 tile battleground for every match.
*   **Authoritative Server:** A cheat-resistant architecture where the server strictly governs all game logic, health, scoring, and physics.
*   **Real-time Networking:** Custom UDP networking layer utilizing ENet for low-latency state synchronization.
*   **Powerup System:** Dynamic drops including Speed Boosts, Rapid Fire, and Health Packs.
*   **Custom Graphics Pipeline:** Built directly on top of OpenGL.

---

## 🛠️ Technical Stack

| Component | Technology | Purpose |
| :--- | :--- | :--- |
| **Language** | Modern C++ | Core engine and game logic |
| **Rendering** | OpenGL, GLAD, GLFW | Window management and hardware-accelerated 2D graphics |
| **Mathematics** | GLM | 2D vector math, velocities, and collision geometry |
| **Networking** | ENet | Reliable and unreliable UDP packet sequencing |
| **Build System** | CMake | Cross-platform compilation |

---

## 🏗️ Architecture Overview

The game utilizes a strict **Client-Server Architecture**. 

### The Server (Authoritative)
Runs at a fixed **45 Ticks Per Second (TPS)**. The server is the absolute source of truth and manages:
*   Procedural map generation
*   Player movement validation and bullet simulation
*   AABB Collision detection
*   Health calculations and respawns
*   Scoring and match timers

### The Client (Dumb Client)
Clients do not make any authoritative game decisions. They are strictly responsible for:
*   Collecting keyboard/mouse input and sending it to the server
*   Receiving the serialized game state
*   Rendering the players, bullets, and map
*   Displaying the UI, Leaderboard, and Ping

---

## 🎮 Controls

*   **W, A, S, D** - Move Player
*   **SPACEBAR** - Shoot Weapon
*   **ESC** - Pause / Exit Game

---

## 🚀 Getting Started

### Prerequisites
To build this project, you will need the following installed on your machine:
*   A modern C++ Compiler (MSVC, GCC, or Clang)
*   [CMake](https://cmake.org/) (Version 3.10+)
*   Git

### Building the Project

1. **Clone the repository**

        git clone https://github.com/pro-me-the-us/gdg_project.git
        cd shooters-arena

2. **Generate build files using CMake**
   
         mkdir build
         cd build
         cmake ..

3. **Compile the executables**

         cmake --build .



📂 Project Structure

      ├── client/          # Client executable source code (Rendering, Input, UI)
      ├── server/          # Server executable source code (Game Loop, State, Physics)
      ├── shared/          # Shared logic (Math, Packets, Constants, Entity Structs)
      ├── assets/          # Textures, Fonts, and Shaders
      ├── third_party/     # Submodules for GLFW, GLAD, GLM, and ENet
      └── CMakeLists.txt   # Root build configuration


 🗺️ Development Roadmap

[ ] Milestone 1: Create OpenGL window and basic rendering pipeline.

[ ] Milestone 2: Implement local player movement and delta time.

[ ] Milestone 3: Establish ENet Server and UDP connection handling.

[ ] Milestone 4: Connect the first client and transmit packet payloads.

[ ] Milestone 5: Synchronize player positions across multiple clients.

[ ] Milestone 6: Implement shooting mechanics and bullet pooling.

[ ] Milestone 7: Synchronize bullet state and lifetimes.

[ ] Milestone 8: Implement AABB collision detection.

[ ] Milestone 9: Add health, damage logic, and respawning.

[ ] Milestone 10: Build procedural map generation algorithm.

[ ] Milestone 11: Introduce powerup spawning and collection.

[ ] Milestone 12: Build match lifecycle (Lobby, Timer, Leaderboard).

[ ] Milestone 13: Code optimization, memory leak checks, and polish.
