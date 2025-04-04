#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Message.h"

// The main chain class, designed for immutable chaining
class OpenAIMessageChain
{
public:
    // Constructor
    OpenAIMessageChain(std::string api_key, std::string model_name = "gpt-4o");

    // Chainable Methods (return new instances)
    [[nodiscard]] OpenAIMessageChain system(const std::string &content) const;
    [[nodiscard]] OpenAIMessageChain user(const std::string &content) const;
    [[nodiscard]] OpenAIMessageChain bot(const std::string &content) const;
    [[nodiscard]] OpenAIMessageChain generate();
    [[nodiscard]] OpenAIMessageChain generate_bot();

    // Getters
    const std::optional<std::string> &last_response() const;
    const std::vector<Message> &messages() const;
    const std::optional<std::string> &system_prompt() const;
    const std::string &model_name() const;
    bool has_error() const;
    const std::string &error_message() const;

private:
    // Private constructor for internal copying/modification
    OpenAIMessageChain(
        std::string api_key,
        std::string model_name,
        std::optional<std::string> system_prompt,
        std::vector<Message> messages,
        std::optional<std::string> last_response,
        bool has_error,
        std::string error_message);

    // Helper to add messages immutably
    [[nodiscard]] OpenAIMessageChain add_message(const std::string &content, const std::string &role) const;
    
    // Error handling method
    void set_error(const std::string &message);

    // Helper for the actual API call
    std::string call_openai_api();

    // State variables
    std::string api_key_;
    std::string model_name_;
    std::optional<std::string> system_prompt_;
    std::vector<Message> messages_;
    std::optional<std::string> last_response_;
    bool has_error_;
    std::string error_message_;
}; 