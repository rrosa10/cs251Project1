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

std::mt19937 Random::rng;

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

  ifstream inFile("dictionary.txt");
  vector<string> dictionary;
  string word;

  if (inFile.is_open()) {
    while (inFile >> word) {
      dictionary.push_back(word);
    }
    inFile.close();
  } else {
    cout << "Unable to open file" << endl;
  }

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    cout << "Printing menu..." << endl;
    printMenu();

    cout << endl << "Enter a command (case does not matter): ";
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
    } else if (command == "D" || command == "d") {
      runCaesarDecrypt(dictionary);
    }
    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

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

  cout << "Enter the text to Caesar encrypt: ";
  getline(cin, messages);

  cout << "\nEnter the number of characters to rotate by: ";
  cin >> rotateAmmount;

  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  encryptedMessage = rot(messages, rotateAmmount);
  cout << encryptedMessage << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  for (string& str : strings) {
    str = rot(str, amount);
  }
}

string clean(const string& s) {
  string newString;
  for (char ch : s) {
    if (isalpha(ch)) {
      newString += toupper(ch);
    }
  }
  return newString;
}

vector<string> splitBySpaces(const string& s) {
  vector<string> extractedWords;
  string word;

  stringstream testStream(s);
  while (testStream >> word) {
    extractedWords.push_back(word);
  }
  return extractedWords;
}

string joinWithSpaces(const vector<string>& words) {
  string joinedWords;
  for (int i = 0; i < words.size(); i++) {
    joinedWords = joinedWords + words.at(i);
    if (i != words.size() - 1) {
      joinedWords = joinedWords + " ";
    }
  }
  return joinedWords;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int numsWordsIn = 0;
  for (const string& word : words) {
    bool found = false;
    for (const string& dictWord : dict) {
      if (word == dictWord) {
        found = true;
        break;
      }
    }
    if (found) {
      numsWordsIn++;
    }
  }
  return numsWordsIn;
}

void runCaesarDecrypt(const vector<string>& dict) {
  //==========================================================================
  string encryptedMessage;
  string decryptedMessage;
  vector<string> splitWords;
  vector<int> rotWordCount;
  vector<int> maxIndexes;
  vector<string> decryptWordVector;
  int wordCount = 0;

  cout << "Enter the text to Caesar decrypt: \n";
  getline(cin, encryptedMessage);
  splitWords = splitBySpaces(encryptedMessage);
  for (string& words : splitWords) {
    words = clean(words);
  }

  // somehting->numwordsin->joinwithspces
  for (int i = 1; i <= 26; i++) {
    rot(splitWords, i);
    wordCount = numWordsIn(splitWords, dict);
    rotWordCount.push_back(wordCount);
    cout << wordCount << endl;
  }

  int maxVal = rotWordCount[0];
  for (int i = 1; i < rotWordCount.size(); i++) {
    if (rotWordCount[i] > maxVal) {
      maxVal = rotWordCount[i];
    }
  }

  for (int i = 0; i < rotWordCount.size(); i++) {
    if (rotWordCount[i] == maxVal) {
    }
  }

  for (string word : splitWords) {
    for (int index : maxIndexes) {
      decryptWordVector.push_back(rot(word, index));
    }
  }
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
