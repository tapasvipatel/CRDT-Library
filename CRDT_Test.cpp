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

    g_counter()
    {
        id = 0;
    }

    //Give each object unique id
    g_Counter(int id)
    {
        this->id = id;
    }

}

main()
{
    cout << "Hello World";
    return 0;
}