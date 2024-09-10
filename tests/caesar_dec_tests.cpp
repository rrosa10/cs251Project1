#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "include/caesar_dec.h"
#include "tests/test_utils.h"

using ::std::endl;
using ::std::make_tuple;
using ::std::string;
using ::std::tuple;
using ::std::vector;
using ::testing::ContainsRegex;
using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::TestParamInfo;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::WithParamInterface;

namespace {

TEST(CaesarDec_RotVec, Empty) {
  vector<string> input;
  rot(input, 10);

  vector<string> expected;
  ASSERT_EQ(input, expected);
}

TEST(CaesarDec_RotVec, OneElem) {
  vector<string> input = {"ABCD"};

  // Make sure that we didn't make a new vector! We want to reuse the existing
  // vector, but the reference allows us to completely reassign. We do this by
  // checking that that the underlying data is at the same place.
  string* start = input.data();

  rot(input, 1);

  vector<string> expected = {"BCDE"};
  ASSERT_EQ(input.data(), start) << "The vector itself was modified. Make sure "
                                    "you only modify its elements!";
  ASSERT_EQ(input, expected);
}

TEST(CaesarDec_RotVec, MultipleElems) {
  vector<string> input = {"ABCD", "DDDD", "ZZZZ"};
  rot(input, 1);

  vector<string> expected = {"BCDE", "EEEE", "AAAA"};
  ASSERT_EQ(input, expected);
}

class CaesarDec_Clean
    : public TestWithParam<tuple<string, string>> {};

TEST_P(CaesarDec_Clean, Case) {
  auto& [input, expected] = GetParam();

  string actual = clean(input);
  ASSERT_EQ(actual, expected);
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_Clean, Values(
      make_tuple("", ""),
      make_tuple("UPPERCASELETTERS", "UPPERCASELETTERS"),
      make_tuple("1234567890!@#$%^&*() .,", ""),
      make_tuple("a-b C", "ABC")
    )
);

class CaesarDec_SplitSpaces
    : public TestWithParam<tuple<string, vector<string>>> {};

TEST_P(CaesarDec_SplitSpaces, Case) {
  auto& [input, expected] = GetParam();

  vector<string> actual = splitBySpaces(input);
  ASSERT_EQ(actual, expected);
}

// Can't use bare brace-init. Values accepts template parameters, and the
// compiler doesn't know what type this should become!
INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_SplitSpaces,
    Values(make_tuple("", vector<string>{}),
           make_tuple("HI", vector<string>{"HI"}),
           make_tuple("HI THERE", vector<string>{"HI", "THERE"}),
           make_tuple("A B C DDD", vector<string>{"A", "B", "C", "DDD"}),
           make_tuple("  LEADING AND TRAILING SPACES  ",
                      vector<string>{"LEADING", "AND", "TRAILING", "SPACES"}),
           make_tuple("  SPACES  ", vector<string>{"SPACES"}),
           make_tuple("MULTIPLE    SPACES     BETWEEN",
                      vector<string>{"MULTIPLE", "SPACES", "BETWEEN"})));

class CaesarDec_JoinWithSpaces
    : public TestWithParam<tuple<vector<string>, string>> {};

TEST_P(CaesarDec_JoinWithSpaces, Case) {
  auto& [input, expected] = GetParam();

  string actual = joinWithSpaces(input);
  ASSERT_EQ(actual, expected);
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_JoinWithSpaces,
    Values(make_tuple(vector<string>{}, ""),
           make_tuple(vector<string>{"WORD"}, "WORD"),
           make_tuple(vector<string>{"MORE", "WORDS"}, "MORE WORDS"),
           make_tuple(vector<string>{"LOOK", "AT", "ALL", "THESE", "WORDS"},
                      "LOOK AT ALL THESE WORDS")));

class CaesarDec_NumWordsIn
    : public TestWithParam<tuple<vector<string>, vector<string>, int>> {};

TEST_P(CaesarDec_NumWordsIn, Case) {
  auto& [input, dict, expected] = GetParam();

  int actual = numWordsIn(input, dict);
  ASSERT_EQ(actual, expected);
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_NumWordsIn,
    Values(make_tuple(vector<string>{"WORD"}, vector<string>{}, 0),
           make_tuple(vector<string>{"WORD", "NOTWORD"}, vector<string>{"WORD"},
                      1),
           make_tuple(vector<string>{"WORD", "WORD", "NOTWORD"},
                      vector<string>{"WORD"}, 2)));

// Note that these tests don't use dictionary.txt!

class CaesarDec_CommandCustomDict
    : public WithParamInterface<tuple<string, vector<string>, string, string>>,
      public CaptureCinCout {};

const string TEXT_PROMPT = "Enter the text to Caesar decrypt:";
const string NO_GOOD_DEC = "No good decryptions found";

TEST_P(CaesarDec_CommandCustomDict, Case) {
  auto& [commandInput, dict, expected_decs, name] = GetParam();

  input << commandInput << endl;

  runCaesarDecrypt(dict);

  string actual_output = output.str();

  ASSERT_THAT(actual_output, HasSubstr(TEXT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr(expected_decs))
      << "Incorrect decryptions or incorrect order of decryptions." << endl
      << "Make sure to test decrypting rotations starting from 0.";

  // The contents of this are a bit weird, but essentially:
  // - Is there a colon (:), which is the end of the prompt and the start of the
  //   decryptions
  // - Followed by any number of spaces or newlines
  // - Followed by all the expected decryptions in this order
  // - The $ is the end of the string, so checking that we're at the end of the
  //   decryptions
  // I tried to avoid regex as much as possible, but I felt it was necessary
  // here.
  ASSERT_THAT(actual_output,
              ContainsRegex(":[[:space:]]*" + expected_decs + "\n$"))
      << "More decryptions than expected";
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_CommandCustomDict,
    Values(make_tuple("aaaaaaaaa", vector<string>{}, NO_GOOD_DEC, "NoResults"),
           make_tuple("hello", vector<string>{"HELLO"}, "HELLO",
                      "OneWordOneResult"),
           make_tuple("nk", vector<string>{"BY", "HE", "IF", "NA"},
                      "BY\nHE\nIF", "OneWordManyResults"),
           make_tuple("or", vector<string>{"BE", "OR"}, "OR\nBE",
                      "OneWordVerifyOrder"),
           make_tuple("ai   nyug", vector<string>{"GO", "TEAM"}, "GO TEAM",
                      "TwoWordsOneResult"),
           make_tuple("FYYFHP FY IFBS", vector<string>{"AT", "ATTACK"},
                      "ATTACK AT DAWN", "ThreeWordsNeedsAtLeastTwo"),
           make_tuple("ai   nyug", vector<string>{"OW", "TEAM", "WE"},
                      NO_GOOD_DEC, "TwoWordsNeedsBothInDict"),
           make_tuple(
               "DOLU PU AOL JVBYZL VM OBTHU LCLUAZ PA ILJVTLZ ULJLZZHYF MVY "
               "VUL WLVWSL AV KPZZVSCL AOL "
               "WVSPAPJHS IHUKZ DOPJO OHCL JVUULJALK AOLT DPAO HUVAOLY HUK AV "
               "HZZBTL HTVUN AOL WVDLYZ VM AOL "
               "LHYAO AOL ZLWHYHAL HUK LXBHS ZAHAPVU AV DOPJO AOL SHDZ VM "
               "UHABYL HUK VM UHABYLZ NVK LUAPASL "
               "AOLT H KLJLUA YLZWLJA AV AOL VWPUPVUZ VM THURPUK YLXBPYLZ AOHA "
               "AOLF ZOVBSK KLJSHYL AOL JHBZLZ "
               "DOPJO PTWLS AOLT AV AOL ZLWHYHAPVU",
               vector<string>{
                   "WHEN",     "IN",     "THE",       "COURSE",  "OF",
                   "HUMAN",    "EVENTS", "IT",        "BECOMES", "NECESSARY",
                   "FOR",      "ONE",    "PEOPLE",    "TO",      "DISSOLVE",
                   "WHICH",    "HAVE",   "CONNECTED", "THEM",    "WITH",
                   "ANOTHER",  "AND",    "ASSUME",    "AMONG",   "EARTH",
                   "SEPARATE", "EQUAL",  "LAWS",      "A",       "RESPECT",
                   "REQUIRES", "THAT",   "THEY",      "SHOULD",  "DECLARE",
               },
               "WHEN IN THE COURSE OF HUMAN EVENTS IT BECOMES NECESSARY FOR "
               "ONE PEOPLE TO DISSOLVE THE "
               "POLITICAL BANDS WHICH HAVE CONNECTED THEM WITH ANOTHER AND TO "
               "ASSUME AMONG THE POWERS OF THE EARTH THE "
               "SEPARATE AND EQUAL STATION TO WHICH THE LAWS OF NATURE AND OF "
               "NATURES GOD ENTITLE THEM A DECENT RESPECT TO "
               "THE OPINIONS OF MANKIND REQUIRES THAT THEY SHOULD DECLARE THE "
               "CAUSES WHICH IMPEL THEM TO THE SEPARATION",
               "LongStressTest")),
    [](const TestParamInfo<CaesarDec_CommandCustomDict::ParamType>& info) {
      return last_elem(info.param);
    });

class CaesarDec_MainFullDict
    : public WithParamInterface<tuple<string, string, string>>,
      public CaptureCinCout {};

TEST_P(CaesarDec_MainFullDict, Case) {
  auto& [commandInput, expected_decs, name] = GetParam();

  input << "d" << endl;
  input << commandInput << endl;
  input << "x";

  ciphers_main();

  string actual_output = output.str();

  ASSERT_THAT(actual_output, HasSubstr(TEXT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr(expected_decs))
      << "Incorrect decryptions or incorrect order of decryptions." << endl
      << "Make sure to test decrypting rotations starting from 0.";

  // See above for the regex explanation. Instead of $ for the end of the
  // string, we expect the menu to be printed again.
  ASSERT_THAT(actual_output, ContainsRegex(":[[:space:]]*" + expected_decs +
                                           "\n\nCiphers Menu"))
      << "More decryptions than expected";
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_MainFullDict,
    Values(make_tuple("hello", "HELLO", "OneWordOneResult"),
           make_tuple("iwxh xh paa p wjvt bxhjcstghipcsxcv",
                      "THIS IS ALL A HUGE MISUNDERSTANDING", "LongerResult")),
    [](const TestParamInfo<CaesarDec_MainFullDict::ParamType>& info) {
      return last_elem(info.param);
    });

}  // namespace
