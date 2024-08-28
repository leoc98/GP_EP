#ifndef NOVELTY_INSTRUCTION_FILTER_H
#define NOVELTY_INSTRUCTION_FILTER_H

#include "../generalized_domain_instructions_handler.h"
#include "unordered_map"

class NoveltyInstructionFilter : public GeneralizedDomainInstructionsHandler {
public:
    NoveltyInstructionFilter(int novelty): novelty_threshold(novelty), in_total(0), out_total(0) {}
    virtual vector< Instruction* >& filter( int program_line, vector< Instruction* >& instructions, const Program* program ) override {
        unordered_map< string, int > instruction_cnt;
        vector< Instruction* > history_instructions = program->getInstructions();

        auto getNoveltyInstructionFilterName = [](Instruction *inst) -> string {
            // RAM Action should not only consider schema
            RAMAction * ram_action = dynamic_cast<RAMAction *>(inst);
            return ram_action?ram_action->getName() : inst->getSchema();
        };

        for (int i = 0; i < program_line; i++) {
            // ignore GOTO instructions
            if (auto schema = getNoveltyInstructionFilterName(history_instructions[i]); schema != "GOTO") {
                instruction_cnt[schema]++;
            }
        }

        // vector< Instruction* > filtered_instructions;
        in_total += instructions.size();
        for (auto& instruction : instructions) {
            if (instruction_cnt[getNoveltyInstructionFilterName(instruction)] + 1 > novelty_threshold) {
                // filtered_instructions.emplace_back(instruction);
                instruction = instructions.back();
                instructions.pop_back();
            }
        }

        out_total += instructions.size();

        return instructions;
    }
    virtual string showPruneResult() override {
        return "NoveltyInstructionFilter: " + to_string(in_total) + " -> " + to_string(out_total) + "\n";
    }
private:
    int novelty_threshold;  
    int in_total;
    int out_total;  
};

#endif // NOVELTY_INSTRUCTION_FILTER_H