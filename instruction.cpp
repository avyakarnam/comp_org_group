#include "instruction.h"

void Instruction::increment_stage() {
    // TODO
}

void Instruction::add_stall(int num_stalls) {
    stall_count += num_stalls;
    // more?
}

bool Instruction::data_hazard(const Instruction &other) const {
    return (other.destination == arg1) || (other.destination == arg2);
}

Instruction* Instruction::make_nop() const {
    // TODO
    return NULL;
}

void Instruction::print() const {
    // TODO
}

Branch::Branch(const std::string& f, const std::string& s) : Instruction(f,s) {
    // TODO
}

NonBranch::NonBranch(const std::string& f, const std::string& s) : Instruction(f,s) {
    // TODO
}

nop::nop() : Instruction("nop","") {
    // TODO
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
