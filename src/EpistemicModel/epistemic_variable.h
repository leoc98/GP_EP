#ifndef EPISTEMIC_VARIABLE_H
#define EPISTEMIC_VARIABLE_H

#include "../variable.h"

class EpistemicVariable : public Variable {
public:
    EpistemicVariable(const string &name, Variable* lhs, Variable* rhs, VariableType vtype = VariableType::EPISTEMIC) : 
        Variable(name, vtype, 0, {}),
        epistemic_predicate_name(name.begin(), name.begin()+ (name.find('(')-1)),
        lhs(lhs), rhs(rhs) {
        // input looks like  name = "b [b] b [a] ( face(c) = 1 )"
    }
    ~EpistemicVariable() {
        delete lhs;
        delete rhs;
    }

    Variable* getLHS() const {
        return lhs;
    }
    Variable* getRHS() const {
        return rhs;
    }
    string getEpistemicPredicateName() const {
        return epistemic_predicate_name;
    }
    static vector< epistemic::predicate > EpistemicPredicateResolve(epistemic::expression exp) {
        vector< epistemic::predicate > ret;
        string pred = "";
        string agent = "";
        string parent_pred = "";
        string depend_pred = "";
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
                        ret.emplace_back("O", agent, parent_pred, depend_pred);
                        ret.emplace_back(pred, agent, parent_pred, depend_pred + (depend_pred.length()?" ":"") + "O " + agent);
                        depend_pred += pred + " " + agent;
                        parent_pred = ret.back().getName();
                    }
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
    Variable* lhs; 
    Variable* rhs; 
};

#endif // EPISTEMIC_VARIABLE_H