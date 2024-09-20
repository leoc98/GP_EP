#ifndef STATE_TYPE_H
#define STATE_TYPE_H
#include <vector>
#include <unordered_map>
using namespace std;

namespace StateType {
    using predicate = map< vector< int >, int >;
    using predicates = vector< predicate >;
}

#endif // STATE_TYPE_H