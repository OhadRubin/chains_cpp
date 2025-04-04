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
    echo "Build successful! Executable created as 'build/oai_chain'"
    echo "Run with: ./build/oai_chain"
else
    echo "Build failed"
fi 