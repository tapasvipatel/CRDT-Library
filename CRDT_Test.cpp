#include <bits/stdc++.h>
using namespace std;

//To do: Implement gCounter using key,value


/* Basic Info about this CRDT 
an integer counter that can only be incremeneted

Function 1 Objective --> TO increment

Funtion 2 Objective --> Request the current value


// To sync with this data structure, the value converges towards the sum of its increments



*/

class g_Counter
{
public:
    //Database
    unordered_map<int, int> m;
    //id
    int id;

    g_Counter(){
    }
     //Give each object unique id
    g_Counter(int id){
        this->id = id;
    }

    g_Counter increment(int val = 1){ //the value represents how much we want to increment by // add function
        g_Counter curr;
        m[id]+=val;
        curr.m[id]=m[id];
        return curr;
    }

    void join(g_Counter replica){   //merge 
        for (auto i: replica.m){
            m[i.first] = max(i.second,m[i.first]);
        }
    }
    int get_curr_val(){  // query lookup
        return m[id];
    }

    int get_total_val(){
        int res = 0;
        for (auto i: m){
            res+=i.second;
        }
        return res;
    }

    void print(){
        cout << "GCounter: (";
        for (auto i: m){
            cout << i.first << "->" << i.second;
        }
        cout << ")";
    }
};

main()
{
    g_Counter replicaA(0);
    g_Counter replicaB(1);
    g_Counter replicaC;
    g_Counter replicaD;
    replicaC.join(replicaA.increment());
    replicaC.join(replicaA.increment(7));
    replicaD.join(replicaB.increment());
    replicaD.join(replicaB.increment(5));
    
    cout << "Here Replica A and Replica C should converge" << endl;
    replicaC.print();
    replicaA.print();
    cout << endl;
    cout << "Here Replica D and Replica B should converge" << endl;
    replicaD.print();
    replicaB.print();
}