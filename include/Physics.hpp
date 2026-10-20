#pragma once

#include <glm/glm.hpp>
#include <cmath>

struct AABB
{
    glm::vec2 pos;
    glm::vec2 size;
};

namespace Physics
{
    inline glm::vec2 getOverlap(const AABB& a, const AABB& b)
    {
        glm::vec2 halfA = a.size / 2.0f;
        glm::vec2 halfB = b.size / 2.0f;

        float overlapX = (halfA.x + halfB.x) - std::abs(a.pos.x - b.pos.x);
        float overlapY = (halfA.y + halfB.y) - std::abs(a.pos.y - b.pos.y);

        return { overlapX, overlapY };
    }

    inline bool checkOverlap(const AABB& a, const AABB& b)
    {
        glm::vec2 overlap = getOverlap(a, b);
        return overlap.x > 0.0f && overlap.y > 0.0f;
    }
}
