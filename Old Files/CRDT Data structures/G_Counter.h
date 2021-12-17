#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

//To do: Implement gCounter using key,value
/* Basic Info about this CRDT 
an integer counter that can only be incremeneted
Function 1 Objective --> TO increment
Funtion 2 Objective --> Request the current value
// To sync with this data structure, the value converges towards the sum of its increments
*/
template <class K=int, class V=int>
class g_Counter
{
public:
    //Database
    map<K, V> m;
    //id
    K id;

    g_Counter(){
    }
     //Give each object unique id
    g_Counter(K id){
        this->id = id;
    }
    void set_id(K id){
        this->id = id;
    }

    K get_id(){
        return this->id;
    }

    g_Counter add(V val = 1){ //the value represents how much we want to increment by // add function
        g_Counter<K,V> curr;
        m[id]+=val;
        curr.m[id]=m[id];
        return curr;
    }
    // g_Counter subtract(V val = 1){
    //     g_Counter<K,V> curr;
    //     m[id]-=val;
    //     curr.m[id]=m[id];
    //     return curr;
    // }

    void merge(vector<g_Counter<K,V>> replicas){   //merge
        for (auto i: replicas){
            for (auto j: i.m){
                m[j.first] = max(j.second,m[j.first]);
            }
        } 
    }
    V get_curr_val(){  // query lookup
        return m[id];
    }

    V get_total_val(){
        V res = 0;
        for (auto i: m){
            res+=i.second;
        }
        return res;
    }

    void print(){
        cout << "GCounter with ID=" << get_id();
        cout << " (";
        int size_map = m.size();
        for (auto i: m){
            if (size_map == 1){
                cout << i.first << "->" << i.second;
            } else{
                cout << i.first << "->" << i.second << " ";
            }
            size_map--;
        }
        cout << ")";
        // display_result.pop_back();
        // display_result+=")";
        // cout << display_result;
    }
};