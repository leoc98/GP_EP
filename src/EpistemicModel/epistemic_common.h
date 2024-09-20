#ifndef EPISTEMIC_COMMON_H
#define EPISTEMIC_COMMON_H
#include <string>
#include "epistemic_predicate.h"
#include "../state_type.h"
#include <unordered_map>
#include <vector>

namespace epistemic {
    using expression = std::string;
    using predicate = EpistemicPredicate;
    using agent = std::string;
    // using history = std::unordered_map < epistemic::predicate, vector< StateType::predicates >>;
    using history = vector< StateType::predicates >;
}

#endif // EPISTEMIC_COMMON_H