#ifndef EPISTEMIC_MODEL_H
#define EPISTEMIC_MODEL_H
#include "epistemic_common.h"


class EpistemicModel {
public:
    static StateType::predicates updateLatestHistory(
        const epistemic::predicate &epistemic_type, 
        const vector<StateType::predicates> &parent_persepectives) {
        return {};
    }
};

#endif // EPISTEMIC_MODEL_H