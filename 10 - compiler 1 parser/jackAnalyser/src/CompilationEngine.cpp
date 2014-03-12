#include "CompilationEngine.h"
#include "CompilationException.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

CompilationEngine::CompilationEngine(string inFileName, string outFileName)
	: inFileName(inFileName), outFileName(outFileName), jt(inFileName),
	  ident(0), identSize(2)
{
    try {
        inFile.open(inFileName);
        outFile.open(outFileName);
    } catch (ifstream::failure ex) {
        throw ex;
    }
}

CompilationEngine::~CompilationEngine()
{
	if (inFile.is_open())
		inFile.close();
	if (outFile.is_open())
		outFile.close();
}

void CompilationEngine::nextToken()
{
    if (!jt.advance())
        throw CompilationException("could not read next token");
}

bool CompilationEngine::compile()
{
    try {
        compileClass();
    } catch (CompilationException e) {
        cerr << e.what() << endl;
        return false;
    }

    return true;
}

void CompilationEngine::compileClass()
{
	/* 'class' className '{' classVarDec* subroutineDec* '}' */

    tagNonTerminal("class");
    nextToken();
    readKeyword("class", Keyword::kCLASS);
    nextToken();
    readIdentifier();
    nextToken();
    readSymbol('{');
    nextToken();

    while (jt.tokenType() == TokenType::kKEYWORD
           && (jt.keyword() == Keyword::kFIELD || jt.keyword() == Keyword::kSTATIC))
        compileClassVarDec();

    while (jt.tokenType() == TokenType::kKEYWORD && (jt.keyword() == Keyword::kCONSTRUCTOR
           || jt.keyword() == Keyword::kFUNCTION || jt.keyword() == Keyword::kMETHOD))
        compileSubroutine();

    readSymbol('}');
    untagNonTerminal("class");

    if (jt.advance()) // jack language does not allow anything else beyond the class
        throw CompilationException("jack language does not allow anything else beyond the class");
}

void CompilationEngine::compileClassVarDec()
{
    /* ('static' | 'field') type varName (',' varName)* ';' */

    tagNonTerminal("classVarDec");

    if (jt.keyword() == Keyword::kFIELD)
        readKeyword("field", Keyword::kFIELD);
    else
        readKeyword("static", Keyword::kSTATIC);

    nextToken();
    readType();
    nextToken();
    readIdentifier();
    nextToken();

    while (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == ',') {
        readSymbol(',');
        nextToken();
        readIdentifier();
        nextToken();
    }

    readSymbol(';');
    untagNonTerminal("classVarDec");
    nextToken();
}

void CompilationEngine::compileSubroutine()
{
    /* ('constructor' | 'function' | 'method')
       ('void' | type) subroutineName '(' parameterList ')'
       subroutineBody */

    tagNonTerminal("subroutineDec");

    if (jt.keyword() == Keyword::kCONSTRUCTOR)
        readKeyword("constructor", Keyword::kCONSTRUCTOR);
    else if (jt.keyword() == Keyword::kFUNCTION)
        readKeyword("function", Keyword::kFUNCTION);
    else
        readKeyword("method", Keyword::kMETHOD);

    nextToken();

    if (jt.tokenType() == TokenType::kKEYWORD && jt.keyword() == Keyword::kVOID)
        readKeyword("void", Keyword::kVOID);
    else
        readType();

    nextToken();
    readIdentifier();
    nextToken();
    readSymbol('(');
    nextToken();
    compileParameterList();
    readSymbol(')');
    nextToken();
    compileSubroutineBody();
    untagNonTerminal("subroutineDec");
}

void CompilationEngine::compileParameterList()
{
    /* ((type varName)(',' type varName)*)? */

    tagNonTerminal("parameterList");

    if (jt.tokenType() != TokenType::kSYMBOL) {

        readType();
        nextToken();
        readIdentifier();
        nextToken();

        while (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == ',') {
            readSymbol(',');
            nextToken();
            readType();
            nextToken();
            readIdentifier();
            nextToken();
        }
    }

    untagNonTerminal("parameterList");
}

void CompilationEngine::compileSubroutineBody()
{
    /* '{' varDec* statements '}' */

    tagNonTerminal("subroutineBody");
    readSymbol('{');
    nextToken();

    while (jt.tokenType() == TokenType::kKEYWORD && jt.keyword() == Keyword::kVAR)
        compileVarDec();

    compileStatements();
    readSymbol('}');
    untagNonTerminal("subroutineBody");
    nextToken();
}

void CompilationEngine::compileVarDec()
{
    /* 'var' type varName (',' varName)* ';' */

    tagNonTerminal("varDec");
    readKeyword("var", Keyword::kVAR);
    nextToken();
    readType();
    nextToken();
    readIdentifier();
    nextToken();

    while (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == ',') {
        readSymbol(',');
        nextToken();
        readIdentifier();
        nextToken();
    }

    readSymbol(';');
    untagNonTerminal("varDec");
    nextToken();
}

void CompilationEngine::compileStatements()
{
    /*  statements: statement*
        statement: letStatement | ifStatement | whileStatement | doStatement | returnStatement */

    tagNonTerminal("statements");

    while (jt.tokenType() == TokenType::kKEYWORD && isStatement(jt.getToken())) {

        switch (jt.keyword()) {

            case Keyword::kLET:
                compileLet();
                break;

            case Keyword::kDO:
                compileDo();
                break;

            case Keyword::kIF:
                compileIf();
                break;

            case Keyword::kWHILE:
                compileWhile();
                break;

            case Keyword::kRETURN:
                compileReturn();

            default:
                break;

        }
    }

    untagNonTerminal("statements");
}

void CompilationEngine::compileDo()
{
    /* 'do' subroutineCall ';' */

    tagNonTerminal("doStatement");
    readKeyword("do", Keyword::kDO);
    nextToken();
    compileSubroutineCall();
    readSymbol(';');
    untagNonTerminal("doStatement");
    nextToken();
}

void CompilationEngine::compileLet()
{
    /*'let' varName('[' expression ']')? '=' expression ';' */

    tagNonTerminal("letStatement");
    readKeyword("let", Keyword::kLET);
    nextToken();
    readIdentifier();
    nextToken();

    if (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == '[') {
        readSymbol('[');
        nextToken();
        compileExpression();
        readSymbol(']');
        nextToken();
    }

    readSymbol('=');
    nextToken();
    compileExpression();
    readSymbol(';');
    untagNonTerminal("letStatement");
    nextToken();
}

void CompilationEngine::compileWhile()
{
    /* 'while' '(' expression ')' '{' statements '}' */

    tagNonTerminal("whileStatement");
    readKeyword("while", Keyword::kWHILE);
    nextToken();
    readSymbol('(');
    nextToken();
    compileExpression();
    readSymbol(')');
    nextToken();
    readSymbol('{');
    nextToken();
    compileStatements();
    readSymbol('}');
    untagNonTerminal("whileStatement");
    nextToken();
}

void CompilationEngine::compileReturn()
{
    /* 'return' expression? ';' */

    tagNonTerminal("returnStatement");
    readKeyword("return", Keyword::kRETURN);
    nextToken();

    if (jt.tokenType() != TokenType::kSYMBOL || (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() != ';'))
        compileExpression();

    readSymbol(';');
    untagNonTerminal("returnStatement");
    nextToken();
}

void CompilationEngine::compileIf()
{
    /*  'if' '(' expression ')' '{' statements '}'
        ('else' '{' statements '}')? */

    tagNonTerminal("ifStatement");
    readKeyword("if", Keyword::kIF);
    nextToken();
    readSymbol('(');
    nextToken();
    compileExpression();
    readSymbol(')');
    nextToken();
    readSymbol('{');
    nextToken();
    compileStatements();
    readSymbol('}');
    nextToken();

    if (jt.tokenType() == TokenType::kKEYWORD && jt.keyword() == Keyword::kELSE) {
        readKeyword("else", Keyword::kELSE);
        nextToken();
        readSymbol('{');
        nextToken();
        compileStatements();
        readSymbol('}');
        nextToken();
    }

    untagNonTerminal("ifStatement");
}

void CompilationEngine::compileExpression()
{
    /* term (op term)* */

    tagNonTerminal("expression");
    compileTerm();

    while (jt.tokenType() == TokenType::kSYMBOL && isOp(jt.symbol())) {
        readOp();
        nextToken();
        compileTerm();
    }

    untagNonTerminal("expression");
}

void CompilationEngine::compileTerm() // 362
{
    //  integerConst | stringConst | keywordConst |
    //    varName | varName '[' expression ']' | subroutineCall |
    //    '(' expression ')' | unaryOp term

    tagNonTerminal("term");

    string id;
    string oldTok;

    switch(jt.tokenType()) {

        case TokenType::kINT_CONST:
            readIntConst();
            break;

        case TokenType::kSTRING_CONST:
            readStringConst();
            break;

        case TokenType::kKEYWORD:
            readKeywordConstant();
            break;

        case TokenType::kIDENTIFIER:

            id = jt.getToken(); // save current token
            nextToken();

            if (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == '[') {

                oldTok = jt.getToken();
                jt.setToken(id);
                readIdentifier();
                jt.setToken(oldTok);

                readSymbol('[');
                nextToken();
                compileExpression();
                readSymbol(']');
                nextToken();

            } else if (jt.tokenType() == TokenType::kSYMBOL && (jt.symbol() == '.' || jt.symbol() == '(')) {

                compileSubroutineCall(id);

            } else {

                oldTok = jt.getToken();
                jt.setToken(id);
                readIdentifier();
                jt.setToken(oldTok);

            }

            untagNonTerminal("term");
            return; // has already advanced
            break;

        case TokenType::kSYMBOL:

            if (jt.symbol() == '(') { // '(' expression ')'

                readSymbol('(');
                nextToken();
                compileExpression();
                readSymbol(')');

            } else { // unaryOp term

                readUnaryOp();
                nextToken();
                compileTerm();
                untagNonTerminal("term");
                return; // has already advanced

            }

            break;

        default:
            break;
    }

    nextToken();
    untagNonTerminal("term");
}

void CompilationEngine::compileExpressionList()
{
    // (expression(',' expression)*)?

    tagNonTerminal("expressionList");

    if (jt.tokenType() != TokenType::kSYMBOL || (jt.symbol() == '(' ))
        compileExpression();

    while (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == ',') {
        readSymbol(',');
        nextToken();
        compileExpression();
    }

    untagNonTerminal("expressionList");
}

void CompilationEngine::compileSubroutineCall()
{
    /*
        subroutineName '(' expressionList ')' |
        (className | varName) '.' subroutineName '(' expressionList ')'
    */

    readIdentifier();
    nextToken();

    if (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == '(') {

        readSymbol('(');
        nextToken();
        compileExpressionList();
        readSymbol(')');

    } else {

        readSymbol('.');
        nextToken();
        readIdentifier();
        nextToken();
        readSymbol('(');
        nextToken();
        compileExpressionList();
        readSymbol(')');

    }

    nextToken();
}

void CompilationEngine::compileSubroutineCall(string subName)
{
    /*
        subroutineName '(' expressionList ')' |
        (className | varName) '.' subroutineName '(' expression ')'
    */

    string oldTok = jt.getToken();
    jt.setToken(subName);
    readIdentifier();
    jt.setToken(oldTok);

    if (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == '(') {

        readSymbol('(');
        nextToken();
        compileExpressionList();
        readSymbol(')');

    } else {

        readSymbol('.');
        nextToken();
        readIdentifier();
        nextToken();
        readSymbol('(');
        nextToken();
        compileExpressionList();
        readSymbol(')');

    }

    nextToken();
}

void CompilationEngine::tag(string value)
{
	outFile << "<" << value << ">" << endl;
}

void CompilationEngine::untag(string value)
{
	outFile << "</" << value << ">" << endl;
}

bool CompilationEngine::write(string str)
{
    string space(ident * identSize, ' ');
    outFile << space << str << endl;
    return outFile.good();
}

bool CompilationEngine::writeTerminal(string terminal)
{
    return write(terminal);
}

bool CompilationEngine::tagNonTerminal(string nonTerminal)
{
    bool b = write("<" + nonTerminal + ">");
    ident++;
    return b;
}

bool CompilationEngine::untagNonTerminal(string nonTerminal)
{
    ident--;
    return write("</" + nonTerminal + ">");
}

string CompilationEngine::getTokenTypeString(TokenType type)
{
    if (type == TokenType::kKEYWORD)
        return "keyword";
    if (type == TokenType::kSYMBOL)
        return "symbol";
    if (type == TokenType::kSTRING_CONST)
        return "string constant";
    if (type == TokenType::kINT_CONST)
        return "integer constant";
    if (type == TokenType::kIDENTIFIER)
        return "identifier";

    return "invalid";
}

string CompilationEngine::errorMessage(TokenType typeExpected, TokenType typeFound, string tokenFound, int linePos)
{
    string typeStr = getTokenTypeString(typeExpected);
    string typeFoundStr = getTokenTypeString(typeFound);
    string r1 = "error at line " + to_string(linePos) + ": ";
    string r2 = "expected " + typeStr + " but found " + typeFoundStr + " '" + tokenFound + "'";
    return r1 + r2;
}

string CompilationEngine::errorMessage(TokenType typeExpected, string tokenExpected, TokenType typeFound, string tokenFound, int linePos)
{
    string typeStr = getTokenTypeString(typeExpected);
    string typeFoundStr = getTokenTypeString(typeFound);
    string r1 = "error at line " + to_string(linePos) + ": ";
    string r2 = "expected " + typeStr + " '" + tokenExpected + "' but found " + typeFoundStr + " '" + tokenFound + "'";
    return r1 + r2;
}

/*
    ##############
    read functions
    ##############
*/

void CompilationEngine::readKeyword(string keyword, Keyword keywordType)
{
    if (jt.tokenType() == TokenType::kKEYWORD && jt.keyword() == keywordType)
        writeTerminal(jt.getTokenWithTags());
    else
        throw CompilationException(errorMessage(TokenType::kKEYWORD, keyword, jt.tokenType(), jt.getToken(), jt.getLinePos()).c_str());
}

void CompilationEngine::readSymbol(char symbol)
{
    if (jt.tokenType() == TokenType::kSYMBOL && jt.symbol() == symbol) {
        writeTerminal(jt.getTokenWithTags());
        return;
    }

    string exp(1, symbol);
    throw CompilationException(errorMessage(TokenType::kSYMBOL, exp, jt.tokenType(), jt.getToken(), jt.getLinePos()).c_str());
}

void CompilationEngine::readIdentifier()
{
    if (jt.tokenType() == TokenType::kIDENTIFIER) {
        writeTerminal(jt.getTokenWithTags());
        return;
    }

    throw CompilationException(errorMessage(TokenType::kIDENTIFIER, jt.tokenType(), jt.getToken(), jt.getLinePos()).c_str());
}

void CompilationEngine::readStringConst()
{
    if (jt.tokenType() == TokenType::kSTRING_CONST) {
        writeTerminal(jt.getTokenWithTags());
        return;
    }

    throw CompilationException(errorMessage(TokenType::kSTRING_CONST, jt.tokenType(), jt.getToken(), jt.getLinePos()).c_str());
}

void CompilationEngine::readIntConst()
{
    if (jt.tokenType() == TokenType::kINT_CONST) {
        writeTerminal(jt.getTokenWithTags());
        return;
    }

    errorMsgExpected("integer constant", jt.getToken());
}

void CompilationEngine::readType()
{
    if (jt.tokenType() == TokenType::kKEYWORD) {

        switch (jt.keyword()) {

            case Keyword::kINT:
                readKeyword("int", Keyword::kINT);
                break;

            case Keyword::kCHAR:
                readKeyword("char", Keyword::kCHAR);
                break;

            case Keyword::kBOOLEAN:
                readKeyword("boolean", Keyword::kBOOLEAN);
                break;

            default:
                break;
        }

    } else {
        readIdentifier();
    }
}

void CompilationEngine::readOp()
{
    if (jt.tokenType() == TokenType::kSYMBOL) {

        char sym = jt.symbol();

        if (isOp(sym)) {
            writeTerminal(jt.getTokenWithTags());
            return;
        }

    }

    errorMsgExpected("op", jt.getToken()); // ?????????????????????????????
}

void CompilationEngine::readUnaryOp()
{
    if (jt.tokenType() == TokenType::kSYMBOL) {

        char sym = jt.symbol();

        if (sym == '-' || sym == '~') {
            writeTerminal(jt.getTokenWithTags());
            return;
        }
    }

    errorMsgExpected("unary op", jt.getToken()); // ?????????????????????????????
}

void CompilationEngine::readKeywordConstant()
{
    if (jt.tokenType() == TokenType::kKEYWORD) {

        switch (jt.keyword()) {

            case Keyword::kTRUE:
                readKeyword("true", Keyword::kTRUE);
                break;

            case Keyword::kFALSE:
                readKeyword("false", Keyword::kFALSE);
                break;

            case Keyword::kNULL:
                readKeyword("null", Keyword::kNULL);
                break;

            case Keyword::kTHIS:
                readKeyword("this", Keyword::kTHIS);
                break;

            default:
                break;

        }
    }
}

bool CompilationEngine::isOp(char ch)
{
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/'
        || ch == '&' || ch == '|' || ch == '<' || ch == '>'
        || ch == '=');
}

bool CompilationEngine::isStatement(string token)
{
    return (token == "let" || token == "if" || token == "while"
        || token == "do" || token == "return");
}

void CompilationEngine::errorMsg(string message)
{
    cerr << message << endl;
}

void CompilationEngine::errorMsgExpected(string type, string found)
{
    string msg = "expected " + type + " but found " + "'" + found + "'";
    cerr << "line " << jt.getLinePos() << ": " << msg << endl;
    string str = trim(jt.getLine());
    cerr << "\t" << str << endl;
}

void CompilationEngine::errorMsgExpected(string type, string expected, string found)
{
    string msg = "expected " + type + " '" + expected + "' but found " + "'" + found + "'";
    cerr << "line " << jt.getLinePos() << ": " << msg << endl;
    string str = trim(jt.getLine());
    cerr << "  -> \"" << str << "\""<< endl;
    cerr << endl << "stack trace:" << endl;
}

inline string CompilationEngine::trim(string str)
{
    str.erase(0, str.find_first_not_of(' '));   //prefixing spaces
    str.erase(0, str.find_first_not_of('\t'));  //prefixing tabs
    str.erase(str.find_last_not_of(' ') + 1);   //surfixing spaces
    str.erase(str.find_last_not_of('\t') + 1);  //surfixing tabs
    return str;
}

// original 1124
