#include <unity.h>
#include "../../src/cli/CommandParser.h"

void test_basic_verb_noun() {
    ParsedCommand cmd;
    TEST_ASSERT_TRUE(CommandParser::parse("test step", cmd));
    TEST_ASSERT_EQUAL_STRING("test", cmd.verb.c_str());
    TEST_ASSERT_EQUAL_STRING("step", cmd.noun.c_str());
}

void test_flag_with_value() {
    ParsedCommand cmd;
    CommandParser::parse("test step --steps 200 --speed 400", cmd);
    TEST_ASSERT_EQUAL_STRING("200", cmd.get("steps", "0"));
    TEST_ASSERT_EQUAL_STRING("400", cmd.get("speed", "0"));
}

void test_default_value_when_missing() {
    ParsedCommand cmd;
    CommandParser::parse("test step", cmd);
    TEST_ASSERT_EQUAL_STRING("100", cmd.get("steps", "100"));
}

void test_bare_flag() {
    ParsedCommand cmd;
    CommandParser::parse("driver config --spread", cmd);
    TEST_ASSERT_TRUE(cmd.flag("spread"));
    TEST_ASSERT_FALSE(cmd.flag("stealthchop"));
}

void test_multiple_bare_flags() {
    ParsedCommand cmd;
    CommandParser::parse("driver config --spread --stealthchop", cmd);
    TEST_ASSERT_TRUE(cmd.flag("spread"));
    TEST_ASSERT_TRUE(cmd.flag("stealthchop"));
}

void test_unknown_flag_does_not_crash() {
    ParsedCommand cmd;
    TEST_ASSERT_TRUE(CommandParser::parse("driver status --json --bogus", cmd));
    TEST_ASSERT_TRUE(cmd.flag("json"));
    TEST_ASSERT_TRUE(cmd.flag("bogus"));
}

void test_empty_line_returns_false() {
    ParsedCommand cmd;
    TEST_ASSERT_FALSE(CommandParser::parse("", cmd));
}

void test_whitespace_only_returns_false() {
    ParsedCommand cmd;
    TEST_ASSERT_FALSE(CommandParser::parse("   ", cmd));
}

void test_verb_only_no_noun() {
    ParsedCommand cmd;
    CommandParser::parse("help", cmd);
    TEST_ASSERT_EQUAL_STRING("help", cmd.verb.c_str());
    TEST_ASSERT_FALSE(cmd.hasNoun());
}

void test_negative_number_value() {
    ParsedCommand cmd;
    CommandParser::parse("driver config --sgt -3", cmd);
    TEST_ASSERT_EQUAL_STRING("-3", cmd.get("sgt", "0"));
}

void test_quoted_string_value() {
    ParsedCommand cmd;
    CommandParser::parse("driver select \"tmc2209\"", cmd);
    TEST_ASSERT_EQUAL_STRING("tmc2209", cmd.noun.c_str());
}

void test_noun_not_consumed_as_flag() {
    ParsedCommand cmd;
    CommandParser::parse("driver select a4988", cmd);
    TEST_ASSERT_EQUAL_STRING("driver", cmd.verb.c_str());
    TEST_ASSERT_EQUAL_STRING("a4988", cmd.noun.c_str());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_basic_verb_noun);
    RUN_TEST(test_flag_with_value);
    RUN_TEST(test_default_value_when_missing);
    RUN_TEST(test_bare_flag);
    RUN_TEST(test_multiple_bare_flags);
    RUN_TEST(test_unknown_flag_does_not_crash);
    RUN_TEST(test_empty_line_returns_false);
    RUN_TEST(test_whitespace_only_returns_false);
    RUN_TEST(test_verb_only_no_noun);
    RUN_TEST(test_negative_number_value);
    RUN_TEST(test_quoted_string_value);
    RUN_TEST(test_noun_not_consumed_as_flag);
    return UNITY_END();
}
