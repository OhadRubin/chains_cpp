#include <iostream>
#include <cstdlib> // For getenv
#include <string>
#include "OpenAIMessageChain.h"

int main()
{
    // --- Get API Key ---
    const char *api_key_cstr = std::getenv("OPENAI_API_KEY");
    if (!api_key_cstr)
    {
        std::cerr << "Error: OPENAI_API_KEY environment variable not set." << std::endl;
        return 1;
    }
    std::string api_key(api_key_cstr);

    // --- Create the Chain ---
    OpenAIMessageChain chain(api_key); // Uses default model "gpt-4o"
    
    if (chain.has_error()) {
        std::cerr << "Chain Error: " << chain.error_message() << std::endl;
        return 1;
    }
    
    // Set system prompt
    chain = chain.system("You are a helpful assistant that provides concise answers.");
    
    std::cout << "Interactive ChatGPT CLI (Type 'exit' or 'quit' to end)\n";
    std::cout << "------------------------------------------------------\n";
    
    std::string user_input;
    
    while (true) {
        // Prompt for user input
        std::cout << "\nYou: ";
        std::getline(std::cin, user_input);
        
        // Check for exit command
        if (user_input == "exit" || user_input == "quit") {
            std::cout << "Goodbye!\n";
            break;
        }
        
        // Send message to the API
        chain = chain.user(user_input).generate_bot();
        
        // Check for errors
        if (chain.has_error()) {
            std::cerr << "Error: " << chain.error_message() << std::endl;
            continue;
        }
        
        // Display response
        if (chain.last_response()) {
            std::cout << "Bot: " << *chain.last_response() << std::endl;
        } else {
            std::cout << "Bot: No response generated." << std::endl;
        }
    }

    return 0;
}