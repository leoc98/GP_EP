#ifndef EPISTEMIC_SEEING_RULE_H
#define EPISTEMIC_SEEING_RULE_H
#include "../state_type.h"
#include "epistemic_common.h"
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

class BBLSeeingRule : public EpistemicSeeingRule {
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
        if (target_predicate_name == "cw_cnct@nt" || 
            target_predicate_name == "ccw_cnct@nt" ||
            target_predicate_name == "agent_at@nt" ||
            target_predicate_name == "view_at@nt"
            )
        {
            return true;
        }
        auto find_loc_by_obj_ind = [&sd, &id_to_obj_name, &obj_to_address, &state](const string& pred_type, const int& obj_ind) -> pair<int, int> {
            StateType::parameter_list pred_param_list(sd->getPredicateVarNamesNumber(pred_type));
            pred_param_list[0] = obj_ind;

            // check each room fullfill pred or not
            int pos_x = 0;
            const string x_pos_type = "x_pos";
            const string y_pos_type = "y_pos";
            // bool find_pos = false;
            while (id_to_obj_name.count({x_pos_type, pos_x})) {
                const string& xpos_name = id_to_obj_name.at({x_pos_type, pos_x});
                pred_param_list[1] = obj_to_address.at(xpos_name);
                int pos_y = 0;
                while (id_to_obj_name.count({y_pos_type, pos_y})) {
                    const string& ypos_name = id_to_obj_name.at({y_pos_type, pos_y});
                    pred_param_list[2] = obj_to_address.at(ypos_name);
                    if (state[sd->getPredicateIDX(pred_type)].at(pred_param_list) == 1)
                    {
                        // find_pos = true;
                        return {pos_x,pos_y};
                    }
                    pos_y++;
                }
                pos_x++;
            }
            // assert(find_pos);
            return {-1,-1};
        };
        // get target loc
        string target_name = id_to_obj_name.at({param_names[0],param_list[0]});
        auto target_pos = find_loc_by_obj_ind(target_predicate_name.find("agent")!=string::npos?"agent_at@nt":"view_at@nt", obj_to_address.at(target_name));
        int& target_x = target_pos.first; 
        int& target_y = target_pos.second; 
        if (target_x == -1 && target_y == -1) {
            return false;
        }
        // get agent loc
        auto agent_pos = find_loc_by_obj_ind("agent_at@nt", obj_to_address.at(agent_name));
        int& agent_x = agent_pos.first; 
        int& agent_y = agent_pos.second;

        if (target_x == agent_x && target_y == agent_y) {
            return true;
        }

        int dx = target_x - agent_x;
        int dy = target_y - agent_y;

        auto find_agt_dir = [&sd, &id_to_obj_name, &obj_to_address, &state](const string& pred_type, const int& agent_ind) {
            vector<string> pred_param_names = sd->getPredicateVarNames(pred_type);
            assert(pred_param_names.size() == 2);
            StateType::parameter_list pred_param_list(pred_param_names.size());
            pred_param_list[0] = agent_ind;

            // check each room fullfill pred or not
            vector<string> dir_list = {"n", "ne", "e", "se", "s", "sw", "w", "nw"};
            for (const string& dir : dir_list) {
                pred_param_list[1] = obj_to_address.at(dir);
                if (state[sd->getPredicateIDX(pred_type)].at(pred_param_list) == 1)
                {
                    return dir;
                }
            }
            // assert(false);
            return string("unknown");
        };
        string dir = find_agt_dir("agent_face", obj_to_address.at(agent_name));

        if (dir == string("unknown")) {
            return false;
        }
        if (dx == dy) {
            if (dx > 0) {
                return (dir == "n") || (dir == "ne") || (dir == "e");
            } else if (dx < 0) {
                return (dir == "s") || (dir == "sw") || (dir == "w");
            }
        }

        if (dx == -dy) {
            if (dx > 0) {
                return (dir == "e") || (dir == "se") || (dir == "s");
            } else if (dx < 0) {
                return (dir == "w") || (dir == "nw") || (dir == "n");
            }
        }

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
            return (angle <= min_angle || angle >= max_angle);
        }
            
        return (angle >= min_angle && angle <= max_angle);
    }
};
#endif // EPISTEMIC_SEEING_RULE_H