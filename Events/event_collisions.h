#pragma once

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include "object.h"

namespace Collision
{
    inline std::vector<Object*> Raycast(
        const Vector2D&               origin,
        float                         angleRad,
        float                         maxDistance,
        const std::vector<Object*>&   objects,
        int                           stopFlagsMask   = 0,
        int                           ignoreFlagsMask = 0
    ) {

        struct Hit { Object* obj; float t; };
        std::vector<Hit> hits;
        hits.reserve(objects.size());
    
        // Direction normalisée du rayon
        Vector2D dir{ std::cos(angleRad), std::sin(angleRad) };
    
        for (auto* o : objects) {
            if (!o->IsActive() || !o->collision) 
                continue;
    
            Vector2D C = o->GetWorldPosition();
            Vector2D E = o->collisionDelimiter;
            float   rot = o->delimiterAffectedByRotation 
                          ? o->GetRotation() * DEG2RAD 
                          : 0.0f;
            float cosR = std::cos(-rot), sinR = std::sin(-rot);
    
            Vector2D L0 {
                (origin.x - C.x) * cosR - (origin.y - C.y) * sinR,
                (origin.x - C.x) * sinR + (origin.y - C.y) * cosR
            };
            Vector2D Ld {
                dir.x * cosR - dir.y * sinR,
                dir.x * sinR + dir.y * cosR
            };
    
            float tmin = -FLT_MAX, tmax = FLT_MAX;
            if (std::fabs(Ld.x) < 1e-6f) {
                if (L0.x < -E.x || L0.x > +E.x) continue;
            } else {
                float tx1 = (-E.x - L0.x) / Ld.x;
                float tx2 = (+E.x - L0.x) / Ld.x;
                if (tx1 > tx2) std::swap(tx1, tx2);
                tmin = std::max(tmin, tx1);
                tmax = std::min(tmax, tx2);
                if (tmin > tmax) continue;
            }
            if (std::fabs(Ld.y) < 1e-6f) {
                if (L0.y < -E.y || L0.y > +E.y) continue;
            } else {
                float ty1 = (-E.y - L0.y) / Ld.y;
                float ty2 = (+E.y - L0.y) / Ld.y;
                if (ty1 > ty2) std::swap(ty1, ty2);
                tmin = std::max(tmin, ty1);
                tmax = std::min(tmax, ty2);
                if (tmin > tmax) continue;
            }
    
            float tHit = (tmin >= 0.0f ? tmin : (tmax >= 0.0f ? tmax : FLT_MAX));
            if (tHit >= 0.0f && tHit <= maxDistance) {
                hits.push_back({ o, tHit });
            }
        }
    
        std::sort(hits.begin(), hits.end(),
                  [](auto const& a, auto const& b){ return a.t < b.t; });
    
        std::vector<Object*> result;
        result.reserve(hits.size());
        for (auto const& h : hits) {
            Object* o = h.obj;
            int flags = o->GetFlags();
            if (ignoreFlagsMask  && (flags & ignoreFlagsMask)) 
                continue;
            if (stopFlagsMask    && (flags & stopFlagsMask)) 
                break;
            result.push_back(o);
        }
        return result;
    }

    // Represents an unordered pair of objects for collision tracking
    struct CollisionPair
    {
        Object* a;
        Object* b;
        CollisionPair(Object* x, Object* y)
        {
            if (x < y) { a = x; b = y; }
            else       { a = y; b = x; }
        }
        bool operator==(CollisionPair const& o) const noexcept
        {
            return a == o.a && b == o.b;
        }
    };

    // Hash function for CollisionPair
    struct PairHash
    {
        std::size_t operator()(CollisionPair const& p) const noexcept
        {
            auto h1 = std::hash<Object*>()(p.a);
            auto h2 = std::hash<Object*>()(p.b);
            return h1 ^ (h2 << 1);
        }
    };

    // Projection of OBB onto an axis, returns half-length of projection
    inline float ProjectRadius(const Vector2D& extents,
                               const Vector2D& u,
                               const Vector2D& axis)
    {
        float r1 = std::fabs(Math::Dot(axis, u)) * extents.x;
        float r2 = std::fabs(Math::Dot(axis, Vector2D{-u.y, u.x})) * extents.y;
        return r1 + r2;
    }

    /**
     * Compute the Minimum Translation Vector (MTV) between two OBBs.
     * Returns true if collision, and sets outAxis (unit vector) and outOverlap (penetration depth).
     */
    inline bool ComputeMTV(Object* o1,
                           Object* o2,
                           Vector2D& outAxis,
                           float& outOverlap)
    {
        Vector2D c1 = o1->GetWorldPosition();
        Vector2D c2 = o2->GetWorldPosition();
        Vector2D e1 = o1->collisionDelimiter;
        Vector2D e2 = o2->collisionDelimiter;
        float r1 = o1->delimiterAffectedByRotation ? o1->GetRotation() * DEG2RAD : 0.0f;
        float r2 = o2->delimiterAffectedByRotation ? o2->GetRotation() * DEG2RAD : 0.0f;
        Vector2D u1{ std::cos(r1), std::sin(r1) };
        Vector2D u2{ std::cos(r2), std::sin(r2) };
        Vector2D axes[4] = { u1, Vector2D{-u1.y, u1.x}, u2, Vector2D{-u2.y, u2.x} };
        Vector2D d{ c2.x - c1.x, c2.y - c1.y };

        float minOverlap = FLT_MAX;
        Vector2D smallestAxis{0,0};

        for (int i = 0; i < 4; ++i)
        {
            Vector2D axis = axes[i];
            float len = std::sqrtf(axis.x*axis.x + axis.y*axis.y);
            axis.x /= len; axis.y /= len;
            float proj1 = ProjectRadius(e1, u1, axis);
            float proj2 = ProjectRadius(e2, u2, axis);
            float dist  = Math::Dot(d, axis);
            float overlap = (proj1 + proj2) - std::fabs(dist);
            if (overlap < 0.0f) return false;
            if (overlap < minOverlap)
            {
                minOverlap = overlap;
                smallestAxis = (dist < 0.0f) ? Vector2D{-axis.x, -axis.y} : axis;
            }
        }
        outOverlap = minOverlap;
        outAxis    = smallestAxis;
        return true;
    }

    class CollisionSystem
    {
    public:
        // Set the objects to test each frame
        void SetObjects(const std::vector<Object*>& objs)
        {
            objects = objs;
        }

        // Call this once per frame after updating all objects
        void Update()
        {
            current.clear();
            // Broad-phase: brute-force SAT with MTV
            for (size_t i = 0; i < objects.size(); ++i)
            {
                for (size_t j = i + 1; j < objects.size(); ++j)
                {
                    Object* o1 = objects[i];
                    Object* o2 = objects[j];
                    if (!o1->IsActive() || !o2->IsActive() || !o1->collision || !o2->collision)
                        continue;
                    Vector2D axis;
                    float overlap;
                    if (ComputeMTV(o1, o2, axis, overlap))
                    {
                        current.insert(CollisionPair(o1, o2));
                    }
                }
            }
            // Dispatch collision events
            for (auto const& pair : current)
            {
                if (previous.find(pair) == previous.end())
                {
                    pair.a->OnCollisionEnter(pair.b);
                    pair.b->OnCollisionEnter(pair.a);
                }
                else
                {
                    pair.a->OnCollisionStay(pair.b);
                    pair.b->OnCollisionStay(pair.a);
                }
            }
            for (auto const& pair : previous)
            {
                if (current.find(pair) == current.end())
                {
                    pair.a->OnCollisionExit(pair.b);
                    pair.b->OnCollisionExit(pair.a);
                }
            }
            previous.swap(current);
        }

    private:
        std::vector<Object*> objects;
        std::unordered_set<CollisionPair, PairHash> previous, current;
    };

}
