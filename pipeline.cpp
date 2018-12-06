#include <iostream>
#include "instruction.h"

/*********************************** Globals **********************************/
int instruction_count;
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
    test();

    return 0;
}
