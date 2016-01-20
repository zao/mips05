/* Computer Architecture, Fall 05
 *  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
 *  Compiler.cpp
 *  The Compiler class reads simplified mips assembly from standard in and
 *  writes machine code to standard out.
 */

#include "Compiler.hpp"
#include "Instructions.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <utility>

using namespace std;

Compiler::Compiler() : comment_tok("","#"), label_tok("", ":"), op_tok(" \t", ",()")
{
  // Setting up lookup tables for instructions.
  // R-instructions
  instructions_["add" ] = 0x00; funct_["add" ] = 0x20;
  instructions_["addu"] = 0x00; funct_["addu"] = 0x21;
  instructions_["sub" ] = 0x00; funct_["sub" ] = 0x22;
  instructions_["subu"] = 0x00; funct_["subu"] = 0x23;
  instructions_["and" ] = 0x00; funct_["and" ] = 0x24;
  instructions_["or"  ] = 0x00; funct_["or"  ] = 0x26;
  instructions_["nor" ] = 0x00; funct_["nor" ] = 0x27;
  instructions_["slt" ] = 0x00; funct_["slt" ] = 0x2A;
  instructions_["sltu"] = 0x00; funct_["sltu"] = 0x2B;
  instructions_["jr"  ] = 0x00; funct_["jr"  ] = 0x08;
  instructions_["sll" ] = 0x00; funct_["sll" ] = 0x00;
  instructions_["srl" ] = 0x00; funct_["srl" ] = 0x02;
  
  // I-instructions
  instructions_["lw"   ] = 0x23;
  instructions_["sw"   ] = 0x2B;
  instructions_["beq"  ] = 0x04;
  instructions_["bne"  ] = 0x05;
  instructions_["addi" ] = 0x08;
  instructions_["addiu"] = 0x09;
  instructions_["andi" ] = 0x0C;
  instructions_["ori"  ] = 0x0D;
  instructions_["lui"  ] = 0x0F;
  instructions_["lbu"  ] = 0x24;
  instructions_["lhu"  ] = 0x25;
  instructions_["slti" ] = 0x0A;
  instructions_["sltiu"] = 0x0B;
  instructions_["sb"   ] = 0x28;
  instructions_["sh"   ] = 0x29;

  // J-instructions
  instructions_["jal"  ] = 0x03;
  instructions_["j"    ] = 0x02;

  // Mapping register names to index
  /*
     $0         $zero     Permanently 0                     
     $1         $at       Assembler Temporary (reserved)    
     $2, $3     $v0, $v1  Value returned by a subroutine    
     $4-$7      $a0-$a3   Arguments to a subroutine         
     $8-$15     $t0-$t7   Temporary                         
     $16-$23    $s0-$s7   Saved registers                   
     $24, $25   $t8, $t9  Temporary              
     $26, $27   $k0, $k1  Kernel (reserved for OS)
     $28        $gp       Global Pointer         
     $29        $sp       Stack Pointer          
     $30        $fp       Frame Pointer          
     $31        $ra       Return Address         
   */
  registers_["$zero"] =  0; registers_["$at"  ] =  1; registers_["$v0"  ] =  2;
  registers_["$v1"  ] =  3; registers_["$a0"  ] =  4; registers_["$a1"  ] =  5;
  registers_["$a2"  ] =  6; registers_["$a3"  ] =  7; registers_["$t0"  ] =  8;
  registers_["$t1"  ] =  9; registers_["$t2"  ] = 10; registers_["$t3"  ] = 11;
  registers_["$t4"  ] = 12; registers_["$t5"  ] = 13; registers_["$t6"  ] = 14;
  registers_["$t7"  ] = 15; registers_["$s0"  ] = 16; registers_["$s1"  ] = 17;
  registers_["$s2"  ] = 18; registers_["$s3"  ] = 19; registers_["$s4"  ] = 20;
  registers_["$s5"  ] = 21; registers_["$s6"  ] = 22; registers_["$s7"  ] = 23;
  registers_["$t8"  ] = 24; registers_["$t9"  ] = 25; registers_["$k0"  ] = 26;
  registers_["$k1"  ] = 27; registers_["$gp"  ] = 28; registers_["$sp"  ] = 29;
  registers_["$fp"  ] = 30; registers_["$ra"  ] = 31;
}

void Compiler::Compile()
{
  vector<uint32_t> instructions;
  vector<string> lines = ReadFile();
  StripComments( lines );
  StripEmptyLines( lines );
  map<string, uint32_t> labels = StripLabels( lines );

  for( uint32_t i = 0; i < lines.size(); ++i )
  {
    cerr << "Compiling " << lines[i] << endl;
    instructions.push_back( Compile( lines[i], labels, i ) );
    cerr << endl;
  }

  // Find the offset of the main label, if it doesn't exist: let the program
  // begin at instruction zero.
  uint32_t mainOffset = labels["main"];
  cout.write((char*)&mainOffset, 4);

  // Write the instructions to standard out.
  vector<uint32_t>::iterator I = instructions.begin();
  while( I != instructions.end() )
    cout.write((char*)&(*(I++)), 4);
}

// Reads the input line by line from standard in.
vector<string> Compiler::ReadFile()
{
  vector<string> lines;
  string line;
  while( 1 )  
  {
    getline(cin, line,'\n');
    if( !cin.eof() )
    {
      lines.push_back(line);
    }
    else break;
  }
  return lines;
}

// Uses a boost tokenizer to find any eventual comments and remove them.
void Compiler::StripComments( vector<string>& lines )
{
  for( uint32_t i = 0; i < lines.size(); ++i )
  {
    tok t(lines[i], comment_tok);
    if( t.begin() != t.end() )
    {
      if( *(t.begin()) == "#" )
        lines[i].clear();
      else if( ++(t.begin()) != t.end() )
        lines[i].assign( *(t.begin()) );
    }
  }
}

// Uses a boost tokenizer to find lines that only contains whitespace.
void Compiler::StripEmptyLines( vector<string>& lines )
{
  for( vector<string>::iterator I = lines.begin(); I != lines.end(); )
  {
    tok t(*I, char_separator<char>("\t "));
    if( t.begin() == t.end() )
      I = lines.erase(I);
    else ++I;
  }
}

// Assumes that all labels is followed by an instruction on the same line. Uses
// a tokenizer to find the labels, removes the label and maps the label to the
// line it came from.
map<string, uint32_t> Compiler::StripLabels( vector<string>& lines )
{
  map<string, uint32_t> labels;
  for( uint32_t i = 0; i < lines.size(); ++i )
  {
    tok t(lines[i], label_tok);
    tok::iterator T = t.begin();
    if( T != t.end() && ++T != t.end() && *T == ":" )
    {
      string label( *(t.begin()) ); 
      lines[i].assign( *(++T) );
      labels[label] = i;
    }
  }
  return labels;
}

// Compile is the heart of the class. It takes the current line, transforms it
// into tokens and verifies that the syntax is correct while parsing. If an
// error occurs it will exit.
uint32_t Compiler::Compile( string line, map<string, uint32_t> labels, uint32_t pc )
{
  uint32_t word = 0;
  tok t(line, op_tok);
  tok::iterator T = t.begin();
  if( T == t.end() || instructions_.find(*T) == instructions_.end() ) 
  {
    ExitWith(line);
  }

  // Find out what kind of instruction we're looking at. 
  string opstr = *T;
  uint32_t op = instructions_[opstr];
  InstructionType itype = s_InstructionType[op];

  switch( itype )
  {
    case INSTR_R: 
      {
        uint32_t shamt = 0;
        uint32_t funct = funct_[opstr];
        uint32_t reg[3] = {0,0,0};
        
        // Read the three register arguments, unless it's a sll, srl or jr
        // instruction which have different syntax.
        for( int i=0; i < 3; ++i )
        {
          // The last argument to sll and srl is an integer amount, not a
          // register.
          if( (opstr == "sll" || opstr == "srl") && i == 2 )
          {
              stringstream ss(*++T);
              ss >> shamt;
          }
          else
          {
            if( ++T == t.end() || registers_.find(*T) == registers_.end() ) 
              ExitWith(line);
            reg[i] = registers_[*T];
          }
          // If the instruction is jr, it only needs one argument and can stop
          // parsing more arguments.
          if( opstr == "jr" )
            break;
          if( i < 2 )
          {
            if( ++T == t.end() || ArgType(*T) != A_COMMA )
              ExitWith(line);
          }
          else
          {
            if( ++T != t.end() )
              ExitWith(line);
          }
        }

        if( opstr == "jr" )
          swap( reg[0], reg[1] );

        // Truncate any values that are too large and stuff them all into a
        // 32-bit word.
        op &= 0x3F;     reg[2] &= 0x1F;   reg[1] &= 0x1F;   reg[0] &= 0x1F;
        shamt &= 0x1F;  funct &= 0x3F; 

        word = (op << 26) | (reg[1] << 21) | (reg[2] << 16) | (reg[0] << 11)
          | (shamt << 6) | (funct << 0);
        cerr << "Compiled " << opstr << " (" << op << ")(" << reg[2] << ")(" <<
          reg[1] << ")(" << reg[0] << ")(" << shamt << ")(" << funct << ")" <<
          " => " << hex << word << endl;
        break;
      }
    case INSTR_I:
      {
        uint32_t rt = 0, rs = 0, im = 0;
        bool ls;

        // Parse argument one.
        if( ++T == t.end( ) )
          ExitWith(line);

        switch( ArgType(*T) )
        {
          case A_REG:
            {
              if( registers_.find(*T) == registers_.end() )
              {
                ExitWith(line);
              }
              rt = registers_[*T];
              break;
            }
          default:
            ExitWith(line); 
        }

        if( ++T == t.end() || ArgType(*T) != A_COMMA )
          ExitWith(line);

        // Parse argument two. If the second argument is an integer value,
        // assume that the instruction is a load-store type instruction. 
        if( ++T == t.end( ) )
          ExitWith(line);

        switch( ArgType(*T) )
        {
          case A_REG:
            {
              if( registers_.find(*T) == registers_.end() )
              {
                ExitWith(line);
              }
              rs = registers_[*T];
              ls = false;
              break;
            }
          case A_IM:
            {
              stringstream ss(*T);
              ss >> im;
              ls = true;
              break;
            }
          default:
            ExitWith(line); 
        }

        // Parse argument three. lui doesn't have a third argument and should
        // thus fail if one if supplied. If the instruction is a load-store
        // instruction: verify that this argument is a register contained
        // within braces. Otherwise, make sure it's an integer value.
        if( opstr != "lui" )
        {
          if( ++T == t.end( ) )
            ExitWith(line);

          if( ls ) // Load-store type instruction
          {
            AType a = ArgType(*T);
            if( a != A_LPAR || ++T == t.end() )
              ExitWith(line);

            switch( ArgType(*T) )
            {
              case A_REG:
                if( registers_.find(*T) == registers_.end() )
                {
                  ExitWith(line);
                }
                rs = registers_[*T];
                ls = false;
                break;
              default:
                ExitWith(line); 
            }
            if( ++T == t.end() || ArgType(*T) != A_RPAR ) 
              ExitWith(line);
          }
          else // Regular I-instruction.
          {
            AType a = ArgType(*T);
            if( a != A_COMMA || ++T == t.end() )
              ExitWith(line);

            switch( ArgType(*T) )
            {
              case A_IM:
                {
                  stringstream ss(*T);
                  ss >> im;
                  break;
                }
              case A_LBL:
                {
                  if( labels.find(*T) == labels.end() )
                    ExitWith(line);
                  im = labels[*T] - pc - 1;
                  ls = true;
                  break;
                }
              default:
                ExitWith(line);
            }
          }
        }

        if( ++T != t.end() )
          ExitWith(line);

        // Truncate any values that are too large and stuff everything into a
        // 32-bit word.
        op &= 0x3F;   rs &= 0x1F;   rt &= 0x1F;   im &= 0xFFFF;

        word = (op << 26) | (rs << 21) | (rt << 16) | (im << 0);
        cerr << "Compiled " << opstr << " (" << op << ")(" << rs << ")(" << rt
          << ")(" << im << ")" << " => " << hex << word << endl;
        // Truncate any big values and stuff them into a 32-bit word.
        break;
      }
    case INSTR_J:
      {
        uint32_t ad = 0;

        // Parsing argument one. If it's a string literal then resolve the
        // label into an offset from the next instruction. Otherwise, just use
        // the value as offset. 
        if( ++T == t.end( ) )
          ExitWith(line);

        switch( ArgType(*T) )
        {
          case A_LBL:
            {
              if( labels.find(*T) == labels.end() )
                ExitWith(line);
              ad = labels[*T] - pc - 1;
              break;
            }
          case A_IM:
            {
              stringstream ss(*T);
              ss >> ad;
              break;
            }
          default:
            ExitWith(line); 
        }

        if( ++T != t.end() )
          ExitWith(line);

        // Truncate any big values and stuff them into a 32-bit word.
        op &= 0x3F;   ad &= 0x3FFFFFFF;

        word = (op << 26) | (ad << 0);
        cerr << "Compiled " << opstr << " (" << op << ")(" << ad << ")" 
          << " => " << hex << word << endl;
       break;
 
        break;
      }
    case I_UNDEF:
      {
        break;
      }
  }
  return word;
}

// Determines what kind of string we're looking at.
AType Compiler::ArgType( string s )
{
  char first = s[0];
  if( (first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z') )
    return A_LBL;
  else if( first == '$' )
    return A_REG; 
  else if( first >= '0' && first <= '9' || first == '-' )
    return A_IM;
  else if( first == '(' )
    return A_LPAR;
  else if( first == ')' )
    return A_RPAR;
  else if( first == ',' )
    return A_COMMA;
  else return A_BAD;
}

// This function exits with the specified message output before exiting. 
void Compiler::ExitWith( string line )
{
  cerr << "Broken code: " << line << endl;
  exit(-2);
}
