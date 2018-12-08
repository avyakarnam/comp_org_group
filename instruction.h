// instruction.h
#ifndef INSTRUCTION_H
#define INSTRUCTION_H 

#include <string>
#include <vector>
#include <map>

extern int clock_cycle;
extern std::vector<std::string> num_to_status;

class Instruction {
public:
    Instruction(const std::string& f, const std::string& s): 
        function(f), printable(s), stage_at_cycle(17,0), is_active(true), is_done(false), stall_count(0) {};
    virtual ~Instruction() {}

    void increment_stage();
    void add_stall(int num_stalls);
    virtual void write_back(std::map<std::string, int>& registers) const = 0;
    bool data_hazard_with(const Instruction* other) const;
    Instruction* make_nop() const;

	void override();
    void print() const;
    virtual bool is_branch() const {return false;}
    int get_stage(int cycle) const {return stage_at_cycle[cycle];}
    bool done() const {return is_done;}

private:
    std::string function;
    std::string printable;
    
protected:
    std::string destination;
    std::string arg1;
    std::string arg2;
    std::vector<int> stage_at_cycle;

private:
    bool is_active;     // if the instruction is currently being incremented
    bool is_done;       // if the instruction has been completed
    int stall_count;
};


class Branch : public Instruction {
public:
    Branch(const std::string& f, const std::string& s);
    virtual ~Branch() {}

    void write_back(std::map<std::string, int>& registers) const {return;};
    virtual bool branch_taken(std::map<std::string, int>& registers) const = 0;

    bool is_branch() const {return true;}
    std::string get_destination() const {return destination;}
};


class NonBranch : public Instruction {
public:
    NonBranch(const std::string& f, const std::string& s);
    virtual ~NonBranch() {}

    virtual void write_back(std::map<std::string, int>& registers) const = 0;    
};


class nop : public Instruction {
public:
    nop();
    void write_back(std::map<std::string, int>& registers) const {return;};
};


// Small classes for each instruction type
// Just handle execution of what the instruction does

// add, addi, and, andi, or, ori, slt, slti, beq, bne

class add : public NonBranch {
public:
    add(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class addi : public NonBranch {
public:
    addi(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class and_ : public NonBranch {     // g++ doesn't like classes named "and"
public:
    and_(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class andi : public NonBranch {
public:
    andi(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class or_ : public NonBranch {      // g++ doesn't like classes named "or"
public:
    or_(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class ori : public NonBranch {
public:
    ori(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class slt : public NonBranch {
public:
    slt(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class slti : public NonBranch {
public:
    slti(const std::string& f, const std::string& s) : NonBranch(f,s) {};
    void write_back(std::map<std::string, int>& registers) const;
};

class beq : public Branch {
public:
    beq(const std::string& f, const std::string& s) : Branch(f,s) {};
    bool branch_taken(std::map<std::string, int>& registers) const;    
};

class bne : public Branch {
public:
    bne(const std::string& f, const std::string& s) : Branch(f,s) {};
    bool branch_taken(std::map<std::string, int>& registers) const;
};

#endif
