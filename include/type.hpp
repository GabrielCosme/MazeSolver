#ifndef TYPE_HPP
#define TYPE_HPP

#include <cstdint>

enum Side {
    LEFT = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 3
};

struct Pose {
    Pose() = default;

    Pose(std::uint8_t x, std::uint8_t y, Side orientation) : x(x), y(y), orientation(orientation) { }

    Pose front() const { return *this + this->orientation; }

    Pose turned_back() const { return {this->x, this->y, static_cast<Side>((this->orientation + 2) % 4)}; }

    Pose turned_left() const { return {this->x, this->y, static_cast<Side>((this->orientation + 3) % 4)}; }

    Pose turned_right() const { return {this->x, this->y, static_cast<Side>((this->orientation + 1) % 4)}; }

    Side direction(const Pose& next) const {
        if (next.x < this->x) {
            return Side::LEFT;
        }

        if (next.y < this->y) {
            return Side::UP;
        }

        if (next.x > this->x) {
            return Side::RIGHT;
        }

        if (next.y > this->y) {
            return Side::DOWN;
        }

        return Side::UP;
    }

    bool together(const Pose& other) const { return this->x == other.x and this->y == other.y; }

    bool operator==(const Pose& other) const {
        return this->x == other.x and this->y == other.y and this->orientation == other.orientation;
    }

    Pose operator+(const Side& side) const {
        switch (side) {
            case Side::LEFT:
                return {static_cast<std::uint8_t>(this->x - 1), this->y, side};
            case Side::UP:
                return {this->x, static_cast<std::uint8_t>(this->y - 1), side};
            case Side::RIGHT:
                return {static_cast<std::uint8_t>(this->x + 1), this->y, side};
            case Side::DOWN:
                return {this->x, static_cast<std::uint8_t>(this->y + 1), side};
        }

        return *this;
    }

    std::uint8_t x;
    std::uint8_t y;
    Side         orientation;
};

#endif  // TYPE_HPP
