#ifndef KNOWN_MAZE_CPP
#define KNOWN_MAZE_CPP

#include <format>
#include <iterator>
#include <queue>

#include "known_maze.hpp"

template <std::uint8_t width, std::uint8_t height>
KnownMaze<width, height>::KnownMaze(const GridPose& start, const GridPose& goal) : start(start), goal(goal) {
    for (std::uint8_t row = 0; row < height; row++) {
        this->cells[row][0].walls[Side::LEFT] = true;
        this->cells[row][width - 1].walls[Side::RIGHT] = true;
    }

    for (std::uint8_t col = 0; col < width; col++) {
        this->cells[0][col].walls[Side::UP] = true;
        this->cells[height - 1][col].walls[Side::DOWN] = true;
    }

    this->get_cell(goal.position).cost = 0;
    this->calculate_costmap();
}

template <std::uint8_t width, std::uint8_t height>
void KnownMaze<width, height>::update(const GridPose& pose, const std::array<bool, 3>& information) {
    if (pose.position == this->goal.position) {
        this->returning = true;
    } else if (pose == start.turned_back()) {
        this->exploring = false;
        this->returning = false;
    }

    if (not this->exploring) {
        return;
    }

    this->update_walls(pose, information[Side::UP]);
    GridPose front_pose = pose.front();

    if (not information[Side::UP]) {
        this->update_walls(front_pose.turned_left(), information[Side::LEFT]);
        this->update_walls(front_pose.turned_right(), information[Side::RIGHT]);
    }

    this->calculate_costmap();
}

template <std::uint8_t width, std::uint8_t height>
GridPose KnownMaze<width, height>::get_action(const GridPose& pose, bool force_costmap) const {
    const Cell&   current_cell = this->get_cell(pose.position);
    std::uint16_t current_cost = current_cell.cost;

    if (not force_costmap and (not this->exploring or this->returning) and this->best_route.contains(current_cost) and
        this->best_route.at(current_cost) == pose.position) {
        const auto& next_position = (this->returning ? std::prev(this->best_route.find(current_cost)) :
                                                       std::next(this->best_route.find(current_cost)))
                                        ->second;

        Side new_dir = pose.position.direction(next_position);

        if (pose.orientation != new_dir) {
            return {pose.position, pose.position.direction(next_position)};
        }

        return {next_position, pose.orientation};
    }

    GridPose next_pose = pose;

    for (std::uint8_t i = Side::LEFT; i <= Side::DOWN; i++) {
        Side      side = static_cast<Side>(i);
        GridPoint front_position = pose.position + side;

        if (not current_cell.walls[side] and this->get_cell(front_position).cost <= current_cost) {
            current_cost = this->get_cell(front_position).cost;

            if (pose.orientation == side) {
                next_pose = {front_position, side};
            } else {
                next_pose = {pose.position, side};
            }
        }
    }

    return next_pose;
}

template <std::uint8_t width, std::uint8_t height>
const KnownMaze<width, height>::Cell& KnownMaze<width, height>::get_cell(const GridPoint& position) const {
    return this->cells.at(position.y).at(position.x);
}

template <std::uint8_t width, std::uint8_t height>
KnownMaze<width, height>::Cell& KnownMaze<width, height>::get_cell(const GridPoint& position) {
    return this->cells.at(position.y).at(position.x);
}

template <std::uint8_t width, std::uint8_t height>
void KnownMaze<width, height>::update_walls(const GridPose& pose, bool wall) {
    try {
        this->get_cell(pose.position).walls[pose.orientation] = wall;
        this->get_cell(pose.position).seen[pose.orientation] = true;

        this->get_cell(pose.front().position).walls[pose.turned_back().orientation] = wall;
        this->get_cell(pose.front().position).seen[pose.turned_back().orientation] = true;
    } catch (const std::out_of_range& e) {
        return;
    }
}

template <std::uint8_t width, std::uint8_t height>
void KnownMaze<width, height>::calculate_costmap() {
    std::array<std::array<bool, width>, height> visited{};
    std::array<std::array<Side, width>, height> origin{};
    std::queue<GridPoint>                       queue;

    queue.push(this->goal.position);
    visited.at(this->goal.position.y).at(this->goal.position.x) = true;

    while (not queue.empty()) {
        GridPoint current_position = queue.front();
        queue.pop();

        Cell& current_cell = this->get_cell(current_position);

        for (std::uint8_t i = Side::LEFT; i <= Side::DOWN; i++) {
            Side      side = static_cast<Side>(i);
            GridPoint front_position = current_position + side;

            if (not current_cell.walls[side] and not visited.at(front_position.y).at(front_position.x)) {
                visited.at(front_position.y).at(front_position.x) = true;
                origin.at(front_position.y).at(front_position.x) = side;

                this->get_cell(front_position).cost =
                    (current_cell.cost + (current_position == this->goal.position or
                                                  (side == origin.at(current_position.y).at(current_position.x)) ?
                                              1 :
                                              2));
                queue.push(front_position);
            }
        }
    }

    if (not this->returning) {
        return;
    }

    GridPose current_pose = this->start;
    this->best_route.clear();
    this->best_route.try_emplace(this->get_cell(this->start.position).cost, this->start.position);

    while (current_pose.position != this->goal.position) {
        current_pose = this->get_action(current_pose, true);
        this->best_route.try_emplace(this->get_cell(current_pose.position).cost, current_pose.position);
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
                if (row / 2 == maze.start.position.y and col / 2 == maze.start.position.x) {
                    drawing_array[row][col] = "()";
                } else if (row / 2 == maze.goal.position.y and col / 2 == maze.goal.position.x) {
                    drawing_array[row][col] = "[]";
                } else {
                    drawing_array[row][col] = std::format("{:02}", maze.cells[row / 2][col / 2].cost);
                }
            }

            os << drawing_array[row][col];
        }

        os << '\n';
    }

    return os;
}

#endif  // KNOWN_MAZE_CPP
