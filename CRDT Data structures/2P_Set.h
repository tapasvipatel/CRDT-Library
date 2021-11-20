#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

template <class K=int>
class TwoPhase_Set //Two-Phase Set
{
public:
    set <K> tombstone;
    set <K> set;
    
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
    
    K get_id() {
        return this->id;
    }
    
    void add(K e) {
        set.insert(e);
    }
    
    bool lookup(K e) {
        if (set.count(e) && !tombstone.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }
    
    void remove(K e) {
        if (lookup(e)) {
            tombstone.insert(e);
        }
    }
    
    bool compare(TwoPhase_Set<K> S, TwoPhase_Set<K> T) {
        return includes(S.set.begin(), S.set.end(), T.set.begin(), T.set.end()) || includes(S.tombstone.begin(), S.tombstone.end(), T.tombstone.begin(), T.tombstone.end());
    }
    
    void merge(vector<TwoPhase_Set<K>> replicas) {
        for (auto replica: replicas){
            set.insert(replica.set.begin(), replica.set.end());
            tombstone.insert(replica.tombstone.begin(), replica.tombstone.end());
        }
        
    }
    
    void print() {
        cout << "2PSet with ID=" << get_id();
        cout << " (";
        int size_of_Set = set.size();
        for (auto const &e: set) {
            if (size_of_Set == 1){
                if (lookup(e)) {
                    std::cout << e;
                }
            } else{
                if (lookup(e)) {
                    std::cout << e << ' ';
                }
            }
            size_of_Set--;
        }
        cout << ")" << endl;
    }
};