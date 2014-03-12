#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include <set>
#include <string>
#include <fstream>

using namespace std;

/**
    The TokenType enum class enumerates the possible types of a token in the
    Jack language.
*/
enum class TokenType {
    kKEYWORD,       /**< a keyword */
    kSYMBOL,        /**< a symbol */
    kIDENTIFIER,    /**< an identifier */
    kINT_CONST,     /**< an integer constante */
    kSTRING_CONST,  /**< a string constant */
    kUNKNOWN        /**< unknown type, used to flag an error */
};

/**
    The KeyWord enum class enumerates the reserved keywords of the Jack language.
*/
enum class Keyword {
    kCLASS,         /**< class */
    kMETHOD,        /**< method */
    kFUNCTION,      /**< function */
    kCONSTRUCTOR,   /**< constructor */
    kINT,           /**< int */
    kBOOLEAN,       /**< boolean */
    kCHAR,          /**< char */
    kVOID,          /**< void */
    kVAR,           /**< var */
    kSTATIC,        /**< static */
    kFIELD,         /**< field */
    kLET,           /**< let */
    kDO,            /**< do */
    kIF,            /**< if */
    kELSE,          /**< else */
    kWHILE,         /**< while */
    kRETURN,        /**< return */
    kTRUE,          /**< true */
    kFALSE,         /**< false */
    kNULL,          /**< null */
    kTHIS,          /**< this */
    kUNKNOWN        /**< unknow, used to flag an error */
};

class JackTokenizer
{
    public:

        /**
            Constructs a JackTokenizer object and binds it to the input file name.

            @param fileName String with the name of the file to open.

            @throw ifstream::failure This exception will be throw if the  object is
            unable to open the input file.

        */
        JackTokenizer(string fileName);

        /**
            Destroys the JackTokenizer object.

            Closes the input file.
        */
        ~JackTokenizer();

        /**
            Returns the name of the input file.

            @return A string with the name of the input file.
        */
        string getFileName();

        /**
            Returns the current token.

            @attention The current token might be modified by advance().

            @return A string with the current token.
        */
        string getToken();

        /**
            Returns the current token tagged with its type.

            @return A string with the current token wrapped in its type tag.
        */
        string getTokenWithTags();

        /**
            Sets the current token. After setting the new token, getToken() will return
            the new token and getTokenWithTags() will return the new token with its
            related tags.

            @warning This will <B> ONLY SET THE CURRENT TOKEN AND THE CURRENT TOKEN
            WITH TAGS</B> (avaiable through getToken() and getTokenWithTags() respectively).
            It will not advance the input file token pointer.

            The main purpose here is to offer a look ahead functionality when parsing. Here is
            a possible example:

            @code
                string currToken = jt.getToken();   // save current token

                if (!jt.advance()) return;
                string lookAhead = jt.getToken();   // get look ahead token

                // decide how to proceed with the look ahead token...

                jt.setToken(currToken);             // set the actual current token

                // process current token here...

                jt.setToken(loadAhead);             // look ahead token now becomes the current token
            @endcode

            @param token A string with the new token.
        */
        void setToken(string token); // for unit testing

        /**
            Returns the current whole line (as read in the input file) that is being
            parsed.

            @return A string with the current line being parsed.
        */
        string getLine();

        /**
            Returns an integer value that represents the line number in the input file
            where the current token was read.

            @return An integer value with the line number where the current token was read.
        */
        int getLinePos();

        /**
            Gets the next token from the input and makes it the current token.
            This method should be called if hasMoreTokens() is true. Initially
            there is no current token.

            @note On the book's original API, advance() actually does not return
            a boolean. Instead, the original API has a function called hasMoreTokens()
            that returns a true if there are more tokens in the input. Here
            advance() takes care of informing either if there are more tokens or not.

            @return True if there are more tokens in the input, false otherwise.
        */
        bool advance();

        /**
            Returns the type of the current token.

            @return kUNKNOWN if the current token is not recognized.
        */
        TokenType tokenType();

        /**
            Returns the keyword which is the current token. Should be called only
            when tokenType() is kKEYWORD.

            @return kUNKNOWN if the current token is not a recognized keyword.
        */
        Keyword keyword();

        /**
            Returns the character which is the current token. Should be called only
            when tokenType() is kSYMBOL.

            @return Current symbol.
        */
        char symbol();

        /**
            Returns the identifier which is the current token. Should be called only
            when tokenType() is kIDENTIFIER.

            @return Current identifier.
        */
        string identifier();

        /**
            Returns the integer value of the current token. Should be called only
            when tokenType() is kINT_CONST.

            @return Integer value of the current integer constant.
        */
        int intVal();

        /**
            Returns the string value of the current token, without the double quotes.
            Should be called only when tokenType() is kSTRING_CONST.

            @return String value of the current string constant without the double quotes.
        */
        string stringVal();

        /**
            Checks whether the token is a valid Jack keyword.

            @param tok The token string to check.

            @return True if the string is a valid Jack keyword, false otherwise.
        */
        bool isKeyword(string tok);

        /**
            Checks whether the token is a valid Jack symbol.

            @param tok The token string to check.

            @return True if the string is a valid Jack symbol, false otherwise.
        */
        bool isSymbol(string tok);

        /**
            Checks whether the token is a valid Jack symbol.

            @param tok The token char to check.

            @return True if the char is a valid Jack symbol, false otherwise.
        */
        bool isSymbol(char tokChar);

        /**
            Checks whether the token is a valid Jack integer constant.

            @attention This function will test whether the string contains an integer constant
            and also check if the actual value of the constant is within the
            range according to the Jack language specification.

            @param tok The token string to check.

            @return True if the string is a valid Jack integer constant, false otherwise.
        */
        bool isIntConst(string tok);

        /**
            Checks whether the token is a valid Jack string constant.

            @param tok The token string to check.

            @return True if the string is a valid Jack string constant, false otherwise.
        */
        bool isStringConst(string tok);

        /**
            Checks whether the token is a valid Jack identifier.

            @param tok The token string to check.

            @return True if the string is a valid Jack identifier, false otherwise.
        */
        bool isIdentifier(string tok);

    private:

        /**
            Reads the next line from the input stream and updates the line count
            (/ref linePos).

            @return True if another line was read successfully. False otherwise.
        */
        bool readNextLine();

        /**
            Updates @ref currentLine with the next valid line. A valid string line
            isa a non empty line.

            @return True if there is a valid (non empty) line to tokenized. False
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
            Updates the input stream iterator @ref it right after a comment.

            Skips a comment (single and multi line comments) in the input stream of
            tokens. This function will call nextValidLine() once for line comments. For
            multi line comments, it can call nextValidLine() any number of times until
            the enclosing chars of the multi line comment are reached.

            @return True if there is one or more valid (non comment) chars in the
            input stream to be read. False otherwise.
        */
        bool skipComments();

        /**
            Populates the sets keywordSet and symbolSet with its related contents (as specified
            by the Jack language). This sets will be accessed by isKeyword() and isSymbol().
        */
        void populateSets();

        /**
            Wraps the current token with its related tag. The tagged token is
            avaiable through getTokenWithTags().
        */
        void tagToken();

        string fileName;            /**< Name of the file being tokenized. */

        string currentLine;         /**< The current line from the stream being
                                    tokenized. */

        string::iterator it;        /**< Iterator of the input stream. */

        string token;               /**< The current token. May be updated after
                                    advance().*/

        string tokenWithTags;       /**< The current /ref token wrapped in its
                                    related tag. */

        int linePos;                /**< Line number of the current line. */

        ifstream file;              /**< Input stream. */

        set<string> keywordSet;     /**< Set holding the language's keywords. */

        set<string> symbolSet;      /**< Set holding the language's symbols. */
};

#endif // JACK_TOKENIZER_H
