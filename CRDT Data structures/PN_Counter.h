#include <bits/stdc++.h>
using namespace std;
#include "G_Counter.h"

template <class K=int, class V=int>
class pn_Counter{
    public:
    g_Counter<K,V> p,n;
    pn_Counter(){
    }
    pn_Counter(K id){
        p.set_id(id);
        n.set_id(id);
    }

    pn_Counter add(V val = 1){
        pn_Counter<K,V> curr;
        curr.p=p.add(val);
        return curr;
    }
    pn_Counter subtract(V val = 1){
        pn_Counter<K,V> curr;
        curr.n=n.add(val);
        return curr;
    }
    
    V get_total_val(){
        return p.get_total_val()-n.get_total_val();
    }
    V get_curr_val(){
        return p.get_curr_val()-n.get_curr_val();
    }
    void merge(vector<pn_Counter<K,V>> replicas){
        for (auto replica: replicas){
            p.merge({replica.p});
            n.merge({replica.n});
        }
    }
    void print(){
        cout << "PNCounter P: ";
        for (auto i: p.m){
            cout << i.first << "->" << i.second << " "; 
        }
        cout << "PNCounter N: ";
        for (auto i: n.m){
            cout << i.first << "->" << i.second << " "; 
        }
    }
};
