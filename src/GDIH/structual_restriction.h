#ifndef STRUCTURAL_RESTRICTION_H
#define STRUCTURAL_RESTRICTION_H

#include "../generalized_domain_instructions_handler.h"
#include "unordered_map"
#include "algorithm"

class LineProhibitFilter : public GeneralizedDomainInstructionsHandler {
public:
    LineProhibitFilter(unordered_map<string, vector<int>> prohibit_rule):prohibit_map(prohibit_rule), in_total(0), out_total(0) {}
    virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) override {
        in_total += instructions.size();
        for (auto& instruction : instructions) {
            if (auto it = prohibit_map.find(instruction->getSchema()); it != prohibit_map.end()) {
                if (find(it->second.begin(), it->second.end(), program_line) != it->second.end()) {
                    instruction = instructions.back();
                    instructions.pop_back();
                }
            }
        }
        out_total += instructions.size();

        return instructions;
    }
    virtual string showPruneResult() override {
        return "LineProhibitFilter: " + to_string(in_total) + " -> " + to_string(out_total) + "\n";
    }
private:
    unordered_map<string, vector<int>> prohibit_map;
    int in_total;
    int out_total;
};

class CascadeGotoFilter : public GeneralizedDomainInstructionsHandler {
public:
    CascadeGotoFilter(): in_total(0), out_total(0) {}
    virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) override {
        in_total += instructions.size();
        vector< Instruction* > history_instructions = program->getInstructions();
        for (auto& instruction : instructions) {
            if (instruction->getSchema() == "GOTO") {
                int dest_line = static_cast<Goto*>(instruction)->getDestinationLine();
                if (history_instructions[dest_line]->getSchema() == "GOTO") {
                    instruction = instructions.back();
                    instructions.pop_back();
                }
            }

        }
        out_total += instructions.size();

        return instructions;
    }
    virtual string showPruneResult() override {
        return "CascadeGotoFilter: " + to_string(in_total) + " -> " + to_string(out_total) + "\n";
    }
private:
    unordered_map<string, vector<int>> prohibit_map;
    int in_total;
    int out_total;
};

#endif // STRUCTURAL_RESTRICTION_H