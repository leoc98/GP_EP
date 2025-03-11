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

class CorridorSeeingRule : public EpistemicSeeingRule {
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
        if (target_predicate_name != "secret_true")
        {
            return true;
        }
        // assume only 1 secret now
        const string secret_type = "secret";
        assert(id_to_obj_name.count({secret_type, 0}));
        int secret_ind = obj_to_address.at(id_to_obj_name.at({secret_type, 0}));

        const string shared_type = "shared";
        vector<string> shared_param_names = sd->getPredicateVarNames(shared_type);
        assert(shared_param_names.size() == 1);
        StateType::parameter_list shared_param_list(shared_param_names.size());
        shared_param_list[0] = secret_ind;
        int pred_ind = sd->getPredicateIDX(shared_type);
        if (state[pred_ind].at(shared_param_list) == 0)
        {
            return false;
        }
        
        auto find_loc_room = [&sd, &id_to_obj_name, &obj_to_address, &state](const string& pred_type, const int& obj_ind) {
            vector<string> pred_param_names = sd->getPredicateVarNames(pred_type);
            assert(pred_param_names.size() == 2);
            StateType::parameter_list pred_param_list(pred_param_names.size());
            pred_param_list[0] = obj_ind;

            // check each room fullfill pred or not
            int room_no = 0;
            const string room_type = "room";
            bool find_room = false;
            while (id_to_obj_name.count({room_type, room_no})) {
                const string& room_name = id_to_obj_name.at({room_type, room_no});
                pred_param_list[1] = obj_to_address.at(room_name);
                if (state[sd->getPredicateIDX(pred_type)].at(pred_param_list) == 1)
                {
                    find_room = true;
                    break;
                }
                room_no++;
            }
            assert(find_room);
            return room_no;
        };


        // get shared_at loc
        int share_at_loc = find_loc_room("shared_at", secret_ind);
        
        // get agent_at loc
        int agent_at_loc = find_loc_room("agent_at", obj_to_address.at(agent_name));

        return abs(share_at_loc - agent_at_loc) <= 1;
    }
};

#endif // EPISTEMIC_SEEING_RULE_H