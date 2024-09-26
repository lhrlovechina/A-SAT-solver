cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(SudokuSolver VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(src)

# 添加源文件
set(SOURCES
    src/main.cpp
    src/solver.cpp
)

# 添加可执行文件
add_executable(SudokuSolver ${SOURCES})

# 链接ph.h库
target_include_directories(SudokuSolver PRIVATE src)
