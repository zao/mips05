/* Computer Architecture, Fall 05
 *  Klas Fahlberg (c03kfg) & Lars Viklund (c03lvd)
 *  CompilerMain.cpp
 *  Instansiates a compiler and lets the compiler compile the inputted assembly
 *  code from standard in to machine code on standard out.
 */

#include "Compiler.hpp"
#include<iostream>
#include<string>

int main(){
  Compiler c;
  c.Compile();
  cerr << "Compile done." << endl;
}
