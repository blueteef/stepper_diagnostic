#pragma once

#include <string>
#include <map>

// =============================================================================
// CommandParser — GNU-style argument tokenizer.
//
// Parses a raw input line into:
//   verb  — first token              ("test", "driver", "help")
//   noun  — second token if not flag ("step", "status", "select")
//   args  — --key value pairs and bare --flags
//
// Uses std::string throughout so it compiles on native for unit testing.
//
// Usage:
//   ParsedCommand cmd;
//   if (CommandParser::parse(line, cmd)) {
//       std::string steps = cmd.get("steps", "200");
//       bool json = cmd.flag("json");
//   }
// =============================================================================

struct ParsedCommand {
    std::string verb;
    std::string noun;
    std::string target;  // third positional, e.g. "a4988" in "driver select a4988"
    std::map<std::string, std::string> args;

    std::string get(const char* key, const char* defaultVal = "") const;
    bool flag(const char* key) const;

    bool hasNoun()   const { return !noun.empty(); }
    bool hasTarget() const { return !target.empty(); }
};

class CommandParser {
public:
    // Parse a null-terminated input line into cmd.
    // Returns false if the line is empty or all whitespace.
    static bool parse(const char* line, ParsedCommand& cmd);
};
