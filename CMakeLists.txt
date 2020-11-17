cmake_minimum_required(VERSION 3.17)
project(connect4)

include_directories(${CMAKE_SOURCE_DIR} /usr/local/opt/ncurses/include /usr/local/opt/cdk/include)
link_directories(/usr/local/opt/ncurses/lib /usr/local/opt/cdk/lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-lmenuw -lncursesw -lformw -lcdk")

add_executable(connect4 main.cpp game.h board.h board.cpp game.cpp player.h mcts_player.h human_player.h
        mcts_player.cpp qlearn_player.h qlearn_player.cpp player.cpp ui.h ui.cpp view_controllers/base.cpp view_controllers/base.h view_controllers/main_menu.cpp view_controllers/main_menu.h view_controllers/game_options.cpp view_controllers/game_options.h ui_state.h view_controller_factory.h view_controller_factory.cpp)
target_link_libraries(connect4)