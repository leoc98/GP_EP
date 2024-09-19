#ifndef EPISTEMIC_COMMON_H
#define EPISTEMIC_COMMON_H
#include <string>
#include "epistemic_predicate.h"

namespace epistemic {
    using expression = std::string;
    using predicate = EpistemicPredicate;
    using agent = std::string;
}

#endif // EPISTEMIC_COMMON_H