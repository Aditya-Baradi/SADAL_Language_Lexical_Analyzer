//
// Created by aditya on 3/11/2025.
//



#include <cctype>
#include <cstdlib>
#include <limits>
#include "lex.h"


using namespace std;


    map<string, Token> keywords = {
    {"IF", IF}, {"ELSE", ELSE}, {"ELSIF", ELSIF}, {"PUT", PUT}, {"PUTLN", PUTLN},
    {"GET", GET}, {"INTEGER", INT}, {"FLOAT", FLOAT}, {"CHARACTER", CHAR},
    {"STRING", STRING}, {"BOOLEAN", BOOL}, {"PROCEDURE", PROCEDURE},
    {"TRUE", TRUE}, {"FALSE", FALSE}, {"END", END}, {"IS", IS}, {"BEGIN", BEGIN},
    {"THEN", THEN}, {"CONST", CONST}, {"AND", AND}, {"OR", OR}, {"NOT", NOT}, {"MOD", MOD}, {"PUTLINE", PUTLN}
};

LexItem id_or_kw(const string& lexeme, int lnumber) {
    string upperLex = lexeme;

    for (char& character: upperLex) {

        character= toupper(character);
    }

    if (upperLex == "CONSTANT") {

        return LexItem(CONST, lexeme, lnumber);
    }

    auto result= keywords.find(upperLex);

    if (result!= keywords.end()) {

        if (result->second == TRUE || result->second == FALSE) {
            return LexItem(BCONST, lexeme, lnumber);
        }

        return LexItem(result->second, lexeme, lnumber);
    }

    return LexItem(IDENT, lexeme, lnumber);
}

ostream& operator<<(ostream& out, const LexItem& token) {
    switch (token.GetToken()) {
        case FCONST:
            out << "FCONST: (" << token.GetLexeme() << ")" << endl;
            break;
        case ERR:
            out << "ERR: In line " << token.GetLinenum() << ", Error Message {" << token.GetLexeme() << "}" << endl;
        break;
        case ICONST:
            out << "ICONST: (" << token.GetLexeme() << ")" << endl;
        break;
        case IDENT:
            out << "IDENT: <" << token.GetLexeme() << ">" << endl;
            break;
        case BCONST:
            out << "BCONST: (" << token.GetLexeme() << ")" << endl;
        break;
        case CCONST:
            out << "CCONST: '" << token.GetLexeme() << "'" << endl;
            break;
        case SCONST:
            out << "SCONST: \"" << token.GetLexeme() << "\"" << endl;
        break;
        default:
            if (token.GetToken() == INT) {
                out << "INT" << endl;
                return out;
            }
            if (token.GetToken() == CHAR) {
                out << "CHAR" << endl;
                return out;
            }
            if (token.GetToken() == BOOL) {
                out << "BOOL" << endl;
                return out;
            }

            for (const auto& kw : keywords) {
                if (token.GetToken() == kw.second) {
                    out << kw.first << endl;
                    return out;
                }
            }

            switch (token.GetToken()) {
                case PLUS:
                    out << "PLUS"; break;
                case MINUS:
                    out << "MINUS"; break;
                case MOD:
                    out << "MOD"; break;
                case COMMA:
                    out << "COMMA"; break;
                case SEMICOL:
                    out << "SEMICOL"; break;
                case MULT:
                    out << "MULT"; break;
                case DIV:
                    out << "DIV"; break;
                case ASSOP:
                    out << "ASSOP"; break;
                case EQ:
                    out << "EQ"; break;
                case NEQ:
                    out << "NEQ"; break;
                case LTHAN:
                    out << "LTHAN"; break;
                case LTE:
                    out << "LTE"; break;
                case GTE:
                    out << "GTE"; break;
                case AND:
                    out << "AND"; break;
                case EXP:
                    out << "EXP"; break;
                case CONCAT:
                    out << "CONCAT"; break;
                case GTHAN:
                    out << "GTHAN"; break;
                case OR:
                    out << "OR"; break;
                case DOT:
                    out << "DOT"; break;
                case COLON:
                    out << "COLON"; break;
                case INT:
                    out << "INT" << endl; break;
                case NOT:
                    out << "NOT"; break;
                case LPAREN:
                    out << "LPAREN"; break;
                case RPAREN:
                    out << "RPAREN"; break;
                default:
                    out << "Token: " << token.GetToken(); break;
            }
            out << endl;
            break;
    }
    return out;
}

LexItem getNextToken(istream& ins, int& lnumber) {
    char character;
    string lexeme;

    while (ins.get(character)) {

        if (character == '\n') {
            lnumber++;
            continue;
        }


        if (isspace(character)) {
            continue;
        }


        if (character == '-' && ins.peek() == '-') {
            ins.get();
            ins.ignore(numeric_limits<streamsize>::max(), '\n');
            lnumber++;
            continue;
        }


        if (isalpha(character) || character == '_') {
            lexeme = character;
            bool prevUnderscore = (character == '_');

            while (ins.get(character) && (isalnum(character) || character == '_')) {
                if (prevUnderscore && character == '_') {
                    ins.putback(character);
                    break;
                }

                prevUnderscore = (character == '_');
                lexeme += character;
            }

            ins.putback(character);


            if (lexeme[0] == '_') {
                return LexItem(ERR, lexeme, lnumber);
            }

            return id_or_kw(lexeme, lnumber);
        }


        if (isdigit(character)) {
            lexeme = character;
            bool hasDot = false, hasExponent = false;

            while (ins.get(character)) {
                if (character == '.' && ins.peek() == '.') {
                    ins.putback(character);
                    break;
                }

                if (character == '.' && hasDot) {
                    return LexItem(ERR, lexeme + ".", lnumber);
                }
                else if (character == '.' && !hasDot && !hasExponent) {
                    hasDot = true;
                    lexeme += character;
                }
                else if ((character == 'E' || character == 'e') && !hasExponent) {
                    char nextChar = ins.peek();
                    bool validExponent = isdigit(nextChar);

                    if (!validExponent && (nextChar == '+' || nextChar == '-')) {
                        ins.get();
                        char digitAfterSign = ins.peek();
                        ins.putback(nextChar);

                        validExponent = isdigit(digitAfterSign);
                    }

                    if (!validExponent) {
                        ins.putback(character);
                        break;
                    }

                    hasExponent = true;
                    lexeme += character;

                    if (ins.peek() == '+' || ins.peek() == '-') {
                        ins.get(character);
                        lexeme += character;
                    }
                }
                else if (isdigit(character)) {
                    lexeme += character;
                }
                else {
                    ins.putback(character);
                    break;
                }
            }

            if (hasDot) {
                return LexItem(FCONST, lexeme, lnumber);
            } else {
                return LexItem(ICONST, lexeme, lnumber);
            }
        }


        if (character == '"') {
            lexeme = "";
            bool unterminated = true;

            while (ins.get(character)) {
                if (character == '"') {
                    unterminated = false;
                    return LexItem(SCONST, lexeme, lnumber);
                }
                if (character == '\n') {
                    return LexItem(ERR, " Invalid string constant \"" + lexeme, lnumber);
                }
                lexeme += character;
            }

            if (unterminated) {
                return LexItem(ERR, " Invalid string constant \"" + lexeme, lnumber);
            }

            return LexItem(ERR, lexeme, lnumber);
        }


        if (character == '\'') {
            lexeme = "";
            string errorContent = "";

            if (ins.get(character)) {
                if (character == '\n') {
                    return LexItem(ERR, "New line is an invalid character constant.", lnumber);
                } else if (character == '\'') {
                    return LexItem(ERR, "Empty character constant.", lnumber);
                } else {
                    lexeme += character;
                    errorContent += character;

                    char ncharacter;
                    while (ins.get(ncharacter) && ncharacter != '\'' && ncharacter != '\n') {
                        errorContent += ncharacter;
                        if (errorContent.length() < 2) {
                            errorContent += ncharacter;
                        }
                    }

                    if (ncharacter == '\'' && errorContent.length() == 1) {
                        return LexItem(CCONST, lexeme, lnumber);
                    } else if (ncharacter == '\n') {
                        return LexItem(ERR, "Unterminated character constant.", lnumber);
                    } else {
                        if (errorContent.length() > 2) {
                            errorContent = errorContent.substr(0, 2);
                        }
                        return LexItem(ERR, " Invalid character constant '" + errorContent + "'", lnumber);
                    }
                }
            } else {
                return LexItem(ERR, "Unterminated character constant.", lnumber);
            }
        }


        switch (character) {
            case '+':
                return LexItem(PLUS, "+", lnumber);
            case '-':
                return LexItem(MINUS, "-", lnumber);
            case '/':
                if (ins.peek() == '=') {
                    ins.get();
                    return LexItem(NEQ, "/=", lnumber);
                }
                return LexItem(DIV, "/", lnumber);
            case '*':
                if (ins.peek() == '*') {
                    ins.get();
                    return LexItem(EXP, "**", lnumber);
                }
            return LexItem(MULT, "*", lnumber);
            case '>':
                if (ins.peek() == '=') {
                    ins.get();
                    return LexItem(GTE, ">=", lnumber);
                }
            return LexItem(GTHAN, ">", lnumber);
            case '<':
                if (ins.peek() == '=') {
                    ins.get();
                    return LexItem(LTE, "<=", lnumber);
                }
                return LexItem(LTHAN, "<", lnumber);
            case '=':
                return LexItem(EQ, "=", lnumber);
            case '!':
                if (ins.peek() == '=') {
                    ins.get();
                    return LexItem(NEQ, "!=", lnumber);
                }
            return LexItem(ERR, "!", lnumber);
            case '&':
                if (ins.peek() == '&') {
                    ins.get();
                    return LexItem(AND, "&&", lnumber);
                }
                return LexItem(CONCAT, "&", lnumber);
            case '|':
                if (ins.peek() == '|') {
                    ins.get();
                    return LexItem(OR, "||", lnumber);
                }
                return LexItem(ERR, "|", lnumber);
            case '%':
                return LexItem(MOD, "%", lnumber);
            case ':':
                if (ins.peek() == '=') {
                    ins.get();
                    return LexItem(ASSOP, ":=", lnumber);
                }
                return LexItem(COLON, ":", lnumber);
            case '(':
                return LexItem(LPAREN, "(", lnumber);
            case ')':
                return LexItem(RPAREN, ")", lnumber);
            case ',':
                return LexItem(COMMA, ",", lnumber);
            case ';':
                return LexItem(SEMICOL, ";", lnumber);
            case '.':
                if (ins.peek() == '.') {
                    ins.get();
                    return LexItem(CONCAT, "..", lnumber);
                }
                return LexItem(DOT, ".", lnumber);
            default:
                return LexItem(ERR, string(1, character), lnumber);
        }
    }

    return LexItem(DONE, "", lnumber);
}
