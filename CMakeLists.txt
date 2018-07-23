cmake_minimum_required(VERSION 3.10)

project(DMMRR) # Dota MMR Recorder

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find tesseract
find_package(Tesseract CONFIG REQUIRED)

# create DMMRR.exe
add_executable(${PROJECT_NAME} WIN32 "include/main.cpp" "include/Recorder.cpp" "include/Recorder.hpp")

# add clip
set(CLIP_DIR "${CMAKE_HOME_DIRECTORY}/external/clip")
set(CLIP_EXAMPLES OFF CACHE INTERNAL "Compile clip examples")
set(CLIP_TESTS OFF CACHE INTERNAL "Compile clip tests")
add_subdirectory(${CLIP_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${Tesseract_INCLUDE_DIRS} ${CMAKE_HOME_DIRECTORY}/external)
target_link_libraries(${PROJECT_NAME} PRIVATE libtesseract clip)

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin"
)