cmake_minimum_required(VERSION 3.10)

# Set the project name
project(DataVisualization)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the Qt5 package
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Find the OpenGL package
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/OpenGLWidget.cpp
    src/DataLoader.cpp
)

# Headers that require MOC
set(HEADERS
    include/MainWindow.h
    include/OpenGLWidget.h
)

# Qt5 automatically handles MOC, but this makes it explicit
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SOURCES})

# Link the Qt5 and OpenGL libraries
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::OpenGL ${OPENGL_LIBRARIES})
