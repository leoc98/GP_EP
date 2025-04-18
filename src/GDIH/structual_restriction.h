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
    int in_total;
    int out_total;
};

class UnusedModificationPointerActionFilter : public GeneralizedDomainInstructionsHandler {
public:
    UnusedModificationPointerActionFilter(unordered_map<string, unordered_set<string>> prohibit_rule): prohibit_map(prohibit_rule), in_total(0), out_total(0) {}
    virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) override {
        in_total += instructions.size();
        vector< Instruction* > history_instructions = program->getInstructions();
        for (auto& instruction : instructions) {
            auto it = prohibit_map.find(instruction->getSchema());
            if (it == prohibit_map.end()) {
                continue;
            }
            const unordered_set<string>& prohibit_set = it->second;
            int cur_line = program_line-1;

            auto getLastParameter = [](const string& instruction_name) {
                auto pos = instruction_name.rfind(',');
                if (pos == string::npos) {
                    pos = instruction_name.find('(');
                }
                return instruction_name.substr(pos + 1, instruction_name.find(')') - pos - 1);
            };

            string modified_pointer = getLastParameter(instruction->getName());

            while (cur_line >= 0) {
                Instruction* cur_instruction = history_instructions[cur_line--];
                if (cur_instruction->getSchema() == "GOTO") {
                    break;
                }
                string cur_instruction_modified_pointer = getLastParameter(cur_instruction->getName());

                if (cur_instruction_modified_pointer == modified_pointer) {
                    auto it = prohibit_set.find(cur_instruction->getSchema());
                    if (it != prohibit_set.end()) {
                        instruction = instructions.back();
                        instructions.pop_back();
                    }
                    break;
                }

            }

        }
        
        out_total += instructions.size();

        return instructions;
    }
    virtual string showPruneResult() override {
        return "UnusedModificationPointerActionFilter: " + to_string(in_total) + " -> " + to_string(out_total) + "\n";
    }
private:
    unordered_map<string, unordered_set<string>> prohibit_map; // key should be this instruction, value should be the instruction that modifies the same pointer
    int in_total;
    int out_total;
};

#endif // STRUCTURAL_RESTRICTION_H