#---------------------------------------#
#			SETUP					#
#---------------------------------------#
cmake_minimum_required(VERSION 3.29.0 FATAL_ERROR)

project(BaboonWindow VERSION 1.0.0 LANGUAGES C)

# Output directories
set(OUTPUT_DIR_BASE "${CMAKE_SOURCE_DIR}/builds/${CMAKE_C_COMPILER_ID}")
message(STATUS "[${PROJECT_NAME}]: Base output directory: ${OUTPUT_DIR_BASE}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR_BASE}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR_BASE}/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR_BASE}/$<CONFIG>")

option(BABOON_VULKAN "Link vulkan" ON)

if (BABOON_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

#---------------------------------------#
#			SOURCES					    #
#---------------------------------------#
list(APPEND BaboonWindow_SOURCES "src/baboon_internal.c")

# Platform-specific sources
if(WIN32)
    list(APPEND BaboonWindow_SOURCES "src/baboon_win32/win32_window.c")
elseif(UNIX AND NOT APPLE)
    # Linux support coming later
    message(STATUS "Linux support not yet implemented")
    list(APPEND BaboonWindow_SOURCES "src/baboon_linux/linux_window.c")
endif()

if (BABOON_VULKAN)
    list(APPEND BaboonWindow_SOURCES "src/baboon_vulkan/baboon_window_vulkan.c")
endif()

#---------------------------------------#
#			BUILD MODE  			    #
#---------------------------------------#
option(BaboonWindow_BUILD_STATIC "Build static libraries" ON)

if(BaboonWindow_BUILD_STATIC)
    add_library(BaboonWindow STATIC ${BaboonWindow_SOURCES})
else()
    add_library(BaboonWindow SHARED ${BaboonWindow_SOURCES})
endif()

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(BaboonWindow PUBLIC "include")

#---------------------------------------#
#                COMPILE MODE           #
#---------------------------------------#
target_compile_features(BaboonWindow PUBLIC c_std_17)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    target_compile_options(BaboonWindow PUBLIC /W4 /WX)
else()
    target_compile_options(BaboonWindow PUBLIC -Wall -Wextra -Wpedantic -Werror)
endif()

# Build position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C compiler extensions
set(CMAKE_C_EXTENSIONS OFF)

if (BABOON_VULKAN)
    target_compile_definitions(BaboonWindow PUBLIC BABOON_VULKAN)
    target_link_libraries(BaboonWindow PUBLIC Vulkan::Vulkan)
endif()

# Link Windows libraries
if(WIN32)
    target_link_libraries(BaboonWindow PUBLIC user32 gdi32)
endif()

target_link_libraries(BaboonWindow PUBLIC opengl32)

#---------------------------------------#
#        EXPORTS / IMPORTS FOR LIB      #
#---------------------------------------#
set_target_properties(BaboonWindow PROPERTIES PREFIX "")
set_target_properties(BaboonWindow PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

#---------------------------------------#
#              ENABLE TESTING           #
#---------------------------------------#
enable_testing()

option(BUILD_EXECUTABLE "Build an executable to live debug the window" ON)

if(BUILD_EXECUTABLE)
    add_subdirectory(test)
endif()