#ifndef EPISTEMIC_MODEL_H
#define EPISTEMIC_MODEL_H
#include "epistemic_common.h"
#include "epistemic_observation.h"
#include "../state_descriptor.h"


class EpistemicModel {
    static EpistemicObservation* _observation;
    static StateDescriptor* _sd;
public:
    static StateType::predicates updateLatestHistory(
        const epistemic::predicate &epistemic_type, 
        const epistemic::history &parent_persepectives,
        const map< pair< string, int> , string >& id_to_obj_name,
        const map< string, int >& obj_to_address
        ) {
        switch (epistemic::epistemic_string_to_type[epistemic_type.getPredicate()]) {
            case epistemic::EpistemicType::OBSERVATION:
            {
                StateType::predicates observation(parent_persepectives.back());
                vector<string> pred_names = _sd->getPredicateTypes();
                for (size_t pred_idx = 0; pred_idx < parent_persepectives.back().size(); pred_idx++) {
                    std::string pred_name = pred_names[pred_idx];
                    for (const auto& [param_list, _] : parent_persepectives.back()[pred_idx]) {
                        
                        vector<string> param_names = _sd->getPredicateVarNames(pred_name);
                       
                        bool visibility = _observation->checkVisibility(
                            parent_persepectives.back(), 
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
                return observation;
            }
            case epistemic::EpistemicType::BELIEF:
                return parent_persepectives.back();
            case epistemic::EpistemicType::KNOWLEDGE:
                return parent_persepectives.back();
            default:
                assert(false);
        }
        return parent_persepectives.back();
    }
    static void setObservation(EpistemicObservation* observation) {
        _observation = observation;
    }
    static void setStateDescriptor(StateDescriptor* sd) {
        _sd = sd;
    }
};

EpistemicObservation* EpistemicModel::_observation = nullptr;
StateDescriptor* EpistemicModel::_sd = nullptr;
#endif // EPISTEMIC_MODEL_H