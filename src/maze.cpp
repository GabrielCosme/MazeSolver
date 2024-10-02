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
        this->walls[height - 1][col][Side::UP] = (buffer[2] == '%');
    }

    file.ignore(1000, '\n');

    for (std::int8_t row = height - 1; row >= 0; row--) {
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

            if (row > 0) {
                this->walls[row - 1][col][Side::UP] = (buffer[2] == '%');
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
Information Maze<width, height>::get_information(const GridPose& pose) const {
    Information information{};

    if (this->walls.at(pose.position.y).at(pose.position.x)[pose.orientation]) {
        information.front = Information::WALL;
        return information;
    }

    information.front = Information::FREE;

    switch (pose.orientation) {
        case Side::RIGHT:
            information.front_left = this->walls.at(pose.position.y).at(pose.position.x + 1)[Side::UP] ?
                                         Information::WALL :
                                         Information::FREE;

            information.front_right = this->walls.at(pose.position.y).at(pose.position.x + 1)[Side::DOWN] ?
                                          Information::WALL :
                                          Information::FREE;
            return information;

        case Side::UP:
            information.front_left = this->walls.at(pose.position.y + 1).at(pose.position.x)[Side::LEFT] ?
                                         Information::WALL :
                                         Information::FREE;

            information.front_right = this->walls.at(pose.position.y + 1).at(pose.position.x)[Side::RIGHT] ?
                                          Information::WALL :
                                          Information::FREE;
            return information;

        case Side::LEFT:
            information.front_left = this->walls.at(pose.position.y).at(pose.position.x - 1)[Side::DOWN] ?
                                         Information::WALL :
                                         Information::FREE;

            information.front_right = this->walls.at(pose.position.y).at(pose.position.x - 1)[Side::UP] ?
                                          Information::WALL :
                                          Information::FREE;
            return information;

        case Side::DOWN:
            information.front_left = this->walls.at(pose.position.y - 1).at(pose.position.x)[Side::RIGHT] ?
                                         Information::WALL :
                                         Information::FREE;

            information.front_right = this->walls.at(pose.position.y - 1).at(pose.position.x)[Side::LEFT] ?
                                          Information::WALL :
                                          Information::FREE;
            return information;
    }

    return information;
}

#endif  // MAZE_CPP
