cmake_minimum_required(VERSION 3.20)
project(ofl-cpp CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ofl-cpp STATIC
        src/lexer.cpp
        src/parser.cpp
        include/ofl/nodes.h
        include/ofl/resolver.h
        src/resolver.cpp
        include/ofl/types.h
        include/ofl/validator.h
        src/validator.cpp
        src/builder.cpp
        include/ofl/builder.h
        include/ofl/writer.h
        src/writer.cpp
)
target_include_directories(ofl-cpp PUBLIC include)

add_executable(ofl_test tests/manual_tests.cpp
        src/lexer.cpp
        src/parser.cpp
        include/ofl/nodes.h
        include/ofl/resolver.h
        src/resolver.cpp
        include/ofl/types.h
        include/ofl/validator.h
        src/validator.cpp
        src/builder.cpp
        include/ofl/builder.h
        include/ofl/writer.h
        src/writer.cpp)
target_link_libraries(ofl_test PRIVATE ofl-cpp)