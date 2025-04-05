#ifndef EPISTEMIC_SEEING_SUPPORT_DESCRIPTOR_H
#define EPISTEMIC_SEEING_SUPPORT_DESCRIPTOR_H
#include "unordered_map"

class EpistemicSeeingSupportDescriptor {
public:
    virtual ~EpistemicSeeingSupportDescriptor() = default;
};

class BBLEpistemicSeeingSupportDescriptor : public EpistemicSeeingSupportDescriptor {
public:
    unordered_map<string, pair<int, int>> obj_pos;
    unordered_map<string, string> obj_dir;
};

#endif // EPISTEMIC_SEEING_SUPPORT_DESCRIPTOR_H