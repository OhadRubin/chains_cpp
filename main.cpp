#include <iostream>
#include <cstdlib> // For getenv
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

    // --- Create and Use the Chain ---
    OpenAIMessageChain chain(api_key); // Uses default model "gpt-4o"
    
    if (chain.has_error()) {
        std::cerr << "Chain Error: " << chain.error_message() << std::endl;
        return 1;
    }

    auto result_chain = chain
                            .system("You are a helpful assistant that provides concise answers.")
                            .user("What is the capital of France?")
                            .generate_bot() // Generates response and adds it as 'assistant'
                            .user("And what about Germany?")
                            .generate(); // Just generates the final response

    if (result_chain.has_error()) {
        std::cerr << "Chain Error: " << result_chain.error_message() << std::endl;
        return 1;
    }

    // --- Print Results ---
    std::cout << "--- Conversation History ---" << std::endl;
    if (result_chain.system_prompt())
    {
        std::cout << "System: " << *result_chain.system_prompt() << std::endl;
    }
    for (const auto &msg : result_chain.messages())
    {
        // Add the assistant's final response which isn't part of the message list yet
        // This logic needs refinement - the last 'generate' doesn't add to messages_
        std::cout << (msg.role == "user" ? "User" : "Bot") << ": " << msg.content << std::endl;
    }
    // Manually print the *last* generated response if it exists and isn't already in messages_
    if (result_chain.last_response() && !result_chain.messages().empty() && result_chain.messages().back().role != "assistant")
    {
        std::cout << "Bot: " << *result_chain.last_response() << std::endl;
    }
    else if (result_chain.last_response() && result_chain.messages().empty() && result_chain.system_prompt())
    {
        std::cout << "Bot: " << *result_chain.last_response() << std::endl; // If only system + user + generate
    }

    std::cout << "\n--- Last Response ---" << std::endl;
    if (result_chain.last_response())
    {
        std::cout << *result_chain.last_response() << std::endl;
    }
    else
    {
        std::cout << "No response generated." << std::endl;
    }

    return 0;
} 