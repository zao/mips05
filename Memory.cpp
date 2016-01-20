/* Computer Architecture, Fall 05
*  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
*  Memory.cpp
*  The implementation of the Memory class.
*/
#include "Memory.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::dec;
using std::hex;
using std::streamsize;

Memory s_Memory;

int32_t& Memory::operator[]( uint32_t adress )
{
  return memory_[adress]; 
}

void Memory::Print()
{
  cout << "Memory (\"Adress in words\": \"value in hex\")" << endl;
  map<uint32_t, int32_t>::const_iterator I = memory_.begin();
	streamsize wi = cout.width(); 
	char ch = cout.fill();
 	while( I != memory_.end() )
  {
		cout << I->first << ": 0x";
		cout.width(8); cout.fill('0');
		cout << hex << I->second << dec << endl;
		cout.width(wi); cout.fill(ch);
		++I;
  }
  cout << endl;
}

bool Memory::adressExists( uint32_t adress )
{
	return memory_.find(adress) != memory_.end();
}
