#ifndef MICRAS_HPP
#define MICRAS_HPP

#include <cstdint>
#include <ostream>

#include "known_maze.hpp"
#include "type.hpp"

template <std::uint8_t width, std::uint8_t height>
class Micras {
public:
    Micras(const GridPose& start, const GridPose& goal);

    void step(const std::array<bool, 3>& information);

    const GridPose& get_pose() const;

    template <std::uint8_t w, std::uint8_t h>
    friend std::ostream& operator<<(std::ostream& os, const Micras<w, h>& micras);

private:
    GridPose pose;

    KnownMaze<width, height> known_maze;
};

#include "../src/micras.cpp"

#endif  // MICRAS_HPP
