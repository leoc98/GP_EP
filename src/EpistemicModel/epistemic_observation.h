#ifndef EPISTEMIC_OBSERVATION_H
#define EPISTEMIC_OBSERVATION_H
#include "epistemic_seeing_rule.h"

class EpistemicObservation {
    EpistemicSeeingRule* _seeing_rule;
public:
    // Constructor
    EpistemicObservation(EpistemicSeeingRule* seeing_rule) : _seeing_rule(seeing_rule) {};

    // Destructor
    virtual ~EpistemicObservation() {
        delete _seeing_rule;
    }

    bool checkVisibility(
        const StateType::predicates& state, 
        const epistemic::agent& agent_name, 
        const string& target_predicate_name,
        const StateType::parameter_list& param_list,
        const vector<string>& param_names,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd

    ) {
        return _seeing_rule->checkVisibility(
            state, 
            agent_name.substr(1, agent_name.size() - 2), 
            target_predicate_name, 
            param_list,
            param_names,
            id_to_obj_name,
            obj_to_address,
            sd
        );
    }

    // Public member functions

private:
    // Private member variables

};

#endif // EPISTEMIC_OBSERVATION_H