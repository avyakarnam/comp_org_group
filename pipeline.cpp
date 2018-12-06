#include <iostream>
#include <fstream>
#include "instruction.h"

/********************************** Typedefs **********************************/
typedef std::pair<std::string, std::string> InstructionData;
typedef std::vector<InstructionData> InstructionDataQueue;
typedef std::vector<Instruction*> InstructionStack;
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
    i -> activate();
    for(clock_cycle = 1; clock_cycle < 17; clock_cycle++) {
        i -> increment_stage();
        i -> print();
        std::cout << i->get_stage() << std::endl;
    }
}

int main(int argc, char const *argv[]) {
    // Command line stuff
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [Forwarding ('F') or Non-Forwarding ('N')]"
                               << " [Input File name]" << std::endl;
        return EXIT_FAILURE;
    }

    if(std::string(argv[1]) != "F" || std::string(argv[1]) != "N") {
        std::cerr << "Invalid forwarding mode: " << argv[1] << std::endl
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
    InstructionDataQueue instruction_queue;

    // maps the branch labels to indexes in the instruction_queue
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
            instruction_queue.push_back(InstructionData(x,s));
            i++;
        }
    }


    return 0;
}
