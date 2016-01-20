#include "Instructions.hpp"
#include "Registers.hpp"
#include "Memory.hpp"
#include <iostream>
#include <math.h>

using std::cerr;
using std::endl;
using std::hex;

Instruction* Instruction::Create( uint32_t rawInstruction )
{
	// Look-up the instruction type and return a new instruction
	// of the type.
	switch( s_InstructionType[rawInstruction>>26] )
  {
    case INSTR_R:
      return new RInstruction( rawInstruction );
    case INSTR_I:
      return new IInstruction( rawInstruction );
    case INSTR_J:
      return new JInstruction( rawInstruction );
    default:
      cerr << "Unknown intruction type: " << (rawInstruction>>26) << endl;
  }
  return NULL;
}

RInstruction::RInstruction( uint32_t rawInstruction )
{
	// Get the different fields.
  op_ = (rawInstruction >> 26) & 0x3F;
  rs_ = (rawInstruction >> 21) & 0x1F; 
  rt_ = (rawInstruction >> 16) & 0x1F;
  rd_ = (rawInstruction >> 11) & 0x1F;
  shamt_ = (rawInstruction >> 6) & 0x1F;
  funct_ = (rawInstruction >> 0) & 0x3F;
}


uint32_t RInstruction::Exec( uint32_t PC )
{
	// Check the funct-field for operation and call the right function.
  switch( funct_ )
  {
    case 0x20: Add(); break;
    case 0x21: Addu(); break;
    case 0x22: Sub(); break;
    case 0x23: Subu(); break;
    case 0x24: And(); break;
    case 0x26: Or();  break;
    case 0x27: Nor();  break;
    case 0x2A: Slt(); break;
    case 0x2B: Sltu(); break;
		case 0x08: PC += Jr();	break;
		case 0x00: Sll();	break;
		case 0x02: Srl();	break;
    default: cerr << "Unsupported R-instruction " << op_ << endl;
  }  
	// Return the incremented PC.
  return PC+4;
}

void RInstruction::Add()
{
  cout << "Instruction add.\tTarget register " << s_Registers.LookupNameOf(rd_) << " before: " << s_Registers[rd_] << "\t";
	int32_t result = s_Registers[rs_] + s_Registers[rt_];
	// Check if overflow occured.
	if( ((result < 0) && (s_Registers[rs_] > 0) && (s_Registers[rt_] > 0))
			||((result > 0) && (s_Registers[rs_] < 0) && (s_Registers[rt_] < 0)) ) 
		cerr << "Instruction add caused an overflow." << endl;
	
	s_Registers[rd_] = result;
  cout << "Target register after: " << s_Registers[rd_] << endl;
}

void RInstruction::Addu()
{
  s_Registers[rd_] = (uint32_t)s_Registers[rs_] + (uint32_t)s_Registers[rt_];
}

void RInstruction::Sub()
{
  cout << "Instruction sub.\tTarget register " << s_Registers.LookupNameOf(rd_) << " before: " << s_Registers[rd_] << "\t";
	int32_t result = s_Registers[rs_] - (int32_t)s_Registers[rt_];
	// Check if overflow occured.
	if( ((result > 0) && (s_Registers[rs_] < 0) && (s_Registers[rt_] > 0))
			||((result < 0) && (s_Registers[rs_] > 0) && (s_Registers[rt_] < 0)) ) 
		cerr << "Instruction sub caused an overflow." << endl;
		
	s_Registers[rd_] = result;
  cout << "Target register after: " << s_Registers[rd_] << endl;
}

void RInstruction::Subu()
{  
	s_Registers[rd_] = (uint32_t)s_Registers[rs_] - (uint32_t)s_Registers[rt_];
}

void RInstruction::And()
{
  cout << "Instruction and.\tTarget register " << s_Registers.LookupNameOf(rd_) << " before: " << s_Registers[rd_] << "\t";
  s_Registers[rd_] = s_Registers[rs_] & s_Registers[rt_];
  cout << "Target register after: " << s_Registers[rd_] << endl;
}

void RInstruction::Or()
{
  cout << "Instruction or. \tTarget register " << s_Registers.LookupNameOf(rd_) << " before: " << s_Registers[rd_] << "\t";
  s_Registers[rd_] = s_Registers[rs_] | s_Registers[rt_];
  cout << "Target register after: " << s_Registers[rd_] << endl;
}

void RInstruction::Nor()
{
  s_Registers[rd_] = ~(s_Registers[rs_] | s_Registers[rt_]);
}

void RInstruction::Slt()
{
  cout << "Instruction slt.\tTarget register " << s_Registers.LookupNameOf(rd_) << " before: " << s_Registers[rd_] << "\t";
  s_Registers[rd_] = s_Registers[rs_] < s_Registers[rt_] ? 1 : 0;
  cout << "Target register after: " << s_Registers[rd_] << endl;
}

void RInstruction::Sltu()
{
  s_Registers[rd_] = (uint32_t)s_Registers[rs_] < (uint32_t)s_Registers[rt_] ? 
											1 : 0;
}

int32_t RInstruction::Jr()
{
	return s_Registers[rs_]<<2;
}

void RInstruction::Sll()
{
	s_Registers[rd_] = s_Registers[rs_] << shamt_;
}

void RInstruction::Srl()
{
	s_Registers[rd_] = s_Registers[rs_] >> shamt_;
}

IInstruction::IInstruction( uint32_t rawInstruction )
{
	// Get the different fields.
  op_ = (rawInstruction >> 26) & 0x3F;
  rs_ = (rawInstruction >> 21) & 0x1F; 
  rt_ = (rawInstruction >> 16) & 0x1F;
  im_ = (rawInstruction >> 0) & 0xFFFF;
}

uint32_t IInstruction::Exec( uint32_t PC )
{
  PC+=4;
	// Check the op-field for operation and call the right function.
	switch( op_ )
  {
    case 0x23: Lw(); 		break;
    case 0x2B: Sw();		break;
    case 0x04: PC += Beq(); break;
    case 0x05: PC += Bne(); break;
    case 0x08: Addi();	break;
    case 0x09: Addiu();	break;
    case 0x0C: Andi();	break;
    case 0x0D: Ori();		break;
		case 0x0F: Lui();		break;
		case 0x24: Lbu();		break;
		case 0x25: Lhu();		break;
		case 0x0A: Slti();	break;
		case 0x0B: Sltiu();	break;
		case 0x28: Sb();		break;
		case 0x29: Sh();		break;
		default: cerr << "Unsupported I-instruction: " << op_ << endl;
  }  
  return PC;
}

void IInstruction::Lw()
{
	// Check if the memory cell has a value.
	if(!s_Memory.adressExists(s_Registers[rs_]/4 + im_/4))
	{
		cerr<< "Attempted to access unallocated memory.\nExiting." << endl;
		exit(-1);
	}
		
  cout << "Instruction lw. \tTarget register " << s_Registers.LookupNameOf(rt_) << " before: " << s_Registers[rt_] << "\t";
  s_Registers[rt_] = s_Memory[s_Registers[rs_]/4 + im_/4];
  cout << "Target register after: " << s_Registers[rt_] << endl;
}

void IInstruction::Sw()
{
  cout << "Instruction sw. \tMemory cell " << (s_Registers[rs_]/4 + im_/4) << " got value " << s_Registers[rt_] << "." << endl;
  s_Memory[s_Registers[rs_]/4 + im_/4] = s_Registers[rt_];
}

int32_t IInstruction::Beq()
{
  cout << "Instruction beq.\t";
  cout << (s_Registers[rs_] == s_Registers[rt_] ? "Branch taken." : "Branch not taken.") << endl;
	return s_Registers[rs_] == s_Registers[rt_] ? ((int16_t)im_)<<2 : 0;
}

int32_t IInstruction::Bne()
{
  return s_Registers[rs_] != s_Registers[rt_] ? ((int16_t)im_)<<2 : 0;
}

void IInstruction::Addi()
{
	int32_t result = s_Registers[rs_] + (int16_t)im_;
	// Check if overflow occured.
	if( ((result < 0) && (s_Registers[rs_] > 0) && (im_ > 0))
			||((result > 0) && (s_Registers[rs_] < 0) && (im_ < 0)) ) 
		cerr << "Instruction addi caused an overflow." << endl;
	
	s_Registers[rt_] = result;
}

void IInstruction::Addiu()
{
  s_Registers[rt_] = (uint32_t)s_Registers[rs_] + (uint16_t)im_;
}

void IInstruction::Andi()
{
	s_Registers[rt_] = s_Registers[rs_] & im_;
}

void IInstruction::Ori()
{
	s_Registers[rt_] = s_Registers[rs_] | im_;
}

void IInstruction::Lui()
{
	s_Registers[rt_] = im_ << 16;
}

void IInstruction::Lbu()
{
	uint32_t addr = s_Registers[rs_] + im_;
	uint32_t word = s_Memory[addr/4];
	word >>= (addr%4)*8;
	word &= 0xFF;
	s_Registers[rt_] = word;
}

void IInstruction::Lhu()
{
	uint32_t addr = s_Registers[rs_] + im_;
	uint32_t word = s_Memory[addr/4];
	word >>= (addr%4)*8;
	word &= 0xFFFF;
	s_Registers[rt_] = word;
}

void IInstruction::Slti()
{
  s_Registers[rt_] = (uint32_t)s_Registers[rs_] < im_ ? 1 : 0;
}

void IInstruction::Sltiu()
{
  s_Registers[rt_] = (uint16_t)s_Registers[rs_] < (uint16_t)im_ ? 1 : 0;
}

void IInstruction::Sb()
{
	// Calculate the shift amount.
	uint32_t shift_amaount = (im_%4)*8;
	// Get the byte from the register.
	int32_t data = s_Registers[rt_] & 0xFF;
	// Put the data in to the memory. The data is placed on the right
	// bits by shifting it shift_amaount bits.
	s_Memory[s_Registers[rs_]/4 + im_/4] |= data << shift_amaount;
}

void IInstruction::Sh()
{
	uint32_t shift_amaount = (im_%2)*16;
	int32_t data = s_Registers[rt_] & 0xFFFF;
	s_Memory[s_Registers[rs_]/4 + im_/4] |= data << shift_amaount;
}

JInstruction::JInstruction( uint32_t rawInstruction )
{
	// Get the different fields. 
  op_ = (rawInstruction >> 26) & 0x3F;
  tar_ = (rawInstruction >> 0) & 0xFFFFFF;
}

uint32_t JInstruction::Exec( uint32_t PC )
{
	PC+=4;
	// Check the op-field for operation and call the right function.
	switch( op_ )
	{
		case 0x03: PC+=Jal(PC);  break;
		case 0x02: PC+=Jump();  break;
		default: cerr << "Unsupported J-instruction: " << op_ << endl;
	}
	return PC;  
}

int32_t JInstruction::Jal(int32_t PC)
{
	s_Registers[31] = PC;
	return tar_<<2;
}

int32_t JInstruction::Jump()
{
	return tar_<<2;
}
