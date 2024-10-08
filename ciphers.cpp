
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

vector<string> loadDictionary(const string& filename);
void decryptFileCommand(const QuadgramScorer& scorer);

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
  Random::seed(time(NULL));  // Seed with current time for normal execution

  string command;

  ifstream quadgramFile("english_quadgrams.txt");
  vector<string> quadgramVector;
  vector<int> countVector;
  string line;

  if (!quadgramFile.is_open()) {
    cerr << "Failed to open file: english_quadgrams.txt" << endl;
    return 1;
  }

  while (getline(quadgramFile, line)) {
    istringstream iss(line);
    string quadgram;
    int count;

    if (getline(iss, quadgram, ',') && iss >> count) {
      quadgramVector.push_back(quadgram);
      countVector.push_back(count);
    }
  }
  quadgramFile.close();

  QuadgramScorer scorer(quadgramVector, countVector);
  vector<string> dictionary = loadDictionary("dictionary.txt");

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;
  cout << "Printing menu..." << endl;

  do {
    printMenu();

    cout << endl << "Enter a command (case does not matter):\n";
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: \n";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    } else if (command == "C" || command == "c") {
      runCaesarEncrypt();
    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    } else if (command == "D" || command == "d") {
      runCaesarDecrypt(dictionary);
    } else if (command == "E" || command == "e") {
      computeEnglishnessCommand(scorer);
    } else if (command == "S" || command == "s") {
      decryptSubstCipherCommand(scorer);
    } else if (command == "F" || command == "f") {
      decryptFileCommand(scorer);
    }
    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void loadQuadgramsAndCounts(const string& filename, vector<string>& quadgrams,
                            vector<int>& counts) {
  ifstream file(filename);
  string quadgram;
  int count;

  if (file.is_open()) {
    while (file >> quadgram >> count) {
      quadgrams.push_back(quadgram);
      counts.push_back(count);
    }
    file.close();
  } else {
    cerr << "Failed to open file: " << filename << endl;
  }
}

vector<string> loadDictionary(const string& filename) {
  vector<string> dictionary;
  ifstream inFile(filename);
  string word;

  if (inFile.is_open()) {
    while (inFile >> word) {
      dictionary.push_back(word);
    }
    inFile.close();
  } else {
    cerr << "Failed to open file: " << filename << endl;
  }

  return dictionary;
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
  string rotateAmmount;
  int rotateAmmountInt;

  cout << "Enter the text to Caesar encrypt: ";
  getline(cin, messages);

  cout << "\nEnter the number of characters to rotate by: ";
  getline(cin, rotateAmmount);
  rotateAmmountInt = stoi(rotateAmmount);

  encryptedMessage = rot(messages, rotateAmmountInt);
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

string cleanText(const string& text) {
  string cleanedText;
  for (char c : text) {
    if (isalpha(c)) {
      cleanedText += toupper(c);
    }
  }
  return cleanedText;
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
  string encryptedMessage;
  vector<string> splitWords;
  vector<string> decryptedMessage;
  int maxValidWords = 0;
  int validWords = 0;
  vector<int> bestRotations;

  cout << "Enter the text to Caesar decrypt:\n";
  getline(cin, encryptedMessage);
  splitWords = splitBySpaces(encryptedMessage);
  for (string& words : splitWords) {
    words = clean(words);
  }

  for (int i = 0; i <= 25; i++) {
    vector<string> rotatedWords = splitWords;
    for (string& word : rotatedWords) {
      word = rot(word, i);
    }

    validWords = numWordsIn(rotatedWords, dict);

    if (validWords > maxValidWords) {
      maxValidWords = validWords;
      bestRotations.clear();
      decryptedMessage.clear();
      bestRotations.push_back(i);
      decryptedMessage.push_back(joinWithSpaces(rotatedWords));
    } else if (validWords == maxValidWords) {
      bestRotations.push_back(i);
      decryptedMessage.push_back(joinWithSpaces(rotatedWords));
    }
  }

  if (maxValidWords <= (splitWords.size() / 2)) {
    cout << "No good decryptions found\n";
  }

  else if (maxValidWords == 0) {
    cout << "No good decryptions found\n";
  }

  else if (maxValidWords > (splitWords.size() / 2)) {
    for (int i = 0; i < bestRotations.size(); ++i) {
      cout << decryptedMessage[i] << endl;
    }
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string result = "";
  for (auto c : s) {
    if (isalpha(c)) {
      result += cipher.at(ALPHABET.find(toupper(c)));
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
  string newString = clean(s);
  string temp;
  double result = 0;

  for (int i = 0; i + 4 <= newString.size(); i++) {
    temp = newString.substr(i, 4);
    result += scorer.getScore(temp);
  }

  return result;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  string text;
  cout << "Enter a string to score: " << endl;
  getline(cin, text);
  string cleanedText;
  double totalScore = 0.0;

  for (char c : text) {
    if (isalpha(c)) {
      cleanedText += toupper(c);
    }
  }

  if (cleanedText.length() >= 4) {
    for (int i = 0; i <= cleanedText.length() - 4; i++) {
      string quadgram = cleanedText.substr(i, 4);
      totalScore += scorer.getScore(quadgram);
    }
  }
  cout << "Englishness: " << totalScore << endl;
}

string decrypt(const string& key, const string& ciphertext) {
  string plaintext = ciphertext;
  for (size_t i = 0; i < ciphertext.size(); ++i) {
    if (isalpha(ciphertext[i])) {
      char c = toupper(ciphertext[i]);
      plaintext[i] = key[c - 'A'];
    }
  }
  return plaintext;
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  vector<char> bestCipher = genRandomSubstCipher();
  double bestScore =
      scoreString(scorer, applySubstCipher(bestCipher, ciphertext));

  for (int runIndex = 0; runIndex < 20; runIndex++) {
    vector<char> currentCipher = genRandomSubstCipher();
    double currentScore =
        scoreString(scorer, applySubstCipher(currentCipher, ciphertext));
    int attempts = 0;

    while (attempts < 1500) {
      int firstIndex = Random::randInt(ALPHABET.size() - 1);
      int secondIndex;
      do {
        secondIndex = Random::randInt(ALPHABET.size() - 1);
      } while (firstIndex == secondIndex);

      vector<char> tempCipher = currentCipher;
      swap(tempCipher[firstIndex], tempCipher[secondIndex]);

      double tempScore =
          scoreString(scorer, applySubstCipher(tempCipher, ciphertext));
      attempts++;

      if (tempScore > currentScore) {
        currentScore = tempScore;
        currentCipher = tempCipher;
        attempts = 0;
      }
    }

    if (currentScore > bestScore) {
      bestScore = currentScore;
      bestCipher = currentCipher;
    }
  }
  return bestCipher;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  cout << "Enter text to decrypt: ";
  string inputCiphertext;
  getline(cin, inputCiphertext);

  vector<char> bestDecryptionKey = decryptSubstCipher(scorer, inputCiphertext);
  string decryptedText = applySubstCipher(bestDecryptionKey, inputCiphertext);

  cout << endl << decryptedText << endl;
}

void decryptFileCommand(const QuadgramScorer& scorer) {
  string inputFilename, outputFilename;

  cout << "Enter input filename: ";
  getline(cin, inputFilename);
  cout << "Enter output filename: ";
  getline(cin, outputFilename);

  ifstream inputFile(inputFilename);
  if (!inputFile.is_open()) {
    cerr << "Failed to open input file: " << inputFilename << endl;
    return;
  }

  stringstream temp;
  temp << inputFile.rdbuf();
  string ciphertext = temp.str();
  inputFile.close();

  for (char& c : ciphertext) {
    if (isalpha(c)) {
      c = toupper(c);
    }
  }

  vector<char> decryptedText = decryptSubstCipher(scorer, ciphertext);

  ofstream outputFile(outputFilename);
  if (!outputFile.is_open()) {
    cerr << "Failed to open output file: " << outputFilename << endl;
    return;
  }
  for (char c : decryptedText) {
    outputFile << c;
  }
  outputFile.close();
  cout << "Decryption complete. Output written to " << outputFilename << endl;
}

#pragma endregion SubstDec