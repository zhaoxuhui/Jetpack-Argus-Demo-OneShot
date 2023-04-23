cmake_minimum_required (VERSION 2.6)

project(argus_oneshot_demo)

# 将Argus库相关的头文件放到了该文件夹中
set(CMAKE_SUPPORT_PATH "${CMAKE_SOURCE_DIR}/libargus_support")
message("${CMAKE_SUPPORT_PATH}")

# 将cmake相关的文件放到该文件夹中
set(CMAKE_MODULE_PATH "${CMAKE_SUPPORT_PATH}/cmake")
message("${CMAKE_MODULE_PATH}")

# 将头文件相关的文件放到该文件夹中
set(CMAKE_INCLUDE_PATH "${CMAKE_SUPPORT_PATH}/include")
message("${CMAKE_INCLUDE_PATH}")

# 查找Argus库并包含头文件目录
find_package(Argus REQUIRED)
include_directories(${ARGUS_INCLUDE_DIR})

add_executable(demo_oneshot_main_all main-all.cpp)
target_link_libraries(demo_oneshot_main_all ${ARGUS_LIBRARIES})

add_executable(demo_oneshot_main_sim main-sim.cpp)
target_link_libraries(demo_oneshot_main_sim ${ARGUS_LIBRARIES})
