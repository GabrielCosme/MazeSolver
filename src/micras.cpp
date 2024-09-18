#ifndef MICRAS_CPP
#define MICRAS_CPP

#include <sstream>

#include "micras.hpp"

template <std::uint8_t width, std::uint8_t height>
Micras<width, height>::Micras(const GridPose& start, const GridPose& goal) : pose(start), known_maze(start, goal) { }

template <std::uint8_t width, std::uint8_t height>
void Micras<width, height>::step(const std::array<bool, 3>& information) {
    this->known_maze.update(this->pose, information);
    this->pose = this->known_maze.get_action(this->pose);
}

template <std::uint8_t width, std::uint8_t height>
const GridPose& Micras<width, height>::get_pose() const {
    return this->pose;
}

template <std::uint8_t width, std::uint8_t height>
std::ostream& operator<<(std::ostream& os, const Micras<width, height>& micras) {
    std::stringstream buffer;

    buffer << micras.known_maze;

    std::int16_t mouse_pos =
        (4 * width + 3) * (2 * (height - 1 - micras.pose.position.y) + 1) + 4 * (width - micras.pose.position.x) + 1;

    buffer.seekp(-mouse_pos, std::ios_base::cur);

    switch (micras.pose.orientation) {
        case Side::LEFT:
            buffer << " <";
            break;
        case Side::UP:
            buffer << "/\\";
            break;
        case Side::RIGHT:
            buffer << "> ";
            break;
        case Side::DOWN:
            buffer << "\\/";
            break;
    }

    buffer.seekp(0, std::ios_base::beg);

    while (buffer.peek() != EOF) {
        char aux = buffer.get();

        if (aux == '%') {
            os << "█";
        } else if (aux == '[' or aux == ']') {
            os << "🮕";
        } else if (aux == '(') {
            os << "╒";
        } else if (aux == ')') {
            os << "╕";
        } else if (aux == '>') {
            os << "🮥";
        } else if (aux == '<') {
            os << "🮤";
        } else {
            os << aux;
        }
    }

    return os;
}

#endif  // MICRAS_CPP
