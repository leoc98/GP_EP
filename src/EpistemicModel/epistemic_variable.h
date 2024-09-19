#ifndef EPISTEMIC_VARIABLE_H
#define EPISTEMIC_VARIABLE_H

#include "../variable.h"

class EpistemicVariable : public Variable {
public:
    EpistemicVariable(const string &name, void* condition, VariableType vtype = VariableType::EPISTEMIC) : 
        Variable(name, vtype, 0, {}),
        epistemic_predicate_name(name.begin(), name.begin()+ (name.find('(')-1)),
        condition(condition) {
        // input looks like  name = "b [b] b [a] ( face(c) = 1 )"
    }
    void setCondition(void* condition) {
        this->condition = condition;
    }
    string getEpistemicPredicateName() const {
        return epistemic_predicate_name;
    }
    static vector< epistemic::predicate > EpistemicPredicateResolve(epistemic::expression exp) {
        vector< epistemic::predicate > ret;
        string pred = "";
        string agent = "";
        string parent_pred = "";
        bool reading_pre = true;
        for (const char& c:exp) {
            if (c == '(') {
                break;
            }
            if (c == ' ') {
                if (reading_pre) {
                    reading_pre = false;
                } else {
                    if (pred == "b") {
                        ret.emplace_back("O", agent, parent_pred);
                    }
                    ret.emplace_back(pred, agent, parent_pred);
                    parent_pred = ret.back().getName();
                    pred.clear();
                    agent.clear();
                    reading_pre = true;
                }
                    
                continue;
            }
            if (reading_pre) {
                pred.push_back(c);
            } else {
                agent.push_back(c);
            }

        }

        return ret;
    }
private:
    string epistemic_predicate_name;
    void* condition; // this can be static_cast to a Condition/Equals pointer
};

#endif // EPISTEMIC_VARIABLE_H