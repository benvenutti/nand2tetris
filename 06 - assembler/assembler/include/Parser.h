#ifndef PARSER_INCLUDED_H
#define PARSER_INCLUDED_H

#include <iostream>
#include <string>

using namespace std;

/**
    The HasmCommandType enum class enumerates the types of commands in the
    Hack Assembly Language ("Hasm" stands for "Hack Assembly").
*/
enum class HasmCommandType {
    A_COMMAND,      /**< A-command: \@Xxx */
    C_COMMAND,      /**< C-command: dest=comp;jump */
    L_COMMAND       /**< L-command (pseudo command): (Xxx) */
};

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
            Returns the type of the current command:

                - A_COMMAND for \@Xxx where Xxx is either a symbol of a decimal number;

                - C_COMMAND for <I>dest=comp;jump</I> (dest of jump might be omitted);

                - L_COMMAND (actually, pseudo command) for (Xxx) where Xxx is a
                symbol.

            @return HasmCommandType constant of the current command.
        */
        HasmCommandType commandType();

        /**
            Returns the symbol or decimal Xxx of the current command \@Xxx or (Xxx).
            Should be called only when commandType() is A_COMMAND or L_COMMAND.

            @return A string with the symbol or decimal of the current command.
        */
        string symbol();

        /**
            Returns the dest mnemonic in the current C-command (8 possibilities).
            Should be called only when commandType() is C_COMMAND.

            @return String with the dest mnemonic of the current C-command.
        */
        string dest();

        /**
            Returns the comp mnemonic in the current C-command (28 possibilities).
            Should be called only when commandType() is C_COMMAND.

            @return String with the comp mnemonic of the current C-command.
        */
        string comp();

        /**
            Returns the jump mnemonic in the current C-command (8 possibilities).
            Should be called only when commandType() is C_COMMAND.

            @return String with the jump mnemonic of the current C-command.
        */
        string jump();

        /**
            Resets the parser by rewinding the input stream.
        */
        void reset();

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
            Updates the input stream iterator @ref it to the next valid char.
            A valid char is any char that is not a space nor a control char.

            @return True if there is one or more valid (non comment) chars in the
            input stream to be read. False otherwise.
        */
        bool nextValidChar();

        /**
            Updates the @ref currentLine iterator @ref it right after a comment.

            Skips a comment (single and multi line comments) in the input stream of
            commands. This function will call nextValidLine() once for line comments. For
            multi line comments, it can call nextValidLine() any number of times until
            the enclosing chars of the multi line comment are reached.

            @return True if there is one or more valid (non comment) chars in the
            input stream to be read. False otherwise.
        */
        bool skipComments();

        istream& inputStream;       /**< Input stream that feeds the parser. */

        string currentLine;         /**< Current line from the input stream being parsed. */

        string::iterator it;        /**< String iterator of the @ref currentLine. */

        string command;             /**< The current command. May be updated after advance(). */

        int linePos;                /**< Line number of the current line. */

};

#endif // PARSER_INCLUDED_H
