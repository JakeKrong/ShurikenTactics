#include "TrajectoryMath.h"
#include "Types.h"
#include "Transform.h"
#include "Renderable.h"
#include "Collider.h"

#include <limits>
#include <optional>

//Debug
#include <print>

static constexpr float EPSILON = 0.001f;


// Solve Ray vs Circle intersection. Returns the distance 't' to the entry point, or nullopt if miss.
std::optional<float> RayCircle(sf::Vector2f origin, sf::Vector2f dir, sf::Vector2f center, float radius)
{
    sf::Vector2f L = center - origin;
    float tca = L.dot(dir);
    float d2 = L.lengthSquared() - (tca * tca); // Dist to closest approach squared
    float r2 = radius * radius;

    if (d2 > r2) return std::nullopt; // Miss

    float thc = std::sqrt(r2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    // Check first hit in front of the ray only
    if (t0 > EPSILON) return t0;
    if (t1 > EPSILON) return t1;

    return std::nullopt;
}


 // *** Ray vs AABB (Slab method) *** //
float RayAABB(const sf::Vector2f& origin, const sf::Vector2f& dir, const AABB& hitBox)
{
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    if (std::abs(dir.x) < 1e-6f) { //If x-value of direction vector near 0, will never intersect the left/right of obstacle
        if (origin.x < hitBox.left() || origin.x > hitBox.right()) return std::numeric_limits<float>::infinity();
    }
    else {
        //x(t) = origin.x + (dir.x * t) <-- Below formula is a re-arranged version of this
        float tx1 = (hitBox.left() - origin.x) / dir.x;
        float tx2 = (hitBox.right() - origin.x) / dir.x;
        tmin = std::max(tmin, std::min(tx1, tx2));
        tmax = std::min(tmax, std::max(tx1, tx2));
    }

    if (std::abs(dir.y) < 1e-6f) {
        if (origin.y < hitBox.top() || origin.y > hitBox.bottom()) return std::numeric_limits<float>::infinity();
    }
    else {
        float ty1 = (hitBox.top() - origin.y) / dir.y;
        float ty2 = (hitBox.bottom() - origin.y) / dir.y;
        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    }

    if (tmax >= tmin && tmax > 0.f)
        return (tmin > 0.f ? tmin : tmax);

    return std::numeric_limits<float>::infinity();
}

// *** Compute Trajectory *** //
std::vector<TrajectorySegment> ComputeTrajectory(
    World& world,
    sf::Vector2f origin,
    sf::Vector2f direction,
    float radius,
    int maxBounces)
{
    std::vector<TrajectorySegment> segments;
    
    direction = direction.normalized(); // Ensure direction is normalized

    for (int bounce = 0; bounce < maxBounces; bounce++)
    {
        float bestDist = std::numeric_limits<float>::infinity();
        sf::FloatRect bestOriginalRect;
        bool hasHit = false;

        bool hitIsCorner = false;       // Flag to indiciate if hit is registered on flat side or a rounded corner
        sf::Vector2f hitCornerCenter;   // To store which corner was hit
        ColliderType hitType;           // To track type of collision

        for (auto& [ent, col] : world.GetAllComponentsOfType<Collider>()) //Iterate over all collider component
        {
            if (col->type != ColliderType::ObstacleBox && col->type != ColliderType::TargetBox) continue;

            sf::FloatRect colliderHitBox = std::get<sf::FloatRect>(col->entityColliders[0]);

            AABB colliderAABB{ colliderHitBox.position, colliderHitBox.size };

            // 1. Check against the Expanded AABB (Sharp Corners)
            AABB expandedAABB{
                colliderHitBox.position - sf::Vector2f(radius, radius),
                colliderHitBox.size + sf::Vector2f(radius * 2.f, radius * 2.f)
            };

            float distBox = RayAABB(origin, direction, expandedAABB);

            // If the box is further than our current best hit, skip it
            if (distBox >= bestDist) continue;

            // 2. Validate the hit against the Rounded Rectangle. Calculate where the ray hits the expanded box.
            sf::Vector2f hitPos = origin + direction * distBox;

            // Determine relative position of hitPos against the original rectangle hitbox
            // 9 zones --> Center, Left, Right, Top, Bottom, 4 Corners
            bool isLeft = hitPos.x < colliderAABB.left();
            bool isRight = hitPos.x > colliderAABB.right();
            bool isTop = hitPos.y < colliderAABB.top();
            bool isBottom = hitPos.y > colliderAABB.bottom();

            float actualDist = distBox;
            bool isCornerHit = false;
            sf::Vector2f currentCornerCenter;

            if (isLeft && isTop) {          // Top-Left Corner Zone
                currentCornerCenter = colliderAABB.pos;
                isCornerHit = true;
            }
            else if (isRight && isTop) {  // Top-Right Corner Zone
                currentCornerCenter = { colliderAABB.right(), colliderAABB.top() };
                isCornerHit = true;
            }
            else if (isLeft && isBottom) {// Bottom-Left Corner Zone
                currentCornerCenter = { colliderAABB.left(), colliderAABB.bottom()};
                isCornerHit = true;
            }
            else if (isRight && isBottom) {// Bottom-Right Corner Zone
                currentCornerCenter = colliderAABB.pos + colliderHitBox.size;
                isCornerHit = true;
            }

            // 3. Final Distance Check
            if (isCornerHit) {
                // Hit detected in corner (sharp collision), secondary check required to validate if collision really occurs
                auto circleHit = RayCircle(origin, direction, currentCornerCenter, radius);

                if (circleHit.has_value()) {
                    actualDist = circleHit.value();
                }
                else {
                    // Mismatch detected, ray hit the expanded box corner, but missed the physics circle. Process as miss.
                    continue;
                }
            }

            // 4. Update Best Hit
            if (actualDist < bestDist) {
                bestDist = actualDist;
                bestOriginalRect = colliderHitBox;
                hasHit = true;
                hitIsCorner = isCornerHit;
                if (isCornerHit) hitCornerCenter = currentCornerCenter;
            }

            hitType = col->type;
        }

        if (!hasHit) break;

        // --- Handle Collision Response ---
        sf::Vector2f endPoint;
        if (!(bounce >= maxBounces - 1 && bestDist > 100)) //Not final bounce
        {
            endPoint = origin + direction * bestDist;
        }
        else {
            endPoint = origin + direction * 100.f;
        }
        
        segments.push_back({ origin, endPoint });

        sf::Vector2f normal;

        if (hitIsCorner) {
            // Precise corner normal. Vector from corner center to impact point

            AABB bestOriginalAABB = { bestOriginalRect.position, bestOriginalRect.size };
            int deflectionDegree;

            if (hitCornerCenter.y == bestOriginalAABB.bottom()) {
                if (hitCornerCenter.x == bestOriginalAABB.right()) deflectionDegree = 45;
                else deflectionDegree = 135;
            }
            else {
                if (hitCornerCenter.x == bestOriginalAABB.left()) deflectionDegree = 235;
                else deflectionDegree = 315;
            }

            direction = sf::Vector2f(direction.length(), sf::Angle(sf::degrees(deflectionDegree)));

        }
        else {
            // Flat Side Normal

            float distLeft = std::abs(endPoint.x - bestOriginalRect.position.x - radius);
            float distRight = std::abs(endPoint.x - (bestOriginalRect.position.x + bestOriginalRect.size.x) - radius);
            float distTop = std::abs(endPoint.y - bestOriginalRect.position.y - radius);
            float distBottom = std::abs(endPoint.y - (bestOriginalRect.position.y + bestOriginalRect.size.y) - radius);

            // Check min for X and Y
            float minX = std::min(distLeft, distRight);
            float minY = std::min(distTop, distBottom);

            if (minX < minY) {
                // Vertical wall hit
                normal = (distLeft < distRight) ? sf::Vector2f(-1.f, 0.f) : sf::Vector2f(1.f, 0.f);
                direction.x = -direction.x;
            }
            else {
                // Horizontal wall hit
                normal = (distTop < distBottom) ? sf::Vector2f(0.f, -1.f) : sf::Vector2f(0.f, 1.f);
                direction.y = -direction.y;
            }
        }

        // Slightly nudge origin to prevent double-hit registering on same obstacle
        origin = endPoint + direction * (EPSILON * 5.f);

        if (hitType == ColliderType::TargetBox) break; // Break loop regardless of remaining bounce if target is hit
    }

    return segments;
}