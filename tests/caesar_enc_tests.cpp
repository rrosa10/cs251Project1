#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "include/caesar_enc.h"
#include "tests/test_utils.h"

using ::std::endl;
using ::std::string;
using ::std::tuple;
using ::testing::HasSubstr;
using ::testing::Values;
using ::testing::WithParamInterface;

namespace {

tuple<char, int, char> ROT_CHAR[] = {
    {'A', 0, 'A'}, {'A', 1, 'B'}, {'A', 10, 'K'}, {'B', 1, 'C'}, {'C', 1, 'D'},
    {'D', 1, 'E'}, {'E', 1, 'F'}, {'F', 1, 'G'},  {'G', 1, 'H'}, {'H', 1, 'I'},
    {'I', 1, 'J'}, {'J', 1, 'K'}, {'K', 1, 'L'},  {'L', 1, 'M'}, {'M', 1, 'N'},
    {'N', 1, 'O'}, {'O', 1, 'P'}, {'P', 1, 'Q'},  {'Q', 1, 'R'}, {'R', 1, 'S'},
    {'S', 1, 'T'}, {'T', 1, 'U'}, {'U', 1, 'V'},  {'V', 1, 'W'}, {'W', 1, 'X'},
    {'X', 1, 'Y'}, {'Y', 1, 'Z'}, {'Z', 1, 'A'},  {'Z', 2, 'B'}, {'Z', 26, 'Z'},
};

TEST(CaesarEnc_RotChar, Chars) {
  for (const auto& [input, i, expected] : ROT_CHAR) {
    ASSERT_EQ(rot(input, i), expected);
  }
}

tuple<string, int, string> ROT_STRING_ALL_UPPER_ALPHA[] = {
    {"A", 1, "B"},       {"B", 1, "C"},       {"C", 1, "D"}, {"D", 1, "E"},
    {"E", 1, "F"},       {"F", 1, "G"},       {"G", 1, "H"}, {"H", 1, "I"},
    {"I", 1, "J"},       {"J", 1, "K"},       {"K", 1, "L"}, {"L", 1, "M"},
    {"M", 1, "N"},       {"N", 1, "O"},       {"O", 1, "P"}, {"P", 1, "Q"},
    {"Q", 1, "R"},       {"R", 1, "S"},       {"S", 1, "T"}, {"T", 1, "U"},
    {"U", 1, "V"},       {"V", 1, "W"},       {"W", 1, "X"}, {"X", 1, "Y"},
    {"Y", 1, "Z"},       {"Z", 1, "A"},       {"Z", 2, "B"}, {"Z", 26, "Z"},
    {"ABCD", 5, "FGHI"}, {"WXYZ", 4, "ABCD"},
};

TEST(CaesarEnc_RotString, AllUpperAlpha) {
  for (const auto& [input, i, expected] : ROT_STRING_ALL_UPPER_ALPHA) {
    ASSERT_EQ(rot(input, i), expected);
  }
}

tuple<string, int, string> ROT_STRING_MIXED_CASE[] = {
    {"a", 0, "A"},       {"a", 1, "B"},     {"z", 1, "A"},
    {"AbcD", 5, "FGHI"}, {"xyz", 3, "ABC"},
};

TEST(CaesarEnc_RotString, MixedCase) {
  for (const auto& [input, i, expected] : ROT_STRING_MIXED_CASE) {
    ASSERT_EQ(rot(input, i), expected);
  }
}

tuple<string, int, string> ROT_STRING_NON_ALPHA[] = {
    {"oh HEL-LO!", 1, "PI IFMMP"},
    {"attack, team", 7, "HAAHJR ALHT"},
    {"_", 5, ""},
    {"", 10, ""},
};

TEST(CaesarEnc_RotString, NonAlpha) {
  for (const auto& [input, i, expected] : ROT_STRING_MIXED_CASE) {
    ASSERT_EQ(rot(input, i), expected);
  }
}

class CaesarEnc_FullCommand
    : public WithParamInterface<tuple<string, int, string>>,
      public CaptureCinCout {};

const string TEXT_PROMPT = "Enter the text to Caesar encrypt:";
const string ROT_PROMPT = "Enter the number of characters to rotate by:";

TEST_P(CaesarEnc_FullCommand, Case) {
  auto& [commandInput, shift, expected_enc] = GetParam();

  input << commandInput << endl;
  input << shift << endl;

  runCaesarEncrypt();

  string actual_output = output.str();

  ASSERT_THAT(actual_output, HasSubstr(TEXT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr(ROT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr(expected_enc)) << "Incorrect encryption";
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarEnc_FullCommand,
    Values(
        // Input, shift, expected
        make_tuple("HELLO", 0, "HELLO"),
        make_tuple("Attack, team!", 7, "HAAHJR ALHT"),
        make_tuple(
            "Can your program handle sentences that are spaced---    very    "
            "weirdly?",
            25,
            "BZM XNTQ OQNFQZL GZMCKD RDMSDMBDR SGZS ZQD ROZBDC    UDQX    "
            "VDHQCKX")));

TEST_F(CaesarEnc_FullCommand, Main) {
  input << "c" << endl;
  input << "QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD" << endl;
  input << "3" << endl;
  input << "x" << endl;

  ciphers_main();

  string actual_output = output.str();
  string expected_enc = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
  ASSERT_THAT(actual_output, HasSubstr(TEXT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr(ROT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr(expected_enc)) << "Incorrect encryption";
}

}  // namespace
