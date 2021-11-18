#include <bits/stdc++.h>
using namespace std;
#include "CRDT Data structures/PN_Counter.h"
#include "CRDT Data structures/2P_Set.h"
#include "CRDT Data structures/G_Set.h"

int main()
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

    cout << "-------Testing 2P_set----------" << endl;
    cout << "Update: Add" << endl;
    TwoPhase_Set<int,int> replica_2P_A(0);
    TwoPhase_Set<int,int> replica_2P_B(1);
    replica_2P_A.add(6);
    replica_2P_A.add(7);
    replica_2P_A.add(8);
    replica_2P_B.add(6);
    replica_2P_B.add(3);
    replica_2P_B.add(5);
    cout << "Before Merge" << endl;
    cout << "replicaA: ";
    replica_2P_A.print();
    cout << "replicaB: ";
    replica_2P_B.print();
    replica_2P_A.merge(replica_2P_B);
    cout << "After Merge replicaA" << endl;
    cout << "replicaA: ";
    replica_2P_A.print();
    cout << "replicaB: ";
    replica_2P_B.print();
    replica_2P_B.merge(replica_2P_A);
    cout << "After Merge replicaB" << endl;
    cout << "replicaA: ";
    replica_2P_A.print();
    cout << "replicaB: ";
    replica_2P_B.print();
    
    cout << "Update: Remove" << endl;
    cout << "Remove 3 from replicaA" << endl;
    replica_2P_A.remove(3);
    cout << "Remove 5 from replicaB" << endl;
    replica_2P_B.remove(5);
    cout << "Before Merge" << endl;
    cout << "replicaA: ";
    replica_2P_A.print();
    cout << "replicaB: ";
    replica_2P_B.print();
    replica_2P_A.merge(replica_2P_B);
    cout << "After Merge replicaA" << endl;
    cout << "replicaA: ";
    replica_2P_A.print();
    cout << "replicaB: ";
    replica_2P_B.print();
    replica_2P_B.merge(replica_2P_A);
    cout << "After Merge replicaB" << endl;
    cout << "replicaA: ";
    replica_2P_A.print();
    cout << "replicaB: ";
    replica_2P_B.print();
    cout << "------------------------------" << endl;

     
    cout << "-------Testing g_set----------" << endl;
    g_Set<int,int> replica_gSet_A(0);
    g_Set<int,int> replica_gSet_B(1);
    replica_gSet_A.add(6);
    replica_gSet_A.add(7);
    replica_gSet_A.add(8);
    replica_gSet_B.add(6);
    replica_gSet_B.add(3);
    replica_gSet_B.add(5);
    cout << "Before Merge" << endl;
    cout << "replicaA: ";
    replica_gSet_A.print();
    cout << "replicaB: ";
    replica_gSet_B.print();
    replica_gSet_A.merge(replica_gSet_B);
    cout << "After Merge replicaA" << endl;
    cout << "replicaA: ";
    replica_gSet_A.print();
    cout << "replicaB: ";
    replica_gSet_B.print();
    replica_gSet_B.merge(replica_gSet_A);
    cout << "After Merge replicaB" << endl;
    cout << "replicaA: ";
    replica_gSet_A.print();
    cout << "replicaB: ";
    replica_gSet_B.print();
    cout << "------------------------------" << endl;
}