#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;
#include "../../CRDT Data structures/PN_Counter.h"
#include "../../CRDT Data structures/2P_Set.h"
#include "../../CRDT Data structures/G_Set.h"




void display_GrowCounter_int_variant(){
    cout << "-- G Counter Test (datatype = <INT,INT>) --" << endl;
    cout << "Creating grow counter with ID = 0" << endl;
    g_Counter<int,int> replica_GCounter_A(0);
    cout << "Creating grow counter with ID = 1" << endl;
    g_Counter<int,int> replica_GCounter_B(1);
    cout << endl;
    cout << "-------- Insert Operation Test --------" << endl;
    cout << "Adding (6 7 8) onto counter ID == 0" << endl;
    replica_GCounter_A.add(6);
    replica_GCounter_A.add(7);
    replica_GCounter_A.add(8); //21
    replica_GCounter_A.print();
    cout << endl;
    cout << "Adding (6 3 5) onto counter ID == 1" << endl;
    replica_GCounter_B.add(6);
    replica_GCounter_B.add(3);
    replica_GCounter_B.add(5); //14
    replica_GCounter_B.print();
    cout << endl;
    cout << endl;
    cout << "-------- Merge Operation Test --------" << endl;
    cout << "Before Merge Operation on ID == 0" << endl;
    replica_GCounter_A.print();
    cout << endl;
    replica_GCounter_A.merge({replica_GCounter_B});
    cout << "After Merge Operation ID == 0" << endl;
    replica_GCounter_A.print();
    cout << endl;
    cout << "Before Merge Operation on ID == 1" << endl;
    replica_GCounter_B.print();
    cout << endl;
    replica_GCounter_B.merge({replica_GCounter_A});
    cout << "After Merge Operation on ID == 1" << endl;
    replica_GCounter_B.print();
    cout << endl;
}

void display_GrowCounter_string_variant(){
    cout << "-- G Counter Test (datatype = <STRING,DOUBLE>) --" << endl;
    cout << "Creating grow counter with ID = Hello" << endl;
    g_Counter<string,double> replica_GCounter_A("Hello");
    cout << "Creating grow counter with ID = World" << endl;
    g_Counter<string,double> replica_GCounter_B("World");
    cout << endl;
    cout << endl;
    cout << "-------- Insert Operation Test --------" << endl;
    cout << "Adding (6.752 7.541 8.295) onto counter ID == Hello" << endl;
    replica_GCounter_A.add(6.752);
    replica_GCounter_A.add(7.541);
    replica_GCounter_A.add(8.295); 
    replica_GCounter_A.print();
    cout << endl;
    cout << "Adding (6.124 3.942 5.541) onto counter ID == World" << endl;
    replica_GCounter_B.add(6.124);
    replica_GCounter_B.add(3.942);
    replica_GCounter_B.add(5.541); 
    replica_GCounter_B.print();
    cout << endl;
    cout << endl;
    cout << "-------- Merge Operation Test --------" << endl;
    cout << "Before Merge Operation on ID == Hello" << endl;
    replica_GCounter_A.print();
    cout << endl;
    replica_GCounter_A.merge({replica_GCounter_B});
    cout << "After Merge Operation ID == Hello" << endl;
    replica_GCounter_A.print();
    cout << endl;
    cout << "Before Merge Operation on ID == World" << endl;
    replica_GCounter_B.print();
    cout << endl;
    replica_GCounter_B.merge({replica_GCounter_A});
    cout << "After Merge Operation on ID == World" << endl;
    replica_GCounter_B.print();
}

void display_PNCounter(){
    cout << "-- PN Counter Set Test (datatype = <INT,INT>) --" << endl;
    cout << "Creating Positive/Negative counter with ID = 0" << endl;
    pn_Counter<int,int> replica_PNCounter_A(0);
    cout << "Creating Positive/Negative counter with ID = 1" << endl;
    pn_Counter<int,int> replica_PNCounter_B(1);
    cout << endl;
    cout << "-------- Insert Operation Test --------" << endl;
    cout << "Adding (6 7 8) onto counter ID == 0" << endl;
    replica_PNCounter_A.add(6);
    replica_PNCounter_A.add(7);
    replica_PNCounter_A.add(8);
    replica_PNCounter_A.print();
    cout << endl;
    cout << "Adding (6 3 5) onto counter ID == 1" << endl;
    replica_PNCounter_B.add(6);
    replica_PNCounter_B.add(3);
    replica_PNCounter_B.add(5);
    replica_PNCounter_B.print();
    cout << endl;
    cout << endl;
    cout << "-------- Merge Operation Test --------" << endl;
    cout << "Before Merge Operation on ID == 0" << endl;
    replica_PNCounter_A.print();
    cout << endl;
    replica_PNCounter_A.merge({replica_PNCounter_B});
    cout << "After Merge Operation ID == 0" << endl;
    replica_PNCounter_A.print();
    cout << endl;
    cout << "Before Merge Operation on ID == 1" << endl;
    replica_PNCounter_B.print();
    cout << endl;
    cout << "After Merge Operation ID == 1" << endl;
    replica_PNCounter_B.merge({replica_PNCounter_A});
    replica_PNCounter_B.print();
    cout << endl;
    cout << endl;
    cout << "-------- Delete Operation Test --------" << endl;
    cout << "Subtracting (6 7) on counter  ID == 0" << endl;
    replica_PNCounter_A.subtract(6);
    replica_PNCounter_A.subtract(7);
    replica_PNCounter_A.print();
    cout << endl;
    cout << "Subtracting (5) on counter ID == 1" << endl;
    replica_PNCounter_B.subtract(5);
    replica_PNCounter_B.print();
    cout << endl;
    cout << endl;
    cout << "-------- Merge After Delete Operation Test --------" << endl;
    cout << "Before Merge Operation on ID == 0" << endl;
    replica_PNCounter_A.print();
    cout << endl;
    cout << "After Merge Operation ID == 0" << endl;
    replica_PNCounter_A.merge({replica_PNCounter_B});
    replica_PNCounter_A.print();
    cout << endl;
    cout << "Before Merge Operation on ID == 1" << endl;
    replica_PNCounter_B.print();
    cout << endl;
    cout << "After Merge Operation on ID == 1" << endl;
    replica_PNCounter_B.merge({replica_PNCounter_A});
    replica_PNCounter_B.print();
    cout << endl;
}

void display_grow_Set(){
    cout << "-- G Set Test (datatype = <STRING>) --" << endl;
    cout << "Creating Grow Set with ID = GrowA" << endl;
    g_Set<string> replica_gSet_A("GrowA");
    cout << "Creating Grow Set with ID = GrowB" << endl;
    g_Set<string> replica_gSet_B("GrowB");
    cout << endl;
    cout << "-------- Insert Operation Test --------" << endl;
    cout << "Inserting the elements (A B C) in set ID == GrowA" << endl;
    replica_gSet_A.add("A");
    replica_gSet_A.add("B");
    replica_gSet_A.add("C");
    replica_gSet_A.print();
    cout << "Inserting the elements (D E F) in set ID == GrowB" << endl;
    replica_gSet_B.add("D");
    replica_gSet_B.add("E");
    replica_gSet_B.add("F");
    replica_gSet_B.print();
    cout << endl;
    cout << "-------- Merge Operation Test --------" << endl;
    cout << "Before Merge Operation on ID == GrowA" << endl;
    replica_gSet_A.print();
    replica_gSet_A.merge({replica_gSet_B});
    cout << "After Merge Operation ID == GrowA" << endl;
    replica_gSet_A.print();
    cout << "Before Merge Operation on ID == GrowB" << endl;
    replica_gSet_B.print();
    replica_gSet_B.merge({replica_gSet_A});
    cout << "After Merge Operation ID == GrowB" << endl;
    replica_gSet_B.print();
}

void display_2P_Set(){
    cout << "-- 2P Set Test (datatype = <DOUBLE>) --" << endl;
    cout << "Creating 2P Set with ID = 0" << endl;
    TwoPhase_Set<double> replica_2P_A(0);
    cout << "Creating 2P Set with ID = 1" << endl;
    TwoPhase_Set<double> replica_2P_B(1);
    cout << endl;
    cout << "-------- Insert Operation Test --------" << endl;
    cout << "Inserting the elements (5.55 6.75 9.21) in set ID == 0" << endl;
    replica_2P_A.add(5.55);
    replica_2P_A.add(6.75);
    replica_2P_A.add(9.21);
    replica_2P_A.print();
    cout << "Inserting the elements (5.72 2.56 3.24) in set ID == 1" << endl;
    replica_2P_B.add(5.72);
    replica_2P_B.add(2.56);
    replica_2P_B.add(3.24);
    replica_2P_B.print();
    cout << endl;
    cout << "-------- Merge Operation Test --------" << endl;
    cout << "Before Merge Operation on ID == 0" << endl;
    replica_2P_A.print();
    replica_2P_A.merge({replica_2P_B});
    cout << "After Merge Operation ID == 0" << endl;
    replica_2P_A.print();
    cout << "Before Merge Operation on ID == 1" << endl;
    replica_2P_B.print();
    replica_2P_B.merge({replica_2P_A});
    cout << "After Merge Operation ID == 1" << endl;
    replica_2P_B.print();
    cout << endl;
    cout << "-------- Delete Operation Test --------" << endl;
    cout << "Deleting the elements (5.55) in set ID == 0" << endl;
    replica_2P_A.remove(5.55);
    replica_2P_A.print();
    cout << "Deleting the elements (2.56, 3.24) in set ID == 1" << endl;
    replica_2P_B.remove(2.56);
    replica_2P_B.remove(3.24);
    replica_2P_B.print();
    cout << endl;
    cout << "-------- Merge After Delete Operation Test --------" << endl;
    cout << "Before Merge Operation on ID == 0" << endl;
    replica_2P_A.print();
    replica_2P_A.merge({replica_2P_B});
    cout << "After Merge Operation ID == 0" << endl;
    replica_2P_A.print();
    cout << "Before Merge Operation on ID == 1" << endl;
    replica_2P_B.print();
    replica_2P_B.merge({replica_2P_A});
    cout << "After Merge Operation ID == 1" << endl;
    replica_2P_B.print();
}

int main(){
    display_GrowCounter_int_variant();
    cout << endl;
    cout << endl;
    display_GrowCounter_string_variant();
    cout << endl;
    cout << endl;
    cout << endl;
    display_PNCounter();
    cout << endl;
    cout << endl;
    display_grow_Set();
    cout << endl;
    cout << endl;
    display_2P_Set();
    cout << endl;
    cout << endl;
    return 0;
}
