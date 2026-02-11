#include "protocol.h"
#include <string>
#include <vector>
#include <sstream>

// helpers (you can keep these tiny)
static bool read_line(const std::string &s, size_t &pos, std::string &out_line) {
    size_t end = s.find('\n', pos);
    // If there is no newline
    if (end == std::string::npos) {
        return false;
    }
    // Read from pos to end - pos (relative distance)
    out_line = s.substr(pos, end - pos);
    pos = end + 1;
    return true;
}

static bool parse_kv(const std::string &line, std::string &k, std::string &v) {
    // TODO:
    // - split on first '='
    // - if no '=', return false
    // - k = left, v = right
    size_t delim = line.find('=');
    // If there is no '='
    if (delim == std::string::npos) {
        return false;
    }
    k = line.substr(0,delim);
    // This is what we care about !!!
    v = line.substr(delim + 1);
    return true;
}

std::string msg_type_to_string(MessageType t) {
    // TODO: switch(t) -> "REGISTER_WORKER", ...
    switch(t) {
        case MessageType::REGISTER_WORKER: return "REGISTER_WORKER";
        case MessageType::HEARTBEAT: return "HEARTBEAT";
        case MessageType::SUBMIT_JOB: return "SUBMIT_JOB";
        case MessageType::POLL_JOB: return "POLL_JOB";
        case MessageType::JOB_ASSIGNMENT: return "JOB_ASSIGNMENT";
        case MessageType::CANCEL_JOB: return "CANCEL_JOB";
        case MessageType::GET_STATUS: return "GET_STATUS";
        case MessageType::JOB_RESULT: return "JOB_RESULT";
        case MessageType::ACK: return "ACK";
        case MessageType::ERROR: return "ERROR";
        default: return "ERROR";
    }
}

bool msg_type_from_string(const std::string &s, MessageType &output) {
    // TODO: if (s == "...") { out = ...; return true; } ...
    if (s == "REGISTER_WORKER") {
        output = MessageType::REGISTER_WORKER;
        return true;
    }
    if (s == "HEARTBEAT") {
        output = MessageType::HEARTBEAT;
        return true;
    }
    if (s == "SUBMIT_JOB") {
        output = MessageType::SUBMIT_JOB;
        return true;
    }
    if (s == "POLL_JOB") {
        output = MessageType::POLL_JOB;
        return true;
    }
    if (s == "JOB_ASSIGNMENT") {
        output = MessageType::JOB_ASSIGNMENT;
        return true;
    }
    if (s == "CANCEL_JOB") {
        output = MessageType::CANCEL_JOB;
        return true; }
    if (s == "GET_STATUS") {
        output = MessageType::GET_STATUS;
        return true;
    }
    if (s == "JOB_RESULT") {
        output = MessageType::JOB_RESULT;
        return true;
    }
    if (s == "ACK") {
        output = MessageType::ACK;
        return true;
    }
    if (s == "ERROR") {
        output = MessageType::ERROR;
        return true;
    }
    return false;
}

/*
Wire format (message body only):
type=<TYPE>\n
sender_id=<INT>\n
request_id=<UINT>\n
payload_len=<N>\n
<payload bytes, exactly N bytes>
*/

std::string serialize(const Message &m) {
    // TODO:
    int sender = m.sender_id;
    //int receiver = m.receiver_id;
    int msg_id = m.msg_id;
    MessageType type = m.type;
    std::string data = m.payload;
    // - build the 4 header lines exactly
    // - append raw payload (no newline required)
    // - return the resulting string
    std::string str_type = msg_type_to_string(type);
    std::string first = "type=" + str_type + "\n";
    std::string second = "sender=" + std::to_string(sender) + "\n";
    std::string third = "msg_id=" + std::to_string(msg_id) + "\n";
    std::string len = "payload_len=" + std::to_string(data.length()) + "\n";
    std::string message = first + second + third + len + data;
    return message;
}

bool deserialize(const std::string &bytes, Message &out) {
    // TODO:
    // - read 4 lines in order: type, sender_id, request_id, payload_len
    // - convert to fields (stoi/stoull)
    // - ensure payload_len doesn't exceed remaining bytes
    // - out.payload = remaining substring of length payload_len
    // - return true on success, false on malformed input
    MessageType type;
    int sender_id;
    //int receiver_id;
    int msg_id;
    size_t payload_len;
    std::string payload;

    size_t pos = 0;
    std::string line, k, v;

    try {
        // This is to extract the type
        if (!read_line(bytes, pos, line)) {
            return false;
        }
        if (!parse_kv(line, k, v)) {
            return false;
        }
        if (k != "type") {
            return false;
        }
        if (!msg_type_from_string(v, type)) {
            return false;
        }

        // Now we extract the sender id
        if (!read_line(bytes, pos, line)) {
            return false;
        }
        if (!parse_kv(line, k, v)) {
            return false;
        }
        if (k != "sender") {
            return false;
        }
        // WHAT if the sender value is NOT an integer??? We will deal with this later !!!
        // for now... just handle as if we pass so far then it will be a valid sender id
        sender_id = std::stoi(v);

        // Now we extract the request id
        if (!read_line(bytes, pos, line)) {
            return false;
        }
        if (!parse_kv(line, k, v)) {
            return false;
        }
        if (k != "msg_id") {
            return false;
        }
        msg_id = std::stoi(v);

        // Now we extract the payload length
        if (!read_line(bytes, pos, line)) {
            return false;
        }
        if (!parse_kv(line, k, v)) {
            return false;
        }
        if (k != "payload_len") {
            return false;
        }
        payload_len = static_cast<size_t>(std::stoull(v));

        // Now we extract the payload itself
        // First: check if payload_len is greater than our remaining string
        if (pos + payload_len > bytes.size()) {
            return false;
        }
        payload = bytes.substr(pos, payload_len);

        // Now we set the fields
        out.type = type;
        out.sender_id = sender_id;
        out.msg_id = msg_id;
        out.payload = payload;

        return true;
    } catch (const std::exception &) {
        return false;
    }
}