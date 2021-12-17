#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <assert.h>
using namespace std;
#include "../CRDT Data structures/PN_Counter.h"
#include "../CRDT Data structures/2P_Set.h"
#include "../CRDT Data structures/G_Set.h"

/*
Rules for current CRDT to follow when creating tests for each new CRTD Objects
Please note --> Subjective to change in the future
1) Create 2 CRDT objects
2) Insert into CRDT --> Validate Insert, Merge CRDT --> Validate Merge after insert
3) If CRDT supports delete operation --> Validate Delete, Merge CRDT --> Validate Merget after Delete
*/

void test_GCounter(){
    g_Counter<int,int> replica_GCounter_A(0);
    g_Counter<int,int> replica_GCounter_B(1);
    cout << "Unit Tests for G_Counter:" << endl;
    cout << "Testing insert Operation: ";
    replica_GCounter_A.add(6);
    replica_GCounter_A.add(7);
    replica_GCounter_A.add(8); //21
    replica_GCounter_B.add(6);
    replica_GCounter_B.add(3);
    replica_GCounter_B.add(5); //14
    assert(replica_GCounter_A.get_total_val() == 21);
    assert(replica_GCounter_B.get_total_val() == 14);
    cout << "Passed." << endl;
    cout << "Testing Merge After Insert Operation: ";
    replica_GCounter_A.merge({replica_GCounter_B});
    replica_GCounter_B.merge({replica_GCounter_A});
    assert(replica_GCounter_A.get_total_val() == replica_GCounter_B.get_total_val()); //35
    cout << "Passed." << endl;
}

void test_PNCounter(){
    cout << "Unit Tests for PN_Counter:" << endl;
    cout << "Testing insert Operation: ";
    pn_Counter<int,int> replica_PNCounter_A(0);
    pn_Counter<int,int> replica_PNCounter_B(1);
    replica_PNCounter_A.add(6);
    replica_PNCounter_A.add(7);
    replica_PNCounter_A.add(8);
    replica_PNCounter_B.add(6);
    replica_PNCounter_B.add(3);
    replica_PNCounter_B.add(5);
    assert(replica_PNCounter_A.get_total_val() == 21);
    assert(replica_PNCounter_B.get_total_val() == 14);
    cout << "Passed." << endl;
    cout << "Testing Merge After Insert Operation: ";
    replica_PNCounter_A.merge({replica_PNCounter_B});
    replica_PNCounter_B.merge({replica_PNCounter_A});
    assert(replica_PNCounter_A.get_total_val() == replica_PNCounter_B.get_total_val()); //35
    cout << "Passed." << endl;
    replica_PNCounter_A.subtract(6);
    replica_PNCounter_B.subtract(3);
    cout << "Testing Delete Operation: ";
    assert(replica_PNCounter_A.get_total_val() == 29);
    assert(replica_PNCounter_B.get_total_val() == 32);
    cout << "Passed." << endl;
    cout << "Testing Merge After Delete Operation: ";
    replica_PNCounter_A.merge({replica_PNCounter_B});
    replica_PNCounter_B.merge({replica_PNCounter_A});
    assert(replica_PNCounter_A.get_total_val() == replica_PNCounter_B.get_total_val()); //35
    cout << "Passed." << endl;
}

void test_gSet(){
    cout << "Unit Tests for G Set:" << endl;
    g_Set<int> replica_gSet_A(0);
    g_Set<int> replica_gSet_B(1);
    replica_gSet_A.add(6);
    replica_gSet_A.add(7);
    replica_gSet_A.add(8);
    replica_gSet_B.add(6);
    replica_gSet_B.add(3);
    replica_gSet_B.add(5);
    cout << "Testing insert Operation: ";
    assert(replica_gSet_A.lookup(6));
    assert(replica_gSet_A.lookup(7));
    assert(replica_gSet_A.lookup(8));
    assert(replica_gSet_B.lookup(6));
    assert(replica_gSet_B.lookup(3));
    assert(replica_gSet_B.lookup(5));
    cout << "Passed." << endl;
    cout << "Testing Merge After Insert Operation: ";
    replica_gSet_A.merge({replica_gSet_B});
    replica_gSet_B.merge({replica_gSet_A});
    assert(replica_gSet_A.compare(replica_gSet_A,replica_gSet_B));
    cout << "Passed." << endl;
}

void test_2PSet(){
    cout << "Unit Tests for 2P Set:" << endl;
    TwoPhase_Set<int> replica_2P_A(0);
    TwoPhase_Set<int> replica_2P_B(1);
    replica_2P_A.add(6);
    replica_2P_A.add(7);
    replica_2P_A.add(8);
    replica_2P_B.add(6);
    replica_2P_B.add(3);
    replica_2P_B.add(5);
    cout << "Testing insert Operation: ";
    assert(replica_2P_A.lookup(6));
    assert(replica_2P_A.lookup(7));
    assert(replica_2P_A.lookup(8));
    assert(replica_2P_B.lookup(6));
    assert(replica_2P_B.lookup(3));
    assert(replica_2P_B.lookup(5));
    cout << "Passed." << endl;
    cout << "Testing Merge After Insert Operation: ";
    replica_2P_A.merge({replica_2P_B});
    replica_2P_B.merge({replica_2P_A});
    assert(replica_2P_A.compare(replica_2P_A,replica_2P_B));
    cout << "Passed." << endl;
    replica_2P_A.remove(3);
    replica_2P_B.remove(5);
    cout << "Testing Delete Operation: ";
    assert(replica_2P_A.lookup(3) == false);
    assert(replica_2P_B.lookup(5) == false);
    cout << "Passed." << endl;
    cout << "Testing Merge After Delete Operation: ";
    replica_2P_A.merge({replica_2P_B});
    replica_2P_B.merge({replica_2P_A});
    assert(replica_2P_A.compare(replica_2P_A,replica_2P_B));
    cout << "Passed." << endl;
}


int main(){
    test_GCounter();
    cout << endl;
    test_PNCounter();
    cout << endl;
    test_gSet();
    cout << endl;
    test_2PSet();
    cout << endl;
    return 0;
}





//All 