#pragma once
#include <vector>

namespace nbody
{
    struct QuadNode
    {
        float center_x, center_y;
        float size;
        // com is for center of mass
        float com_x = 0.0f;
        float com_y = 0.0f;
        float total_mass = 0.0f;
        // Tree Structure
        int particle_idx = -1;              // -1 if this quad / square holds none or multiple particles
        int children[4] = {-1, -1, -1, -1}; // Indices of the 4 sub quads (the children)
        bool is_leaf = true;                // has no children
    };

    class QuadTree
    {
    public:
        std::vector<QuadNode> nodes;

        void reset()
        {
            nodes.clear();
            nodes.reserve(100000);
        }

        int create_node(float cx, float cy, float size)
        {
            QuadNode node;
            node.center_x = cx;
            node.center_y = cy;
            node.size = size;
            nodes.push_back(node);
            return nodes.size() - 1;
        }

        int get_quadrant(float cx, float cy, float px, float py)
        {
            if (px <= cx && py <= cy)
                return 0; // Top-Left
            if (px > cx && py <= cy)
                return 1; // Top-Right
            if (px <= cx && py > cy)
                return 2; // Bottom-Left
            return 3;
        }

        void insert(int node_idx, int p_idx, const ParticleSystem &p)
        {
            float new_mass = nodes[node_idx].total_mass + p.mass[p_idx];

            nodes[node_idx].com_x = (nodes[node_idx].com_x * nodes[node_idx].total_mass + p.x[p_idx] * p.mass[p_idx]) / new_mass;
            nodes[node_idx].com_y = (nodes[node_idx].com_y * nodes[node_idx].total_mass + p.y[p_idx] * p.mass[p_idx]) / new_mass;
            nodes[node_idx].total_mass = new_mass;
            // Base Case for our Recursion (Cecking if it is a leaf and if it is empty)
            if (nodes[node_idx].is_leaf && nodes[node_idx].particle_idx == -1)
            {
                nodes[node_idx].particle_idx = p_idx;
                return;
            }

            // Main Process of  dividing up a quad tree square if it has multiple residents
            if (nodes[node_idx].is_leaf)
            {
                nodes[node_idx].is_leaf = false;

                int old_p_idx = nodes[node_idx].particle_idx;
                nodes[node_idx].particle_idx = -1;

                float cx = nodes[node_idx].center_x;
                float cy = nodes[node_idx].center_y;
                float q_size = nodes[node_idx].size / 2.0f;
                float offset = q_size / 2.0f;

                nodes[node_idx].children[0] = create_node(cx - offset, cy - offset, q_size);
                nodes[node_idx].children[1] = create_node(cx + offset, cy - offset, q_size);
                nodes[node_idx].children[2] = create_node(cx - offset, cy + offset, q_size);
                nodes[node_idx].children[3] = create_node(cx + offset, cy + offset, q_size);

                int old_quad = get_quadrant(cx, cy, p.x[old_p_idx], p.y[old_p_idx]);
                insert(nodes[node_idx].children[old_quad], old_p_idx, p);
            }

            int new_quad = get_quadrant(nodes[node_idx].center_x, nodes[node_idx].center_y, p.x[p_idx], p.y[p_idx]);
            insert(nodes[node_idx].children[new_quad], p_idx, p);
        }

        void calculate_force(int node_idx, const ParticleSystem &p, int p_idx, float &ax, float &ay, float G, float softening, float theta = 0.5f) const
        {
            if (node_idx == -1)
                return;

            const QuadNode &node = nodes[node_idx];

            if (node.is_leaf && node.particle_idx == -1)
                return;
            float dx = node.com_x - p.x[p_idx];
            float dy = node.com_y - p.y[p_idx];
            float dist_sqr = (dx * dx) + (dy * dy) + softening;
            float dist = std::sqrt(dist_sqr);
            if (node.is_leaf && node.particle_idx == p_idx)
                return;
            float inv_dist = 1.0f / dist;
            float inv_dist3 = inv_dist * inv_dist * inv_dist;
            float accel = G * node.total_mass * inv_dist3;

            ax += dx * accel;
            ay += dy * accel;
        }
    };
}