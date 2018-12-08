#include <iomanip>
#include <iostream>
#include <sstream>
#include "instruction.h"

void Instruction::increment_stage() {
    if(is_active) {
        if(stall_count == 0) {
            // current stage = previous stage + 1
            stage_at_cycle[clock_cycle] = stage_at_cycle[clock_cycle-1] + 1;
            // check if done/active
            is_active = !(is_done);
            is_done = is_done
                    || stage_at_cycle[clock_cycle] == 5         // WB
                    || stage_at_cycle[clock_cycle] == 9;        // last nop stage
       }else {
            // current stage = previous stage
            stage_at_cycle[clock_cycle] = stage_at_cycle[clock_cycle-1];
            stall_count--;
       }
    }
}

void Instruction::add_stall(int num_stalls) {
    stall_count += num_stalls;
    // more?
}

bool Instruction::data_hazard_with(const Instruction* other) const {
    return (other -> destination == arg1) || (other -> destination == arg2);
}

Instruction* Instruction::make_nop() const {
    Instruction* res = new nop();
    res -> stage_at_cycle = this -> stage_at_cycle;
    res -> stage_at_cycle[clock_cycle] = 7;         // first nop symbol = 7
    return res;
}

void Instruction::override() {
	stall_count += (5 - get_stage(clock_cycle - 1) - 2);
    stage_at_cycle[clock_cycle] = 8;
}

void Instruction::print() const {
    std::cout << std::setw(20) << std::left << (function + " " + printable);        // why is setw absolute garbage????
    for(int i = 1; i < 17; i++) {
        std::cout << " " << std::setw(4) << std::left << num_to_status[stage_at_cycle[i]];
    }
    std::cout << std::endl;
}

Branch::Branch(const std::string& f, const std::string& s) : Instruction(f,s) {
    std::stringstream ss(s);
    getline( ss, this -> arg1, ',' );
    getline( ss, this -> arg2, ',' );
    ss >> this -> destination;
}

NonBranch::NonBranch(const std::string& f, const std::string& s) : Instruction(f,s) {
    std::stringstream ss(s);
    getline( ss, this -> destination, ',' );
    getline( ss, this -> arg1, ',' );
    ss >> this -> arg2;
}

nop::nop() : Instruction("nop","") {
    destination = "";
    arg1 = "";
    arg2 = "";
}

void add::write_back(std::map<std::string, int>& registers) const {
    registers[destination] = registers[arg1] + registers[arg2];
}

void addi::write_back(std::map<std::string, int>& registers) const {
    if(arg1[0] != '$')  // arg1 is the immediate operand
        registers[destination] = atoi(arg1.c_str()) + registers[arg2];
    else    // arg2 is the immediate operand
        registers[destination] = registers[arg1] + atoi(arg2.c_str());
}

void and_::write_back(std::map<std::string, int>& registers) const {
    registers[destination] = registers[arg1] & registers[arg2];
}

void andi::write_back(std::map<std::string, int>& registers) const {
    if(arg1[0] != '$')  // arg1 is the immediate operand
        registers[destination] = atoi(arg1.c_str()) & registers[arg2];
    else    // arg2 is the immediate operand
        registers[destination] = registers[arg1] & atoi(arg2.c_str());
}

void or_::write_back(std::map<std::string, int>& registers) const {
    registers[destination] = registers[arg1] | registers[arg2];
}

void ori::write_back(std::map<std::string, int>& registers) const {
    if(arg1[0] != '$')  // arg1 is the immediate operand
        registers[destination] = atoi(arg1.c_str()) | registers[arg2];
    else    // arg2 is the immediate operand
        registers[destination] = registers[arg1] | atoi(arg2.c_str());
}

void slt::write_back(std::map<std::string, int>& registers) const {
    registers[destination] = registers[arg1] < registers[arg2];
}

void slti::write_back(std::map<std::string, int>& registers) const {
    if(arg1[0] != '$')  // arg1 is the immediate operand
        registers[destination] = atoi(arg1.c_str()) < registers[arg2];
    else    // arg2 is the immediate operand
        registers[destination] = registers[arg1] < atoi(arg2.c_str());
}

// Note: I don't think you can have a beq/bne with an immediate operand
bool beq::branch_taken(std::map<std::string, int>& registers) const {
    return registers[arg1] == registers[arg2];
}

bool bne::branch_taken(std::map<std::string, int>& registers) const {
    return registers[arg1] != registers[arg2];
}
