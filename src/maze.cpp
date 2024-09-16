#ifndef MAZE_CPP
#define MAZE_CPP

#include <fstream>

#include "maze.hpp"

template <std::uint8_t width, std::uint8_t height>
Maze<width, height>::Maze(const std::string& filename) {
    std::ifstream file(filename);
    std::string   buffer(4, ' ');

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    for (std::uint8_t col = 0; col < width; col++) {
        file.read(buffer.data(), 4);
        this->walls[0][col][Side::UP] = (buffer[2] == '%');
    }

    file.ignore(1000, '\n');

    for (std::uint8_t row = 0; row < height; row++) {
        file.ignore(1);

        for (std::uint8_t col = 0; col < width; col++) {
            file.read(buffer.data(), 4);
            this->walls[row][col][Side::LEFT] = (buffer[0] == '%');
            this->walls[row][col][Side::RIGHT] = (buffer[3] == '%');
        }

        file.ignore(1000, '\n');

        for (std::uint8_t col = 0; col < width; col++) {
            file.read(buffer.data(), 4);
            this->walls[row][col][Side::DOWN] = (buffer[2] == '%');

            if (row < height - 1) {
                this->walls[row + 1][col][Side::UP] = (buffer[2] == '%');
            }
        }

        file.ignore(1000, '\n');
    }
}

template <std::uint8_t width, std::uint8_t height>
std::ostream& operator<<(std::ostream& os, const Maze<width, height>& maze) {
    std::array<std::array<std::string, (width * 2) + 1>, (height * 2) + 1> drawing_array;

    for (std::uint8_t row = 0; row < height; row++) {
        for (std::uint8_t col = 0; col < width; col++) {
            drawing_array[2 * row][2 * col + 1] = maze.walls[row][col][Side::UP] ? "██" : "  ";
            drawing_array[2 * row + 1][2 * col + 2] = maze.walls[row][col][Side::RIGHT] ? "██" : "  ";
            drawing_array[2 * row + 2][2 * col + 1] = maze.walls[row][col][Side::DOWN] ? "██" : "  ";
            drawing_array[2 * row + 1][2 * col] = maze.walls[row][col][Side::LEFT] ? "██" : "  ";
        }
    }

    for (std::uint8_t row = 0; row <= height * 2; row++) {
        for (std::uint8_t col = 0; col <= width * 2; col++) {
            if ((row % 2 == 0) and (col % 2 == 0)) {
                drawing_array[row][col] = "██";
            } else if ((row % 2 == 1) and (col % 2 == 1)) {
                drawing_array[row][col] = "  ";
            }

            os << drawing_array[row][col];
        }

        os << '\n';
    }

    return os;
}

template <std::uint8_t width, std::uint8_t height>
std::array<bool, 3> Maze<width, height>::get_information(const Pose& pose) const {
    if (this->walls.at(pose.y).at(pose.x)[pose.orientation]) {
        return {false, true, false};
    }

    switch (pose.orientation) {
        case Side::LEFT:
            return {
                this->walls.at(pose.y).at(pose.x - 1)[Side::DOWN], false,
                this->walls.at(pose.y).at(pose.x - 1)[Side::UP]
            };
        case Side::UP:
            return {
                this->walls.at(pose.y - 1).at(pose.x)[Side::LEFT], false,
                this->walls.at(pose.y - 1).at(pose.x)[Side::RIGHT]
            };
        case Side::RIGHT:
            return {
                this->walls.at(pose.y).at(pose.x + 1)[Side::UP], false,
                this->walls.at(pose.y).at(pose.x + 1)[Side::DOWN]
            };
        case Side::DOWN:
            return {
                this->walls.at(pose.y + 1).at(pose.x)[Side::RIGHT], false,
                this->walls.at(pose.y + 1).at(pose.x)[Side::LEFT]
            };
    }

    return {false, false, false};
}

#endif  // MAZE_CPP
