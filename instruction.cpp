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

bool Instruction::data_hazard_with(const Instruction &other) const {
    return (other.destination == arg1) || (other.destination == arg2);
}

Instruction* Instruction::make_nop() const {
    Instruction* res = new nop();
    res -> stage_at_cycle = this -> stage_at_cycle;
    res -> stage_at_cycle[clock_cycle] = 7;         // first nop symbol = 7
    return res;
}

void Instruction::print() const {
    std::cout << std::setw(20) << std::left << function << " " << printable;
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
    // TODO
}

void addi::write_back(std::map<std::string, int>& registers) const {
    //TODO
}

void or_::write_back(std::map<std::string, int>& registers) const {
    //TODO
}

void ori::write_back(std::map<std::string, int>& registers) const {
    //TODO
}

void slt::write_back(std::map<std::string, int>& registers) const {
    //TODO
}

void slti::write_back(std::map<std::string, int>& registers) const {
    //TODO
}

bool beq::branch_taken(std::map<std::string, int>& registers) const {
    // TODO
    return false;
}

bool bne::branch_taken(std::map<std::string, int>& registers) const {
    // TODO
    return false;
}
