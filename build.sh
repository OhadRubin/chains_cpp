#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Executables created as 'build/oai_chain' and 'build/oai_chat'"
    echo "Run chain example with: ./build/oai_chain"
    echo "Run interactive chat with: ./build/oai_chat"
else
    echo "Compilation failed"
fi 