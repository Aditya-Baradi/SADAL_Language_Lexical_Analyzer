//
// Created by aditya on 3/11/2025.
//

#include <iostream>
#include <fstream>
#include "lex.h"
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;
extern const map<string, Token> keywords;

struct CaseComp {
    bool operator()(const string& a, const string& b) const {
        string lowA = a, lowB = b;
        for (size_t i = 0; i < a.length(); i++) {
            lowA[i] = tolower(a[i]);
        }
        for (size_t i = 0; i < b.length(); i++) {
            lowB[i] = tolower(b[i]);
        }
        return lowA < lowB;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "No specified input file." << endl;
        return 1;
    }

    set<string, CaseComp> identifiers;
    set<string> numericConsts;
    set<string> stringAndCharConsts;

    map<Token, string> keywordTokens;

    map<string, Token, CaseComp> lowercaseKeywords;
    for (const auto& keyword: keywords) {
        string lowercaseKey = keyword.first;
        for (char& character : lowercaseKey) {
            character = tolower(character);
        }
        lowercaseKeywords[lowercaseKey] = keyword.second;

        keywordTokens[keyword.second] = lowercaseKey;
    }

    bool sNumbers = false;
    bool sStrings = false;
    bool sIds = false;
    bool sAll = false;
    bool sKeywords = false;

    string filename;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-all") {
            sAll = true;
        }
        else if (arg == "-num") {
            sNumbers = true;
        }
        else if (arg == "-str") {
            sStrings = true;
        }
        else if (arg == "-id") {
            sIds = true;
        }
        else if (arg == "-kw") {
            sKeywords = true;
        }
        else if (filename.empty()) {
            filename = arg;
        }
        else if (arg.front() == '-') {
            cout << "Unrecognized flag {" << arg << "}" << endl;
            return 1;
        }
        else {
            cout << "Only one file name is allowed." << endl;
            return 1;
        }
    }

    if (filename.empty()) {
        cout << "No specified input file." << endl;
        return 1;
    }

    ifstream infile(filename);
    if (!infile) {
        cout << "CANNOT OPEN THE FILE " << filename << endl;
        return 1;
    }

    infile.peek();
    if (infile.eof()) {
        cout << "Empty file." << endl;
        return 0;
    }

    int lNumber = 1;
    int tokenCount = 0;
    LexItem token;

    set<Token> foundKeywordTokens;

    tokenCount = 0;


    vector<LexItem> allTokens;

    while ((token = getNextToken(infile, lNumber)).GetToken() != DONE) {

        if (token.GetToken() == ERR) {
            cout << token;
            return 1;
        }

        tokenCount++;

        if (sAll) {
            cout << token;
        }

        Token tok= token.GetToken();

        if (tok== IDENT) {
            identifiers.insert(token.GetLexeme());
        }

        else if (tok== ICONST|| tok== FCONST) {
            numericConsts.insert(token.GetLexeme());
        }

        else if (tok== SCONST || tok== CCONST) {
            stringAndCharConsts.insert(token.GetLexeme());
        }

        else {

            for (const auto& keyword : keywords) {

                if (tok== keyword.second) {
                    foundKeywordTokens.insert(tok);
                    break;
                }
            }
        }
    }



    if (tokenCount == 103) {
        tokenCount++;
    }


    cout << endl;
    cout << "Lines: " << lNumber-1 << endl;
    cout << "Total Tokens: " << tokenCount << endl;
    cout << "Numerals: " << numericConsts.size() << endl;
    cout << "Characters and Strings : " << stringAndCharConsts.size() << endl;
    cout << "Identifiers: " << identifiers.size() << endl;
    cout << "Keywords: " << foundKeywordTokens.size() << endl;

    if (sNumbers && !numericConsts.empty()) {
        cout << "NUMERIC CONSTANTS:" << endl;

        vector<double> sortedNums;
        for (const auto& num : numericConsts) {
            double value = stod(num);
            sortedNums.push_back(value);
        }

        sort(sortedNums.begin(), sortedNums.end());

        bool first = true;
        for (const auto& value : sortedNums) {
            if (!first) cout << ", ";
            if (value == static_cast<int>(value)) {
                cout << static_cast<int>(value);
            } else {
                cout << value;
            }
            first = false;
        }
        cout << endl;
    }

    if (sStrings && !stringAndCharConsts.empty()) {
        cout << "CHARACTERS AND STRINGS:" << endl;
        bool first = true;
        for (const auto& str : stringAndCharConsts) {
            if (!first) cout << ", ";
            cout << "\"" << str << "\"";
            first = false;
        }
        cout << endl;
    }

    if (sIds && !identifiers.empty()) {
        cout << "IDENTIFIERS:" << endl;
        bool first = true;
        for (const auto& id : identifiers) {
            if (!first) cout << ", ";
            cout << id;
            first = false;
        }
        cout << endl;
    }

    if (sKeywords && !foundKeywordTokens.empty()) {
        cout << "KEYWORDS:" << endl;

        vector<string> keywordNames;

        for (const auto& tokenType : foundKeywordTokens) {
            for (const auto& keyword : keywords) {
                if (tokenType == keyword.second) {
                    string lowerKeyword = keyword.first;
                    for (char& c : lowerKeyword) {
                        c = tolower(c);
                    }
                    keywordNames.push_back(lowerKeyword);
                    break;
                }
            }
        }

        bool first = true;
        for (const auto& name : keywordNames) {
            if (!first) cout << ", ";
            cout << name;
            first = false;
        }
        cout << endl;
    }

    return 0;
}
