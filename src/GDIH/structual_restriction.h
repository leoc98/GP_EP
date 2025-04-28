#ifndef STRUCTURAL_RESTRICTION_H
#define STRUCTURAL_RESTRICTION_H

#include "../generalized_domain_instructions_handler.h"
#include "unordered_map"
#include "algorithm"

class LineProhibitFilter : public GeneralizedDomainInstructionsHandler {
public:
    LineProhibitFilter(unordered_map<string, vector<int>> prohibit_rule):GeneralizedDomainInstructionsHandler("LineProhibitFilter"), prohibit_map(prohibit_rule) {}
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
private:
    unordered_map<string, vector<int>> prohibit_map;
};

class CascadeGotoFilter : public GeneralizedDomainInstructionsHandler {
public:
    CascadeGotoFilter(): GeneralizedDomainInstructionsHandler("CascadeGotoFilter") {}
    virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) override {
        in_total += instructions.size();
        vector< Instruction* > history_instructions = program->getInstructions();
        for (auto& instruction : instructions) {
            if (dynamic_cast<Goto*>(instruction) != nullptr) {
                int dest_line = static_cast<Goto*>(instruction)->getDestinationLine();
                if (dynamic_cast<Goto*>(history_instructions[dest_line]) != nullptr) {
                    // Destination line should not be GOTO
                    instruction = instructions.back();
                    instructions.pop_back();
                } else if (CLEAR_GOTO_ALLOWED && dynamic_cast<ClearGoto*>(instruction) != nullptr) {
                    if (program_line == 0) {
                        // clear-goto first instruction should be GOTO
                        instruction = instructions.back();
                        instructions.pop_back();
                    }
                    // clear-goto previous instruction should be GOTO
                    Instruction* last_instruction = history_instructions[program_line - 1];
                    if (last_instruction != nullptr && dynamic_cast<Goto*>(last_instruction) == nullptr) {
                        instruction = instructions.back();
                        instructions.pop_back();
                    }
                }
            }
        }
        out_total += instructions.size();

        return instructions;
    }
};

class UnusedModificationPointerActionFilter : public GeneralizedDomainInstructionsHandler {
public:
    UnusedModificationPointerActionFilter(unordered_map<string, unordered_set<string>> prohibit_rule): GeneralizedDomainInstructionsHandler("UnusedModificationPointerActionFilter"), prohibit_map(prohibit_rule) {}
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
private:
    unordered_map<string, unordered_set<string>> prohibit_map; // key should be this instruction, value should be the instruction that modifies the same pointer
};

// class DomainKnowledgeSuggestion : public GeneralizedDomainInstructionsHandler {
// public:
//     DomainKnowledgeSuggestion(unordered_map<string, vector<string>> suggest_rule): GeneralizedDomainInstructionsHandler("DomainKnowledgeSuggestion"), suggest_map(suggest_rule) {}
//     virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) override {
//         in_total += instructions.size();
//         Instruction* last_instruction = program->getInstructions()[program_line - 1];
//         for (auto& instruction : instructions) {
//             auto it = suggest_map.find(instruction->getSchema());
//             if (it != suggest_map.end()) {
//                 auto& suggest_list = it->second;

//             }
//         }
//         out_total += instructions.size();

//         return instructions;
//     }
// private:
//     unordered_map<string, vector<string>> suggest_map; // key should be previous instruction, value should be the instruction that sugget to try first in this line
// };

#endif // STRUCTURAL_RESTRICTION_H