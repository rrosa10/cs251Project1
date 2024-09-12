#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

/**
 * Print instructions for using the program.
 */
void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

int main() {
  Random::seed(time(NULL));
  string command;

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    } else if (command == "C" || command == "c") {
      runCaesarEncrypt();
    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  char cUpper = toupper(c);
  int newCharASCIIVall = cUpper + amount;
  if (cUpper >= 'A' && cUpper <= 'Z') {
    if (newCharASCIIVall > 'Z') {
      newCharASCIIVall = 'A' + (newCharASCIIVall - 'Z' - 1);
    }
  }
  return (char)newCharASCIIVall;
}

string rot(const string& line, int amount) {
  string newString;
  for (char ch : line) {
    if (ch == ' ') {
      newString += ' ';
    } else if (isalpha(ch)) {
      newString += rot(ch, amount);
    }
  }
  return newString;
}

void runCaesarEncrypt() {
  string messages;
  string encryptedMessage;
  int rotateAmmount;

  cout << "Enter the text to Caesar encrypt: " << endl;
  getline(cin, messages);

  cout << "\nEnter the number of characters to rotate by: ";
  cin >> rotateAmmount;

  encryptedMessage = rot(messages, rotateAmmount);
  cout << encryptedMessage << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  // TODO: student fill this in
}

string clean(const string& s) {
  // TODO: student fill this in
  return "";
}

vector<string> splitBySpaces(const string& s) {
  // TODO: student fill this in
  return vector<string>{};
}

string joinWithSpaces(const vector<string>& words) {
  // TODO: student fill this in
  return "";
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  // TODO: student fill this in
  return 0;
}

void runCaesarDecrypt(const vector<string>& dict) {
  // TODO: student fill this in
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string result;
  for (char c : s) {
    if (c >= 'a' && c <= 'z') {
      c = toupper(c);
    }
    if (c >= 'A' && c <= 'Z') {
      result += cipher[c - 'A'];
    } else {
      result += c;
    }
  }
  return result;
}

void applyRandSubstCipherCommand() {
  vector<char> cipher = genRandomSubstCipher();
  string s;
  cout << "Enter the text to substitution-cipher encrypt: ";
  getline(cin, s);
  string result = applySubstCipher(cipher, s);

  cout << result << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  // TODO: student fill this in
  return 0.0;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  // TODO: student fill this in
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  // TODO: student fill this in
  return vector<char>{};
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // TODO: student fill this in
}

#pragma endregion SubstDec
