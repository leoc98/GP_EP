#ifndef GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H
#define GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H
#include "common.h"
#include "instruction.h"
#include "program.h"

// Define your class for the generalized domain instructions handler
class GeneralizedDomainInstructionsHandler {
public:
    virtual vector< Instruction* > filter( int program_line, const vector< Instruction* >& instructions, const Program* program ) = 0;
};

#endif // GENERALIZED_DOMAIN_INSTRUCTIONS_HANDLER_H