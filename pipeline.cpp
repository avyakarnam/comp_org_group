#include <iostream>
#include <fstream>
#include <list>
#include <iomanip>
#include <sstream>
#include "instruction.h"

/********************************** Typedefs **********************************/
typedef std::pair<std::string, std::string> InstructionData;
typedef std::vector<InstructionData> InstructionDataQueue;
typedef std::list<Instruction*> InstructionStack;
typedef std::map<std::string, int> RegisterMap;
typedef std::map<std::string, int> BranchMap;

/*********************************** Globals **********************************/
// global clock cycle
int clock_cycle;
// To convert stage numbers to status symbols
std::vector<std::string> num_to_status = {".", "IF", "ID", "EX", "MEM", "WB", ".", "*", "*", "*", "."};


void test() {
    Instruction* i = new add("add", "$t0,$t0,$s0");
    i -> print();
    std::cout << i -> is_branch() << std::endl;
    for(clock_cycle = 1; clock_cycle < 17; clock_cycle++) {
        i -> increment_stage();
        i -> print();
        std::cout << i->get_stage(clock_cycle) << std::endl;
    }
}

Instruction* create_instruction(const InstructionData& i) {
    std::string f = i.first;
    std::string s = i.second;
    // add, addi, and, andi, or, ori, slt, slti, beq, bne
    if(f == "add")
        return new add(f, s);
    else if(f == "addi")
        return new addi(f, s);
    else if(f == "and_")
        return new and_(f, s);
    else if(f == "andi")
        return new andi(f, s);
    else if(f == "or_")
        return new or_(f, s);
    else if(f == "ori")
        return new ori(f, s);
    else if(f == "slt")
        return new slt(f, s);
    else if(f == "slti")
        return new slti(f, s);
    else if(f == "beq")
        return new beq(f, s);
    else if(f == "bne")
        return new bne(f, s);
    else{
        // you done fucked up
        std::cerr << "Error: unrecognized instruction " << f << std::endl;
        exit(1); 
    }
}

void data_hazards(InstructionStack& instructions, const InstructionStack::iterator instr, bool forwarding) {
    if(instr == instructions.begin())       // no data hazard for first instruction
        return;
    InstructionStack::iterator other = instr;

    // Checking 1 instruction back
    other--;
    if((*instr) -> data_hazard_with(*other)) {
        for(int i = 0; i < 1 + !forwarding; i++) {
            Instruction* new_nop = (*instr) -> make_nop();
            new_nop -> print();
            instructions.insert(instr, new_nop);
        }
        (*instr) -> add_stall(1 + !forwarding);
        return;
    }

    // Checking 2 instructions back
    if(!forwarding && other != instructions.begin()) {
        other--;
        if((*instr) -> data_hazard_with(*other)) {
            Instruction* new_nop = (*instr) -> make_nop();
            new_nop -> print();
            instructions.insert(instr, new_nop);
            (*instr) -> add_stall(1);
        }
    }
}

void control_hazards(InstructionStack& instructions, const InstructionStack::iterator instr){
    //update instruction numbers
    //insert the new instruction
    //call the override function
    int instr_count = 0;
    for(InstructionStack::iterator i = instr; i != instructions.end() && instr_count<3 ; i++, instr_count++) {
        (*instr) -> override();
    }
}

void print_dashes() {
    std::cout << std::string(20 + 5*16, '-') << std::endl;
}

void print_cycles() {
    std::cout << std::setw(20) << std::left << "CPU Cyles ===>";
    for(int i = 1; i < 17; i++) {
        std::cout << " " << std::setw(4) << std::left << i;
    }
    std::cout << std::endl;
}

void print_registers(const RegisterMap& registers) {
    std::cout << std::endl;
    RegisterMap::const_iterator i = registers.begin();
    int counter = 1;
    for(; i != registers.end(); i++, counter++) {
        if(i -> first == "$zero")
            continue;
        std::stringstream unit;     // absolute unit
        unit << i -> first << " = " << i -> second;
        std::cout << std::setw(20) << std::left << unit.str();      // setw remains the most useless function in the STL
        if(counter % 4 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char const *argv[]) {
    // Command line stuff
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [Forwarding mode]" << " [Input File name]" << std::endl;
        return EXIT_FAILURE;
    }

    if(std::string(argv[1]) != "F" && std::string(argv[1]) != "N") {
        std::cerr << "Invalid forwarding mode: |" << std::string(argv[1]) << "|" << std::endl
                  << "Please use 'F' for forwarding or 'N' for non-forwarding" << std::endl;
        return EXIT_FAILURE;
    }

    bool forwarding = std::string(argv[1]) == "F";

    std::ifstream input(argv[2]);
    if(!input.good()) {
        std::cerr << "Error: Could not open file " << argv[2] << " to read" << std::endl;
        return EXIT_FAILURE;
    }


    // Input parsing

    // stores the information about instructions in string form
    // these are used to make Instruction objects later
    InstructionDataQueue instruction_memory;

    // maps the branch labels to indexes in the instruction_memory
    // so we know where branches take us
    BranchMap branch_labels;

    std::string x;
    int i = 0;
    while(input >> x) {
        if(x[x.length() - 1] == ':') {    // branch labels end with a colon
            std::string label = x.substr(0,x.length()-1);

            // labels map to the next instruction
            branch_labels.insert(std::pair<std::string, int>(label,i));
        }else {
            // x is the function name, s is the rest of the line
            std::string s;
            input >> s;
            instruction_memory.push_back(InstructionData(x,s));
            i++;
        }
    }


    // Setup

    RegisterMap registers {     // I <3 initializer lists
        {"$s0", 0},
        {"$s1", 0},
        {"$s2", 0},
        {"$s3", 0},
        {"$s4", 0},
        {"$s5", 0},
        {"$s6", 0},
        {"$s7", 0},
        {"$t0", 0},
        {"$t1", 0},
        {"$t2", 0},
        {"$t3", 0},
        {"$t4", 0},
        {"$t5", 0},
        {"$t6", 0},
        {"$t7", 0},
        {"$t8", 0},
        {"$t9", 0},
        {"$zero", 0}
    };
    InstructionStack instructions;
    unsigned int instruction_num = 0;


    // Simulation

    std::cout << "START OF SIMULATION (" << (forwarding ? "" : "no ") << "forwarding)" << std::endl;

    for(clock_cycle = 1; clock_cycle < 17; clock_cycle++) {
        print_dashes();
        print_cycles();
        // add instruction (if possible)
        // check for hazards
        //      inset nops
        // increment stage
        // print

        if(instruction_num != instruction_memory.size()) {    // not out of instructions
            instructions.push_back(create_instruction(instruction_memory[instruction_num]));
            instruction_num++;
        }
        
        for(InstructionStack::iterator i = instructions.begin(); i != instructions.end(); i++) {
            if((*i) -> get_stage(clock_cycle-1) == 2) {         // ID stage
                data_hazards(instructions, i, forwarding);
            }
            (*i) -> increment_stage();

            if((*i) -> get_stage(clock_cycle) == 5) {             // WB stage
                (*i) -> write_back(registers);
                if((*i) -> is_branch()) {
                    if(dynamic_cast<Branch*>(*i) -> branch_taken(registers)) {
                        control_hazards(instructions, i);

                        // update instruction_num to appropriate post-loop position
                        instruction_num = branch_labels[dynamic_cast<Branch*>(*i) -> get_destination()];
                        instructions.push_back(create_instruction(instruction_memory[instruction_num]));
                        instruction_num++;
                    }
                }
            }

            (*i) -> print();
        }

        print_registers(registers);

        if((*(--instructions.end())) -> done())     // the last instruction is done
            break;
    }

    print_dashes();
    std::cout << "END OF SIMULATION" << std::endl;


    return EXIT_SUCCESS;
}
