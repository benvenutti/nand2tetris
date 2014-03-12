#include "SymbolTable.h"

void SymbolTable::loadMap(map<string, int> symbolMap)
{
	table.clear();
	table = symbolMap;
}

void SymbolTable::addEntry(string symbol, int address)
{
	table.insert(pair<string, int>(symbol, address));
}

bool SymbolTable::contains(string symbol)
{
	map<string,int>::iterator it = table.find(symbol);
	return it != table.end() ? true : false;
}

int SymbolTable::getAddress(string symbol)
{
	map<string,int>::iterator it = table.find(symbol);
	return it != table.end() ? it->second : -1;
}

map<string, int> SymbolTable::getTable()
{
	return table;
}
