cmake_minimum_required(VERSION 3.10)

# Set the project name
project(DataVisualization)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Qt5 package
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Find OpenGL and set the preference to GLVND
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# Find GLFW
find_package(glfw3 REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/OpenGLWidget.cpp
    src/OpenGLWidget2D.cpp
    src/DataLoader.cpp
    src/GraphWidget.cpp
    src/pbPlots.cpp
)

# Headers that require MOC
set(HEADERS
    include/MainWindow.h
    include/OpenGLWidget.h
    include/OpenGLWidget2D.h
    include/GraphWidget.h
    include/pbPlots.h
)

# Qt5 automatically handles MOC, but this makes it explicit
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SOURCES})

# Link the Qt5, OpenGL, GLEW, and GLFW libraries
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::OpenGL ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)
