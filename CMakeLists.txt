cmake_minimum_required(VERSION 3.17)
project(connect4)

include_directories(/usr/local/opt/ncurses/include)
link_directories(/usr/local/opt/ncurses/lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-lmenuw -lncursesw")

add_executable(connect4 main.cpp game.h board.h board.cpp game.cpp player.h mcts_player.h human_player.h mcts_player.cpp qlearn_player.h qlearn_player.cpp player.cpp ui.h ui.cpp)
