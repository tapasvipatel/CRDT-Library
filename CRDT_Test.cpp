#include <bits/stdc++.h>
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
    unordered_map<K, V> m;
    //id
    K id;

    g_Counter(){
    }
     //Give each object unique id
    g_Counter(int id){
        this->id = id;
    }
    void set_id(K id){
        this->id = id;
    }

    g_Counter increment(V val = 1){ //the value represents how much we want to increment by // add function
        g_Counter<K,V> curr;
        m[id]+=val;
        curr.m[id]=m[id];
        return curr;
    }

    void join(g_Counter replica){   //merge 
        for (auto i: replica.m){
            m[i.first] = max(i.second,m[i.first]);
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
        cout << "GCounter: (";
        for (auto i: m){
            cout << i.first << "->" << i.second;
        }
        cout << ")";
    }
};

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

    pn_Counter increment(V val = 1){
        pn_Counter<K,V> curr;
        curr.p=p.increment(val);
        return curr;
    }
    pn_Counter decrement(V val = 1){
        pn_Counter<K,V> curr;
        curr.n=n.increment(val);
        return curr;
    }
    
    V get_total_val(){
        return p.get_total_val()-n.get_total_val();
    }
    V get_curr_val(){
        return p.get_curr_val()-n.get_curr_val();
    }
    void join(pn_Counter replica){
        p.join(replica.p);
        n.join(replica.n);
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



main()
{
    cout << "-------Testing g_counters----------";
    g_Counter<int,int> replicaA(0);
    g_Counter<int,double> replicaB(1);
    g_Counter<int,int> replicaC;
    g_Counter<int,double> replicaD;
    replicaC.join(replicaA.increment());
    replicaC.join(replicaA.increment(7));
    replicaD.join(replicaB.increment());
    replicaD.join(replicaB.increment(5.5));
    
    cout << "Here Replica A and Replica C should converge" << endl;
    replicaC.print();
    replicaA.print();
    cout << endl;
    cout << "Here Replica D and Replica B should converge" << endl;
    replicaD.print();
    replicaB.print();
    cout << endl;
    cout << "-------Testing pn_counters----------" << endl;
    pn_Counter<int,int> replica_A(2);
    pn_Counter<int,double> replica_B(5);
    pn_Counter<int,int> replica_C;
    pn_Counter<int,double> replica_D;
    replica_C.join(replica_A.increment(3));
    replica_C.join(replica_A.decrement(2));
    replica_D.join(replica_B.increment());
    replica_D.join(replica_B.increment(5.7));
    cout << "Here Replica_A and Replica_C should converge" << endl;
    replica_C.print();
    replica_A.print();
    cout << endl;
    cout << replica_C.get_total_val() << " " << replica_A.get_total_val() << endl; //The value converges towards the sum of its increments
    cout << "Here Replica_D and Replica_B should converge" << endl;
    replica_D.print();
    replica_B.print();
    cout << endl;
    cout << replica_D.get_total_val() << " " << replica_B.get_total_val() << endl; //The value converges towards the sum of its increments
    cout << endl;
}