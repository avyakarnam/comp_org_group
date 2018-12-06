#include "instruction.h"

/*********************************** Globals **********************************/

// To convert stage numbers to status symbols
std::vector<std::string> num_to_status = {".", "IF", "ID", "EX", "MEM", "WB", ".", "*", "*", "*", "."};

int instruction_count;
