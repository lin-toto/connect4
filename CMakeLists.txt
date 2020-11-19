cmake_minimum_required(VERSION 3.17)
project(connect4)

include_directories(${CMAKE_SOURCE_DIR} /usr/local/opt/ncurses/include /usr/local/opt/cdk/include)
link_directories(/usr/local/opt/ncurses/lib /usr/local/opt/cdk/lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-lmenuw -lncursesw -lformw -lcdk")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(connect4 main.cpp game.h board.h board.cpp game.cpp players/base_player.h players/mcts_player.h players/human_player.h
        players/mcts_player.cpp players/qlearn_player.h players/qlearn_player.cpp ui.h ui.cpp view_controllers/base_view_controller.cpp view_controllers/base_view_controller.h view_controllers/main_menu.cpp view_controllers/main_menu.h view_controllers/game_options.cpp view_controllers/game_options.h ui_state.h view_controller_factory.h view_controller_factory.cpp view_controllers/game_play.cpp view_controllers/game_play.h player_factory.cpp player_factory.h view_controllers/credits.cpp view_controllers/credits.h)
target_link_libraries(connect4)