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

# Run the single-example application (uses api.openai.com by default)
./build/oai_chain

# Run with a custom API endpoint
./build/oai_chain localhost:11434

# Run the interactive CLI application
./build/oai_chat

# Run the interactive CLI with a custom API endpoint
./build/oai_chat localhost:11434
```

## Available Executables

- **oai_chain**: Runs a simple example that asks two questions and prints the conversation history.
- **oai_chat**: Launches an interactive CLI where you can chat with the AI model.

## Command Line Arguments

Both executables accept an optional command line argument to specify the API endpoint:

```bash
./build/oai_chain [api_endpoint]
./build/oai_chat [api_endpoint]
```

Examples:
- Use OpenAI API: `./build/oai_chain api.openai.com` (default)
- Use local Ollama: `./build/oai_chain localhost:11434`
- Use another API provider: `./build/oai_chain api.provider.com`

## Legacy build method

The original shell script is still available:
```bash
./compile.sh
```
