cmake_minimum_required(VERSION 3.13)
set(THREADS_PREFER_PTHREAD_FLAG ON)

project(cedurs-encoder-v4l2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstack-usage=1000")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstack-usage=1000")

execute_process(COMMAND bash "thirdpart/setup.sh" ${CMAKE_C_COMPILER} arm
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

add_compile_definitions(DEBUG)

set(LIBAVLOCATION ${CMAKE_CURRENT_SOURCE_DIR}/build-install/usr/local/lib/)
set(LIBAVINCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/build-install/usr/local/include)

link_directories(${LIBAVLOCATION})
include_directories(${LIBAVINCLUDE})

add_executable(v4l2-cedurs main.c)
target_link_libraries(v4l2-cedurs ${LIBAVLOCATION}/libavcodec.a)
target_link_libraries(v4l2-cedurs ${LIBAVLOCATION}/libavformat.a)
target_link_libraries(v4l2-cedurs ${LIBAVLOCATION}/libavutil.a)
target_link_libraries(v4l2-cedurs ${LIBAVLOCATION}/libavdevice.a)
target_link_libraries(v4l2-cedurs ${LIBAVLOCATION}/libswscale.a)
