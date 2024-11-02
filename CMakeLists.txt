cmake_minimum_required(VERSION 3.28)
project(Aether)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set libraries
set(SDLIMAGE_BUILD_SHARED_LIBS ON)
set(SDLMIXER_BUILD_SHARED_LIBS ON)
set(SDL_SHARED ON)
set(SDL_STATIC OFF)
set(FLECS_STATIC OFF)
set(FLECS_SHARD ON)

if (NOT DEFINED CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -Wl,/DEBUG -Wl,/pdb:${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pdb")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Wl,/DEBUG -Wl,/pdb:${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pdb")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O1")
endif ()

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CatBirdRemake_SOURCE_DIR}/cmake")

# add_subdirectory(...)
# find_package()

find_package(stb)
find_package(imgui)

set( SOURCE_CODE
        src/main.cpp
        src/Core/Application.cpp
)

set( DEBUGGING_CODE

)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND SOURCE_CODE ${DEBUGGING_CODE})
endif ()

add_executable(Aether ${SOURCE_CODE})

install(TARGETS Aether
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(Aether PRIVATE
        inc
)

target_link_libraries(Aether PRIVATE
        # Shared

        # Static
        stb::stb
        imgui::imgui
)

#install(TARGETS Aether SDL3-shared SDL3_image-shared SDL3_mixer-shared
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)

add_custom_command(TARGET Aether POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/install/bin/res
)