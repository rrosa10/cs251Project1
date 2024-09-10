#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "include/subst_enc.h"
#include "tests/test_utils.h"

using ::std::endl;
using ::std::string;
using ::std::vector;
using ::testing::HasSubstr;

namespace {

const string FOX_PANGRAM = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
const string MR_PANGRAM_PUNCT = "Mr. Jock, TV quiz PhD, bags few lynx.";

TEST(SubstEnc_ApplyCipher, Identity) {
  vector<char> identity_cipher = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                                  'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                                  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

  string alpha_actual =
      applySubstCipher(identity_cipher, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  ASSERT_EQ(alpha_actual, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  string boring_actual = applySubstCipher(identity_cipher, FOX_PANGRAM);
  ASSERT_EQ(boring_actual, FOX_PANGRAM);

  string cool_actual = applySubstCipher(identity_cipher, MR_PANGRAM_PUNCT);
  ASSERT_EQ(cool_actual, "MR. JOCK, TV QUIZ PHD, BAGS FEW LYNX.");
}

TEST(SubstEnc_ApplyCipher, Cipher) {
  vector<char> cipher = {'V', 'Y', 'B', 'L', 'Z', 'O', 'F', 'M', 'A',
                         'I', 'D', 'Q', 'G', 'J', 'K', 'X', 'H', 'N',
                         'W', 'E', 'R', 'S', 'U', 'P', 'C', 'T'};

  string alpha_actual = applySubstCipher(cipher, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  ASSERT_EQ(alpha_actual, "VYBLZOFMAIDQGJKXHNWERSUPCT");

  string boring_actual = applySubstCipher(cipher, FOX_PANGRAM);
  ASSERT_EQ(boring_actual, "EMZ HRABD YNKUJ OKP IRGXW KSZN EMZ QVTC LKF");

  string cool_actual = applySubstCipher(cipher, MR_PANGRAM_PUNCT);
  ASSERT_EQ(cool_actual, "GN. IKBD, ES HRAT XML, YVFW OZU QCJP.");
}

class SubstEnc_RandCipherCommand : public CaptureCinCout {};

const string TEXT_PROMPT = "Enter the text to substitution-cipher encrypt: ";

TEST_F(SubstEnc_RandCipherCommand, FullCommand) {
  // Seed prng
  input << "r" << endl;
  input << 0 << endl;

  input << "a" << endl;
  input << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << endl;

  // Must run in the same command since main re-seeds
  input << "a" << endl;
  input << MR_PANGRAM_PUNCT << endl;
  input << "x" << endl;

  ciphers_main();

  string actual_output = output.str();

  ASSERT_THAT(actual_output, HasSubstr(TEXT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr("UHJCKSMYXWRINQALDGVFZBOEPT"))
      << "Incorrect substitution of random cipher";

  ASSERT_THAT(actual_output, HasSubstr(TEXT_PROMPT)) << "Incorrect prompt";
  ASSERT_THAT(actual_output, HasSubstr("LZ. MXTI, OS DBPQ AFG, YEUR CWJ VNHK."))
      << "Incorrect substitution of random cipher";
}

}  // namespace
