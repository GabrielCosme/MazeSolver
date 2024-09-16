#ifndef KNOWN_MAZE_HPP
#define KNOWN_MAZE_HPP

#include <array>
#include <cstdint>
#include <ostream>
#include <map>

#include "type.hpp"

template <std::uint8_t width, std::uint8_t height>
class KnownMaze {
public:
    KnownMaze(const Pose& start, const Pose& goal);

    void update(const Pose& pose, const std::array<bool, 3>& information);

    Pose get_action(const Pose& pose, bool force_costmap = false) const;

    template <std::uint8_t w, std::uint8_t h>
    friend std::ostream& operator<<(std::ostream& os, const KnownMaze<w, h>& maze);

private:
    struct Cell {
        std::array<bool, 4> walls{};
        std::array<bool, 4> seen{};
        std::uint16_t       cost{0xFFFF};
    };

    void calculate_costmap();

    const Cell& get_cell(const Pose& pose) const;

    Cell& get_cell(const Pose& pose);

    void update_walls(const Pose& pose, bool wall);

    std::array<std::array<Cell, width>, height> cells{};

    Pose start;
    Pose goal;

    bool returning{};
    bool exploring{true};

    std::map<std::uint16_t, std::pair<std::uint8_t, std::uint8_t>, std::greater<std::uint16_t>> best_route;
};

#include "../src/known_maze.cpp"

#endif  // KNOWN_MAZE_HPP
