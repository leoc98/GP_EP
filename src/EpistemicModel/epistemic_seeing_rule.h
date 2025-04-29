#ifndef EPISTEMIC_SEEING_RULE_H
#define EPISTEMIC_SEEING_RULE_H
#include "../state_type.h"
#include "epistemic_common.h"
#include "epistemic_seeing_support_descriptor.h"
#include "../state_descriptor.h"
#include <map>
#include <unordered_map>
#include <cmath>


class EpistemicSeeingRule {
public:
    virtual bool checkVisibility(
        const StateType::predicates& state, 
        const epistemic::agent& agent_name, 
        const string& target_predicate_name,
        const StateType::parameter_list& param_list,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd,
        EpistemicSeeingSupportDescriptor* essd
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
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd,
        EpistemicSeeingSupportDescriptor* essd
    ) {
        if (target_predicate_name != "face")
        {
            return true;
        }
        StateType::parameter_list peeking_param_list(sd->getPredicateVarNamesNumber("peeking"));
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
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd,
        EpistemicSeeingSupportDescriptor* essd
    ) {
        if (target_predicate_name != "secret_true")
        {
            return true;
        }
        CorridorEpistemicSeeingSupportDescriptor* crd_essd = dynamic_cast<CorridorEpistemicSeeingSupportDescriptor*>(essd);
        // assume only 1 secret now
        const string secret_type = "secret";
        assert(id_to_obj_name.count({secret_type, 0}));
        int secret_ind = obj_to_address.at(id_to_obj_name.at({secret_type, 0}));

        const string shared_type = "shared";
        StateType::parameter_list shared_param_list(sd->getPredicateVarNamesNumber(shared_type));
        shared_param_list[0] = secret_ind;
        int pred_ind = sd->getPredicateIDX(shared_type);
        if (state[pred_ind].at(shared_param_list) == 0)
        {
            return false;
        }
        
        // get shared_at loc
        int share_at_loc = crd_essd->obj_pos.at(id_to_obj_name.at({secret_type, 0}));
        
        // get agent_at loc
        int agent_at_loc = crd_essd->obj_pos.at(agent_name);

        return abs(share_at_loc - agent_at_loc) <= 1;
    }
};

class BBLSeeingRule : public EpistemicSeeingRule {
public:
    unordered_map<string, bool> cache;
    
    bool checkVisibility(
        const StateType::predicates& state, 
        const epistemic::agent& agent_name, 
        const string& target_predicate_name,
        const StateType::parameter_list& param_list,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd,
        EpistemicSeeingSupportDescriptor* essd
    ) {
        if (target_predicate_name == "cw_cnct@nt" || 
            target_predicate_name == "ccw_cnct@nt" ||
            target_predicate_name == "agent_at@nt" ||
            target_predicate_name == "view_at@nt"
            )
        {
            return true;
        }
        BBLEpistemicSeeingSupportDescriptor* bbl_essd = dynamic_cast<BBLEpistemicSeeingSupportDescriptor*>(essd);
        // get target loc
        string target_name = id_to_obj_name.at({sd->getPredicateVarNamesByPos(target_predicate_name, 0),param_list[0]});
        const auto& target_pos = bbl_essd->obj_pos.at(target_name);
        const int& target_x = target_pos.first; 
        const int& target_y = target_pos.second; 
        if (target_x == -1 && target_y == -1) {
            return false;
        }
        // get agent loc
        const auto& agent_pos = bbl_essd->obj_pos.at(agent_name);
        const int& agent_x = agent_pos.first; 
        const int& agent_y = agent_pos.second;

        if (target_x == agent_x && target_y == agent_y) {
            return true;
        }

        const string& dir = bbl_essd->obj_dir.at(agent_name);

        if (dir == string("unknown")) {
            return false;
        }

        return getCache(agent_x, agent_y, dir, target_x, target_y);
    }

    bool getCache(
        int agent_x,
        int agent_y,
        const string& dir,
        int target_x,
        int target_y
    ) {
        string key = 
            std::to_string(agent_x) + "_" +
            std::to_string(agent_y) + "_" +
            dir + "_" +
            std::to_string(target_x) + "_" +
            std::to_string(target_y);
        auto it = cache.find(key);
        int dx = target_x - agent_x;
        int dy = target_y - agent_y;
        if (it == cache.end()) {

            if (dx == dy) {
                if (dx > 0) {
                    cache.insert({ key, 
                        (dir == "n") || (dir == "ne") || (dir == "e") });
                } else if (dx < 0) {
                    cache.insert({ key, 
                        (dir == "s") || (dir == "sw") || (dir == "w") });
                }
            }

            if (dx == -dy) {
                if (dx > 0) {
                    cache.insert({ key, 
                        (dir == "e") || (dir == "se") || (dir == "s") });
                } else if (dx < 0) {
                    cache.insert({ key, 
                        (dir == "w") || (dir == "nw") || (dir == "n") });
                }
            }

            it = cache.find(key);
        }

        if (it == cache.end()) {
            const std::unordered_map<std::string, std::pair<int, int>> directions = {
                {"n", {45, 135}},
                {"ne", {0, 90}},
                {"e", {-45, 45}},
                {"se", {-90, 0}},
                {"s", {-135, -45}},
                {"sw", {-180, -90}},
                {"w", {-135, 135}},
                {"nw", {90, 180}}
            };

            // Calculate the angle of the target relative to the agent.
            double angle = std::atan2(dy, dx) * 180 / M_PI;
            if (angle > 180) {
                angle -= 360;
            } else if (angle < -180) {
                angle += 360;
            }

            // Check if the target is within the 90 - degree viewing angle of the agent's direction.
            assert(directions.find(dir) != directions.end());
            int min_angle = directions.at(dir).first;
            int max_angle = directions.at(dir).second;
            if (dir == "w") {
                cache.insert({ key, 
                    (angle <= min_angle || angle >= max_angle) });
            } else {
                cache.insert({ key, 
                    (angle >= min_angle && angle <= max_angle) });
            }

            it = cache.find(key);
        }
        return it->second;
    }
};
#endif // EPISTEMIC_SEEING_RULE_H