/* Computer Architecture, Fall 05
*  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
*  Instructions.hpp
*  The Instruction class holds and executes an instruction. 
*/
#ifndef DARK_INSTRUCTIONS_HPP
#define DARK_INSTRUCTIONS_HPP

#include <stdint.h>

// The different instruction types.
typedef enum
{
	INSTR_R,
	INSTR_I,
	INSTR_J,
	I_UNDEF
} InstructionType;

// Look-up table for the opcode-field.
static const InstructionType s_InstructionType[64] = 
{
  INSTR_R, INSTR_I, INSTR_J, INSTR_J, INSTR_I, INSTR_I, INSTR_I, INSTR_I,
  INSTR_I, INSTR_I, INSTR_I, INSTR_I, INSTR_I, INSTR_I, INSTR_I, INSTR_I, 
  I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, 
  I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, 
  INSTR_I, INSTR_I, INSTR_I, INSTR_I, INSTR_I, INSTR_I, INSTR_I, I_UNDEF,
  INSTR_I, INSTR_I, INSTR_I, INSTR_I, I_UNDEF, I_UNDEF, INSTR_I, I_UNDEF, 
  I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, 
  I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF, I_UNDEF 
};

// The abstract base class.
class Instruction
{
public:
	// Execute the instruction. Returns the PC for next instruction.
	virtual uint32_t Exec( uint32_t PC )=0;
	// Create a new instruction.
	static Instruction* Create( uint32_t rawInstruction );
private:
};

// The class for R-instructions.
class RInstruction: public Instruction 
{
public:
	uint32_t Exec( uint32_t PC );
	RInstruction( uint32_t rawInstruction );
private:
	// The different R-instructions.
	void Add();
	void Addu();
	void Sub();
	void Subu();
	void And();
	void Or();
	void Nor();
	void Slt();
	void Sltu();
	int32_t Jr();
	void Sll();
	void Srl();

	// The fields of the R-instruction.
	uint32_t op_;
	uint32_t rs_;
	uint32_t rt_;
	uint32_t rd_;	
	uint32_t shamt_;
	uint32_t funct_;
};

// The class for I-instructions.
class IInstruction: public Instruction
{
public:
	uint32_t Exec( uint32_t PC );
	IInstruction( uint32_t rawInstruction );
private:
	// The different I-instructions.
	void Lw();
	void Sw();
	int32_t Beq();
	int32_t Bne();
  void Addi();
	void Addiu();
	void Andi();
	void Ori();
	void Lui();
	void Lbu();
	void Lhu();
	void Slti();
	void Sltiu();
	void Sb();
	void Sh();

	// The fields of the I-instruction.
	uint32_t op_;
	uint32_t rs_;
	uint32_t rt_;
	uint32_t im_;
};

// The class for J-instructions.
class JInstruction: public Instruction
{
public:
	uint32_t Exec( uint32_t PC );
	JInstruction( uint32_t rawInstruction );

private:
	// The different J-instructions.
	int32_t Jal(int32_t PC);
	int32_t Jump();

	// The fields of the J-instruction.
	uint32_t op_;
	uint32_t tar_;
};

#endif

