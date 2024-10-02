#include <iostream>

#include "maze.hpp"
#include "micras.hpp"

int main() {
    Maze<5, 5>   maze("/home/gabriel-cosme/Codes/Micras/MazeSolver/mazes/test2.txt");
    Micras<5, 5> micras{{0, 0, Side::UP}};

    std::cout << maze << '\n';
    std::cout << micras << '\n';

    while (true) {
        while (std::cin.get() != '\n') { }

        micras.step(maze.get_information(micras.get_pose()));
        std::cout << micras << '\n';
    }

    return 0;
}
