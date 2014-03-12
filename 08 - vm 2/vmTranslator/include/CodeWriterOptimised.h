#ifndef CODE_WRITER_OPTIMISED_INCLUDED_H
#define CODE_WRITER_OPTIMISED_INCLUDED_H

#include "CodeWriter.h"

class CodeWriterOptimised : public CodeWriter
{

	public:

		/**
			Constructs a CodeWriter object and binds it to the output stream.

			@param outputStream Output stream where Hack assembly code will be written.

			@param programName Name of the program.

			@param writeComments Flags the CodeWriter to output comments (original VM commands plus
			program and module headers and other comments) on the .asm file.
		*/
		CodeWriterOptimised(ostream& outputStream, string programName, bool writeComments);

		/**
			Writes assembly code that effects the VM initialization, also called <I>bootstrap
			code</I>. This code must be placed at the beginning of the output file.
		*/
		void writeInit();

		/**
			Writes assembly code that effects the call command.

			@param functionName String with the name of the function to call.

			@param numArgs Number of arguments required by the function.
		*/
		void writeCall(string functionName, int numArgs);

		/**
			Writes assembly code that effects the return command.
		*/
		void writeReturn();

		/**
			Writes assembly code that effects the function command.

			@param functionName String with the name of the function.

			@param numLocals Number of local variables required by the function.
		*/
		void writeFunction(string functionName, int numLocals);

        /**
		*/
		void writeGlobalFunctions();

        /**
        */
        void writeSeqPushConstantAdd(int index);

        /**
        */
        void writeSeqPushConstantSub(int index);

        /**
        */
        void writeSeqPushConstantNeg(int index);

        /**
        */
        void writeSeqPushArgLocalThisThatAdd(string segment, int index);

        /**
        */
        void writeSeqPushArgLocalThisThatSub(string segment, int index);

        /**
        */
        void writeSeqPushStaticAdd(int index);

        /**
        */
        void writeSeqPushTempPopThat(int tempIndex, int thatIndex);

        /**
        */
        void writeSeqPushArgumentPopPointer(int argIndex, int pointerIndex);

        /**
        */
        void writeSeqPushConstantPushTempAdd(int constant, int index);

	private:

		/**
			Writes Hack assembly code for the 'eq' arithmetic VM command.
		*/
		void writeArithmeticEq();

		/**
			Writes Hack assembly code for the 'gt' arithmetic VM command.
		*/
		void writeArithmeticGt();

		/**
			Writes Hack assembly code for the 'lt' arithmetic VM command.
		*/
		void writeArithmeticLt();

		/**
			Writes Hack assembly code for the push VM command.

			@param segment String with the name of the segment which data will be pushed
			to the stack.

			@param index Index of the data being pushed.
		*/
		void writePush(string segment, int index);

		/**
        */
		void writePushConstant(int index);

		int eqGlobalID; 			/**< Global ID for VM eq command labels. */

		int gtGlobalID; 			/**< Global ID for VM gt command labels. */

		int ltGlobalID; 			/**< Global ID for VM lt command labels. */

};

#endif // CODE_WRITER_OPTIMISED_INCLUDED_H
