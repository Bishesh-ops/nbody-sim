#include <iostream>
#include <chrono>
#include <random>
#include "physics.hpp"
#include "quadtree.hpp"

void brute_force_step(nbody::ParticleSystem &p, float dt)
{
    const float G = 50000.0f;
    const float softening = 5000.0f;
    size_t n = p.size();

    for (size_t i = 0; i < n; ++i)
    {
        float ax = 0.0f, ay = 0.0f;
        for (size_t j = 0; j < n; ++j)
        {
            if (i == j)
                continue;
            float dx = p.x[j] - p.x[i];
            float dy = p.y[j] - p.y[i];
            float dist_sqr = (dx * dx) + (dy * dy) + softening;
            float inv_dist = 1.0f / std::sqrt(dist_sqr);
            float accel = G * p.mass[j] * (inv_dist * inv_dist * inv_dist);
            ax += dx * accel;
            ay += dy * accel;
        }
        // Just calculating, not updating positions so we don't mess up the next test
    }
}

int main()
{
    size_t particle_count = 10000;
    float dt = 0.016f;

    nbody::ParticleSystem universe_a;
    universe_a.init(particle_count);

    // Fill with random dummy data
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(0.0f, 1000.0f);
    for (size_t i = 0; i < particle_count; ++i)
    {
        universe_a.x[i] = dist(gen);
        universe_a.y[i] = dist(gen);
        universe_a.mass[i] = 1.0f;
    }

    nbody::ParticleSystem universe_b = universe_a; // Exact copy for fairness
    nbody::QuadTree tree;

    std::cout << "Benchmarking " << particle_count << " particles...\n";
    std::cout << "--------------------------------------------------\n";

    // --- TEST 1: Brute Force O(N^2) ---
    auto start_brute = std::chrono::high_resolution_clock::now();
    brute_force_step(universe_a, dt);
    auto end_brute = std::chrono::high_resolution_clock::now();
    auto duration_brute = std::chrono::duration_cast<std::chrono::milliseconds>(end_brute - start_brute);

    std::cout << "[O(N^2) Brute Force]      Time: " << duration_brute.count() << " ms\n";

    // --- TEST 2: Barnes-Hut O(N log N) + Multithreading ---
    auto start_bh = std::chrono::high_resolution_clock::now();
    nbody::step_simulation(universe_b, tree, dt);
    auto end_bh = std::chrono::high_resolution_clock::now();
    auto duration_bh = std::chrono::duration_cast<std::chrono::milliseconds>(end_bh - start_bh);

    std::cout << "[O(N log N) Barnes-Hut]   Time: " << duration_bh.count() << " ms\n";

    if (duration_bh.count() > 0)
    {
        float speedup = static_cast<float>(duration_brute.count()) / duration_bh.count();
        std::cout << "\nPerformance Increase: " << speedup << "x faster!\n";
    }

    return 0;
}