#ifndef EPISTEMIC_SEEING_RULE_H
#define EPISTEMIC_SEEING_RULE_H
#include "../state_type.h"
#include "epistemic_common.h"
#include "../state_descriptor.h"
#include <map>


class EpistemicSeeingRule {
public:
    virtual bool checkVisibility(
        const StateType::predicates& state, 
        const epistemic::agent& agent_name, 
        const string& target_predicate_name,
        const StateType::parameter_list& param_list,
        const vector<string>& param_names,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd
    ) = 0;
    virtual ~EpistemicSeeingRule() = default;
};

class CoinSeeingRule : public EpistemicSeeingRule {
public:
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
        if (target_predicate_name != "face")
        {
            return true;
        }
        vector<string> peeking_param_names = sd->getPredicateVarNames("peeking");
        assert(peeking_param_names.size() == 1);
        StateType::parameter_list peeking_param_list(peeking_param_names.size());
        peeking_param_list[0] = obj_to_address.at(agent_name);
        int pred_ind = sd->getPredicateIDX( "peeking" );
        return state[pred_ind].at(peeking_param_list) == 1;

    }
};

#endif // EPISTEMIC_SEEING_RULE_H