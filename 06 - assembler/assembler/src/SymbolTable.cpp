#include "SymbolTable.h"

void SymbolTable::loadMap(map<string, int> map)
{
	table.clear();
	table = map;
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
