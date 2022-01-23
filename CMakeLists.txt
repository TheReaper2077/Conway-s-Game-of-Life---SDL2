cmake_minimum_required(VERSION 3.0.0)
project(new_project VERSION 0.1.0)
include(CTest)
enable_testing()

file(GLOB TARGET_SRC "./src/*.cpp")

add_executable(main ${TARGET_SRC})

#---------------------------------------------------------------------------#	// Library Imports Here

include_directories("./dependencies/SDL2/include" ".")

ADD_LIBRARY(SDL2 STATIC IMPORTED)
SET_TARGET_PROPERTIES(SDL2 PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/SDL2/lib/libSDL2.dll.a")
ADD_LIBRARY(SDL2main STATIC IMPORTED)
SET_TARGET_PROPERTIES(SDL2main PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/SDL2/lib/libSDL2main.a")
add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/dependencies/SDL2/bin/ ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SDL2/bin/*.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(main PUBLIC mingw32 SDL2main SDL2)

#---------------------------------------------------------------------------#

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
