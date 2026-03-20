cmake_minimum_required(VERSION 3.16)

project(PancakeAndRisk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pancake_risk
        src/main.cpp
        src/Game.cpp
        src/Player.cpp
        src/FoodService.cpp
        src/Casino.cpp
        src/Event.cpp
        src/DecisionCard.cpp
        src/Upgrade.cpp
        src/Achievement.cpp
)

target_include_directories(pancake_risk PRIVATE src)
