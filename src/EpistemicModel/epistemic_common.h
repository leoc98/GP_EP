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

    enum EpistemicValue {
        FALSE = 0,
        TRUE = 1,
        UNKNOWN = 2,
    };

    enum class EpistemicType {
        OBSERVATION,
        BELIEF,
        KNOWLEDGE,
    };

    unordered_map< string, EpistemicType > epistemic_string_to_type = {
        {"O", EpistemicType::OBSERVATION},
        {"b", EpistemicType::BELIEF},
        {"k", EpistemicType::KNOWLEDGE},
    };
}

#endif // EPISTEMIC_COMMON_H