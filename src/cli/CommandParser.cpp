#include "CommandParser.h"
#include <vector>

// -----------------------------------------------------------------------------
// Internal: split line into tokens on whitespace, respecting quoted strings.
// -----------------------------------------------------------------------------
static std::vector<std::string> tokenize(const char* line) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;

    for (const char* p = line; *p; p++) {
        if (*p == '"') {
            inQuotes = !inQuotes;
        } else if ((*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += *p;
        }
    }
    if (!current.empty()) tokens.push_back(current);
    return tokens;
}

// Returns true if token is a long flag (starts with "--").
static bool isFlag(const std::string& token) {
    return token.size() >= 2 && token[0] == '-' && token[1] == '-';
}

// -----------------------------------------------------------------------------

bool CommandParser::parse(const char* line, ParsedCommand& cmd) {
    cmd.verb.clear();
    cmd.noun.clear();
    cmd.target.clear();
    cmd.args.clear();

    auto tokens = tokenize(line);
    if (tokens.empty()) return false;

    size_t i = 0;

    // First token is always the verb.
    cmd.verb = tokens[i++];

    // Second token is the noun if it doesn't start with "--".
    if (i < tokens.size() && !isFlag(tokens[i])) {
        cmd.noun = tokens[i++];
    }

    // Third positional (if present and not a flag) is the target.
    if (i < tokens.size() && !isFlag(tokens[i])) {
        cmd.target = tokens[i++];
    }

    // Remaining tokens: --key [value] pairs.
    // A value is any token that does NOT start with "--".
    // Single-dash tokens like "-3" are valid values, not flags.
    while (i < tokens.size()) {
        if (isFlag(tokens[i])) {
            std::string key = tokens[i].substr(2);  // strip "--"
            i++;
            if (i < tokens.size() && !isFlag(tokens[i])) {
                cmd.args[key] = tokens[i++];         // --key value
            } else {
                cmd.args[key] = "";                  // bare --flag
            }
        } else {
            i++;  // skip unexpected positional (shouldn't normally occur)
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

std::string ParsedCommand::get(const char* key, const char* defaultVal) const {
    auto it = args.find(key);
    if (it == args.end() || it->second.empty()) return defaultVal;
    return it->second;
}

bool ParsedCommand::flag(const char* key) const {
    return args.find(key) != args.end();
}
