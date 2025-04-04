cmake_minimum_required(VERSION 3.14)
project(oai_chain VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warning flags
add_compile_options(-Wall -Wextra -pedantic)

# Find OpenSSL package
find_package(OpenSSL REQUIRED)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Download dependencies if needed (similar to compile.sh script)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/json.hpp)
    message(STATUS "Downloading json.hpp...")
    file(DOWNLOAD
        https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/json.hpp
        SHOW_PROGRESS
    )
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/httplib.h)
    message(STATUS "Downloading httplib.h...")
    file(DOWNLOAD
        https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/httplib.h
        SHOW_PROGRESS
    )
endif()

# Common source files (excluding main files)
set(COMMON_SOURCES
    OpenAIMessageChain.cpp
)

# Create executable for main.cpp
add_executable(oai_chain main.cpp ${COMMON_SOURCES})
target_link_libraries(oai_chain PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Create executable for chat.cpp (interactive CLI)
add_executable(oai_chat chat.cpp ${COMMON_SOURCES})
target_link_libraries(oai_chat PRIVATE OpenSSL::SSL OpenSSL::Crypto) 