# chains_cpp

A C++ implementation of the OpenAI API client with a chain-based interface.

## Requirements
- C++17 compatible compiler
- CMake 3.14 or higher
- OpenSSL

## Building with CMake

```bash
# Build the project
./build.sh

# Or manually:
mkdir -p build
cd build
cmake ..
cmake --build .
```

## Running the application

```bash
# Make sure OPENAI_API_KEY is set
export OPENAI_API_KEY=your_api_key_here

# Run the single-example application
./build/oai_chain

# Run the interactive CLI application
./build/oai_chat
```

## Available Executables

- **oai_chain**: Runs a simple example that asks two questions and prints the conversation history.
- **oai_chat**: Launches an interactive CLI where you can chat with the AI model.

## Legacy build method

The original shell script is still available:
```bash
./compile.sh
```
