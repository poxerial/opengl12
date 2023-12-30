cmake_minimum_required(VERSION 3.0)

project(OpenglExample)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

function(buildGL file_name)
    add_executable(${file_name} ${file_name}.cpp)
    target_include_directories(${file_name} PRIVATE
        ${OpenGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS})
    target_link_libraries(${file_name} OpenGL::GL GLUT::GLUT)
endfunction()

foreach(i RANGE 1 12)
    buildGL(${i})
endforeach()


