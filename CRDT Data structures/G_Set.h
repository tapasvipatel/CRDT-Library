#include <bits/stdc++.h>
using namespace std;

template <class K=int>
class g_Set  //Grow-Only Set
{
public:
    set<K> set;
    K id;
    
    g_Set() {
    }
    
    //Give each object unique id
    g_Set(K id) {
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
        if (set.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }
    
    bool compare(g_Set<K> S, g_Set<K> T) {
        return includes(S.set.begin(), S.set.end(), T.set.begin(), T.set.end());
    }
    
    void merge(vector<g_Set<K>> replicas) {
        for (auto replica: replicas){
            set.insert(replica.set.begin(), replica.set.end());
        }
    }
    
    void print() {
        int gSet_size = set.size(); 
        cout << "GSet with ID=" << get_id();
        cout << " (";
        for (auto const &e: set) {
            if (gSet_size == 1){
                std::cout << e;
            } else{
                std::cout << e << ' ';
            }
            gSet_size--;   
        }
        cout << ")" << endl;
    }
};
