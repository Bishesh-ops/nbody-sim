# N-Body Simulation Engine

A highly optimized, multithreaded 2D gravitational physics engine written in modern C++20. 

What started as a brute-force O(N^2) physics loop was refactored into a high-performance simulation capable of calculating gravitational interactions for tens of thousands of particles in real-time at 60 FPS. The visualizer uses SFML with additive blending and velocity-mapped vector trails to simulate fluid galactic mechanics and accretion disks.

![Simulation Demo](placeholder_for_your_gif_here.gif) 

## Under the Hood: The Architecture

This project was built to explore low-level systems optimizations, parallel compute, and algorithmic efficiency.

### 1. Algorithmic Complexity: The Barnes-Hut Tree
The naive approach to N-Body simulations requires calculating the gravitational pull between every single pair of particles. For 30,000 particles, that is 900 million calculations per frame. 

To solve this, the engine implements the **Barnes-Hut algorithm**. It dynamically builds a Quadtree from scratch every frame, grouping distant particles into single "Centers of Mass." This drops the mathematical complexity from O(N^2) to O(N log N), allowing the engine to handle massive particle counts without locking up the CPU.

### 2. Data-Oriented Design (DoD)
Instead of traditional Object-Oriented Programming (an Array of Structs like `std::vector<Particle>`), the engine uses a **Struct of Arrays (SoA)** memory layout.
* Positions, velocities, and masses are stored in completely separate, contiguous blocks of memory.
* This maximizes CPU cache locality, preventing cache misses during tight mathematical loops, and ensures memory bandwidth is never wasted on data the CPU doesn't immediately need.

### 3. Modern Concurrency (`std::jthread`)
The tree traversal and force calculations are heavily parallelized. Using C++20's `std::jthread` and hardware concurrency polling, the workload is evenly chunked and distributed across all available logical CPU cores. Because of the SoA memory layout, threads only write to their specific slices of the velocity arrays, completely eliminating data races and the need for expensive mutex locks.

## Controls
* **Scroll Wheel**: Zoom in/out of the galaxy core.
* **W, A, S, D**: Pan the camera across the simulation.

## Build Instructions

This project uses CMake and requires SFML for the rendering engine.

**Dependencies (Debian/Ubuntu):**
```bash
sudo apt update
sudo apt install build-essential cmake libsfml-dev

// Build and Run
git clone [https://github.com/Bishesh-ops/nbody-sim.git](https://github.com/Bishesh-ops/nbody-sim.git)
cd nbody-sim
mkdir build && cd build
cmake ..
make
./nbody_sim