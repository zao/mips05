/* Computer Architecture, Fall 05
*  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
*  Registers.cpp
*  The implementation of Registers. 
*/
#include "Registers.hpp"
#include <algorithm>

using std::fill_n;
using std::hex;
using std::dec;
using std::streamsize;

Registers s_Registers;

Registers::Registers()
{
	// Set all registers to zero.
  fill_n( registers_, 32, 0 );
	regmap_["$zero"] =  0; regmap_["$at"] =  1; regmap_["$v0"] =  2; regmap_["$v1"] =  3;
  regmap_["$a0"  ] =  4; regmap_["$a1"] =  5; regmap_["$a2"] =  6; regmap_["$a3"] =  7;
  regmap_["$t0"  ] =  8; regmap_["$t1"] =  9; regmap_["$t2"] = 10; regmap_["$t3"] = 11;
  regmap_["$t4"  ] = 12; regmap_["$t5"] = 13; regmap_["$t6"] = 14; regmap_["$t7"] = 15;
  regmap_["$s0"  ] = 16; regmap_["$s1"] = 17; regmap_["$s2"] = 18; regmap_["$s3"] = 19;
  regmap_["$s4"  ] = 20; regmap_["$s5"] = 21; regmap_["$s6"] = 22; regmap_["$s7"] = 23;
  regmap_["$t8"  ] = 24; regmap_["$t9"] = 25; regmap_["$k0"] = 26; regmap_["$k1"] = 27;
  regmap_["$gp"  ] = 28; regmap_["$sp"] = 29; regmap_["$fp"] = 30; regmap_["$ra"] = 31;
}

int32_t& Registers::operator[]( uint32_t reg )
{
  return registers_[reg]; 
}

string Registers::LookupNameOf( uint32_t reg )
{
  map<string, uint32_t>::iterator I = regmap_.begin();
	while( I != regmap_.end() )
	{
    if( I->second == reg )
      return I->first;
    ++I;
	}
  return "";
}

void Registers::Print()
{
	cout << "Registers";
	map<string, uint32_t>::iterator I = regmap_.begin();
	uint32_t i = 0;
	streamsize wi = cout.width();
	char ch = cout.fill();
	while( I != regmap_.end() )
	{
	 if( i % 4 == 0 )
		cout << endl;
		cout << I->first << ":\t0x";
		cout.width(8); cout.fill('0');
		cout << hex << registers_[I->second] << dec <<"\t";
		cout.width(wi); cout.fill(ch);
		++i;
		++I;
	}
	cout << endl << endl;
}
