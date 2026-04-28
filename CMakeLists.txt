cmake_minimum_required(VERSION 3.16)
project(NumericalRootFinder VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/GraphWidget.cpp
    src/CRTOverlay.cpp
    src/FunctionParser.cpp
    src/BisectionSolver.cpp
    src/NewtonSolver.cpp
    src/SecantSolver.cpp
)

set(HEADERS
    include/MainWindow.h
    include/GraphWidget.h
    include/CRTOverlay.h
    include/FunctionParser.h
    include/BisectionSolver.h
    include/NewtonSolver.h
    include/SecantSolver.h
    include/IterationRecord.h
)

qt_add_executable(NumericalRootFinder ${SOURCES} ${HEADERS})

target_include_directories(NumericalRootFinder PRIVATE include)

target_link_libraries(NumericalRootFinder PRIVATE Qt6::Widgets)

target_compile_options(NumericalRootFinder PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
