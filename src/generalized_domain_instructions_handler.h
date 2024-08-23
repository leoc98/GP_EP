#ifndef GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H
#define GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H
#include "common.h"
#include "instruction.h"
#include "program.h"

// Define your class for the generalized domain instructions handler
class GeneralizedDomainInstructionsHandler {
public:
    GeneralizedDomainInstructionsHandler()= default;
    virtual ~GeneralizedDomainInstructionsHandler()= default;
    virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) = 0;
    virtual string showPruneResult() { return "not implement\n";}
};

#endif // GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H