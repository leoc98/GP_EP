#ifndef STATE_TYPE_H
#define STATE_TYPE_H
#include <vector>
#include <unordered_map>
using namespace std;

namespace StateType {
    using parameter_list = vector< int >;
    using predicate = map< parameter_list, int >;
    using predicates = vector< predicate >;
}

#endif // STATE_TYPE_H