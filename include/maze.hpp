#ifndef MAZE_HPP
#define MAZE_HPP

#include <array>
#include <cstdint>
#include <ostream>
#include <string>

#include "type.hpp"

template <std::uint8_t width, std::uint8_t height>
class Maze {
public:
    Maze(const std::string& filename);

    std::array<bool, 3> get_information(const Pose& pose) const;

    template <std::uint8_t w, std::uint8_t h>
    friend std::ostream& operator<<(std::ostream& os, const Maze<w, h>& maze);

private:
    std::array<std::array<std::array<bool, 4>, width>, height> walls{};
};

#include "../src/maze.cpp"

#endif  // MAZE_HPP
