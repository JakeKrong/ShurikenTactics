#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "World.h"

// A line segment of the predicted path
struct TrajectorySegment {
    sf::Vector2f start;
    sf::Vector2f end;
};

std::vector<TrajectorySegment> ComputeTrajectory(
    World& world,
    sf::Vector2f origin,
    sf::Vector2f direction,
    float radius,         // projectile radius (for circle collision)
    int maxBounces        // number of predicted ricochets
);

bool HasLineOfSight(World& world, const sf::Vector2f& origin, const sf::Vector2f& target);