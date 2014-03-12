#ifndef PARSER_INCLUDED_H
#define PARSER_INCLUDED_H

#include "VmCommandType.h"
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class Parser
{

    public:

        /**
            Constructs a Parser object and binds it to the input stream.

            @param inputStream Input stream object that feeds the parser.
        */
        Parser(istream& inputStream);

        /**
            Returns the current command.

            @return A string with the current command.
        */
        string getCommand();

        /**
            Returns the current whole line (as read in the input stream) that is being
            parsed.

            @return A string with the current line being parsed.
        */
        string getLine();

        /**
            Returns an integer value that represents the line/entry number in the input stream
            where the current command was read.

            @return An integer value with the line number where the current command was read.
        */
        int getLinePos();

        /**
            Gets the next command from the input and makes it the current command.
            Initially there is no current command.

            @note On the book's original API, advance() actually does not return
            a boolean. Instead, the original API has a function called hasMoreCommands()
            that returns a true if there are more commands in the input stream. Here
            advance() takes care of informing if there are more commands or not.

            @return True if there are more commands in the input, false otherwise.
        */
        bool advance();

        /**
            Returns the type of the current VM command. C_ARITHMETIC is returned for all
            arithmetic commands.

            @return VmCommandType constant of the current command.
        */
        VmCommandType commandType();

        /**
            Returns the first argument of the current command. In the case of C_ARITHMETIC,
            the command itself (add, sub, etc.) is returned. Should not be called if the
            current commmand is C_RETURN.

            @return A string with the first argument of the current commmand. If the current
            command is a C_ARITHMETICH command, a string with the command itself is returned.
        */
        string arg1();

        /**
            Returns the second argument of the current command. Should be called only if the
            current command is C_PUSH, C_POP, C_FUNCTION or C_CALL.

            @return An integer with the value of the second argument of the current command.
        */
        int arg2();

    private:

        /**
            Reads the next line from the input stream and updates both line count
            @ref linePos and @ref currentLine.

            @return True if another line was read successfully. False otherwise.
        */
        bool readNextLine();

        /**
            Updates @ref currentLine with the next valid line. A valid string line
            is a non empty line. Line count @ref linePos is also updated.

            @return True if there is a valid (non empty) line to parse. False
            otherwise.
        */
        bool nextValidLine();

        /**
            Removes single line comments (//) from @ref currentLine.
        */
        void clearCommentsFromLine();

        /**
            Removes leading and trailing spaces from @ref currentLine.
        */
        void trimLine();

        /**
            Reads the @ref currentLine and parses its contents into @ref cmdVector entries. The member
            @ref command will also be updated to represent the current command.

            @warning Call nextValidLine() before calling this method. This is due to the fact that
            @ref currentLine should be without comments for getTokensFromLine() to function
            properly.
        */
        void getTokensFromLine();

        istream& inputStream;       /**< Input stream that feeds the parser. */

        string command;             /**< The current command. May be updated after advance().*/

        string currentLine;         /**< Current line from the input stream being parsed. */

        int linePos;                /**< Line number of the current line. */

        vector<string> cmdVector;   /**< Vector that holds the tokens from the current command. */

};

#endif // PARSER_INCLUDED_H
