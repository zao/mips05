/* Computer Architecture, Fall 05
*  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
*  Registers.cpp
*  The Registers class simulates the registers with an array of integers.
*/
#ifndef DARK_REGISTERS_HPP
#define DARK_REGISTERS_HPP

#include <stdint.h>
#include <iostream>
#include <string>
#include <map>


using std::cout;
using std::endl;
using std::map;
using std::string;

class Registers
{
  public:
    Registers();
		// Print all the registers on stdout.
		void Print();
    int32_t& operator[]( uint32_t reg );
    string LookupNameOf( uint32_t reg );
  private:
		map<string, uint32_t> regmap_;
		int32_t registers_[32];
};

extern Registers s_Registers;

#endif
