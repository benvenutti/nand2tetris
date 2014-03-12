#include "Parser.h"
#include <sstream>
#include <iostream>

Parser::Parser(istream& inputStream)
    : inputStream(inputStream), linePos(0)
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
    if (cmdVector.front() == "push") return VmCommandType::C_PUSH;
    if (cmdVector.front() == "pop")  return VmCommandType::C_POP;

    return VmCommandType::C_ARITHMETIC;
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
