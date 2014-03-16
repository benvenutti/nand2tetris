#ifndef CODE_WRITER_OPT_INCLUDED_H
#define CODE_WRITER_OPT_INCLUDED_H

#include "CodeWriter.h"

class CodeWriterOpt : public CodeWriter
{

	public:

		/**
			Constructs a CodeWriterOpt object and binds it to the output stream.

			@param outputStream Output stream where Hack assembly code will be written.

			@param programName Name of the program.

			@param writeComments Flags the CodeWriterOpt to output comments (original VM commands 
			plus program and module headers and optimisation comments) on the .asm file.
		*/
		CodeWriterOpt(ostream& outputStream, string programName, bool writeComments);

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
        	ok
        */
        void writeSeq2PushConstantAdd(int index);

        /**
        	ok
        */
        void writeSeq2PushConstantSub(int index);

        /**
        	ok
        */
        void writeSeq2PushConstantNeg(int index);

        /**
        	ok
        */
        void writeSeq2PushArgLocalThisThatAdd(string segment, int index);

        /**
        	ok
        */
        void writeSeq2PushArgLocalThisThatSub(string segment, int index);

        /**
        	ok
        */
        void writeSeq2PushStaticAdd(int index);

        /**
        	ok
        */
        void writeSeq2PushTempPopThat(int tempIndex, int thatIndex);

        /**
        	ok
        */
        void writeSeq2PushArgumentPopPointer(int argIndex, int pointerIndex);

        /**
        	ok
        */
        void writeSeq2PushArgumentPopTemp(int argIndex, int tempIndex);

        /**
        	ok
        */
        void writeSeq2PushArgumentPopArgLocal(int argIndex, string segment, int segIndex);

        /**
        	ok
        */
        void writeSeq2PushConstantPopStaticTemp(int constant, string segment, int segIndex);

        /**
        	ok
        */
        void writeSeq2PushConstantPopArgLocalThisThat(int constant, string segment, int segIndex);

        /**
        	NOT OK!!!!
        */
        void writeSeq2PushPointerPopStaticTemp(int pointerIndex, string segment, int segIndex);

        /**
        	ok
        */
        void writeSeq3PushConstantPushLocalAdd(int constant, int index);

        /**
        	ok
        */
        void writeSeq3PushLocalPushLocalAdd(int local1Index, int local2Index);

        /**
        	ok
        */
        void writeSeq3PushLocalPushStaticAdd(int localIndex, int staticIndex);

        /**
        	ok
        */
        void writePushConstantXTimes(int constant, int times);

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

		int eqGlobalID; 				/**< Global ID for VM eq command labels. */

		int gtGlobalID; 				/**< Global ID for VM gt command labels. */

		int ltGlobalID; 				/**< Global ID for VM lt command labels. */

		int pushConstXTimesGlobalID; 	/**< Global ID for "Push Constant X Times" labels. */

};

#endif // CODE_WRITER_OPT_INCLUDED_H
