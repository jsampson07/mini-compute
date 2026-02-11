#pragma once
#include <string>

enum class MessageType{
    REGISTER_WORKER,
    HEARTBEAT,
    SUBMIT_JOB,
    POLL_JOB,
    JOB_ASSIGNMENT,
    CANCEL_JOB,
    GET_STATUS,
    JOB_RESULT,
    ACK,
    ERROR
};

struct Message {
    MessageType type;
    int sender_id;
    //int receiver_id;
    int msg_id;
    size_t payload_len;
    std::string payload; // For now our payload will be JSON string or key-value pairs (easy to work with)
};

// Used to convert to a string for printing
std::string msg_type_to_string(MessageType t);

// Used to convert a string to a MessageType --> easy coding
bool msg_type_from_string(const std::string &s, MessageType &output);

// These functions do (msg --> bytes) and (bytes --> msg) respectively
std::string serialize(const Message &msg);
bool deserialize(const std::string &bytes, Message &output);