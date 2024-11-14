cmake_minimum_required(VERSION 3.28)
project(Aether)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set libraries
set(SDLIMAGE_BUILD_SHARED_LIBS ON)
set(SDLMIXER_BUILD_SHARED_LIBS ON)
set(SDL_SHARED ON)
set(SDL_STATIC OFF)
set(FLECS_STATIC ON)
set(FLECS_SHARD OFF)

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
add_subdirectory(ext/sdl)
add_subdirectory(ext/sdl_image)
add_subdirectory(ext/sdl_mixer)
# find_package()

find_package(stb)
find_package(imgui)
find_package(Vulkan)
find_package(spdlog)
find_package(flecs)

set( SOURCE_CODE
        src/main.cpp
        src/Core/Application.cpp
        src/Core/Contexts/VulkanContext.cpp
        src/Core/ServiceLocator/ServiceLocator.cpp
        src/Core/Services/Swapchain.cpp
        src/Core/Render/Shader.cpp
        src/Core/Render/RenderProcess.cpp
        src/Core/Render/Renderer.cpp
        src/Core/Render/Buffer.cpp
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

target_compile_definitions( Aether PUBLIC
        -DIMGUI_IMPL_SDL3
        -DIMGUI_IMPL_VULKAN
)

target_include_directories(Aether PRIVATE
        inc
        ${SDL_INCLUDE_DIRS}
        ${SDLIMAGE_INCLUDE_DIRS}
        ${SDLMIXER_INCLUDE_DIRS}
)

target_link_libraries(Aether PRIVATE
        # Shared
        SDL3::SDL3-shared
        SDL3_image::SDL3_image-shared
        SDL3_mixer::SDL3_mixer-shared

        # Static
        stb::stb
        imgui::imgui
        Vulkan::Vulkan
        spdlog::spdlog
        flecs::flecs_static
)

install(TARGETS Aether SDL3-shared SDL3_image-shared SDL3_mixer-shared
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_custom_command(TARGET Aether POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/install/bin/res
)