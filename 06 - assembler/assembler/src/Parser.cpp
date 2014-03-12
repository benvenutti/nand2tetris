#include "Parser.h"

Parser::Parser(istream& inputStream)
    : inputStream(inputStream),
      linePos(0)
{
    readNextLine(); // ???????????
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
    if (!nextValidChar())
        return false;

    command.clear();

    while (it != currentLine.end() && !isspace(*it)) {
        command.append(1, *it);
        ++it;
    }

    return true;
}

HasmCommandType Parser::commandType()
{
    if (command.front() == '@')
        return HasmCommandType::A_COMMAND;
    if (command.front() == '(')
        return HasmCommandType::L_COMMAND;

    return HasmCommandType::C_COMMAND;
}

string Parser::symbol()
{
    if (commandType() == HasmCommandType::A_COMMAND)
        return command.substr(1);

    return command.substr(1, command.size() - 2);
}

string Parser::dest()
{
    // C_COMMAND dest=comp;jump

    if (command.find('=') != string::npos)
        return command.substr(0, command.find('='));

    return "";
}

string Parser::comp()
{
    // C_COMMAND dest=comp;jump

    if (command.find('=') == string::npos)  // dest omitted
        return command.substr(0, command.find(';'));

    if (command.find(';') == string::npos)  // jump omitted
        return command.substr(command.find('=') + 1);

    return command.substr(command.find('=') + 1, command.find(';'));
}

string Parser::jump()
{
    // C_COMMAND dest=comp;jump

    if (command.find(';') != string::npos)
        return command.substr(command.find(';') + 1);

    return "";
}

void Parser::reset()
{
    inputStream.clear();
    inputStream.seekg(0);
    linePos = 0;
    readNextLine(); // ?????????????????????????????????????????
}

bool Parser::readNextLine()
{
    if (getline(inputStream, currentLine)) {
        linePos++;
        it = currentLine.begin();
        return true;
    }

    return false;
}

bool Parser::nextValidLine()
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

bool Parser::nextValidChar()
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

bool Parser::skipComments()
{
    if (*it != '/') // if not char '/' then it can't be a comment
        return true;

    ++it;

    if (*it == '/') // single line comment
        return nextValidLine() && nextValidChar();

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
