#include <bits/stdc++.h>
using namespace std;

template <class K=int, class V=int>
class g_Set  //Grow-Only Set
{
public:
    unordered_set<K> set;
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
    
    int get_id(K id) {
        return this->id;
    }
    
    void add(V e) {
        set.insert(e);
    }
    
    bool lookup(V e) {
        if (set.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }
    
    bool compare(g_Set<K,V> S, g_Set<K,V> T) {
        return includes(S.set.begin(), S.set.end(), T.set.begin(), T.set.end());
    }
    
    void merge(g_Set<K,V> replica) {
        set.insert(replica.set.begin(), replica.set.end());
    }
    
    void print() {
        cout << "GSet: (";
        for (auto const &e: set) {
            std::cout << e << ' ';
        }
        cout << ")" << endl;
    }
};
