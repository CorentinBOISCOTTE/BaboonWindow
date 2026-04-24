# --------------------------------------- #
#               CMAKE CONFIG              #
# --------------------------------------- #

# Define minimum CMake version required
cmake_minimum_required(VERSION 3.29.0)



project(Window VERSION 1.0.0 LANGUAGES C)


#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#

add_executable(BaboonWindow_TEST "main.c" "glad/src/glad.c")



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

target_include_directories(BaboonWindow_TEST PRIVATE "glad/include")
