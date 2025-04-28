#ifndef GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H
#define GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H
#include "common.h"
#include "instruction.h"
#include "program.h"

// Define your class for the generalized domain instructions handler
class GeneralizedDomainInstructionsHandler {
public:
    GeneralizedDomainInstructionsHandler(const string& name):
        in_total(0),
        out_total(0),
        name(name) {};
    virtual ~GeneralizedDomainInstructionsHandler()= default;
    virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) = 0;
    virtual string showPruneResult() { return name + ": " + to_string(in_total) + " -> " + to_string(out_total) + "\n";}
protected:
    unordered_map<string, vector<int>> prohibit_map;
    int in_total;
    int out_total;
    string name;
};

#endif // GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H