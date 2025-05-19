#pragma once

#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <algorithm>

namespace Astar
{

    using Grid = std::vector<std::vector<bool>>;

    struct ANode
    {
        int x, y;
        float g, f;
        ANode *parent = nullptr;
    };

    struct NodeCmp
    {
        bool operator()(ANode *a, ANode *b) const
        {
            return a->f > b->f;
        }
    };

    inline float Heuristic(int x1, int y1, int x2, int y2)
    {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    static std::vector<Vector2D> AStar(
        const Grid &grid,
        const Vector2D startPos,
        const Vector2D goalPos)
    {
        int H = (int)grid.size();
        int W = H ? (int)grid[0].size() : 0;
        if (W == 0)
            return {};

        auto inBounds = [&](int x, int y)
        {
            return x >= 0 && y >= 0 && x < W && y < H;
        };

        int sx = (int)startPos.x;
        int sy = (int)startPos.y;
        int gx = (int)goalPos.x;
        int gy = (int)goalPos.y;
        if (!inBounds(sx, sy) || !inBounds(gx, gy) || grid[sy][sx] || grid[gy][gx])
            return {};

        std::vector<std::vector<ANode>> nodes(H, std::vector<ANode>(W));
        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                nodes[y][x].x = x;
                nodes[y][x].y = y;
                nodes[y][x].g = std::numeric_limits<float>::infinity();
                nodes[y][x].f = std::numeric_limits<float>::infinity();
                nodes[y][x].parent = nullptr;
            }
        }

        ANode *start = &nodes[sy][sx];
        ANode *goal = &nodes[gy][gx];
        start->g = 0;
        start->f = Heuristic(sx, sy, gx, gy);

        std::priority_queue<ANode *, std::vector<ANode *>, NodeCmp> openSet;
        openSet.push(start);

        std::vector<std::vector<bool>> closed(H, std::vector<bool>(W, false));

        constexpr int dx[4] = {1, -1, 0, 0};
        constexpr int dy[4] = {0, 0, 1, -1};

        while (!openSet.empty())
        {
            ANode *current = openSet.top();
            openSet.pop();
            if (current == goal)
                break;
            if (closed[current->y][current->x])
                continue;
            closed[current->y][current->x] = true;

            for (int i = 0; i < 4; ++i)
            {
                int nx = current->x + dx[i];
                int ny = current->y + dy[i];
                if (!inBounds(nx, ny) || grid[ny][nx] || closed[ny][nx])
                    continue;

                ANode *neighbor = &nodes[ny][nx];
                float tentative_g = current->g + 1.0f;
                if (tentative_g < neighbor->g)
                {
                    neighbor->parent = current;
                    neighbor->g = tentative_g;
                    neighbor->f = tentative_g + Heuristic(nx, ny, gx, gy);
                    openSet.push(neighbor);
                }
            }
        }

        std::vector<Vector2D> path;
        for (ANode *p = goal; p != nullptr; p = p->parent)
        {
            path.push_back({(float)p->x, (float)p->y});
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<Vector2D> MakeRelativePath(const std::vector<Vector2D> &absPath)
    {
        std::vector<Vector2D> rel;
        if (absPath.size() < 2)
            return rel;
        rel.reserve(absPath.size() - 1);
        for (size_t i = 1; i < absPath.size(); ++i)
        {
            rel.push_back(absPath[i] - absPath[i - 1]);
        }
        return rel;
    }
}
