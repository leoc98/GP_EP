#ifndef EPISTEMIC_STATE_PREPROCESSOR_H
#define EPISTEMIC_STATE_PREPROCESSOR_H

#include "../state_type.h"
#include "../state_descriptor.h"
#include "epistemic_common.h"
#include "epistemic_seeing_support_descriptor.h"

class EpistemicStatePreprocessor {
public:
    virtual EpistemicSeeingSupportDescriptor* preprocessEpistemicState(
        const StateType::predicates& state,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd
    ) {
        return nullptr;
    }

    virtual ~EpistemicStatePreprocessor() = default;

    string find_true_unary_pred_by_ind(
        const StateType::predicates& state,
        const string& pred_type,
        const int& tgt_ind,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        const vector<string>& possible_var_names,
        StateDescriptor* sd,
        const string& default_value = "unknown"
    ) {
        StateType::parameter_list pred_param_list(sd->getPredicateVarNamesNumber(pred_type));
        pred_param_list[0] = tgt_ind;

        for (const auto& var_name : possible_var_names) {
            pred_param_list[1] = obj_to_address.at(var_name);
            if (state[sd->getPredicateIDX(pred_type)].at(pred_param_list) == 1) {
                return var_name;
            }
        }
        return default_value;
    }

    pair<string,string> find_true_binary_pred_by_ind(
        const StateType::predicates& state,
        const string& pred_type,
        const int& tgt_ind,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        const vector<string>& possible_var_names1,
        const vector<string>& possible_var_names2,
        StateDescriptor* sd,
        const string& default_value1 = "unknown",
        const string& default_value2 = "unknown"
    ) {
        StateType::parameter_list pred_param_list(sd->getPredicateVarNamesNumber(pred_type));
        pred_param_list[0] = tgt_ind;

        for (const auto& var_name1 : possible_var_names1) {
            pred_param_list[1] = obj_to_address.at(var_name1);
            for (const auto& var_name2 : possible_var_names2) {
                pred_param_list[2] = obj_to_address.at(var_name2);
                if (state[sd->getPredicateIDX(pred_type)].at(pred_param_list) == 1) {
                    return {var_name1, var_name2};
                }
            }
        }
        return {default_value1, default_value2};
    }
};

class CorridorEpistemicStatePreprocessor : public EpistemicStatePreprocessor {
public:
    vector<string> const_agnt_names;
    string mv_agnt_name;
    vector<string> all_obj_names;
    void init(
        const StateType::predicates& state,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd
    ) {
        if (!const_agnt_names.empty() && !all_obj_names.empty()) {
            return;
        }
        int agent_ind = 0;
        while (id_to_obj_name.count({"agent", agent_ind})) {
            string obj_name = id_to_obj_name.at({"agent", agent_ind});
            if (obj_name != "unknown") {
                const_agnt_names.push_back(obj_name);
            }
            agent_ind++;
        }

        if (id_to_obj_name.count({"movable_agent", 0})) {
            mv_agnt_name = id_to_obj_name.at({"movable_agent", 0});
        }

        int obj_ind = 0;
        while (id_to_obj_name.count({"secret", obj_ind})) {
            all_obj_names.push_back(id_to_obj_name.at({"secret", obj_ind}));
            obj_ind++;
        }
    }
    EpistemicSeeingSupportDescriptor* preprocessEpistemicState(
        const StateType::predicates& state,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd
    ) override {
        init(
            state,
            id_to_obj_name,
            obj_to_address,
            sd
        );
        
        CorridorEpistemicSeeingSupportDescriptor* crd_essd = new CorridorEpistemicSeeingSupportDescriptor();
        const vector<string> r_list = {"r1", "r2", "r3", "r4"};

        for (const auto& agent_name : const_agnt_names) {
            int agent_ind = obj_to_address.at(agent_name);
            auto loc = find_true_unary_pred_by_ind(
                state,
                "agent_at@nt",
                agent_ind,
                id_to_obj_name,
                obj_to_address,
                r_list,
                sd
            );
            crd_essd->obj_pos[agent_name] = loc[1] - '1';
        }

        {
            int agent_ind = obj_to_address.at(mv_agnt_name);
            auto loc = find_true_unary_pred_by_ind(
                state,
                "agent_at",
                agent_ind,
                id_to_obj_name,
                obj_to_address,
                r_list,
                sd
            );
            crd_essd->obj_pos[mv_agnt_name] = loc[1] - '1';
        }

        for (const auto& obj_name : all_obj_names) {
            int obj_ind = obj_to_address.at(obj_name);
            auto loc = find_true_unary_pred_by_ind(
                state,
                "shared_at",
                obj_ind,
                id_to_obj_name,
                obj_to_address,
                r_list,
                sd
            );
            crd_essd->obj_pos[obj_name] = loc[1] - '1';
        }

        return crd_essd;
    }
};

class BBLEpistemicStatePreprocessor : public EpistemicStatePreprocessor {
public:
    vector<string> all_agnt_names;
    vector<string> all_obj_names;
    void init(
        const StateType::predicates& state,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd
    ) {
        if (!all_agnt_names.empty() && !all_obj_names.empty()) {
            return;
        }
        int agent_ind = 0;
        while (id_to_obj_name.count({"agent", agent_ind})) {
            all_agnt_names.push_back(id_to_obj_name.at({"agent", agent_ind}));
            agent_ind++;
        }

        int obj_ind = 0;
        while (id_to_obj_name.count({"view", obj_ind})) {
            all_obj_names.push_back(id_to_obj_name.at({"view", obj_ind}));
            obj_ind++;
        }
    }
    EpistemicSeeingSupportDescriptor* preprocessEpistemicState(
        const StateType::predicates& state,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address,
        StateDescriptor* sd
    ) override {
        init(
            state,
            id_to_obj_name,
            obj_to_address,
            sd
        );
        
        BBLEpistemicSeeingSupportDescriptor* bbl_essd = new BBLEpistemicSeeingSupportDescriptor();
        const vector<string> dir_list = {"n", "ne", "e", "se", "s", "sw", "w", "nw"};
        const vector<string> x_pos_list = {"x1", "x2", "x3"};
        const vector<string> y_pos_list = {"y1", "y2", "y3"};

        for (const auto& agent_name : all_agnt_names) {
            int agent_ind = obj_to_address.at(agent_name);
            string dir = find_true_unary_pred_by_ind(
                state,
                "agent_face",
                agent_ind,
                id_to_obj_name,
                obj_to_address,
                dir_list,
                sd
            );
            bbl_essd->obj_dir[agent_name] = dir;
        }

        for (const auto& obj_name : all_obj_names) {
            int obj_ind = obj_to_address.at(obj_name);
            auto loc = find_true_binary_pred_by_ind(
                state,
                "view_at@nt",
                obj_ind,
                id_to_obj_name,
                obj_to_address,
                x_pos_list,
                y_pos_list,
                sd
            );
            bbl_essd->obj_pos[obj_name] = {loc.first[1]-'1', loc.second[1]-'1'};
        }

        for (const auto& agent_name : all_agnt_names) {
            int agent_ind = obj_to_address.at(agent_name);
            auto loc = find_true_binary_pred_by_ind(
                state,
                "agent_at@nt",
                agent_ind,
                id_to_obj_name,
                obj_to_address,
                x_pos_list,
                y_pos_list,
                sd
            );
            bbl_essd->obj_pos[agent_name] = {loc.first[1]-'1', loc.second[1]-'1'};
        }

        return bbl_essd;
    }
};
#endif // EPISTEMIC_STATE_PREPROCESSOR_H