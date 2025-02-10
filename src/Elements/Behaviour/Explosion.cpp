#include "Elements/Behaviour/Explosion.hpp"
#include "Utility/Random.hpp"
#include <unordered_set>

using cached_points = std::unordered_set<sf::Vector2i, Vector2iHash>;

bool CacheCell(sf::Vector2i point, cached_points cachedCells) {
    if (cachedCells.find(point) != cachedCells.end()) {
        return true;
    }

    cachedCells.insert(point);
    return false;
}

bool ExplodeCell(sf::Vector2i point, float force, sf::Vector2f direction, WorldDelegate &d, cached_points cachedExplosion) {
    if (CacheCell(point, cachedExplosion)) { return false; } // Ignore cells that have been exploded already.
        
    // Account for explosions crossing rooms.
    roomID_t newRoomID = d.ContainingRoomID(point);
    if (!VALID_ROOM(newRoomID)) { return; } // No need to spawn new rooms, as an invalid room implies it's empty (nothing to explode).
    SandRoom *explosionRoom = d.GetRoom(newRoomID);

    // Dampen the explosion when it hits hard elements.
    const ConstProperties &prop = d.GetProperties(point);
    force -= prop.hardness;
    if (force <= 0.f) {
        return true;
    }

    // Chance to destroy - Always destroy immovable elements.
    if (Probability(60) || prop.Immoveable()) {
        if (Probability(80))
            explosionRoom->QueueAction(explosionRoom->ToIndex(point), Element::air);
        else
            explosionRoom->QueueAction(explosionRoom->ToIndex(point), Element::spark);
    // Chance to throw debris.
    } else {
        if (Probability(5)) { // Shoot sparks out that can catch fire.
            d.SpawnParticle(point, (force + QuickRandInt(2 * force)) * direction,
                Element::fire, d.properties.Colour(Element::fire));

        } else if (prop.Moveable()) { // Shoot moveable debris around.
            size_t cellIndex = explosionRoom->ToIndex(point);

            d.CellToParticle(point, (force + QuickRandInt(2 * force)) * direction);
        }
    }

    return false;
}

/**
 * Generates an explosive line from point pCentre to pRadius. An additional random force will be extended past this
 * in order to draw scorch marks on unmovable element cells.
 * Moveable cells may also be thrown out as particles.
 */
void ExplodeRadius(sf::Vector2i pCentre, sf::Vector2i pRadius, float force, WorldDelegate &d,
    cached_points &cachedCells, cached_points &cachedShockwave) {
    // Set up the variables for a supercover line.
    int dx      = pRadius.x - pCentre.x,    dy      = pRadius.y - pCentre.y;    // The difference in point positions.
    float nx    = std::abs(dx),             ny      = std::abs(dy);             // The number of grid spaces to move.
    int sgnx    = (dx > 0) - (dx < 0),      sgny    = (dy > 0) - (dy < 0);      // The direction to step.
    // The current room that the explosion is occuring in.
    SandRoom *explosionRoom {d.room};
    // The direction that the explosion is travelling.
    sf::Vector2f dir {pRadius - pCentre};
    dir *= (1.f / std::sqrtf(dir.x * dir.x + dir.y * dir.y));

    // Lambda expression for stepping towards the explosion radius.
    auto updateStep = [nx, ny, sgnx, sgny](sf::Vector2i point, int &x, int &y) {
        if ((0.5 + x) / nx < (0.5 + y) / ny) {
            // Take a horizontal step.
            point.x += sgnx;
            x++;
        } else {
            // Take a vertical step.
            point.y += sgny;
            y++;
        }
        return point;
    };

    sf::Vector2i point {pCentre}; // The point of the explosion.
    bool dampened = false;  // Whether or not the explosion was stopped by something.
    // Step through the destructive distance of the explosion.
    for (int ix = 0, iy = 0; ix < nx || iy < ny; point = updateStep(point, ix, iy)) {
        if (CacheCell(point, cachedCells)) { continue; } // Ignore cells that have been exploded already.
        
        // Account for explosions crossing rooms.
        roomID_t newRoomID = d.ContainingRoomID(point);
        if (!VALID_ROOM(newRoomID)) { return; } // No need to spawn new rooms, as an invalid room implies it's empty (nothing to explode).
        explosionRoom = d.GetRoom(newRoomID);

        // Dampen the explosion when it hits hard elements.
        const ConstProperties &prop = d.GetProperties(point);
        force -= prop.hardness;
        if (force <= 0.f) {
            dampened = true;
            break;
        }

        // Chance to destroy - Always destroy immovable elements.
        if (Probability(60) || prop.Immoveable()) {
            if (Probability(80))
                explosionRoom->QueueAction(explosionRoom->ToIndex(point), Element::air);
            else
                explosionRoom->QueueAction(explosionRoom->ToIndex(point), Element::spark);
        // Chance to throw debris.
        } else {
            float launchForce = force + QuickRandInt(2 * force);
            // Shoot sparks out that can catch fire.
            if      (Probability(5))  { d.SpawnParticle(point, launchForce * dir, Element::fire, d.properties.Colour(Element::fire)); }
            // Shoot moveable debris around.
            else if (prop.Moveable()) { d.CellToParticle(point, launchForce * dir); }
        }
    }

    int shockwaveRadius = RandInt(std::max(nx, ny) / 2);
    // Extend a shockwave past the immediate destructive radius.
    for (int ix = 0, iy = 0; ix < shockwaveRadius && iy < shockwaveRadius; point = updateStep(point, ix, iy)) {
        if (CacheCell(point, cachedShockwave)) { continue; } // Ignore cells that have been exploded already.

        // Account for shockwave crossing rooms.
        roomID_t newRoomID = d.ContainingRoomID(point);
        if (!VALID_ROOM(newRoomID)) { return; } // No need to spawn new rooms, as an invalid room implies it's empty (nothing to explode).
        explosionRoom = d.GetRoom(newRoomID);


        const ConstProperties &prop = d.GetProperties(point);
        // Darken immovable elements to create scorch marks.
        if (prop.Immoveable()) {
            explosionRoom->grid.Darken(explosionRoom->ToIndex(point));
            continue;
        }

        // Throw moveable elements as debris, only if the explosion has NOT been dampened.
        if (!dampened && prop.Moveable()) {
            d.CellToParticle(point, (force + QuickRandInt(2 * force)) * dir);
        }
    }
}

bool Explosion::ActOnSelf  (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) {
    float radius {25.5};
    cached_points cachedCells;
    cached_points cachedShockwave;
    float force {100.f};
    for (int h = 0; h <= std::round(radius * std::sqrtf(0.5f)); ++h) {
        int b {static_cast<int>(std::round(std::sqrtf(radius * radius - h * h)))};

        // The circumference calculation can be repeated for each octant.
        ExplodeRadius(p, sf::Vector2i(p.x + b, p.y + h), force, delegate, cachedCells, cachedShockwave);
        ExplodeRadius(p, sf::Vector2i(p.x - b, p.y + h), force, delegate, cachedCells, cachedShockwave);
        ExplodeRadius(p, sf::Vector2i(p.x + b, p.y - h), force, delegate, cachedCells, cachedShockwave);
        ExplodeRadius(p, sf::Vector2i(p.x - b, p.y - h), force, delegate, cachedCells, cachedShockwave);
        ExplodeRadius(p, sf::Vector2i(p.x + h, p.y + b), force, delegate, cachedCells, cachedShockwave);
        ExplodeRadius(p, sf::Vector2i(p.x - h, p.y + b), force, delegate, cachedCells, cachedShockwave);
        ExplodeRadius(p, sf::Vector2i(p.x + h, p.y - b), force, delegate, cachedCells, cachedShockwave);
        ExplodeRadius(p, sf::Vector2i(p.x - h, p.y - b), force, delegate, cachedCells, cachedShockwave);
    }

    return true;
}

bool Explosion::ActOnOther (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) {

}
