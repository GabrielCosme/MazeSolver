#ifndef KNOWN_MAZE_CPP
#define KNOWN_MAZE_CPP

#include <format>
#include <iterator>
#include <queue>

#include "known_maze.hpp"

template <uint8_t width, uint8_t height>
KnownMaze<width, height>::KnownMaze(const GridPose& start) :
    start(start),
    goal(
        {{width / 2, height / 2},
         {(width - 1) / 2, height / 2},
         {width / 2, (height - 1) / 2},
         {(width - 1) / 2, (height - 1) / 2}}
    ) {
    for (uint8_t row = 0; row < height; row++) {
        this->cells[row][0].wall_count[Side::LEFT] = 0xFFFF;
        this->cells[row][width - 1].wall_count[Side::RIGHT] = 0xFFFF;
    }

    for (uint8_t col = 0; col < width; col++) {
        this->cells[0][col].wall_count[Side::DOWN] = 0xFFFF;
        this->cells[height - 1][col].wall_count[Side::UP] = 0xFFFF;
    }

    for (const auto& position : this->goal) {
        this->get_cell(position).cost = 0;
    }

    this->calculate_costmap();
}

template <uint8_t width, uint8_t height>
void KnownMaze<width, height>::update(const GridPose& pose, Information information) {
    if (this->goal.contains(pose.position)) {
        this->returning = true;
    } else if (pose == start.turned_back()) {
        this->exploring = false;
        this->returning = false;
    }

    if (not this->exploring) {
        return;
    }

    if (information.left != Information::UNKNOWN) {
        this->update_wall(pose.turned_left(), information.left == Information::WALL);
    }

    if (information.front_left != Information::UNKNOWN) {
        this->update_wall(pose.front().turned_left(), information.front_left == Information::WALL);
    }

    if (information.front != Information::UNKNOWN) {
        this->update_wall(pose, information.front == Information::WALL);
    }

    if (information.front_right != Information::UNKNOWN) {
        this->update_wall(pose.front().turned_right(), information.front_right == Information::WALL);
    }

    if (information.right != Information::UNKNOWN) {
        this->update_wall(pose.turned_right(), information.right == Information::WALL);
    }

    this->calculate_costmap();
}

template <uint8_t width, uint8_t height>
GridPoint KnownMaze<width, height>::get_current_goal(const GridPoint& position, bool force_costmap) const {
    uint16_t current_cost = this->get_cell(position).cost;

    if (not force_costmap and (not this->exploring or this->returning) and this->best_route.contains(current_cost) and
        this->best_route.at(current_cost) == position) {
        return (this->returning ? std::prev(this->best_route.find(current_cost)) :
                                  std::next(this->best_route.find(current_cost)))
            ->second;
    }

    GridPoint next_position = position;

    for (uint8_t i = Side::RIGHT; i <= Side::DOWN; i++) {
        Side      side = static_cast<Side>(i);
        GridPoint front_position = position + side;

        if (not this->has_wall({position, side}) and this->get_cell(front_position).cost <= current_cost) {
            current_cost = this->get_cell(front_position).cost;
            next_position = front_position;
        }
    }

    return next_position;
}

template <uint8_t width, uint8_t height>
const KnownMaze<width, height>::Cell& KnownMaze<width, height>::get_cell(const GridPoint& position) const {
    return this->cells.at(position.y).at(position.x);
}

template <uint8_t width, uint8_t height>
KnownMaze<width, height>::Cell& KnownMaze<width, height>::get_cell(const GridPoint& position) {
    return this->cells.at(position.y).at(position.x);
}

template <uint8_t width, uint8_t height>
void KnownMaze<width, height>::update_wall(const GridPose& pose, bool wall) {
    if (wall) {
        this->get_cell(pose.position).wall_count[pose.orientation]++;
    } else {
        this->get_cell(pose.position).free_count[pose.orientation]++;
    }

    GridPose front_pose = pose.front();

    if (front_pose.position.x >= width or front_pose.position.y >= height) {
        return;
    }

    if (wall) {
        this->get_cell(front_pose.position).wall_count[pose.turned_back().orientation]++;
    } else {
        this->get_cell(front_pose.position).free_count[pose.turned_back().orientation]++;
    }
}

template <uint8_t width, uint8_t height>
bool KnownMaze<width, height>::has_wall(const GridPose& pose) const {
    return this->get_cell(pose.position).wall_count[pose.orientation] >
           this->get_cell(pose.position).free_count[pose.orientation];
}

template <uint8_t width, uint8_t height>
void KnownMaze<width, height>::calculate_costmap() {
    std::array<std::array<bool, width>, height> visited{};
    std::array<std::array<Side, width>, height> origin{};
    std::queue<GridPoint>                       queue;

    for (const auto& position : this->goal) {
        queue.push(position);
        visited.at(position.y).at(position.x) = true;
    }

    while (not queue.empty()) {
        GridPoint current_position = queue.front();
        queue.pop();

        const Cell& current_cell = this->get_cell(current_position);

        for (uint8_t i = Side::RIGHT; i <= Side::DOWN; i++) {
            Side      side = static_cast<Side>(i);
            GridPoint front_position = current_position + side;

            if (not this->has_wall({current_position, side}) and
                not visited.at(front_position.y).at(front_position.x)) {
                visited.at(front_position.y).at(front_position.x) = true;
                origin.at(front_position.y).at(front_position.x) = side;

                this->get_cell(front_position).cost =
                    (current_cell.cost + (this->goal.contains(current_position) or
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

    GridPoint current_position = this->start.position;
    this->best_route.clear();
    this->best_route.try_emplace(this->get_cell(this->start.position).cost, this->start.position);

    while (not this->goal.contains(current_position)) {
        current_position = this->get_current_goal(current_position, true);
        this->best_route.try_emplace(this->get_cell(current_position).cost, current_position);
    }
}

template <std::uint8_t width, std::uint8_t height>
std::ostream& operator<<(std::ostream& os, const KnownMaze<width, height>& maze) {
    std::array<std::array<std::string, (width * 2) + 1>, (height * 2) + 1> drawing_array;

    for (std::uint8_t row = 0; row < height; row++) {
        for (std::uint8_t col = 0; col < width; col++) {
            drawing_array[2 * row + 2][2 * col + 1] = maze.has_wall({{col, row}, Side::UP}) ? "%%" : "  ";
            drawing_array[2 * row + 1][2 * col + 2] = maze.has_wall({{col, row}, Side::RIGHT}) ? "%%" : "  ";
            drawing_array[2 * row][2 * col + 1] = maze.has_wall({{col, row}, Side::DOWN}) ? "%%" : "  ";
            drawing_array[2 * row + 1][2 * col] = maze.has_wall({{col, row}, Side::LEFT}) ? "%%" : "  ";
        }
    }

    for (std::int8_t row = height * 2; row >= 0; row--) {
        for (std::uint8_t col = 0; col <= width * 2; col++) {
            if ((row % 2 == 0) and (col % 2 == 0)) {
                drawing_array[row][col] = "%%";
            } else if ((row % 2 == 1) and (col % 2 == 1)) {
                if (row / 2 == maze.start.position.y and col / 2 == maze.start.position.x) {
                    drawing_array[row][col] = "()";
                } else if (maze.goal.contains({static_cast<uint8_t>(col / 2), static_cast<uint8_t>(row / 2)})) {
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
