#ifndef VM_COMMAND_OPT_TYPE_H_INCLUDED
#define VM_COMMAND_OPT_TYPE_H_INCLUDED

/**
    The VmCommandOptType enum class enumerates the types of commands in the Virtual
    Machine for the optimisation translation.
*/
enum class VmCommandOptType {
    C_ARITHMETIC_ADD,       /**< Arithmetic add command. */
    C_ARITHMETIC_SUB,       /**< Arithmetic sub command. */
    C_ARITHMETIC_NEG,       /**< Arithmetic neg command. */
    C_ARITHMETIC_EQ,        /**< Arithmetic eq command. */
    C_ARITHMETIC_GT,        /**< Arithmetic gt command. */
    C_ARITHMETIC_LT,        /**< Arithmetic lt command. */
    C_ARITHMETIC_AND,       /**< Arithmetic and command. */
    C_ARITHMETIC_OR,        /**< Arithmetic or command. */
    C_ARITHMETIC_NOT,       /**< Arithmetic not command. */

    C_PUSH_ARGUMENT,        /**< Push argument command. */
    C_PUSH_LOCAL,           /**< Push local command. */
    C_PUSH_STATIC,          /**< Push static command. */
    C_PUSH_CONSTANT,        /**< Push constant command. */
    C_PUSH_THIS,            /**< Push this command. */
    C_PUSH_THAT,            /**< Push that command. */
    C_PUSH_POINTER,         /**< Push pointer command. */
    C_PUSH_TEMP,            /**< Push temp command. */

    C_POP_ARGUMENT,         /**< Pop argument command. */
    C_POP_LOCAL,            /**< Pop local command. */
    C_POP_STATIC,           /**< Pop static command. */
    C_POP_CONSTANT,         /**< Pop constant command. */
    C_POP_THIS,             /**< Pop this command. */
    C_POP_THAT,             /**< Pop that command. */
    C_POP_POINTER,          /**< Pop pointer command. */
    C_POP_TEMP,             /**< Pop temp command. */

    C_LABEL,                /**< Label command. */
    C_GOTO,                 /**< Goto command. */
    C_IF,                   /**< If Goto command. */
    C_FUNCTION,             /**< Function command. */
    C_RETURN,               /**< Return command. */
    C_CALL                  /**< Call command. */
};

#endif // VM_COMMAND_OPT_TYPE_H_INCLUDED
