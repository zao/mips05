/* Computer Architecture, Fall 05
 *  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
 *  Compiler.hpp
 *  The Compiler class reads simplified mips assembly from standard in and
 *  writes machine code to standard out.
 */

#ifndef DARK_COMPILER_HPP
#define DARK_COMPILER_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <stdint.h>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

typedef tokenizer< char_separator<char> > tok;

// This typedef is used to determine the class of arguments in the assembly
// code.
typedef enum { A_REG, A_IM, A_LBL, A_LPAR, A_RPAR, A_COMMA, A_BAD } AType;

class Compiler 
{
public:
	Compiler();
	void Compile();
private:
	vector<string> ReadFile();
  void StripComments( vector<string>& lines );
  void StripEmptyLines( vector<string>& lines );
  map<string, uint32_t> StripLabels( vector<string>& lines );
  uint32_t Compile( string line, map<string, uint32_t> labels, uint32_t pc );
  AType ArgType( string s );
  void ExitWith( string line );

  // Lookup tables that maps strings to numbers for instructions and registers.
  map<string, uint32_t> instructions_;
  map<string, uint32_t> registers_;
  map<string, uint32_t> funct_;

  // Tokenizers for parsing out comments, labels and subdividing instructions.
  char_separator<char> comment_tok;
  char_separator<char> label_tok;
  char_separator<char> op_tok;
};

#endif
