#ifndef EPISTEMIC_PREDICATE_H
#define EPISTEMIC_PREDICATE_H
#include <string>

class EpistemicPredicate {
private:
    std::string predicate;
    std::string agent;
    std::string parent_predicate;
public:
    EpistemicPredicate(const std::string &predicate, const std::string &agent, const std::string &parent_predicate) : predicate(predicate), agent(agent), parent_predicate(parent_predicate) {}
    std::string getPredicate() const {
        return predicate;
    }
    std::string getAgent() const {
        return agent;
    }
    std::string getParentPredicate() const {
        return parent_predicate;
    }
    void setPredicate(const std::string &predicate) {
        this->predicate = predicate;
    }
    void setAgent(const std::string &agent) {
        this->agent = agent;
    }
    void setParentPredicate(const std::string &parent_predicate) {
        this->parent_predicate = parent_predicate;
    }
    std::string getName() const {
        return parent_predicate + (parent_predicate.length()?" ":"") + predicate + " " + agent;
    }
    std::string toString() const {
        return getName();
    }
    bool operator==(const EpistemicPredicate &other) const {
        return getName() == other.getName();
    }
    bool operator==(const std::string &other) const {
        return getName() == other;
    }
};
// hash function
namespace std {
    template<> struct hash<EpistemicPredicate> {
        std::size_t operator()(EpistemicPredicate const& ep) const noexcept {
            return std::hash<std::string>()(ep.getName());
        }
        std::size_t operator()(string const& s) const noexcept {
            return std::hash<std::string>()(s);
        }
    };
}

#endif // EPISTEMIC_PREDICATE_H