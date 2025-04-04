#pragma once
#include <string>

// Represents a single message in the conversation
struct Message
{
    std::string role;
    std::string content;
}; 