#pragma once
#include <vector>
#include <cstddef>
#include <cmath>
#include <thread>
#include <algorithm>
#include "particle_system.hpp"
#include "quadtree.hpp"
namespace nbody
{
    inline void step_simulation(ParticleSystem &p, QuadTree &tree, float dt)
    {
        const float G = 50000.0f;
        const float softening = 5000.0f;
        size_t n = p.size();

        float min_x = p.x[0], max_x = p.x[0];
        float min_y = p.y[0], max_y = p.y[0];
        for (size_t i = 1; i < n; ++i)
        {
            if (p.x[i] < min_x)
                min_x = p.x[i];
            if (p.x[i] > max_x)
                max_x = p.x[i];
            if (p.y[i] < min_y)
                min_y = p.y[i];
            if (p.y[i] > max_y)
                max_y = p.y[i];
        }

        float width = max_x - min_x;
        float height = max_y - min_y;
        float max_size = std::max(width, height) + 1.0f;
        float center_x = min_x + width / 2.0f;
        float center_y = min_y + height / 2.0f;

        tree.reset();
        int root = tree.create_node(center_x, center_y, max_size);
        for (size_t i = 0; i < n; ++i)
        {
            tree.insert(root, i, p);
        }

        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0)
            num_threads = 4;
        size_t chunk_size = n / num_threads;

        {
            std::vector<std::jthread> threads;
            auto worker = [&](size_t start, size_t end)
            {
                for (size_t i = start; i < end; ++i)
                {
                    float ax = 0.0f;
                    float ay = 0.0f;

                    tree.calculate_force(root, p, i, ax, ay, G, softening);

                    p.vx[i] += ax * dt;
                    p.vy[i] += ay * dt;
                }
            };

            for (unsigned int t = 0; t < num_threads; ++t)
            {
                size_t start = t * chunk_size;
                size_t end = (t == num_threads - 1) ? n : start + chunk_size;
                threads.emplace_back(worker, start, end);
            }
        }

        for (size_t i = 0; i < n; ++i)
        {
            p.x[i] += p.vx[i] * dt;
            p.y[i] += p.vy[i] * dt;
        }
    }
}