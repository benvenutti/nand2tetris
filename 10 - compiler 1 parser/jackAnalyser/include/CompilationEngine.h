#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <string>
#include <fstream>
#include "JackTokenizer.h"

using namespace std;

class CompilationEngine
{
	public:
		CompilationEngine(string inFileName, string outFileName);
		~CompilationEngine();

        void nextToken();

        bool compile();
		void compileClass();
		void compileClassVarDec();
		void compileSubroutine();
		void compileParameterList();
		void compileSubroutineBody();
		void compileVarDec();
		void compileStatements();
		void compileDo();
		void compileLet();
		void compileWhile();
		void compileReturn();
		void compileIf();
		void compileExpression();
		void compileTerm();
		void compileExpressionList();
		void compileSubroutineCall();
		void compileSubroutineCall(string subName);

	private:
		string inFileName;
		string outFileName;
		ifstream inFile;
		ofstream outFile;
		JackTokenizer jt;
        int ident;
        int identSize;

        void tag(string value);
		void untag(string value);
		bool tagNonTerminal(string nonTerminal);
        bool untagNonTerminal(string nonTerminal);

        bool write(string str);
        bool writeTerminal(string terminal);

        void readKeyword(string keyword, Keyword keywordType);
        void readSymbol(char symbol);
        void readIdentifier();
        void readStringConst();
        void readIntConst();
        void readType();
        void readOp();
        void readUnaryOp();
        void readKeywordConstant();

        bool isOp(char ch);
        bool isStatement(string token);

        inline string trim(string str);


        string errorMessage(TokenType typeExpected, TokenType typeFound, string tokenFound, int linePos);
        string errorMessage(TokenType typeExpected, string tokenExpected, TokenType typeFound, string tokenFound, int linePos);
        string getTokenTypeString(TokenType type);



		void errorMsg(string message);
		void errorMsgExpected(string type, string found);
		void errorMsgExpected(string type, string expected, string found);

};

#endif // COMPILATION_ENGINE_H
