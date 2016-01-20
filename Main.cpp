/* Computer Architecture, Fall 05
*  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
*  Main.cpp
*  The main function reads the instructions from stdin and executes them.
*/

//#include "Compiler.hpp"
#include "Instructions.hpp"
#include "Memory.hpp"
#include "Registers.hpp"
#include <list>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

int main( int argc, char* argv[] )
{
	// The first word contains the offset to the instruction that
	// should be executed first.
	uint32_t start_instruction;
	cin.read((char*)&start_instruction, 4);
	cout << "Start instruction: " << start_instruction << endl;
	
	vector<Instruction*> instructions;
	uint32_t instr;
	// Put all instructions in a vector.
	while(cin.read((char*)&instr, 4))
	{
		instructions.push_back(Instruction::Create(instr));
	}
	
	cout << "Execution of instructions begin." << endl;
		
	uint32_t PC = start_instruction*4;
	// Execute all the instructions.
	while( PC/4 < instructions.size() )
  {
		PC = instructions[PC/4]->Exec( PC );
	}
	
	cout << "Execution of instructions end." << endl;
	
	// Free the memory allocated for the instructions.
	for( uint32_t i = 0; i < instructions.size(); ++i )
  {
    delete instructions[i];
  }

	// Print the registers and the memory.
  s_Registers.Print();
  s_Memory.Print();
  
	return 0;
}
