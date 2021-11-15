#include <bits/stdc++.h>
using namespace std;

class g_Set  //Grow-Only Set
{
public:
    unordered_set<int> set;
    int id;
    
    g_Set() {
    }
    
    //Give each object unique id
    g_Set(int id) {
        this->id = id;
    }
    
    void set_id(int id) {
        this->id = id;
    }
    
    int get_id(int id) {
        return this->id;
    }
    
    void add(int e) {
        set.insert(e);
    }
    
    bool lookup(int e) {
        if (set.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }
    
    bool compare(g_Set S, g_Set T) {
        return includes(S.set.begin(), S.set.end(), T.set.begin(), T.set.end());
    }
    
    void merge(g_Set replica) {
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


class TwoPhase_Set //Two-Phase Set
{
public:
    unordered_set<int> set;
    unordered_set<int> tombstone;
    int id;
    
    TwoPhase_Set(){
    }
    
    //Give each object unique id
    TwoPhase_Set(int id) {
        this->id = id;
    }
    
    void set_id(int id) {
        this->id = id;
    }
    
    int get_id(int id) {
        return this->id;
    }
    
    void add(int e) {
        set.insert(e);
    }
    
    bool lookup(int e) {
        if (set.count(e) && !tombstone.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }
    
    void remove(int e) {
        if (lookup(e)) {
            tombstone.insert(e);
        }
    }
    
    bool compare(TwoPhase_Set S, TwoPhase_Set T) {
        return includes(S.set.begin(), S.set.end(), T.set.begin(), T.set.end()) || includes(S.tombstone.begin(), S.tombstone.end(), T.tombstone.begin(), T.tombstone.end());
    }
    
    void merge(TwoPhase_Set replica) {
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


main() {
    
    /*
    cout << "-------Testing g_set----------" << endl;
    g_Set replicaA(0);
    g_Set replicaB(1);
    replicaA.add(6);
    replicaA.add(7);
    replicaA.add(8);
    replicaB.add(6);
    replicaB.add(3);
    replicaB.add(5);
    cout << "Before Merge" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    replicaA.merge(replicaB);
    cout << "After Merge replicaA" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    replicaB.merge(replicaA);
    cout << "After Merge replicaB" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    */
    
    cout << "-------Testing 2P_set----------" << endl;
    cout << "Update: Add" << endl;
    TwoPhase_Set replicaA(0);
    TwoPhase_Set replicaB(1);
    replicaA.add(6);
    replicaA.add(7);
    replicaA.add(8);
    replicaB.add(6);
    replicaB.add(3);
    replicaB.add(5);
    cout << "Before Merge" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    replicaA.merge(replicaB);
    cout << "After Merge replicaA" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    replicaB.merge(replicaA);
    cout << "After Merge replicaB" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    
    cout << "Update: Remove" << endl;
    cout << "Remove 3 from replicaA" << endl;
    replicaA.remove(3);
    cout << "Remove 5 from replicaB" << endl;
    replicaB.remove(5);
    cout << "Before Merge" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    replicaA.merge(replicaB);
    cout << "After Merge replicaA" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    replicaB.merge(replicaA);
    cout << "After Merge replicaB" << endl;
    cout << "replicaA: ";
    replicaA.print();
    cout << "replicaB: ";
    replicaB.print();
    
    return 0;
}
