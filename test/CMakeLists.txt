# --------------------------------------- #
#               CMAKE CONFIG              #
# --------------------------------------- #

# Define minimum CMake version required
cmake_minimum_required(VERSION 3.29.0)

project(Window VERSION 1.0.0 LANGUAGES C)


#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#

option(TEST_OPENGL "Use OpenGL renderer" OFF)
option(TEST_VULKAN "Use Vulkan renderer" OFF)
option(TEST_D3D12 "Use D3D12 renderer" ON)

if(TEST_OPENGL)
    list(APPEND BaboonWindow_TEST_SOURCES "glad/src/glad.c" "opengl_renderer.c")
elseif(TEST_VULKAN AND BABOON_VULKAN)
    list(APPEND BaboonWindow_TEST_SOURCES "vulkan_renderer.c")
elseif(TEST_D3D12 AND WIN32)
    list(APPEND BaboonWindow_TEST_SOURCES "d3d12_renderer.c")
else()
    message(FATAL_ERROR "No rendering API specified")
endif()

list(APPEND BaboonWindow_TEST_SOURCES "main.c")

add_executable(BaboonWindow_TEST ${BaboonWindow_TEST_SOURCES})



#---------------------------------------#
#                COMPILE MODE           #
#---------------------------------------#
target_compile_features(BaboonWindow_TEST PUBLIC c_std_17)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    target_compile_options(BaboonWindow_TEST PUBLIC /W4 /WX)
else()
    target_compile_options(BaboonWindow_TEST PUBLIC -Wall -Wextra -Wpedantic -Werror)
endif()



# Build position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C compiler extensions.
set(CMAKE_C_EXTENSIONS OFF)

target_link_libraries(BaboonWindow_TEST PRIVATE BaboonWindow)

if(WIN32)
target_link_libraries(BaboonWindow_TEST PRIVATE d3d12 dxgi)
endif()

target_include_directories(BaboonWindow_TEST PRIVATE "glad/include")
