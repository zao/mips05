/* Computer Architecture, Fall 05
*  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
*  Memory.hpp
*  The Memory class simulates the memory with a map (adress to value). 
*/
#ifndef DARK_MEMORY_HPP
#define DARK_MEMORY_HPP

#include <stdint.h>
#include <map>

using std::map;

class Memory
{
public:
	int32_t& operator[]( uint32_t adress );
	// Print the memory on stdout.
	void Print();
	// Check if an adress exists (has a value).
	bool adressExists( uint32_t adress );
private:
	map<uint32_t, int32_t> memory_; 
};

extern Memory s_Memory;
#endif
