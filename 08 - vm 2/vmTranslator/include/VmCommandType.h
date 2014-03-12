#ifndef VM_COMMAND_TYPE_H_INCLUDED
#define VM_COMMAND_TYPE_H_INCLUDED

/**
    The VmCommandType enum class enumerates the types of commands in the Virtual Machine.
*/
enum class VmCommandType {
    C_ARITHMETIC,       /**< Arithmetic command. */
    C_PUSH,             /**< Push command. */
    C_POP,              /**< Pop command. */
    C_LABEL,            /**< Label command. */
    C_GOTO,             /**< Goto command. */
    C_IF,               /**< If Goto command. */
    C_FUNCTION,         /**< Function command. */
    C_RETURN,           /**< Return command. */
    C_CALL              /**< Call command. */
};

#endif // VM_COMMAND_TYPE_H_INCLUDED
