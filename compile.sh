#!/bin/bash

# Check if json.hpp exists, if not download it
if [ ! -f include/json.hpp ]; then
    echo "Downloading json.hpp..."
    mkdir -p include
    curl -o include/json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
else
    echo "json.hpp already exists, skipping download"
fi
# Check if httplib.h exists, if not download it
if [ ! -f include/httplib.h ]; then
    echo "Downloading httplib.h..."
    curl -o include/httplib.h https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
else
    echo "httplib.h already exists, skipping download"
fi
# Get OpenSSL path from homebrew
OPENSSL_PATH=$(brew --prefix openssl)

# Compile the OpenAI API client with all source files
g++ -std=c++17 -Wall -Wextra -pedantic chat.cpp OpenAIMessageChain.cpp -o oai_chain \
    -I. -I"$OPENSSL_PATH/include" \
    -L"$OPENSSL_PATH/lib" -lssl -lcrypto

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Executable created as 'oai_chain'"
    echo "Run with: ./oai_chain"
else
    echo "Compilation failed"
fi