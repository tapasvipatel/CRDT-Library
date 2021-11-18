#include <bits/stdc++.h>
using namespace std;

template <class K=int, class V=int>
class TwoPhase_Set //Two-Phase Set
{
public:
    unordered_set<K> set;
    unordered_set<K> tombstone;
    K id;
    
    TwoPhase_Set(){
    }
    
    //Give each object unique id
    TwoPhase_Set(K id) {
        this->id = id;
    }
    
    void set_id(K id) {
        this->id = id;
    }
    
    int get_id(K id) {
        return this->id;
    }
    
    void add(V e) {
        set.insert(e);
    }
    
    bool lookup(V e) {
        if (set.count(e) && !tombstone.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }
    
    void remove(V e) {
        if (lookup(e)) {
            tombstone.insert(e);
        }
    }
    
    bool compare(TwoPhase_Set<K,V> S, TwoPhase_Set<K,V> T) {
        return includes(S.set.begin(), S.set.end(), T.set.begin(), T.set.end()) || includes(S.tombstone.begin(), S.tombstone.end(), T.tombstone.begin(), T.tombstone.end());
    }
    
    void merge(TwoPhase_Set<K,V> replica) {
        set.insert(replica.set.begin(), replica.set.end());
        tombstone.insert(replica.tombstone.begin(), replica.tombstone.end());
    }
    
    void print() {
        cout << "2PSet: (";
        for (auto const &e: set) {
            if (lookup(e)) {
                std::cout << e << ' ';
            }
        }
        cout << ")" << endl;
    }
};