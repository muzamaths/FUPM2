#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

bool read_file( vector<string> *buffer, const string &filename ) {
  ifstream file(filename);

  // TODO: make reading file faster
  if (file.is_open()) {
    string tmp;

    while (!file.eof()) {
      file >> tmp;
      buffer->push_back(tmp);
    }
    buffer->pop_back();  //TODO: find the cause of the doubled 'main' in the end of file

    file.close();
    return true;
  }

  return false;
}

void vec_print( const vector<string> &buffer ) {
  for_each(buffer.begin(), buffer.end(), [](const string &str) { cout << str << ' '; });
  cout << endl;
}

class Operations {
protected:
  enum operation_code {
    HALT = 0,
    SYSCALL = 1,
    ADD = 2,
    ADDI = 3,
    SUB = 4,
    SUBI = 5,
    MUL = 6,
    MULI = 7,
    DIV = 8,
    DIVI = 9,
    LC = 12,
    SHL = 13,
    SHLI = 14,
    SHR = 15,
    SHRI = 16,
    AND = 17,
    ANDI = 18,
    OR = 19,
    ORI = 20,
    XOR = 21,
    XORI = 22,
    NOT = 23,
    MOV = 24,
    ADDD = 32,
    SUBD = 33,
    MULD = 34,
    DIVD = 35,
    ITOD = 36,
    DTOI = 37,
    PUSH = 38,
    POP = 39,
    CALL = 40,
    CALLI = 41,
    RET = 42,
    CMP = 43,
    CMPI = 44,
    CMPD = 45,
    JMP = 46,
    JNE = 47,
    JEQ = 48,
    JLE = 49,
    JL = 50,
    JGE = 51,
    JG = 52,
    LOAD = 64,
    STORE = 65,
    LOAD2 = 66,
    STORE2 = 67,
    LOADR = 68,
    LOADR2 = 69,
    STORER = 70,
    STORER2 = 71
  };
  enum operation_type {
    RM = 0,
    RR = 1,
    RI = 2,
    J = 3
  };

  typedef pair<operation_code, operation_type> Operation;

  map<string, Operation> operator_dictionary = {
    {"halt",    {HALT, RI}},
    {"syscall", {SYSCALL, RI}},
    {"add",     {ADD, RR}},
    {"addi",    {ADDI, RI}},
    {"sub",     {SUB, RR}},
    {"subi",    {SUBI, RI}},
    {"mul",     {MUL, RR}},
    {"muli",    {MULI, RI}},
    {"div",     {DIV, RR}},
    {"divi",    {DIVI, RI}},
    {"lc",      {LC, RI}},
    {"shl",     {SHL, RR}},
    {"shli",    {SHLI, RI}},
    {"shr",     {SHR, RR}},
    {"shri",    {SHRI, RI}},
    {"and",     {AND, RR}},
    {"andi",    {ANDI, RI}},
    {"or",      {OR, RR}},
    {"ori",     {ORI, RI}},
    {"xor",     {XOR, RR}},
    {"xori",    {XORI, RI}},
    {"not",     {NOT, RI}},
    {"mov",     {MOV, RR}},
    {"addd",    {ADDD, RR}},
    {"subd",    {SUBD, RR}},
    {"muld",    {MULD, RR}},
    {"divd",    {DIVD, RR}},
    {"itod",    {ITOD, RR}},
    {"dtoi",    {DTOI,RR}},
    {"push",    {PUSH, RI}},
    {"pop",     {POP, RI}},
    {"call",    {CALL, RR}},
    {"calli",   {CALLI, J}},
    {"ret",     {RET, J}},  //TODO: 'ret' usage in examples does not fit the 'RI' type
    {"cmp",     {CMP, RR}},
    {"cmpi",    {CMPI, RI}},
    {"cmpd",    {CMPD, RR}},
    {"jmp",     {JMP, J}},
    {"jne",     {JNE, J}},
    {"jeq",     {JEQ, J}},
    {"jle",     {JLE, J}},
    {"jl",      {JL, J}},
    {"jge",     {JGE, J}},
    {"jg",      {JG, J}},
    {"load",    {LOAD, RM}},
    {"store",   {STORE, RM}},
    {"load2",   {LOAD2, RM}},
    {"store2",  {STORE2, RM}},
    {"loadr",   {LOADR, RR}},
    {"storer",  {STORER, RR}},
    {"loadr2",  {LOADR2, RR}},
    {"storer2", {STORER2, RR}}
  };

};

class FUPM2 : private Operations {
private:
  vector<unsigned> memory;
  vector<unsigned> registers;
  map<string, int> labels;
  int program_enter_point;
  int flags;
  int error_line;

  map<string, unsigned> register_dictionary = {
    {"r0", 0},
    {"r1", 1},
    {"r2", 2},
    {"r3", 3},
    {"r4", 4},
    {"r5", 5},
    {"r6", 6},
    {"r7", 7},
    {"r8", 8},
    {"r9", 9},
    {"r10", 10},
    {"r11", 11},
    {"r12", 12},
    {"r13", 13},
    {"r14", 14},
    {"r15", 15},
  };

  bool parser( const string &filename ) {
    vector<string> buffer;
    if (read_file(&buffer, filename)) {
      vec_print(buffer);

      int current_word_number = 0;
      auto word = memory.begin();

      for (auto word_part = buffer.begin(); word_part != buffer.end(); ) {
        // label check
        if (word_part->back() == ':') {
          word_part->pop_back();
          labels[*word_part] = current_word_number;
          cout << "Label '" << *word_part << "' " << "refers to line " << current_word_number << endl;
          word_part++;
        }

        else {
          if (operator_dictionary.find(*word_part) == operator_dictionary.end()) {
            if (*word_part++ == "end") {
              program_enter_point = isdigit((*word_part)[0]) ? (unsigned)(stoi(*word_part++)) : labels[*word_part++];
              if (word_part != buffer.end()) {
                cout << "error in line" << current_word_number << ": code after 'end' label" << endl; //TODO: edit
                return false;
              }
              return true;
            }
            cout << "error in line " << current_word_number << " : No appropriate operator for expression '"
                 << *word_part << "'" << endl; //TODO: edit
            buffer.clear();
            return false;
          }

          /*operation_code current_operation_code = operator_dictionary[word_part].first;
          operation_type current_operation_type = operator_dictionary[word_part].second;*/
          Operation current_operation = operator_dictionary[*word_part++];
          current_word_number++;


          switch (current_operation.second) {
            case RM: {
              auto register_code = (unsigned) (register_dictionary[*word_part++]);
              auto operator_modificator = (unsigned) (stoi(*word_part++));
              memory.insert(word++, ((unsigned) current_operation.first) | (register_code << 8u) |
                                    (operator_modificator << 12u));
              break;
            }

            case RR: {
              auto first_register_code = (unsigned) register_dictionary[*word_part++];
              auto second_register_code = (unsigned) register_dictionary[*word_part++];
              signed short operator_modificator;
              sscanf((*word_part++).c_str(), "%hd", &operator_modificator); //TODO: find not c-style solution
              memory.insert(word++, ((unsigned) current_operation.first) | (first_register_code << 8u) |
                                    (second_register_code << 12u) | ((unsigned) operator_modificator << 16u));
              break;
            }

            case RI: {
              auto register_code = (unsigned) (register_dictionary[*word_part++]);
              auto operator_modificator = stoi(*word_part++); //TODO: how to obtain 20-bit number??????????
              memory.insert(word++, ((unsigned) current_operation.first) | (register_code << 8u) |
                                    ((unsigned) operator_modificator << 12u));
              break;
            }

            case J: {
              //TODO: labels can be modificators e.g. 'calli' uses labels
              auto operator_modificator = isdigit((*word_part)[0]) ? (unsigned)(stoi(*word_part++)) : labels[*word_part++];
              memory.insert(word++, ((unsigned) current_operation.first) | (operator_modificator << 12u));
              break;
            }

            default: {
              break;
            }
          } /* End of 'switch' */
        } /* End of 'else' case */
      } /* End of 'for' loop */
      
      return true;
    }

    return false;
  }

public:
  FUPM2() {
    memory.reserve(1u<<20u);
    registers.resize(16);
    registers[14] = (1u<<20u) - 1;
    flags = 0;

    if (!parser("input.fasm")) {
      cout << endl << endl << "ERROR!!! Failed to parse file" << endl << endl; //TODO: remove this error message and obtain error
    }
  }
  
  void get_mem_info() {
    cout << "Memory reserved: " << memory.capacity() << endl;
    cout << "Memory used: " << memory.size() << endl;
    cout << "Registers:" << endl;
    for (int i = 0; i < registers.size(); i++) {
      cout << "  " << "r" << i << ": " << registers[i] << endl;
    }
    cout << "  " << "Flags: " << flags << endl;
    cout << "  " << "Size of one register is " << sizeof(registers[0]) << " bytes" << endl;
  }
};

int main()
{
  FUPM2 Instance;
  Instance.get_mem_info();

/*  auto tmp = -123;
  auto tmp1 = (unsigned)tmp;
  cout << tmp1 << endl;
  tmp1 <<= 4u;

  cout << tmp << endl;
  cout << tmp1 << endl;
  tmp1 >>= 4u;
  cout << (signed int)tmp1;*/

  /*int tmp = -32768;
  cout << tmp << endl;
  tmp = (unsigned short)tmp << 2u;
  cout << tmp << endl;
  tmp = (unsigned int)tmp >> 2u;
  cout  << (int)tmp;*/


  return 0;
}