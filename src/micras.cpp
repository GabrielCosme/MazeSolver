#ifndef MICRAS_CPP
#define MICRAS_CPP

#include <sstream>

#include "micras.hpp"

template <std::uint8_t width, std::uint8_t height>
Micras<width, height>::Micras(const GridPose& start) : pose(start), known_maze(start) { }

template <std::uint8_t width, std::uint8_t height>
void Micras<width, height>::step(const Information& information) {
    this->known_maze.update(this->pose, information);
    GridPoint current_goal = this->known_maze.get_current_goal(this->pose.position);

    if (this->pose.position.direction(current_goal) == this->pose.orientation) {
        this->pose.position = current_goal;
    } else {
        this->pose.orientation = this->pose.position.direction(current_goal);
    }
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
        (4 * width + 3) * (2 * (micras.pose.position.y) + 1) + 4 * (width - micras.pose.position.x) + 1;

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
