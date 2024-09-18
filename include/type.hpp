#ifndef TYPE_HPP
#define TYPE_HPP

#include <cstdint>

enum Side {
    LEFT = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 3
};

struct GridPoint {
    GridPoint() = default;

    GridPoint(std::uint8_t x, std::uint8_t y);

    Side direction(const GridPoint& next) const;

    GridPoint operator+(const GridPoint& GridPoint) const;

    GridPoint operator+(const Side& side) const;

    bool operator==(const GridPoint& other) const;

    std::uint8_t x;
    std::uint8_t y;
};

struct GridPose {
    GridPose() = default;

    GridPose(GridPoint position, Side orientation);

    GridPose(std::uint8_t x, std::uint8_t y, Side orientation);

    GridPose front() const;

    GridPose turned_back() const;

    GridPose turned_left() const;

    GridPose turned_right() const;

    bool operator==(const GridPose& other) const;

    GridPoint position;
    Side      orientation;
};

#endif  // TYPE_HPP
