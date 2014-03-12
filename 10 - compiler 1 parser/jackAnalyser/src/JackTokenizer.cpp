#include "JackTokenizer.h"

JackTokenizer::JackTokenizer(string fileName)
    : fileName(fileName)
{
    file.exceptions(ifstream::failbit | ifstream::badbit);

    try {
        file.open(fileName);
    }
    catch (ifstream::failure ex) {
        throw ex;
    }

    linePos = 0;
    readNextLine();
    populateSets();
}

JackTokenizer::~JackTokenizer()
{
    if (file.is_open())
        file.close();
}

string JackTokenizer::getFileName()
{
    return fileName;
}

string JackTokenizer::getToken()
{
    return token;
}

string JackTokenizer::getTokenWithTags()
{
    return tokenWithTags;
}

void JackTokenizer::setToken(string token)
{
    this->token = token;
    tagToken();
}

string JackTokenizer::getLine()
{
    return currentLine;
}

int JackTokenizer::getLinePos()
{
    return linePos;
}

bool JackTokenizer::isKeyword(string tok)
{
    return keywordSet.find(tok) != keywordSet.end();
}

bool JackTokenizer::isSymbol(string tok)
{
    return symbolSet.find(tok) != symbolSet.end();
}

bool JackTokenizer::isSymbol(char tokChar)
{
    string tokStr(1, tokChar);
    return isSymbol(tokStr);
}

bool JackTokenizer::isIntConst(string tok)
{
    string::const_iterator it = tok.begin();

    while (it != tok.end() && isdigit(*it))
        ++it;

    if (!tok.empty() && it == tok.end()) {
        int num = stoi(tok);
        return num >= 0 && num <= 32767; // possible Jack integer range
    }

    return false;
}

bool JackTokenizer::isStringConst(string tok)
{
    if (tok.front() == '\"' && tok.back() == '\"') {
        string::const_iterator it = tok.begin();

        while (it != tok.end() && *it != '\n') // Jack str const cannot have a new line
            ++it;

        return it == tok.end();
    }

    return false;
}

bool JackTokenizer::isIdentifier(string tok)
{
    if (isdigit(tok.front()))
        return false;

    string::const_iterator it = tok.begin();

    while (it != tok.end() && (isalpha(*it) || isdigit(*it) || *it == '_'))
        ++it;

    return it == tok.end();
}

TokenType JackTokenizer::tokenType()
{
    if (isKeyword(token))
        return TokenType::kKEYWORD;
    if (isSymbol(token))
        return TokenType::kSYMBOL;
    if (isStringConst(token))
        return TokenType::kSTRING_CONST;
    if (isIntConst(token))
        return TokenType::kINT_CONST;
    if (isIdentifier(token))
        return TokenType::kIDENTIFIER;

    return TokenType::kUNKNOWN;
}

Keyword JackTokenizer::keyword()
{
    if (token == "class")
        return Keyword::kCLASS;
    if (token == "method")
        return Keyword::kMETHOD;
    if (token == "function")
        return Keyword::kFUNCTION;
    if (token == "constructor")
        return Keyword::kCONSTRUCTOR;
    if (token == "int")
        return Keyword::kINT;
    if (token == "boolean")
        return Keyword::kBOOLEAN;
    if (token == "char")
        return Keyword::kCHAR;
    if (token == "void")
        return Keyword::kVOID;
    if (token == "var")
        return Keyword::kVAR;
    if (token == "static")
        return Keyword::kSTATIC;
    if (token == "field")
        return Keyword::kFIELD;
    if (token == "let")
        return Keyword::kLET;
    if (token == "do")
        return Keyword::kDO;
    if (token == "if")
        return Keyword::kIF;
    if (token == "else")
        return Keyword::kELSE;
    if (token == "while")
        return Keyword::kWHILE;
    if (token == "return")
        return Keyword::kRETURN;
    if (token == "true")
        return Keyword::kTRUE;
    if (token == "false")
        return Keyword::kFALSE;
    if (token == "null")
        return Keyword::kNULL;

    return Keyword::kTHIS;
}

char JackTokenizer::symbol()
{
    return token.front();
}

string JackTokenizer::identifier()
{
    return token;
}

int JackTokenizer::intVal()
{
    return stoi(token);
}

string JackTokenizer::stringVal()
{
    return token.substr(1, token.size() - 2); // -2 for the initial and final double quotes
}

void JackTokenizer::populateSets()
{
    keywordSet.insert("class");
    keywordSet.insert("constructor");
    keywordSet.insert("function");
    keywordSet.insert("method");
    keywordSet.insert("field");
    keywordSet.insert("static");
    keywordSet.insert("var");
    keywordSet.insert("int");
    keywordSet.insert("char");
    keywordSet.insert("boolean");
    keywordSet.insert("void");
    keywordSet.insert("true");
    keywordSet.insert("false");
    keywordSet.insert("null");
    keywordSet.insert("this");
    keywordSet.insert("let");
    keywordSet.insert("do");
    keywordSet.insert("if");
    keywordSet.insert("else");
    keywordSet.insert("while");
    keywordSet.insert("return");

    symbolSet.insert("{");
    symbolSet.insert("}");
    symbolSet.insert("(");
    symbolSet.insert(")");
    symbolSet.insert("[");
    symbolSet.insert("]");
    symbolSet.insert(".");
    symbolSet.insert(",");
    symbolSet.insert(";");
    symbolSet.insert("+");
    symbolSet.insert("-");
    symbolSet.insert("*");
    symbolSet.insert("/");
    symbolSet.insert("&");
    symbolSet.insert("|");
    symbolSet.insert("<");
    symbolSet.insert(">");
    symbolSet.insert("=");
    symbolSet.insert("~");
}

bool JackTokenizer::readNextLine()
{
    try {
        if (getline(file, currentLine)) {
            linePos++;
            it = currentLine.begin();
            return true;
        }
    } catch (ifstream::failure ex) {
        // throw ex;
    }

    return false;
}

bool JackTokenizer::nextValidLine()
{
    if (!readNextLine())
        return false;

    while (true) {
        if (currentLine.empty() || it == currentLine.end()) {
            if (!readNextLine())
                return false;
        } else {
            break;
        }
    }

    return true;
}

bool JackTokenizer::nextValidChar()
{
    while (isspace(*it) || it == currentLine.end()) { // skipping whitespaces

        if (*it == '\n' || it == currentLine.end()) { // if so, read next valid line
            if (!nextValidLine())
                return false;
        } else {
            it++;
        }
    }

    if (!skipComments()) // skip comments here
        return false;

    return true;
}

bool JackTokenizer::skipComments()
{
    if (*it != '/') // if not char '/' then it can't be a comment
        return true;

    ++it;

    if (*it == '/') { // single line comment
        return nextValidLine() && nextValidChar();
    }

    if (*it == '*') { // multi line comment

        while (true) {

            int dist = distance(currentLine.begin(), it);
            size_t pos = currentLine.find("*/", dist);

            if (pos != string::npos) {
                it = currentLine.begin() + pos + 2; // + 2 for num of chars in "*/"
                return nextValidChar();
            }

            if (!nextValidLine())
                return false;
        }
    }

    --it; // if not a comment, go back to previous valid char
    return true;
}

bool JackTokenizer::advance()
{
    if (!nextValidChar())
        return false;

    token.clear();

    if (isSymbol(*it)) {
        token.append(1, *it);
        tagToken();
        ++it;
        return true;
    }

    while (it != currentLine.end() && !isspace(*it) && !isSymbol(*it)) {
        token.append(1, *it);
        ++it;
    }

    tagToken();

    return true;
}

void JackTokenizer::tagToken()
{
    switch (tokenType()) {

        case TokenType::kKEYWORD:
            tokenWithTags = "<keyword> " + token + " </keyword>";
            break;

        case TokenType::kSYMBOL:
            if (symbol() == '<')
                tokenWithTags = "<symbol> &lt; </symbol>";
            else if (symbol() == '>')
                tokenWithTags = "<symbol> &gt; </symbol>";
            else if (symbol() == '\"')
                tokenWithTags = "<symbol> &quot; </symbol>";
            else if (symbol() == '&')
                tokenWithTags = "<symbol> &amp; </symbol>";
            else {
                string sym(1, symbol());
                tokenWithTags = "<symbol> " + sym + " </symbol>";
            }
            break;

        case TokenType::kIDENTIFIER:
            tokenWithTags = "<identifier> " + identifier() + " </identifier>";
            break;

        case TokenType::kINT_CONST:
            tokenWithTags = "<integerConstant> " + to_string(intVal()) +" </integerConstant>";
            break;

        case TokenType::kSTRING_CONST:
            tokenWithTags = "<stringConstant> " + stringVal() + " </stringConstant>";
            break;

        default:
            tokenWithTags = "<ERROR> " + token + " </ERROR>";
            break;
    }
}
