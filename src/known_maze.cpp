#ifndef KNOWN_MAZE_CPP
#define KNOWN_MAZE_CPP

#include <format>
#include <queue>

#include "known_maze.hpp"

template <std::uint8_t width, std::uint8_t height>
KnownMaze<width, height>::KnownMaze(const Pose& start, const Pose& goal) : start(start), goal(goal) {
    for (std::uint8_t row = 0; row < height; row++) {
        this->cells[row][0].walls[Side::LEFT] = true;
        this->cells[row][width - 1].walls[Side::RIGHT] = true;
    }

    for (std::uint8_t col = 0; col < width; col++) {
        this->cells[0][col].walls[Side::UP] = true;
        this->cells[height - 1][col].walls[Side::DOWN] = true;
    }

    this->get_cell(goal).cost = 0;
    this->calculate_costmap();
}

template <std::uint8_t width, std::uint8_t height>
void KnownMaze<width, height>::update(const Pose& pose, const std::array<bool, 3>& information) {
    if (pose.x == this->goal.x and pose.y == this->goal.y) {
        this->returning = true;
    } else if (pose == start.turned_back()) {
        this->exploring = false;
        this->returning = false;
    }

    if (not this->exploring) {
        return;
    }

    this->update_walls(pose, information[Side::UP]);
    Pose front_cell = pose.front();

    if (not information[Side::UP]) {
        this->update_walls(front_cell.turned_left(), information[Side::LEFT]);
        this->update_walls(front_cell.turned_right(), information[Side::RIGHT]);
    }

    this->calculate_costmap();
}

template <std::uint8_t width, std::uint8_t height>
Pose KnownMaze<width, height>::get_action(const Pose& pose) const {
    const Cell&   current_cell = this->get_cell(pose);
    std::uint16_t smallest_cost = current_cell.cost;
    Pose          next_pose = pose;

    for (std::uint8_t i = Side::LEFT; i <= Side::DOWN; i++) {
        Side side = static_cast<Side>(i);
        Pose front_cell = pose + side;

        if (not current_cell.walls[side] and this->get_cell(front_cell).cost < smallest_cost) {
            smallest_cost = this->get_cell(front_cell).cost;

            if (pose.orientation == side) {
                next_pose = front_cell;
            } else {
                next_pose = {pose.x, pose.y, side};
            }
        }
    }

    return next_pose;
}

template <std::uint8_t width, std::uint8_t height>
const KnownMaze<width, height>::Cell& KnownMaze<width, height>::get_cell(const Pose& pose) const {
    return this->cells.at(pose.y).at(pose.x);
}

template <std::uint8_t width, std::uint8_t height>
KnownMaze<width, height>::Cell& KnownMaze<width, height>::get_cell(const Pose& pose) {
    return this->cells.at(pose.y).at(pose.x);
}

template <std::uint8_t width, std::uint8_t height>
void KnownMaze<width, height>::update_walls(const Pose& pose, bool wall) {
    try {
        this->get_cell(pose).walls[pose.orientation] = wall;
        this->get_cell(pose).seen[pose.orientation] = true;

        this->get_cell(pose.front()).walls[pose.turned_back().orientation] = wall;
        this->get_cell(pose.front()).seen[pose.turned_back().orientation] = true;
    } catch (const std::out_of_range& e) {
        return;
    }
}

template <std::uint8_t width, std::uint8_t height>
void KnownMaze<width, height>::calculate_costmap() {
    std::array<std::array<bool, width>, height> visited{};
    std::queue<Pose>                            queue;

    queue.push(this->goal);
    visited.at(this->goal.y).at(this->goal.x) = true;

    while (not queue.empty()) {
        Pose current_pose = queue.front();
        queue.pop();

        Cell& current_cell = this->get_cell(current_pose);

        for (std::uint8_t i = Side::LEFT; i <= Side::DOWN; i++) {
            Side side = static_cast<Side>(i);
            Pose front_cell = current_pose + side;

            if (not current_cell.walls[side] and not visited.at(front_cell.y).at(front_cell.x)) {
                this->get_cell(front_cell).cost = current_cell.cost + 1;
                visited.at(front_cell.y).at(front_cell.x) = true;
                queue.push(front_cell);
            }
        }
    }
}

template <std::uint8_t width, std::uint8_t height>
std::ostream& operator<<(std::ostream& os, const KnownMaze<width, height>& maze) {
    std::array<std::array<std::string, (width * 2) + 1>, (height * 2) + 1> drawing_array;

    for (std::uint8_t row = 0; row < height; row++) {
        for (std::uint8_t col = 0; col < width; col++) {
            drawing_array[2 * row][2 * col + 1] = maze.cells[row][col].walls[Side::UP] ? "%%" : "  ";
            drawing_array[2 * row + 1][2 * col + 2] = maze.cells[row][col].walls[Side::RIGHT] ? "%%" : "  ";
            drawing_array[2 * row + 2][2 * col + 1] = maze.cells[row][col].walls[Side::DOWN] ? "%%" : "  ";
            drawing_array[2 * row + 1][2 * col] = maze.cells[row][col].walls[Side::LEFT] ? "%%" : "  ";
        }
    }

    for (std::uint8_t row = 0; row <= height * 2; row++) {
        for (std::uint8_t col = 0; col <= width * 2; col++) {
            if ((row % 2 == 0) and (col % 2 == 0)) {
                drawing_array[row][col] = "%%";
            } else if ((row % 2 == 1) and (col % 2 == 1)) {
                drawing_array[row][col] = std::format("{:02}", maze.cells[row / 2][col / 2].cost);
            }

            os << drawing_array[row][col];
        }

        os << '\n';
    }

    return os;
}

#endif  // KNOWN_MAZE_CPP
