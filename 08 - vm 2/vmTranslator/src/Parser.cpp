#include "Parser.h"
#include <sstream>
#include <iostream>

Parser::Parser(istream& inputStream)
    : inputStream(inputStream),
      linePos(0)
{
}

string Parser::getCommand()
{
    return command;
}

string Parser::getLine()
{
    return currentLine;
}

int Parser::getLinePos()
{
    return linePos;
}

bool Parser::advance()
{
    while (nextValidLine()) {
        getTokensFromLine();
        return true;
    }

    return false;
}

VmCommandType Parser::commandType()
{
    if (cmdVector.front() == "push")     return VmCommandType::C_PUSH;
    if (cmdVector.front() == "pop")      return VmCommandType::C_POP;
    if (cmdVector.front() == "label")    return VmCommandType::C_LABEL;
    if (cmdVector.front() == "goto")     return VmCommandType::C_GOTO;
    if (cmdVector.front() == "if-goto")  return VmCommandType::C_IF;
    if (cmdVector.front() == "function") return VmCommandType::C_FUNCTION;
    if (cmdVector.front() == "call")     return VmCommandType::C_CALL;
    if (cmdVector.front() == "return")   return VmCommandType::C_RETURN;

    return VmCommandType::C_ARITHMETIC;
}

VmCommandOptType Parser::commandOptType()
{
    switch (commandType()) {

        case VmCommandType::C_ARITHMETIC:
            if (cmdVector.front() == "add") return VmCommandOptType::C_ARITHMETIC_ADD;
            if (cmdVector.front() == "sub") return VmCommandOptType::C_ARITHMETIC_SUB;
            if (cmdVector.front() == "neg") return VmCommandOptType::C_ARITHMETIC_NEG;
            if (cmdVector.front() == "eq") return  VmCommandOptType::C_ARITHMETIC_EQ;
            if (cmdVector.front() == "gt") return  VmCommandOptType::C_ARITHMETIC_GT;
            if (cmdVector.front() == "lt") return  VmCommandOptType::C_ARITHMETIC_LT;
            if (cmdVector.front() == "and") return VmCommandOptType::C_ARITHMETIC_AND;
            if (cmdVector.front() == "or") return  VmCommandOptType::C_ARITHMETIC_OR;
            if (cmdVector.front() == "not") return VmCommandOptType::C_ARITHMETIC_NOT;
            break;

        case VmCommandType::C_PUSH:
            if (cmdVector.at(1) == "argument") return VmCommandOptType::C_PUSH_ARGUMENT;
            if (cmdVector.at(1) == "local")    return VmCommandOptType::C_PUSH_LOCAL;
            if (cmdVector.at(1) == "static")   return VmCommandOptType::C_PUSH_STATIC;
            if (cmdVector.at(1) == "constant") return VmCommandOptType::C_PUSH_CONSTANT;
            if (cmdVector.at(1) == "this")     return VmCommandOptType::C_PUSH_THIS;
            if (cmdVector.at(1) == "that")     return VmCommandOptType::C_PUSH_THAT;
            if (cmdVector.at(1) == "pointer")  return VmCommandOptType::C_PUSH_POINTER;
            if (cmdVector.at(1) == "temp")     return VmCommandOptType::C_PUSH_TEMP;

        case VmCommandType::C_POP:
            if (cmdVector.at(1) == "argument") return VmCommandOptType::C_POP_ARGUMENT;
            if (cmdVector.at(1) == "local")    return VmCommandOptType::C_POP_LOCAL;
            if (cmdVector.at(1) == "static")   return VmCommandOptType::C_POP_STATIC;
            if (cmdVector.at(1) == "constant") return VmCommandOptType::C_POP_CONSTANT;
            if (cmdVector.at(1) == "this")     return VmCommandOptType::C_POP_THIS;
            if (cmdVector.at(1) == "that")     return VmCommandOptType::C_POP_THAT;
            if (cmdVector.at(1) == "pointer")  return VmCommandOptType::C_POP_POINTER;
            if (cmdVector.at(1) == "temp")     return VmCommandOptType::C_POP_TEMP;

        case VmCommandType::C_LABEL:
            return VmCommandOptType::C_LABEL;

        case VmCommandType::C_GOTO:
            return VmCommandOptType::C_GOTO;

        case VmCommandType::C_IF:
            return VmCommandOptType::C_IF;

        case VmCommandType::C_FUNCTION:
            return VmCommandOptType::C_FUNCTION;

        case VmCommandType::C_CALL:
            return VmCommandOptType::C_CALL;

        default:
            break;
    }

    return VmCommandOptType::C_RETURN;
}

string Parser::arg1()
{
    if (commandType() == VmCommandType::C_ARITHMETIC)
        return cmdVector.front();

    return cmdVector.at(1);
}

int Parser::arg2()
{
    return stoi(cmdVector.at(2));
}

void Parser::reset()
{
    inputStream.clear();
    inputStream.seekg(0);
    linePos = 0;
}

bool Parser::readNextLine()
{
    if (getline(inputStream, currentLine)) {
        linePos++;
        return true;
    }

    return false;
}

bool Parser::nextValidLine()
{
    while (true) {

        if (!readNextLine())
            return false;

        clearCommentsFromLine();
        trimLine();

        if (!currentLine.empty() && currentLine.size() > 1) // size > 1 for '\n' (empty lines)
            return true;
    }
}

void Parser::getTokensFromLine()
{
    cmdVector.clear();
    string::iterator it = currentLine.begin();

    while (it != currentLine.end()) {

        while (isspace(*it) && it != currentLine.end())
            it++;

        string token;

        while (!isspace(*it) && it != currentLine.end()) {
            token.append(1, *it);
            it++;
        }

        if (!token.empty())
            cmdVector.push_back(token);
    }

    command.clear();
    vector<string>::iterator vectorIt;

    /* reassembles command from the vector entries, so it's free of white spaces */
    for (vectorIt = cmdVector.begin(); vectorIt != cmdVector.end(); ++vectorIt)
        command += *vectorIt + " ";

    command.erase(command.find_last_not_of(' ') + 1); // removes last spaced added in the above for loop
}

void Parser::trimLine()
{
    currentLine.erase(0, currentLine.find_first_not_of(' '));   //prefixing spaces
    currentLine.erase(0, currentLine.find_first_not_of('\t'));  //prefixing tabs
    currentLine.erase(currentLine.find_last_not_of(' ') + 1);   //surfixing spaces
    currentLine.erase(currentLine.find_last_not_of('\t') + 1);  //surfixing tabs
}

void Parser::clearCommentsFromLine()
{
    if (currentLine.find("//") != string::npos)
        currentLine.erase(currentLine.find("//"));
}
