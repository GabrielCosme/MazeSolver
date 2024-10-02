#ifndef TYPE_HPP
#define TYPE_HPP

#include <cstdint>
#include <functional>

/**
 * @brief Possible directions in the grid
 */
enum Side : uint8_t {
    RIGHT = 0,
    UP = 1,
    LEFT = 2,
    DOWN = 3
};

/**
 * @brief Converts an angle in radians to a Grid side
 *
 * @param angle The angle in radians
 * @return The corresponding Grid side
 */
Side angle_to_grid(float angle);

/**
 * @brief Type to store information originating from the distance sensors
 */
struct Information {
    /**
     * @brief Possible values for the existence of a wall
     */
    enum Existence : uint8_t {
        UNKNOWN = 0,
        FREE = 1,
        WALL = 2
    };

    /**
     * @brief Possible walls in the grid to be checked
     */
    Existence left;
    Existence front_left;
    Existence front;
    Existence front_right;
    Existence right;
};

/**
 * @brief Type to store a point in the grid
 */
struct GridPoint {
    /**
     * @brief Returns the direction from this point to the next one
     *
     * @param next The next point
     * @return The direction from this point to the next one
     */
    Side direction(const GridPoint& next) const;

    /**
     * @brief Moves in the grid in the direction of the side
     *
     * @param side The side to move to
     * @return The new point after moving
     */
    GridPoint operator+(const Side& side) const;

    /**
     * @brief Compares two points for equality
     *
     * @param other The other point to compare
     * @return True if the points are equal, false otherwise
     */
    bool operator==(const GridPoint& other) const;

    /**
     * @brief The x coordinate of the point on the grid
     */
    uint8_t x;

    /**
     * @brief The y coordinate of the point on the grid
     */
    uint8_t y;
};

struct GridPose {
    /**
     * @brief Returns the pose after moving forward
     *
     * @return The pose after moving forward
     */
    GridPose front() const;

    /**
     * @brief Returns the pose after turning back
     *
     * @return The pose after turning back
     */
    GridPose turned_back() const;

    /**
     * @brief Returns the pose after turning left
     *
     * @return The pose after turning left
     */
    GridPose turned_left() const;

    /**
     * @brief Returns the pose after turning right
     *
     * @return The pose after turning right
     */
    GridPose turned_right() const;

    /**
     * @brief Compares two poses for equality
     *
     * @param other The other pose to compare
     * @return True if the poses are equal, false otherwise
     */
    bool operator==(const GridPose& other) const;

    /**
     * @brief The position of the pose on the grid
     */
    GridPoint position;

    /**
     * @brief The orientation of the pose on the grid
     */
    Side orientation;
};

namespace std {
/**
 * @brief Hash specialization for the GridPoint type
 *
 * @tparam T GridPoint type
 */
template <>
struct hash<GridPoint> {
    size_t operator()(const GridPoint& point) const noexcept {
        size_t h1 = hash<uint8_t>{}(point.x);
        size_t h2 = hash<uint8_t>{}(point.y);
        return h1 ^ (h2 << 1);
    }
};
}  // namespace std

#endif  // TYPE_HPP
