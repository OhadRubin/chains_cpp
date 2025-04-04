#include "OpenAIMessageChain.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <utility>

// External Libraries
#include "include/json.hpp"
#define CPPHTTPLIB_OPENSSL_SUPPORT // Enable SSL support *before* including httplib.h
#include "include/httplib.h"

using json = nlohmann::json;

// Constructor
OpenAIMessageChain::OpenAIMessageChain(std::string api_key, std::string model_name, std::string api_endpoint)
    : api_key_(std::move(api_key)),
        model_name_(std::move(model_name)),
        api_endpoint_(std::move(api_endpoint)),
        system_prompt_(std::nullopt),
        messages_(),
        last_response_(std::nullopt),
        has_error_(false),
        error_message_("")
{
    if (api_key_.empty())
    {
        set_error("API key cannot be empty.");
    }
}

// Private constructor for internal copying/modification
OpenAIMessageChain::OpenAIMessageChain(
    std::string api_key,
    std::string model_name,
    std::string api_endpoint,
    std::optional<std::string> system_prompt,
    std::vector<Message> messages,
    std::optional<std::string> last_response,
    bool has_error,
    std::string error_message)
    : api_key_(std::move(api_key)),
        model_name_(std::move(model_name)),
        api_endpoint_(std::move(api_endpoint)),
        system_prompt_(std::move(system_prompt)),
        messages_(std::move(messages)),
        last_response_(std::move(last_response)),
        has_error_(has_error),
        error_message_(std::move(error_message)) {}

// Chainable Methods
OpenAIMessageChain OpenAIMessageChain::system(const std::string &content) const
{
    OpenAIMessageChain next_chain = *this; // Copy current state
    next_chain.system_prompt_ = content;
    return next_chain; // Return the modified copy
}

OpenAIMessageChain OpenAIMessageChain::user(const std::string &content) const
{
    return add_message(content, "user");
}

OpenAIMessageChain OpenAIMessageChain::bot(const std::string &content) const
{
    return add_message(content, "assistant");
}

OpenAIMessageChain OpenAIMessageChain::generate()
{
    OpenAIMessageChain next_chain = *this;
    
    if (has_error_) {
        return next_chain;
    }
    
    std::string response_body = call_openai_api();
    if (has_error_) {
        return next_chain;
    }

    // Parse the response
    std::string generated_content;
    
    json parsed_response = json::parse(response_body, nullptr, false);
    if (parsed_response.is_discarded()) {
        set_error("Failed to parse response");
        return next_chain;
    }

    if (parsed_response.contains("error"))
    {
        set_error("OpenAI API Error");
        return next_chain;
    }

    if (parsed_response.contains("choices") && parsed_response["choices"].is_array() && !parsed_response["choices"].empty())
    {
        const auto &first_choice = parsed_response["choices"][0];
        if (first_choice.contains("message") && first_choice["message"].contains("content"))
        {
            generated_content = first_choice["message"]["content"].get<std::string>();
        }
        else
        {
            set_error("Missing message content");
            return next_chain;
        }
    }
    else
    {
        set_error("Invalid response format");
        return next_chain;
    }

    // Create a new chain state with the results
    next_chain.last_response_ = generated_content;
    return next_chain;
}

OpenAIMessageChain OpenAIMessageChain::generate_bot()
{
    OpenAIMessageChain result_chain = this->generate(); // Generate response
    
    if (result_chain.has_error_) {
        return result_chain;
    }
    
    if (result_chain.last_response_)
    {
        // Add the generated response as a bot message to the *new* chain state
        return result_chain.bot(*result_chain.last_response_);
    }
    else
    {
        result_chain.set_error("No response generated");
        return result_chain;
    }
}

// Getters
const std::optional<std::string> &OpenAIMessageChain::last_response() const { return last_response_; }

const std::vector<Message> &OpenAIMessageChain::messages() const { return messages_; }

const std::optional<std::string> &OpenAIMessageChain::system_prompt() const { return system_prompt_; }

const std::string &OpenAIMessageChain::model_name() const { return model_name_; }

const std::string &OpenAIMessageChain::api_endpoint() const { return api_endpoint_; }

bool OpenAIMessageChain::has_error() const { return has_error_; }

const std::string &OpenAIMessageChain::error_message() const { return error_message_; }

// Private methods
OpenAIMessageChain OpenAIMessageChain::add_message(const std::string &content, const std::string &role) const
{
    OpenAIMessageChain next_chain = *this;
    next_chain.messages_.push_back({role, content});
    return next_chain;
}

void OpenAIMessageChain::set_error(const std::string &message)
{
    has_error_ = true;
    error_message_ = message;
}

std::pair<std::string, int> parse_endpoint(const std::string& endpoint_str) {
    std::string host;
    int port = -1; // Default port (will be 80 for http, 443 for https if not specified)

    size_t protocol_end = endpoint_str.find("://");
    std::string remaining = endpoint_str;
    if (protocol_end != std::string::npos) {
        remaining = endpoint_str.substr(protocol_end + 3);
    }

    size_t port_start = remaining.find(':');
    if (port_start != std::string::npos) {
        host = remaining.substr(0, port_start);
        try {
            port = std::stoi(remaining.substr(port_start + 1));
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Warning: Invalid port in endpoint: " << endpoint_str << std::endl;
             // Keep port as -1, httplib might handle default ports
        } catch (const std::out_of_range& oor) {
             std::cerr << "Warning: Port out of range in endpoint: " << endpoint_str << std::endl;
             // Keep port as -1
        }
    } else {
        host = remaining;
    }
    
    // Remove trailing slash if present
    if (!host.empty() && host.back() == '/') {
        host.pop_back();
    }

    return {host, port};
}

std::string OpenAIMessageChain::call_openai_api()
{

    json payload;
    payload["model"] = model_name_;
    payload["messages"] = json::array();
    if (system_prompt_)
    {
        payload["messages"].push_back({{"role", "system"}, {"content", *system_prompt_}});
    }
    for (const auto &msg : messages_)
    {
        payload["messages"].push_back({{"role", msg.role}, {"content", msg.content}});
    }
    std::string payload_str = payload.dump();

    httplib::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}};
    
    httplib::Result result; // Store the result

    httplib::SSLClient cli(api_endpoint_);
    cli.set_connection_timeout(10, 0); // 10 seconds
    cli.set_read_timeout(60, 0);       // 60 seconds
    cli.set_follow_location(true);
    result = cli.Post("/v1/chat/completions", headers, payload_str, "application/json");


    if (result) // Check if the request was successful
    {
        auto& res = result.value(); // Get the response object
        if (res.status == 200)
        {
            return res.body;
        }
        else
        {
            set_error("API request failed: " + std::to_string(res.status) + " - " + res.body);
            return "";
        }
    }
    else
    {
        // Handle the error case from the Result object
        auto err_code = result.error();
        set_error("HTTP request failed: " + httplib::to_string(err_code));
        return "";
    }
} 