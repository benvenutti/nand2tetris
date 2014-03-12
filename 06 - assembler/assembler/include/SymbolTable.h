#ifndef SYMBOL_TABLE_INCLUDED_H
#define SYMBOL_TABLE_INCLUDED_H

#include <string>
#include <map>

using namespace std;

class SymbolTable
{

	public:

        /**
            Loads all entries of a map<string, int> to the symbol table. Previous
			entries will be erased.

			@param symbolMap A map with symbols to be loaded into the symbol table.
		*/
		void loadMap(map<string, int> symbolMap);

		/**
			Adds the pair <symbol, address> to the table.

			@param symbol String with the symbol to be added.

			@param address Integer with the address of the symbol being added.
		*/
		void addEntry(string symbol, int address);

		/**
			Does the symbol table contain the given symbol?

			@param symbol String with the symbol to be tested if contained or not contained
			in the symbol table.

			@return True if the table contains the given symbol, False otherwise.
		*/
		bool contains(string symbol);

		/**
			Returns the address associated with the symbol.

			@param symbol String with the symbol which its address is requested.

			@return An integer with the address of the given symbol.
		*/
		int getAddress(string symbol);

		/**
			Returns a map populated with pairs <symbol, address>, where symbol is a string and
			address an int.

			@return A map<string, int> with the symbol table.
		*/
		map<string, int> getTable();

	private:

		map<string, int> table;		/**< Symbol map. */

};

#endif // #ifndef SYMBOL_TABLE_INCLUDED_H
