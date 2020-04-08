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
    buffer->pop_back();  //TODO: find the cause of the doubled 'main' in the end of file in the first sample

    file.close();
    return true;
  }

  return false;
}

void vec_print( const vector<string> &buffer ) {
  for_each(buffer.begin(), buffer.end(), [](const string &str) { cout << str << ' '; });
  cout << endl;
}

class Commands {
protected:
  enum command_code {
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
    STORER2 = 71,
    WORD = 72, // directive
    END = 73   // directive
  };
  enum command_type {
    RM = 0,
    RR = 1,
    RI = 2,
    J = 3,
    D = 4  // directive format
  };

  typedef pair<command_code, command_type> Command;

  map<string, Command> command_dictionary = {
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
    {"ret",     {RET, J}},    //TODO: 'ret' usage in examples does not fit the 'RI' type
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
    {"storer2", {STORER2, RR}},
    {"word",    {WORD, D}},
    {"end",     {END, D}}
  };

  map<int, string> debug_comm_dictionary {
    {0, "halt"},
    {1, "syscall"},
    {2, "add"},
    {3, "addi"},
    {4, "sub"},
    {5, "subi"},
    {6, "mul"},
    {7, "muli"},
    {8, "div"},
    {9, "divi"},
    {12, "lc"},
    {13, "shl"},
    {14, "shli"},
    {15, "shr"},
    {16, "shri"},
    {17, "and"},
    {18, "andi"},
    {19, "or"},
    {20, "ori"},
    {21, "xor"},
    {22, "xori"},
    {23, "not"},
    {24, "mov"},
    {32, "addd"},
    {33, "subd"},
    {34, "muld"},
    {35, "divd"},
    {36, "itod"},
    {37, "dtoi"},
    {38, "push"},
    {39, "pop"},
    {40, "call"},
    {41, "calli"},
    {42, "ret"},
    {43, "cmp"},
    {44, "cmpi"},
    {45, "cmpd"},
    {46, "jmp"},
    {47, "jne"},
    {48, "jeq"},
    {49, "jle"},
    {50, "jl"},
    {51, "jge"},
    {52, "jg"},
    {64, "load"},
    {65, "store"},
    {66, "load2"},
    {67, "store2"},
    {68, "loadr"},
    {69, "loadr2"},
    {70, "storer"},
    {71, "storer2"},
    {72, "word"},
    {73, "end"}
  };

  /***
   * Functions for getting operands and register numbers from 32-bit command words in memory.
   ***/

  static int get_RM_operand( unsigned const &command ) {
    return (int)(command >> 12u);
  }

  static unsigned get_RM_reg_num( unsigned const &command ) {
    return command << 20u >> 28u;
  }

  static int get_RR_operand( unsigned const &command ) {
    unsigned bit_mask = command >> 31u ? 1048575u << 16u : 0;
    return (int)((command >> 16u) | bit_mask);
  }

  static unsigned get_RR_first_reg_num( unsigned const &command ) {
    return command << 20u >> 28u;
  }

  static unsigned get_RR_second_reg_num( unsigned const &command ) {
    return command << 16u >> 28u;
  }

  static int get_RI_operand( unsigned const &command ) {
    unsigned bit_mask = command >> 31u ? 4095u << 20u : 0;
    return (int)((command >> 12u) | bit_mask);
  }

  static unsigned get_RI_reg_num( unsigned const &command ) {
    return command << 20u >> 28u;
  }

  static int get_J_operand( unsigned const &command ) {
    return (int)(command >> 12u);
  }

  /***
   ***
   ***/
};
/* End of 'Operaitons' class */

class FUPM2 : private Commands {
private:
  vector<unsigned> memory;
  vector<int> r;
  map<string, int> labels;
  int flags;
  int error_line;

  map<string, int> register_dictionary = {
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

  map <int, string> debug_reg_dictionary = {
    {0, "r0"},
    {1, "r1"},
    {2, "r2"},
    {3, "r3"},
    {4, "r4"},
    {5, "r5"},
    {6, "r6"},
    {7, "r7"},
    {8, "r8"},
    {9, "r9"},
    {10, "r10"},
    {11, "r11"},
    {12, "r12"},
    {13, "r13"},
    {14, "r14"},
    {15, "r15"}
  };

  enum system_call {
    EXIT = 0,
    SCANINT = 100,
    SCANDOUBLE = 101,
    PRINTINT = 102,
    PRINTDOUBLE = 103,
    GETCHAR = 104,
    PUTCHAR = 105
  };

  /***
   * decode_word Decodes 32-bit word from the inner representation of 'FUPM2OS'.
   * @param word Unsigned integer constant which contains word.
   * @param type command_type constant which contains the type of command in word param.
   */
  void decode_word( const unsigned &word, const command_type &type ) {
    switch (type) {
      case RM: {
       /* cout << "Command code: " << ((word << 24u) >> 24u) << endl;
        cout << "Register code: " << ((word << 20u) >> 28u) << endl;
        cout << "Modificator: " << (word >> 12u) << endl << endl;*/
        cout << debug_comm_dictionary[((word << 24u) >> 24u)] << " " << debug_reg_dictionary[((word << 20u) >> 28u)] << " " << (word >> 12u) << endl;
        break;
      }

      case RR: {
        /*cout << "Command code: " << ((word << 24u) >> 24u) << endl;
        cout << "Register #1 code: " << ((word << 20u) >> 28u) << endl;
        cout << "Register #2 code: " << ((word << 16u) >> 28u) << endl;
        cout << "Modificator: " << (word >> 16u) << endl << endl;*/
        unsigned lead_bit_mask = word >> 31u ? 1048575u << 16u : 0;
        cout << debug_comm_dictionary[((word << 24u) >> 24u)] << " " << debug_reg_dictionary[((word << 20u) >> 28u)] << " "
             << debug_reg_dictionary[((word << 16u) >> 28u)] << " " << (signed int)(word >> 16u | lead_bit_mask) << endl;
        break;
      }

      case RI: {
       /*  cout << "Command code: " << ((word << 24u) >> 24u) << endl;
        cout << "Register code: " << ((word << 20u) >> 28u) << endl;
        cout << "Modificator: " << (signed int)(word >> 12u) << endl << endl;*/
        unsigned lead_bit_mask = word >> 31u ? 4095u << 20u : 0;
        cout << debug_comm_dictionary[((word << 24u) >> 24u)] << " " << debug_reg_dictionary[((word << 20u) >> 28u)] << " "
             << (signed int)((word >> 12u) | lead_bit_mask) << endl;

        break;
      }

      case J: {
        /*cout << "Command code: " << ((word << 24u) >> 24u) << endl;
        cout << "Modificator: " << (word >> 12u) << endl << endl;*/
        cout << debug_comm_dictionary[((word << 24u) >> 24u)] << " " << (word >> 12u) << endl;
        break;
      }

      case D: {
        cout << debug_comm_dictionary[word] << endl;
        break;
      }

      default:
        ;
    }
  }
  /* End of 'decode_word' function */

  /**
   * parser Converts assembler code in the text file to the inner representation of 'FUPM2OS'.
   * @param filename String constant which contains the name of file with assembler code.
   * @return The result of command: 'true' if successful, 'false' otherwise.
   */
  bool parser( const string &filename ) {
    vector<string> buffer;

    if (read_file(&buffer, filename)) {
      vec_print(buffer);

      auto curr_word = memory.begin();

      // creating map of labels
      for (auto word_part = buffer.begin(); word_part != buffer.end(); ) {
        if (word_part->back() == ':') {
          if (isdigit((*word_part)[0])) {
            cout << "error in line " << curr_word - memory.begin() << " : Label begins with numeral" << endl; // TODO: write error function
            return false;
          }
          word_part->pop_back();
          labels[*word_part] = curr_word - memory.begin();
          cout << "Label '" << *word_part << "' " << "refers to line " << curr_word - memory.begin() << endl;
          word_part->push_back(':');
          word_part++;
        }
        else if (command_dictionary.find(*word_part++) != command_dictionary.end()) {
          curr_word++;
        }
      }
      cout << endl;

      // obtaining commands
      curr_word = memory.begin();
      for (auto word_part = buffer.begin(); word_part != buffer.end(); ) {
        if (word_part->back() == ':') {       // label check
          cout << *word_part++ << endl;
        }
        else {
          if (command_dictionary.find(*word_part) == command_dictionary.end()) {
            cout << "error in line " << curr_word - memory.begin() << " : No appropriate command for expression '"
                 << *word_part << "'" << endl; //TODO: edit
            buffer.clear();
            return false;
          }

          Command current_command = command_dictionary[*word_part++];

          switch (current_command.second) {
            case RM: {
              auto register_code = (unsigned)register_dictionary[*word_part++];
              auto command_modificator = isdigit((*word_part)[0]) ? (unsigned)stoi(*word_part++) : labels[*word_part++];
              memory.insert(curr_word++, ((unsigned)current_command.first) | (register_code << 8u) |
                                    (command_modificator << 12u));
              decode_word(*(curr_word-1), RM);
              break;
            }

            case RR: {
              auto first_register_code = (unsigned)register_dictionary[*word_part++];
              auto second_register_code = (unsigned) register_dictionary[*word_part++];
              auto command_modificator = (unsigned)(stoi(*word_part++));;
              memory.insert(curr_word++, ((unsigned) current_command.first) | (first_register_code << 8u) |
                                    (second_register_code << 12u) | ((unsigned) command_modificator << 16u));
              decode_word(*(curr_word-1), RR);
              break;
            }

            case RI: {
              auto register_code = (unsigned)(register_dictionary[*word_part++]);
              auto command_modificator = stoi(*word_part++);
              memory.insert(curr_word++, ((unsigned) current_command.first) | (register_code << 8u) |
                                    ((unsigned) command_modificator << 12u));
              decode_word(*(curr_word-1), RI);
              //unsigned lead_bit_mask = *(curr_word-1) >> 31u ? 4095u << 20u : 0;
              //cout << (signed int)((*(curr_word-1) >> 12u) | lead_bit_mask) << endl; //TODO: why it did not work: "(*(curr_word-1) >> 31u) ? 4095u << 20u : 0u) << endl;" ?
              break;
            }

            case J: {
              auto command_modificator = isdigit((*word_part)[0]) ? (unsigned)(stoi(*word_part++)) : labels[*word_part++];
              memory.insert(curr_word++, ((unsigned) current_command.first) | (command_modificator << 12u));
              decode_word(*(curr_word-1), J);
              break;
            }

            case D: {
              if (current_command.first == WORD) {
                memory.insert(curr_word++, WORD);
                decode_word(*(curr_word-1), D);
              }
              else if (current_command.first == END) {
                r[15] = isdigit((*word_part)[0]) ? stoi(*word_part++) : labels[*word_part++];
                memory.insert(curr_word, END); // curr_word was not incremented and points to the end of the used memory part
                decode_word(*(curr_word), D);
                if (word_part != buffer.end()) {
                  cout << "error in line" << curr_word - memory.begin() << ": code after 'end' label" << endl; //TODO: edit
                  return false;
                }
              }
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
  /* End of 'parser' function */

  /**
   * program_run initiates and runs the execution of the sequence of commands in the memory.
   */
  void program_run() {
    while(true) {
      unsigned curr_comm = memory[r[15]];
      switch (curr_comm & 255u) {
        case HALT: {
          cout << "HALT" << endl;
          break;
        }

        case SYSCALL: {
          cout << "SYSCALL " << get_RI_operand(curr_comm) << endl;
          int curr_reg = get_RI_reg_num(curr_comm);
          switch (get_RI_operand(curr_comm)) {
            case EXIT: {
              break;
            }

            case SCANINT: {
              scanf("%i", &r[curr_reg]);
              break;
            }

            case SCANDOUBLE: {
              // TODO: how to scan one 64-bit double in two 32-bit registers?
              // TODO: as two registers are used it is necessary curr_reg < 12
              break;
            }

            case PRINTINT: {
              printf("%i", r[curr_reg]);
              break;
            }

            case PRINTDOUBLE: {
              break;
            }

            case GETCHAR: {
              char tmp;
              scanf("%c", &tmp);
              r[curr_reg] = tmp;
              break;
            }

            case PUTCHAR: {
              printf("%c", r[curr_reg]);
              break;
            }

            default: {
              cout << "ERROR: No appropriate system call code. Memory address: " << r[15] << endl;
              return;
            }
          }

          r[15]++;
          break;
        }

        case ADD: {
          cout << "ADD" << endl;
          r[get_RR_first_reg_num(curr_comm)] += r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm);
          r[15]++;
          break;
        }

        case ADDI: {
          cout << "ADDI" << endl;
          r[get_RI_reg_num(curr_comm)] += r[get_RI_operand(curr_comm)];
          r[15]++;
          break;
        }

        case SUB: {
          cout << "SUB" << endl;
          r[get_RR_first_reg_num(curr_comm)] -= r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm);
          r[15]++;
          break;
        }

        case SUBI: {
          cout << "SUBI" << endl;
          r[get_RI_reg_num(curr_comm)] -= r[get_RI_operand(curr_comm)];
          r[15]++;
          break;
        }

        case MUL: {
          cout << "MUL" << endl;
          r[15]++;
          break;
        }

        case MULI: {
          cout << "MULI" << endl;
          r[15]++;
          break;
        }

        case DIV: {
          cout << "DIV" << endl;
          r[15]++;
          break;
        }

        case DIVI: {
          cout << "DIVI" << endl;
          r[15]++;
          break;
        }

        case LC: {
          cout << "LC" << endl;
          r[get_RI_reg_num(curr_comm)] = get_RI_operand(curr_comm);
          r[15]++;
          break;
        }

        // TODO: how to shift signed numbers, looking at sign bit or not?
        case SHL: {
          cout << "SHL" << endl;
          r[get_RR_first_reg_num(curr_comm)] = (unsigned)r[get_RR_first_reg_num(curr_comm)] <<
                                               (unsigned)(r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm));
          r[15]++;
          break;
        }

        case SHLI: {
          cout << "SHLI" << endl;
          r[get_RI_reg_num(curr_comm)] = (unsigned)r[get_RI_reg_num(curr_comm)] << (unsigned)r[get_RI_operand(curr_comm)];
          r[15]++;
          break;
        }

        case SHR: {
          cout << "SHR" << endl;
          r[get_RR_first_reg_num(curr_comm)] = (unsigned)r[get_RR_first_reg_num(curr_comm)] >>
                                               (unsigned)(r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm));
          r[15]++;
          break;
        }

        case SHRI: {
          cout << "SHRI" << endl;
          r[get_RI_reg_num(curr_comm)] = (unsigned)r[get_RI_reg_num(curr_comm)] >> (unsigned)r[get_RI_operand(curr_comm)];
          r[15]++;
          break;
        }

        case AND: {
          cout << "AND" << endl;
          r[get_RR_first_reg_num(curr_comm)] = (unsigned)r[get_RR_first_reg_num(curr_comm)] &
                                               (unsigned)(r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm));
          r[15]++;
          break;
        }

        case ANDI: {
          cout << "ANDI" << endl;
          r[get_RI_reg_num(curr_comm)] = (unsigned)r[get_RI_reg_num(curr_comm)] & (unsigned)r[get_RI_operand(curr_comm)];
          r[15]++;
          break;
        }

        case OR: {
          cout << "OR" << endl;
          r[get_RR_first_reg_num(curr_comm)] = (unsigned)r[get_RR_first_reg_num(curr_comm)] |
                                               (unsigned)(r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm));
          r[15]++;
          break;
        }

        case ORI: {
          cout << "ORI" << endl;
          r[get_RI_reg_num(curr_comm)] = (unsigned)r[get_RI_reg_num(curr_comm)] | (unsigned)r[get_RI_operand(curr_comm)];
          r[15]++;
          break;
        }

        case XOR: {
          cout << "XOR" << endl;
          r[get_RR_first_reg_num(curr_comm)] = (unsigned)r[get_RR_first_reg_num(curr_comm)] ^
                                               (unsigned)(r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm));
          r[15]++;
          break;
        }

        case XORI: {
          cout << "XORI" << endl;
          r[get_RI_reg_num(curr_comm)] = (unsigned)r[get_RI_reg_num(curr_comm)] ^ (unsigned)r[get_RI_operand(curr_comm)];
          r[15]++;
          break;
        }

        case NOT: {
          cout << "NOT" << endl;
          r[get_RI_reg_num(curr_comm)] = !((unsigned)r[get_RI_reg_num(curr_comm)]);
          r[15]++;
          break;
        }

        case MOV: {
          cout << "MOV" << endl;
          r[get_RR_first_reg_num(curr_comm)] = r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm);
          r[15]++;
          break;
        }

        case ADDD: {
          cout << "ADDD" << endl;
          break;
        }

        case SUBD: {
          cout << "SUBD" << endl;
          break;
        }

        case MULD: {
          cout << "MULD" << endl;
          break;
        }

        case DIVD: {
          cout << "DIVD" << endl;
          break;
        }

        case ITOD: {
          cout << "ITOD" << endl;
          break;
        }

        case DTOI: {
          cout << "DTOI" << endl;
          break;
        }

        case PUSH: {
          cout << "PUSH" << endl;
          memory[--r[14]] = r[get_RI_reg_num(curr_comm)] + get_RI_operand(curr_comm);
          r[15]++;
          break;
        }

        case POP: {
          cout << "POP" << endl;
          r[get_RI_reg_num(curr_comm)] = memory[r[14]++] + get_RI_operand(curr_comm);
          r[15]++;
          break;
        }

        case CALL: {
          cout << "CALL" << endl;
          memory[--r[14]] = r[get_RR_first_reg_num(curr_comm)] = r[15] + 1;
          r[15] = r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm);
          break;
        }

        case CALLI: {
          cout << "CALLI" << endl;
          memory[--r[14]] = r[15] + 1;
          r[15] = get_J_operand(curr_comm);
          break;
        }

        case RET: {
          cout << "RET" << endl;
          break;
        }

        case CMP: {
          cout << "CMP" << endl;
          if (r[get_RR_first_reg_num(curr_comm)] == r[get_RR_second_reg_num(curr_comm)]) {
            flags = 0;
          }
          else if (r[get_RR_first_reg_num(curr_comm)] > r[get_RR_second_reg_num(curr_comm)]) {
            flags = 1;
          }
          else {
            flags = -1;
          }
          r[15]++;
          break;
        }

        case CMPI: {
          cout << "CMPI" << endl;
          if (r[get_RI_reg_num(curr_comm)] == get_RI_operand(curr_comm)) {
            flags = 0;
          }
          else if (r[get_RI_reg_num(curr_comm)] > get_RI_operand(curr_comm)) {
            flags = 1;
          }
          else {
            flags = -1;
          }
          r[15]++;
          break;
        }

        case CMPD: {
          cout << "CMPD" << endl;
          break;
        }

        case JMP: {
          cout << "JMP" << endl;
          r[15] = get_J_operand(curr_comm);
          break;
        }

        case JNE: {
          cout << "JNE" << endl;
          if (flags != 0) {
            r[15] = get_J_operand(curr_comm);
          }
          else {
            r[15]++;
          }
          break;
        }

        case JEQ: {
          cout << "JEQ" << endl;
          if (flags == 0) {
            r[15] = get_J_operand(curr_comm);
          }
          else {
            r[15]++;
          }
          break;
        }

        case JLE: {
          cout << "JLE" << endl;
          if (flags <= 0) {
            r[15] = get_J_operand(curr_comm);
          }
          else {
            r[15]++;
          }
          break;
        }

        case JL: {
          cout << "JL" << endl;
          if (flags < 0) {
            r[15] = get_J_operand(curr_comm);
          }
          else {
            r[15]++;
          }
          break;
        }

        case JGE: {
          cout << "JGE" << endl;
          if (flags >= 0) {
            r[15] = get_J_operand(curr_comm);
          }
          else {
            r[15]++;
          }
          break;
        }

        case JG: {
          cout << "JG" << endl;
          if (flags > 0) {
            r[15] = get_J_operand(curr_comm);
          }
          else {
            r[15]++;
          }
          break;
        }

        case LOAD: {
          cout << "LOAD" << endl;
          r[get_RM_reg_num(curr_comm)] = memory[get_RM_operand(curr_comm)];
          r[15]++;
          break;
        }

        case STORE: {
          cout << "STORE" << endl;
          memory[get_RM_operand(curr_comm)] = r[get_RM_reg_num(curr_comm)];
          r[15]++;
          break;
        }

        case LOAD2: {
          cout << "LOAD2" << endl;
          r[get_RM_reg_num(curr_comm)] = memory[get_RM_operand(curr_comm)];
          r[get_RM_reg_num(curr_comm) + 1] = memory[get_RM_operand(curr_comm) + 1];
          r[15]++;
          break;
        }

        case STORE2: {
          cout << "STORE2" << endl;
          memory[get_RM_operand(curr_comm)] = r[get_RM_reg_num(curr_comm)];
          memory[get_RM_operand(curr_comm) + 1] = r[get_RM_reg_num(curr_comm) + 1];
          r[15]++;
          break;
        }

        case LOADR: {
          cout << "LOADR" << endl;
          r[get_RR_first_reg_num(curr_comm)] = memory[r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm)];
          r[15]++;
          break;
        }

        case STORER: {
          cout << "STORER" << endl;
          memory[r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm)] = r[get_RR_first_reg_num(curr_comm)];
          r[15]++;
          break;
        }

        case LOADR2: {
          cout << "LOADR2" << endl;
          r[get_RR_first_reg_num(curr_comm)] = memory[r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm)];
          r[get_RR_first_reg_num(curr_comm) + 1] = memory[r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm) + 1];
          r[15]++;
          break;
        }


        case STORER2: {
          cout << "STORER2" << endl;
          memory[r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm)] = r[get_RR_first_reg_num(curr_comm)];
          memory[r[get_RR_second_reg_num(curr_comm)] + get_RR_operand(curr_comm) + 1] = r[get_RR_first_reg_num(curr_comm) + 1];
          r[15]++;
          break;
        }

        default: {
          return;
        }
      }
    }
  }
  /* End of 'program_run' function */



public:
  FUPM2() {
    memory.reserve(1u<<20u);
    r.resize(16);
    r[14] = (1u<<20u) - 1;
    flags = 0;

    if (!parser("input.fasm")) {
      cout << endl << endl << "ERROR!!! Failed to parse file" << endl << endl; //TODO: remove this error message and obtain error
    }
  }
  
  void get_mem_info() {
    cout << "Memory reserved: " << memory.capacity() << endl;
    cout << "Memory used: " << memory.size() << endl;
    cout << "Registers:" << endl;
    for (int i = 0; i < r.size(); i++) {
      cout << "  " << "r" << i << ": " << r[i] << endl;
    }
    cout << "  " << "Flags: " << flags << endl;
    cout << "  " << "Size of one register is " << sizeof(r[0]) << " bytes" << endl;
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

  /*for (int i = 0; i < 52; i++) {
    string tmp;
    cin >> tmp;
    transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    cout << "{" << Instance.command_dictionary[tmp].first << ", \"" << tmp << "\"}," << endl;
  }*/

  /*for (int i = 0; i < 16; i++) {
    cout << "{" << i << ", \"r" << i << "\"}," << endl;
  }*/
  /*unsigned int a = 1;
  for (int i = 1; i <= 12; i++) {
    cout << i << " - " << a << endl;
    a <<= 1u;
    a |= 1u;
  }*/

 /* for (int i = 0; i < 52; i++) {
    string tmp;
    cin >> tmp;
    cout << "case " << tmp << ": {" << endl;
    cout << "  cout << \"" << tmp << "\" << endl;" << endl;
    cout << "  break;" << endl << "}" << endl << endl;
  }*/

  return 0;
}