#pragma once
#include <vector>
#include <cstddef>
#include <cmath>
#include <thread>

namespace nbody
{
    struct ParticleSystem
    {
        std::vector<float> x;
        std::vector<float> y;
        std::vector<float> vx;
        std::vector<float> vy;
        std::vector<float> mass;

        void init(size_t num_particles)
        {
            x.resize(num_particles, 0.0f);
            y.resize(num_particles, 0.0f);
            vx.resize(num_particles, 0.0f);
            vy.resize(num_particles, 0.0f);
            mass.resize(num_particles, 1.0f);
        }

        size_t size() const
        {
            return x.size();
        }
    };

    inline void step_simulation(ParticleSystem &p, float dt)
    {
        const float G = 50000.0f;
        const float softening = 5000.0f;
        size_t n = p.size();

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

                    for (size_t j = 0; j < n; ++j)
                    {
                        if (i == j)
                            continue;

                        float dx = p.x[j] - p.x[i];
                        float dy = p.y[j] - p.y[i];
                        float dist_sqr = (dx * dx) + (dy * dy) + softening;

                        float inv_dist = 1.0f / std::sqrt(dist_sqr);
                        float inv_dist3 = inv_dist * inv_dist * inv_dist;

                        float accel = G * p.mass[j] * inv_dist3;

                        ax += dx * accel;
                        ay += dy * accel;
                    }

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