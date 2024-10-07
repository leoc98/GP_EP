#ifndef EPISTEMIC_MODEL_H
#define EPISTEMIC_MODEL_H
#include "epistemic_common.h"
#include "epistemic_observation.h"
#include "epistemic_perspectives.h"
#include "../state_descriptor.h"
#include <functional>


class EpistemicModel {
    static EpistemicObservation* _observation;
    static EpistemicPerspectives* _perspectives;
    static StateDescriptor* _sd;
public:
    static StateType::predicates updateLatestHistory(
        const epistemic::predicate &epistemic_type, 
        function<epistemic::history&(const epistemic::predicate&, size_t)> getEpistemicHistory,
        function<const epistemic::predicate&(const string&)> getEpistemicPredicate,
        size_t _lastest_history_index,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address
        ) {
        
        const epistemic::predicate& depend_predicate = getEpistemicPredicate(epistemic_type.getDependPredicate());
        const epistemic::history &parent_states = getEpistemicHistory(depend_predicate, _lastest_history_index);
        const StateType::predicates& last_state = parent_states.back();

        StateType::predicates ret(last_state);
        switch (epistemic::epistemic_string_to_type[epistemic_type.getPredicate()]) {
            case epistemic::EpistemicType::KNOWLEDGE:
            case epistemic::EpistemicType::OBSERVATION:
            {
                StateType::predicates& observation = ret;
                vector<string> pred_names = _sd->getPredicateTypes();
                for (size_t pred_idx = 0; pred_idx < last_state.size(); pred_idx++) {
                    std::string pred_name = pred_names[pred_idx];
                    for (const auto& [param_list, _] : last_state[pred_idx]) {
                        
                        vector<string> param_names = _sd->getPredicateVarNames(pred_name);
                       
                        bool visibility = _observation->checkVisibility(
                            last_state, 
                            epistemic_type.getAgent(), 
                            pred_name,
                            param_list,
                            param_names,
                            id_to_obj_name,
                            obj_to_address,
                            _sd
                        );
                        if (false == visibility) {
                            observation[pred_idx][param_list] = epistemic::EpistemicValue::UNKNOWN;
                        }
                    }
                }
            }
            break;
            case epistemic::EpistemicType::BELIEF:
            {
                StateType::predicates& new_perspectives = ret;
                // get parent perspectives
                const epistemic::predicate& depend_depend_predicate = getEpistemicPredicate(depend_predicate.getDependPredicate());
                const epistemic::history& parent_perspectives = getEpistemicHistory(depend_depend_predicate, _lastest_history_index);
                // assert(parent_perspectives.size() == last_state.size());
                
                const epistemic::history& observation = parent_states;
                /*
                    for i in range(len(os)):
                    temp_ps = self.generate_p(os[:i+1],parent_ps[:i+1])
                    ps.append(temp_ps)
                */
                new_perspectives = _perspectives->generatePerspectives(observation, parent_perspectives);
            }
            break;
            default:
                assert(false);
        }
        return ret;
    }
    static void setObservation(EpistemicObservation* observation) {
        _observation = observation;
    }
    static void setStateDescriptor(StateDescriptor* sd) {
        _sd = sd;
    }
    static void setPerspectives(EpistemicPerspectives* perspectives) {
        _perspectives = perspectives;
    }
};

EpistemicObservation* EpistemicModel::_observation = nullptr;
StateDescriptor* EpistemicModel::_sd = nullptr;
EpistemicPerspectives* EpistemicModel::_perspectives = nullptr;
#endif // EPISTEMIC_MODEL_H