#ifndef EPISTEMIC_PERSPECTIVES_H
#define EPISTEMIC_PERSPECTIVES_H

#include "../state_type.h"
#include "epistemic_common.h"
#include "../state_descriptor.h"

class EpistemicPerspectives {
public:
    EpistemicPerspectives() = default;
    virtual ~EpistemicPerspectives() = default;

    StateType::predicates generatePerspectives(
        const epistemic::history& observation, 
        const epistemic::history& parent_perspectives
        ) {
        /*
        python code:
        translate into C++
        new_state = partial_os[-1].copy()
        for key,value in new_state.items():
            if value == special_value.UNSEEN:
                ts = self.identify_last_seen_timestamp(key,partial_os)
                new_state[key] = self.retrieval_function(partial_ps,ts,key)

        return new_state
        */
        StateType::predicates new_state = observation.back();
        for (size_t i = 0; i < new_state.size(); ++i) {
            for (const auto& [param_list, _] : new_state[i]) {
                if (new_state[i][param_list] == epistemic::EpistemicValue::UNKNOWN) {
                    int ts = identify_last_seen_timestamp(observation, i, param_list);
                    new_state[i][param_list] = retrieveFuction(parent_perspectives, ts, i, param_list);
                }
            }
        }
        return new_state;
    }

    int identify_last_seen_timestamp(
        const epistemic::history& observation, 
        int pred_idx, 
        const StateType::parameter_list& param_list) {
        /*
        length = len(partial_os)
        for i in range(length):
            ts = length-i-1
            if variable_name in partial_os[ts].keys():
                if not partial_os[ts][variable_name] == special_value.UNSEEN:
                    return ts
            else:
                raise ValueError("variable is not in the observation list",variable_name,ts)
        return -1
        */
        int length = observation.size();
        for (int i = 0; i < length; ++i) {
            int ts = length - i - 1;
            if (observation[ts][pred_idx].find(param_list) != observation[ts][pred_idx].end()) {
                if (observation[ts][pred_idx].at(param_list) != epistemic::EpistemicValue::UNKNOWN) {
                    return ts;
                }
            } else {
                throw std::invalid_argument("variable is not in the observation list");
            }
        }
        return -1;
    }

    epistemic::EpistemicValue retrieveFuction(
        const epistemic::history& parent_perspectives, 
        int timestamp, 
        int pred_idx, 
        const StateType::parameter_list& param_list) {
        /* 
        temp_ts = ts
        if temp_ts < 0:
            return special_value.HAVENT_SEEN
        while temp_ts >= 0:
            if variable_name in partial_ps[temp_ts].keys():
                if partial_ps[temp_ts][variable_name] == special_value.HAVENT_SEEN:
                    temp_ts += -1
                elif partial_ps[temp_ts][variable_name] == special_value.UNSEEN:
                    raise ValueError("variable is not seen by the agent, should not happen",variable_name,partial_ps,temp_ts)
                else:
                    return partial_ps[temp_ts][variable_name]
            else:
                raise ValueError("variable is not in the observation list",variable_name,partial_ps,ts)
        
        temp_ts = ts + 1
        while temp_ts < len(partial_ps):
            if variable_name in partial_ps[temp_ts].keys():
                if partial_ps[temp_ts][variable_name] == special_value.HAVENT_SEEN:
                    temp_ts += 1
                elif partial_ps[temp_ts][variable_name] == special_value.UNSEEN:
                    raise ValueError("variable is not seen by the agent, should not happen",variable_name,partial_ps,temp_ts)
                else:
                    return partial_ps[temp_ts][variable_name]
            else:
                raise ValueError("variable is not in the observation list",variable_name,partial_ps,ts)
        return special_value.HAVENT_SEEN
        */
        int temp_ts = timestamp;
        if (temp_ts < 0) {
            return epistemic::EpistemicValue::HAVENT_SEEN;
        }
        while (temp_ts >= 0) {
            if (parent_perspectives[temp_ts][pred_idx].find(param_list) != parent_perspectives[temp_ts][pred_idx].end()) {
                if (parent_perspectives[temp_ts][pred_idx].at(param_list) == epistemic::EpistemicValue::HAVENT_SEEN) {
                    temp_ts -= 1;
                } else if (parent_perspectives[temp_ts][pred_idx].at(param_list) == epistemic::EpistemicValue::UNKNOWN) {
                    throw std::invalid_argument("variable is not seen by the agent, should not happen");
                } else {
                    return epistemic::EpistemicValue(parent_perspectives[temp_ts][pred_idx].at(param_list));
                }
            } else {
                throw std::invalid_argument("variable is not in the observation list");
            }
        }

        temp_ts = timestamp + 1;
        while (temp_ts < parent_perspectives.size()) {
            if (parent_perspectives[temp_ts][pred_idx].find(param_list) != parent_perspectives[temp_ts][pred_idx].end()) {
                if (parent_perspectives[temp_ts][pred_idx].at(param_list) == epistemic::EpistemicValue::HAVENT_SEEN) {
                    temp_ts += 1;
                } else if (parent_perspectives[temp_ts][pred_idx].at(param_list) == epistemic::EpistemicValue::UNKNOWN) {
                    throw std::invalid_argument("variable is not seen by the agent, should not happen");
                } else {
                    return epistemic::EpistemicValue(parent_perspectives[temp_ts][pred_idx].at(param_list));
                }
            } else {
                throw std::invalid_argument("variable is not in the observation list");
            }
        }
        return epistemic::EpistemicValue::HAVENT_SEEN;
    }

private:
    
};

#endif // EPISTEMIC_PERSPECTIVES_H