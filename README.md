<h1 align="center">
  <br>
  <a href="https://github.com/tapasvipatel/CRDT-Library"><img src="https://i.ibb.co/xMSp6wj/Merge-Logo.png"></a>
  
  MergeRDT
</h1>
<h3 align=center> A CRDT libary supporting 12 composite data structures written in C++ framework</h3>

------------------

![](https://img.shields.io/badge/Version-Version%201.0-green) ![](https://img.shields.io/badge/Language-C%2B%2B%2011-brightgreen) ![](https://img.shields.io/badge/Build-Passing-green) ![](https://img.shields.io/badge/Graphics_Framework-SFML%2C%20TGUI-blue) ![](https://img.shields.io/badge/Pipeline-Catch2-orange) ![](https://img.shields.io/badge/OS%20System-Ubuntu%2020.04-yellow) 

<div align="center">
  <h3>Table of Contents</h3>
  <a href="#about">About</a>
  — 
  <a href="#features">Content</a>
  — 
  <a href="#setup">Set Up</a>
  — 
  <a href="#guide">Guide for each CRDT</a>
  — 
  <a href="#showcase">Application </a>
  — 
  <a href="#UnitTesting">CI/CD Framework</a>
  — 
  <a href="#Benchmark">Performance Analysis</a>
  — 
  <a href="#Contribute">Contribute</a>
  — 
  <a href="#license">License</a>
</div>
<hr>

<h2 id="about"> ❓ About </h2>
The goal of this project is to provide an alternative to strong eventual consistency models in a
distributed system using multiple primitive data types. The use of conflict-free replicated data types (CRDTs) 
can be used to prevent single point of failures as well as provide a good model for achieving strong eventual consistency. 
CRDTs are data structures that can be used by applications without the need of a central server. They are able to
communicate with other applications to send and receive data. CRDTs work by using simple
mathematical rules to guarantee strong eventual consistency. CRDTs guarantee that if an application in
the distributed system goes offline, once it comes back online, it will eventually converge to the same
state as all the other applications in the system.
CRDTs can be used in offline systems, peer-to-peer communication networks, or traditional client
to server networks


<h2 id="features"> 📃 Features </h2>
This libary currently supports a combined of 12 CRDTs. We have also created a Trello-clone that utilizes different CRDTs in this libary.
This is to showcase how CRDTs can be used in a practical application. 



<h2 id="setup"> ⚙ Set Up </h2>
This libary has only been validated on Linux OS, and support for Windows/MAC will be provided at a later time. To use the libary itself:

<h3 id="Dependencies"> Dependencies </h3>

<h4 id="CMake"> CMake </h3>

```
sudo-apt get install cmake
```
<h4 id="Catch2"> Catch2 </h3>

```
git clone https://github.com/catchorg/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
```
<h4 id="SFML"> SFML </h3>

```
sudo apt-get install libsfml-dev
```
<h4 id="TGUI"> TGUI </h3>

```
sudo add-apt-repository PPA:texus/tgui
sudo apt-get update
sudo apt-get install ligtgui-0.9-dev
```

<h3 id="BuildScripts"> Build Scripts </h3>

```
./generate.sh : Generate CRDT library and auxiliary binaries
  * testing : test binary
  * test_application: test application binary
  * trello_application : trello application binary
  * performance : performance binary
  * clean : clean build of CRDT library
  * networking : networking binary
```

```
./generate_prebuilt.sh : Generate prebuilt versions of test cases for GitHub Workflow
```

<h3 id="RunScripts"> Run Scripts </h3>

```
run_performance.sh : Run performance binaries for all CRDTs
```

```
run_test_application.sh : Run test application
```

```
run_tests.sh : Run all tests
```

```
run_trello_application.sh : Run single instance of trello application
```

<h2 id="guide"> 🐕‍🦺 Guide for CRDT data types</h2>

```
Each CRDT class has a handler and a metadata class. On each of your server, you create the handler once. 
To actually instatiate the CRDT data structure use the metadata. 
To add the CRDT to the server, add the metadata to the handler. 
```
<div align="center">
  <h3>CRDTs Supported</h3>
  <a href="#GrowCounter">Grow Only Counter</a>
  — 
  <a href="#PNCounter">Positive Negative Counter</a>
  — 
  <a href="#2P Graph">Two Phase Two Phase Graph</a>
  — 
  <a href="#GSet">Grow Only Set</a>
  — 
  <a href="#OrSet">Or-Set</a>
  — 
  <a href="#2PSet">Two-Phase Set</a>
   — 
  <a href="#GMS">Grow-Only Multiset</a>
   — 
  <a href="#LWWSet">Last-Write-Wins Multiset</a>
   — 
  <a href="#Vector">Grow-Only Vector</a>
   — 
  <a href="#GPQ">Grow-Only PriorityQueue</a>
   — 
  <a href="#GMAPA">Grow-Only Map (Generic)</a>
   — 
  <a href="#GMAPB">Grow-Only Map (String Focused)</a>
   — 
  <a href="#String">Mutable String</a>
</div>
<hr>



<h3 id="GrowCounter"> Grow Only Counter </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A counter that can only increase upwards. |  `+More memory efficient compared to other counters` `+Better performance compared to other counters` `-Does not support decrease operation` | Keeping track of everyone's ages as age can never go downwards. | Pick the greater value to resolve conflicts |


| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `GCounterSB` | `.updatePayload(uint32_t replicaID, T payload)`, `.updateInternalPayload()`, `.queryId()`, `.queryPayload()`, `.queryPayloadwithID(uint32_t replicaID)`, `.addExternalReplica(std::vector<GCounterMetadata<T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<GCounterSB> handlers)` | `int`, `char`,  `bool` , `double` |
| Metadata | `GCounterMetadata` |   `.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.queryPayload()`, `.updatePayload(T payload)`, `.setPayload(T payload)`    | `int`, `char` ,  `bool`, `double` |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
 `.updatePayload(uint32_t replicaID, T payload) ` | Handler updates metadata by the payload using the ID of the metadata |
`.updateInternalPayload()` | Handler merges all the CRDTs that it contains. Equivalent to doing a localMerge |
 `.queryId()` | Get the id of the handler |
 `.queryPayload()` | Get the total payload for the CRDTs in the handler | 
 `.queryPayloadwithID(uint32_t replicaID)` | Get the payload of the CRDT using the ID of the metadata
 `.addExternalReplica(std::vector<GCounterMetadata<T>> external_replica_metadata)` | Add as many metadatas as you want to the handler. 
 `.updateLocalExternalPayload(std::vector<GCounterSB> handlers)` | Handler fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
 `.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the ID of the metadata
`.queryPayload()` | Get the payload of the metadata
`.updatePayload(T payload)` | Update the payload of of the metadata
`.setPayload(T payload)`  | Set the payload of the metadata

<h4> Example </h4>

```cpp
crdt::state::GCounterSB<uint32_t> handler(1); //Represents Server 1 with id = 1
//Create several replicas all with key = 1 creating conflict
crdt::state::GCounterMetadata<uint32_t> replica1A(1,6);
crdt::state::GCounterMetadata<uint32_t> replica1B(1,15); 
crdt::state::GCounterMetadata<uint32_t> replica1C(1,8);
crdt::state::GCounterMetadata<uint32_t> replica1D(1,2);
handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D});
cout << handler.queryPayload() //Will print 15
/*Replica1A, Replica1B, Replica1C, Replica1D ann have conflicts
All will converge to same state and have final value = 15
*/
```

<h3 id="PNCounter"> Positive Negative Counter </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A counter that can go upwards and downwards |  `+Less memory efficient compared to other counters` `+Worst performance compared to other counters` `-Supports both increase and decrease increments` | User is modifying Reddit Comment thread both on his laptop and phone at the same time | Take the max of the positive counter. Take the max of the negative counter. The payload is the sum of the positive and negative counter. 

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `PNCounterSB` | `.increasePayload(uint32_t replicaID, T payload)`, `.decreasePayload(uint32_t replicaID, T payload)`, `.updateInternalPayload()`, `.serializeObject())`, `.queryId()`, `.queryPayload()`, `.queryPayloadwithID(uint32_t replicaID)`, `.addExternalReplica(std::vector<PNCounterMetadata<T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<PNCounterSB> handlers)` | `int`, `char`,  `bool`, `double` |
| Metadata | `GCounterMetadata` |   `.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.queryPayloadT()`,`.queryPayloadP()`, `.queryPayloadN()`, `.increasePayload(T positivePayload)`, `.decreasePayload(T negativePayload)`, `.setPayloadT(T payload)` , `.setPayloadP(T payload)` , `.setPayloadN(T payload)` | `int`, `char`,  `bool`, `double`  |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.increasePayload(uint32_t replicaID, T payload)` | Increases metadata by the payload using the ID of the metadata |
`.decreasePayload(uint32_t replicaID, T payload)` |  Decreases metadata by the payload using the ID of the metadata |
`.updateInternalPayload()` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`.queryPayload()` | Get the payload of all the metadatas |
`.queryPayloadwithID(uint32_t replicaID)` | Gets the payload of the metadata using ID | 
`.addExternalReplica(std::vector<PNCounterMetadata<T>> external_replica_metadata)` | Add as many metadatas as you want to the handler. 
`.updateLocalExternalPayload(std::vector<PNCounterSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the ID of the metadata |
`.queryPayloadT()` | Get the payload of the metadata |
`.queryPayloadP()` | Get the payload of the positive counter|
`.queryPayloadN()` | Get the payload of the negative counter|
`.increasePayload(T positivePayload)` | Increase the payload of the positive counter |
`.decreasePayload(T negativePayload)` | Increase the payload of the negative counter |
`.setPayloadT(T payload)` | Set the payload of the positive counter |
`.setPayloadN(T payload)` | Set the payload of the negative counter |


<h4> Example </h4>

```cpp
crdt::state::PNCounterSB<uint32_t> handler(1); //Represents Server 1
crdt::state::PNCounterMetadata<uint32_t> replica1A(2,0); //id = 2
crdt::state::PNCounterMetadata<uint32_t> replica1B(3,0); //id = 3
replica1A.increasePayload(6);
replica1A.increasePayload(7);
replica1A.increasePayload(8); 
// Postive Counter -> (7 + 6 + 8) = 21
replica1B.increasePayload(6);
replica1B.increasePayload(3);
replica1B.increasePayload(5); 
// Postive Counter -> (6 + 3 + 5) = 14
replica1A.decreasePayload(6); 
// Negative Counter --> 6, Counter = Positive Counter - Negative Counter --> (21) - (6) = 15
replica1B.decreasePayload(5); 
// Negative Counter --> 5, Counter = Positive Counter - Negative Counter --> (14) - (5) = 9
crdt::state::PNCounterMetadata<uint32_t> replica1C(2,200); 
//Conflict with replica1A because matching id = 2, Postive Counter -> 200
replica1C.decreasePayload(4);
replica1C.decreasePayload(20);
// Negative Counter --> (20 + 4) = 24,  Counter = Positive Counter - Negative Counter --> (200) - (24) = 185
handler.addExternalReplica({replica1A,replica1B,replica1C}); //Adds 3 PN Counter to Server 1
cout << handler.queryPayload() //Sum of all the PN Counters, should equal to 185 
/*
Replica 1A = 21 (Conflict with Replica1C)
Replica 1B = 9
Replica 1C = 176 (Conflict with Replica 1A)
Resolution: replica1A will converge to 1C -->
Replica 1B = 9
Replica 1C = Replica1A = 176 
Sum = 176+9 = 185
*/
```

<h3 id="2P Graph"> Two Phase Two Phase Graph</h3>


| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
Tas Explain |  `Tas Explain` | Tas Explain | Tas Explain |



| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `CounterOB` | Tas Explain | `int`, `char` ,  `bool`, `double` |
| Metadata | `CounterMetadata` | Tas Explain   | `int`, `char` ,  `bool`, `double`  |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
Tas Explain | Tas Explain |


| Supported Operations (Metadata) | Functionality | 
|----------|------------|
Tas Explain | Tas Explain |


<h4> Example </h4>

```cpp
//Tas Explain
```

<h3 id="GSet"> Grow-Only Set</h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A set that only elements can be added to it |   `+Does not allow duplicate elements` `+Elements are sorted from least to greatest` `-Cannot remove an added element` `+Better performance and more memory efficient compared to other sets` | Add elements on the a list | Take the union of the conflicted sets |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `GSetSB` | `.insert(uint32_t replicaID, T value)`, `.insert(uint32_t replicaID, std::vector<T> values)`, `.updateInternalPayload()`, `.compare(GSetSB<T> handler, uint32_t setId)`, `.compare_sets(std::set<T> set1, std::set<T> set2)`, `.queryId() `, `.queryPayloadwithID(uint32_t replicaID)` , `.addExternalReplica(std::vector<GSetMetadata<T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<GSetSB> handlers)`   | `int`, `char` ,  `bool`, `string`, `double`  |
| Metadata | `GSetMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.setPayload(std::set<T> payload)`,`queryPayload()`, `.insert(T value) `, ` .insert(std::vector<T> values) ` | `int`, `char` ,  `bool`, `string`, `double`   |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.insert(uint32_t replicaID, T value)` | Add an element into the metadata by using the ID via the handler |
`.insert(uint32_t replicaID, std::vector<T> values)` |  Add multiple element into the metadata by using the ID via the handler |
`.updateInternalPayload()` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`.compare(GSetSB<T> handler, uint32_t setId)` | Checks if metadata exists in the handler via ID | 
`.compare_sets(std::set<T> set1, std::set<T> set2)` | Compare two sets to see if they are a match |
`.queryId() ` | Gets the payload of the metadata using ID | 
`.queryPayloadwithID(uint32_t replicaID)` | Get the metadata by using the ID | 
`.addExternalReplica(std::vector<GSetMetadata<T>> external_replica_metadata)` | Add as many metadatas into the handler  |
`.updateLocalExternalPayload(std::vector<GSetSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the ID of the metadata |
`.setPayload(std::set<T> payload)` | Set the payload |
`.queryPayload()` | Get all the elements inserted into the set |
`.insert(T value) ` | Insert an element into the set |
`.insert(std::vector<T> values) ` | Insert multiple elements into the set |

<h4> Example </h4>

```cpp
crdt::state::GSetSB<uint32_t> handler1(1); //Represents Server 1
crdt::state::GSetMetadata<uint32_t> replica1A(1,2); //id = 1, set =  {2}
crdt::state::GSetMetadata<uint32_t> replica1B(1,4); //id = 1 --> First Conflict, set =  {4}
crdt::state::GSetMetadata<uint32_t> replica1C(1,6); //id = 1 --> Second Conflict, set = {6}
handler1.addExternalReplica({replica1A,replica1B,replica1C}); //Add to server and resolve Conflict
for (int i: handler1.queryPayload()) cout << i; // Will print (2 4 6)   
```

<h3 id="OrSet"> Or-Set</h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A set allowing to insert and remove elements. Similar to a 2PSet, but the element could be re-inserted after removal. |   `+Does not allow duplicate elements` `+Elements are sorted from least to greatest` `+Can remove elments` `-Worst performance and more memory efficient compared to other sets` | A shopping list or grocery list where user can add or delete item from it | Take the union of the conflicted sets |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `ORSetSB` | `.insert(uint32_t replicaID, T value)`, `.insert(uint32_t replicaID, std::vector<T> values)`, `.fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)`, `.updateInternalPayload()`, `.compare(ORSetSB<T> handler, uint32_t vectorId) `, `.compare_vectors(std::vector<T> vector1, std::vector<T> vector2)`, `.queryId()` , `.queryPayload()`, `.queryORSet()` , `.queryPayloadwithID(uint32_t replicaID)`, `.queryORSetwithID(uint32_t replicaID)` , `.addExternalReplica(std::vector<ORSetMetadata<T>> external_replica_metadata, std::unordered_map<uint32_t,std::vector<T>> removed_items = {})`, `.updateLocalExternalPayload(std::vector<ORSetSB> handlers,  std::unordered_map<uint32_t,std::vector<T>> removed_items = {})`  | `int`, `char` ,  `bool`, `string`, `double`  |
| Metadata | `ORSetMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.setPayload(std::set<T> payload)`,`queryPayload()`, `.queryORSet() `,  `.insert(T value) ` , `.insert(std::vector<T> values)` , `.remove(T value)`  | `int`, `char` ,  `bool`, `string`, `double`   |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.insert(uint32_t replicaID, T value)` | Add an element into the metadata by using the ID via the handler |
`.insert(uint32_t replicaID, std::vector<T> values)` |  Add multiple element into the metadata by using the ID via the handler |
`.fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)` | Helper function to fix conflicts within handler |
`.updateInternalPayload()` | Merge all the CRDTs that it contains. Equivalent to doing a localMerge |
 `.compare(ORSetSB<T> handler, uint32_t vectorId) ` | Check if metadata is found within the handler | 
`.compare_sets(std::set<T> set1, std::set<T> set2)` | Compare two sets to see if they are a match |
`.queryPayload()` | Get the payload of the metadata using ID | 
`.queryORSet()` | Get the payload of all ORSet elements | 
`.queryPayloadwithID(uint32_t replicaID)`| Get the metadata inside the handler via ID  |
`.queryORSetwithID(uint32_t replicaID)` | Get the payload of the ORSet elements via ID |
`.addExternalReplica(std::vector<ORSetMetadata<T>> external_replica_metadata, std::unordered_map<uint32_t,std::vector<T>> removed_items = {})` |  Add as many metadatas into the handler  |
`.updateLocalExternalPayload(std::vector<ORSetSB> handlers,  std::unordered_map<uint32_t,std::vector<T>> removed_items = {})` | Fetch all the other handlers and does a merge. Equivalent of doing merge between multiple servers |
 

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the ID of the metadata |
`.setPayload(std::set<T> payload)` | Set the payload |
`.queryORSet() ` | Get the payload of the ORSet metadata |
`.insert(T value) ` | Insert value into the set |
`.insert(std::vector<T> values)` | Insert multiple values into the set |
`.remove(T value)` | Remove value from the set |

<h4> Example </h4>

```cpp
crdt::state::ORSetSB<uint32_t> handler(1); //Represents Server 1
crdt::state::ORSetMetadata<uint32_t> replica1A(3,{1,2}); //id = 3 (Conflict) -> Set = {1,2}
crdt::state::ORSetMetadata<uint32_t> replica1B(3,{3,4}); //id = 3 (Conflict) -> Set = {3,4}
crdt::state::ORSetMetadata<uint32_t> replica1C(3,{5,6}); //id = 3 (Conflict) -> Set = {5,6}
replica1A.remove(2); //id = 3 (Conflict) -> Set = {1}
replica1B.remove(4); //id = 3 (Conflict) -> Set = {3}
std::vector<uint32_t> removed_items1 = {3,4}; 
handler.addExternalReplica({replica1A,replica1B,replica3C}, {{replica1A.queryId(),removed_items1}});
for (int i: handler.queryORSetwithID(3)) cout << i;  //Converge and flix all conflicts
// Will print (1, 5, 6)
```

<h3 id="2PSet"> Two-Phase Set </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A set allowing to insert and remove elements |   `+Does not allow duplicate elements` `+Elements are sorted from least to greatest` `-Once an element is removed, cannot be added back in again` `+Average performance and more memory efficient compared to other sets` | A MMORPG where the set holds user information. If the user is banned, cannot be added back in again. | Takes the union |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `TwoPSetSB` | `.insert(uint32_t replicaID, T value)`, `.insert(uint32_t replicaID, std::vector<T> values)`, `.updateInternalPayload()`, `queryId()`, `queryPayload() `, `.queryTombstone()`, `.queryTwoPSet()` , `.lookup(T e)`, `.lookupwithID(T e, uint32_t replicaID)` , `.queryTwoPSetwithID(uint32_t replicaID)`, `addExternalReplica(std::vector<TwoPSetMetadata<T>> external_replica_metadata)` , `updateLocalExternalPayload(std::vector<TwoPSetSB> handlers)` | `int`, `char` , `bool`, `string`, `double`  |
| Metadata | `TwoPSetMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.setPayload(std::set<T> payload)`,`.setTombstone(std::set<T> tombstone)` ,`.queryPayload()`,`.queryTombstone()` , `.insert(T value)` , `.insert(std::vector<T> values)` , `.remove(T value)`  | `int`, `char` ,  `bool`, `string`, `double`   |

| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.insert(uint32_t replicaID, T value)` | Add an element into the metadata by using the ID via the handler |
`.insert(uint32_t replicaID, std::vector<T> values)` |  Add multiple element into the metadata by using the ID via the handler |
`.updateInternalPayload()` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`queryId()` | Get the ID of the metadata | 
`.queryPayload()` | Gets all the first-phase sets in handler | 
`.queryTombstone()` | Gets all the second-phase sets in handler | 
`.queryTwoPSet()` | Gets the payload of all the metadatas in handler | 
`.lookupwithID(T e, uint32_t replicaID)` | See if metadata exists via ID |
`.queryTwoPSetwithID(uint32_t replicaID)` | Return specific metadata in the handler |
`addExternalReplica(std::vector<TwoPSetMetadata<T>> external_replica_metadata)` | Add as many metadatas into the handler |
`updateLocalExternalPayload(std::vector<TwoPSetSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers  |


| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()`|  Get the ID of the metadata  |
`.setPayload(std::set<T> payload)`| Insert the first-phase set into the 2P set |
`.setTombstone(std::set<T> tombstone)` | Insert the second-phase set into the 2P set |
`.queryPayload()`, | Return the first-phase set |
`.queryTombstone()` | Return the second-phase set | 
`.insert(T value)` | Insert a element into the set |
`.insert(std::vector<T> values)` | Insert multiple values into the set |
`.remove(T value)` | Remove element from the set |
<h4> Example </h4>

```cpp
crdt::state::TwoPSetSB<uint32_t> handler(1); //Represents Server 1
crdt::state::TwoPSetMetadata<uint32_t> replica1A(3,{1,2,3}); //id = 3 (Conflict) -> Phase1Set = {1,2,3}
crdt::state::TwoPSetMetadata<uint32_t> replica1B(3,{2,3,4}); //id = 3 (Conflict) -> Phase1Set = {2,3,4}
crdt::state::TwoPSetMetadata<uint32_t> replica1C(3,{4,5,6}); //id = 3 (Conflict) -> Phase1Set = {4,5,6}
replica2A.remove(3); //id = 3 (Conflict) -> Phase1Set = {1,2,3} , Phase2Set = {3}
replica2C.remove(4); //id = 3 (Conflict) ->  Phase1Set = {2,3,4}, Phase2Set = {4}
handler.addExternalReplica({replica2A,replica2B,replica2C});
/*
For id = 3 ->
Phase1Set = {1,2,3,4,5,6}
Phase2Set = {3,4}
*/
for (int i: handler.queryTwoPSet()) cout << i; // Will print (1, 2 ,5, 6)
```

<h3 id="GMS"> Grow-Only Multiset </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A multi-set where only elements are allowed to be added |   `+Allow duplicate elements` `-Does not support removing elements` `+Elements are sorted from least to greatest` `+More memory efficient compared to LWW` | To obtain chat log. Each message sent between multiple users is saved in the set. Since no message can be deleted, the uncensored log can be obtained | Takes the union |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `MultiSetSB` | `.fixlocalConflict(std::multiset<T> multisetA, std::multiset<T> multisetB)`, `.insert(uint32_t replicaID, std::vector<T> value)`, `.updateInternalPayload()`,  `queryId() `, `.queryPayload()`, `.queryPayloadwithID(uint32_t replicaID)` , `.addExternalReplica(std::vector<MultiSetMetadata<T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<MultiSetSB> handlers)` | `int`, `char` , `bool`, `string`, `double`  |
| Metadata | `MultiSetMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.insert(T value)` , `.setPayload(std::multiset<T> payload)` , `.queryPayload()`  | `int`, `char` ,  `bool`, `string`, `double`   |

| Supported Operations (Handler) | Functionality | 
|----------|------------|
 `.fixlocalConflict(std::multiset<T> multisetA, std::multiset<T> multisetB)`| Fix Conflict between two multisets |
 `.insert(uint32_t replicaID, std::vector<T> value)` | Insert a value into the metadata given the ID |
 `.updateInternalPayload()` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
 `queryId() ` | Get the ID of the metadata |
 `.queryPayload()` | Return a multiset that is a combined of all the metadata in the handler |
 `.queryPayloadwithID(uint32_t replicaID)` | Return a multiset in the handler given the id | 
 `.addExternalReplica(std::vector<MultiSetMetadata<T>> external_replica_metadata)`| Add as many metadatas into the handler |
 `.updateLocalExternalPayload(std::vector<MultiSetSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the id of the multiset |
`.insert(T value)` | Insert a value inside the multiset |
`.setPayload(std::multiset<T> payload)` | Initialize the multiset |
`.queryPayload()` | Return the multiset |
<h4> Example </h4>

```cpp
crdt::state::MultiSetSB<uint32_t> handler1(1); //Represents Server 1
crdt::state::MultiSetMetadata<uint32_t> replica1A(0,5); //id = 0, multiset = {5}
crdt::state::MultiSetMetadata<uint32_t> replica1B(0); //conflict id = 0, multiset = {}
crdt::state::MultiSetMetadata<uint32_t> replica1C(0); //conflict id = 0, multiset = {}
replica1A.insert({20,25,30}); //id = 0, multiset = {5,20,25,30}
replica1B.insert({30,35}); //id = 0, multiset = {30,35}
replica1C.insert({40,45,30,30}); //id = 0, multiset = {30,30,40,45}
handler1.addExternalReplica({replica1A,replica1B,replica1C});
std::multiset<uint32_t> test1 = {5,20,25,30,30,35,40,45};
for (int i: handler1.queryPayload()) cout << i; 
// Will print (5, 20 ,25 , 30 , 30 , 35, 40, 45}
```

<h3 id="LWWSet"> Last-Write-Wins Multiset </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A multi-set where allowing for insertion and deletion |   `+Allow duplicate elements` `+Support removing elements` `+Elements are sorted from least to greatest` `+More memory efficient compared to LWW` | For the same shopping cart shared between multiple users | The last time stamp wins |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `LWWMultiSetSB` | `.insert(uint32_t replicaID, long long int timestamp, T value) `, `.insert(uint32_t replicaID, long long int timestamp, std::vector<T> value) `, `.remove(uint32_t replicaID, long long int timestamp, T value) `,  `.remove(uint32_t replicaID, long long int timestamp, std::vector<T> value)  `, `.updateInternalPayload()`, `.queryId()` , `.queryLWWMultiSet()`, `.queryLWWMultiSetwithID(uint32_t replicaID)`, `.addExternalReplica(std::vector<LWWMultiSetMetadata<T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<LWWMultiSetSB> handlers)` | `int`, `char` , `bool`, `string`, `double`  |
| Metadata | `LWWMultiSetMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.queryId()` , `.queryTime()` , `.insert(long long int timestamp, T value)`, `.insert(long long int timestamp, std::vector<T> value)`, `.remove(long long int timestamp, T value)`, `.remove(long long int timestamp, std::vector<T> value)`, `.queryPayload()`, `.setPayload(std::multiset<T> payload, long long int timestamp)`, | `int`, `char` ,  `bool`, `string`, `double`   |

| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.insert(uint32_t replicaID, long long int timestamp, T value) ` | Insert element into multiset in handler via ID and timestamp | 
`.insert(uint32_t replicaID, long long int timestamp, std::vector<T> value) ` | Insert multiple elements into multiset in handler via ID and timestamp |
`.remove(uint32_t replicaID, long long int timestamp, T value) ` | Remove element from multiset in handler via ID and timestamp
`.remove(uint32_t replicaID, long long int timestamp, std::vector<T> value)  `| Remove multiple elements from multiset in handler via ID and timestamp |
`.updateInternalPayload()`, `.queryId()` |  Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`.queryLWWMultiSet()`| Returns all the elements of all the multisets within the handler |
`.queryLWWMultiSetwithID(uint32_t replicaID)`| Return a multiset within the handler via ID |
`.addExternalReplica(std::vector<LWWMultiSetMetadata<T>> external_replica_metadata)`| Add as many metadatas into the handler |
`.updateLocalExternalPayload(std::vector<LWWMultiSetSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |


| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the id of the multiset |
`.queryTime()` , `.insert(long long int timestamp, T value)` | Get the time of the last write for multiset |
`.insert(long long int timestamp, std::vector<T> value)` | Insert element into multiset providing the time |
`.remove(long long int timestamp, T value)`| Remove element from multiset proving the time. If time provided is less than the last write time, the remove operations will not remove |
`.remove(long long int timestamp, std::vector<T> value)` |  Remove multiple elements from multiset providing the time. |
`.queryPayload()`| Return the multiset |
`.setPayload(std::multiset<T> payload, long long int timestamp)`| Intialize the multiset |
<h4> Example </h4>

```cpp
crdt::state::LWWMultiSetSB<uint32_t> handler1(1); //Represents Server 1
crdt::state::LWWMultiSetMetadata<uint32_t> replica1A(0); //id = 0, time = 0, multiset = {}
crdt::state::LWWMultiSetMetadata<uint32_t> replica1B(0); //conflict id = 0, time = 0, multiset = {}
crdt::state::LWWMultiSetMetadata<uint32_t> replica1C(0); //conflict id = 0, time = 0, multiset = {}
replica1A.insert(0,{20,25,30}); //id = 0, time = 1, multiset = {20,25,30}
replica1B.insert(1,{30,35}); // id = 0, time = 1, multiset = {30,35}
replica1C.insert(2, {40,45,30,30}); // id = 0, time = 2, multiset = {30,30,40,45}
handler1.addExternalReplica({replica1A,replica1B,replica1C}); //Added 3 replicas to handler and converge to fix conflict
// the last time id = 0 was written was @ time = 2 --> merge to the multiset at time = 2 --> multiset = {30,30,40,45}
handler1.remove(0,3,{30,30}); // id = 0, time = 3, multiset = {40,45}
handler1.remove(0,4,30);  // id = 0, time = 4, multiset = {40,45}
handler1.remove(0,5,{30,30,45}); // id = 0, time = 5, multiset = {40}
handler1.insert(0,6,{30,30}); // id = 0, time = 6, multiset = {30,30,40}
handler1.remove(0,8,30); // id = 0, time = 8, multiset = {30,40}
for (int i: handler1.queryPayload()) cout << i; //Should print (30, 40)
```
<h3 id="Vector"> Grow-Only Vector</h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A array where only elements are allowed to be added |   `+Allow duplicate elements` `-Does not support removing elements` `+Elements are put in the order they come in` | To view the order at which users clock in at work. | First Write Wins |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `VectorSB` | `.push_back(uint32_t replicaID, T value)`, `.push_back(uint32_t replicaID,std::vector<T> v) `, `.fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)`, `.updateInternalPayload()`, `.compare(VectorSB<T> handler, uint32_t vectorId) `,`.compare_vectors(std::vector<T> vector1, std::vector<T> vector2)` , `.compare_vectors(std::vector<T> vector1, std::vector<T> vector2)`, `.queryId()` , `.queryPayload()` , `.queryPayloadwithID(uint32_t replicaID)` , `.addExternalReplica(std::vector<VectorMetadata<T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<VectorSB> handlers)`  | `int`, `char` ,  `bool`, `string`, `double`  |
| Metadata | `VectorMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.setPayload(std::set<T> payload)`,`queryPayload()`, `.push_back(T value) `,  `.push_back(std::vector<T> v)  ` ,  `.clear()`  | `int`, `char` ,  `bool`, `string`, `double`   |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.push_back(uint32_t replicaID, T value)` | Add an element into the metadata by using the ID via the handler | 
`.push_back(uint32_t replicaID,std::vector<T> v) `| Add multiple element into the metadata by using the ID via the handler | 
`.fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)` | Helper function to fix conflicts within handler |
`.updateInternalPayload()`|  Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`.compare(VectorSB<T> handler, uint32_t vectorId) ` | Checks if metadata is found within the handler |
`.compare_vectors(std::vector<T> vector1, std::vector<T> vector2)` | Compare two vectors to see if they are a match |
`.queryId()` | Gets the payload of the metadata using ID |
`.queryPayloadwithID(uint32_t replicaID)` | Get the metadata inside the handler via ID  |
`.addExternalReplica(std::vector<VectorMetadata<T>> external_replica_metadata)` | Add as many metadatas into the handler |
`.updateLocalExternalPayload(std::vector<VectorSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |

 

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the ID of the metadata |
`.setPayload(std::set<T> payload)`| Intialize the vector |
`queryPayload()`| Return the vector |
`.push_back(T value) ` | Insert element into vector |
`.push_back(std::vector<T> v)  ` | Insert multiple elements into the vector |
`.clear()` | Clear the vector |

<h4> Example </h4>

```cpp
crdt::state::VectorSB<uint32_t> handler(1); //Represents Server 1
crdt::state::VectorMetadata<uint32_t> replica1A(3,{1,2,3}); //id = 3, vector = {1,2,3}
crdt::state::VectorMetadata<uint32_t> replica1B(3,{2,3,4,2}); //Conflict, id = 3, vector = {2,3,4,2}
crdt::state::VectorMetadata<uint32_t> replica1C(3,{4,5,6}); //Conflict, id = 3, vector = {4,5,6}
handler.addExternalReplica({replica1A,replica1B,replica1C}); //Insert into handler, merge and fix conflict
for (int i: handler.queryPayload()) cout << i; //Will print(1,2,3,2,4,5,6)
```

<h3 id="GPQ"> Grow-Only PriorityQueue </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A priority_queue where only elements are allowed to be added |   `+Allow duplicate elements` `-Does not support removing elements` `+Elements are sorted from greatest to least` `-Does not support random access` | To determine task priority | Takes the union |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `PriorityQueueSB` | `.fixlocalConflict(std::priority_queue<T> pq1, std::priority_queue<T> pq2)`, `.updateInternalPayload()`,  `queryId() `, `.queryPayload()`, `.queryPayloadwithID(uint32_t replicaID)` ,`.queryPayloadVector()`, `.convertPQtoVector(std::priority_queue<T> replica)`,  `.addExternalReplica(std::vector<PriorityQueueMetadata<T>> external_replica_metadata)`, `updateLocalExternalPayload(std::vector<PriorityQueueSB> handlers)` | `int`, `char` , `bool`, `string`, `double`  |
| Metadata | `PriorityQueueMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.push(T value)` , `.push(std::vector<T> value)`, `.setPayload(std::priority_queue<T> payload)` , `.queryPayload()`  | `int`, `char` ,  `bool`, `string`, `double`   |

| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.fixlocalConflict(std::priority_queue<T> pq1, std::priority_queue<T> pq2)`| Helper function to fix conflicts between two priority_queues |
`.updateInternalPayload()`| Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`.queryId() `| Get the ID of the metadata |
`.queryPayload()`| Return a priority_queue that is a combined of all the metadata in the handler | 
`.queryPayloadwithID(uint32_t replicaID)` | Return a priority_queue in the handler given the id |
`.queryPayloadVector()`| Convert `.queryPayload()` to a vector and return. Useful because priority_queues do not have iterators |  
`.convertPQtoVector(std::priority_queue<T> replica)`| Converts the passed in priority_queue in vector data stucture added an iterator |
`.addExternalReplica(std::vector<PriorityQueueMetadata<T>> external_replica_metadata)`| Add as many metadatas into the handler |
`updateLocalExternalPayload(std::vector<PriorityQueueSB> handlers)`| Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers | 


| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the id of the multiset |
`.push(T value)` | Insert a value inside the priority_queue |
`.push(std::vector<T> value)` | Insert multiple values inside the priority_queue |
`.setPayload(std::priority_queue<T> payload)` | Initialize the priority_queue |
`.queryPayload()` | Return the priority_queue |
<h4> Example </h4>

```cpp
crdt::state::PriorityQueueSB<uint32_t> handler1(1);
crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0,5); //id = 0, priority_queue = {5}
crdt::state::PriorityQueueMetadata<uint32_t> replica1B(0);  //Conflict id = 0, priority_queue = {}
crdt::state::PriorityQueueMetadata<uint32_t> replica1C(0); //Conflict id = 0, priority_queue = {}
replica1A.push(20); //id = 0, priority_queue = {20,5}
replica1A.push(25); //id = 0, priority_queue = {25,20,5}
replica1B.push(30); //id = 0, priority_queue = {30}
replica1B.push(35); //id = 0, priority_queue = {35,30}
replica1C.push(40); //id = 0, priority_queue = {40}
replica1C.push(45); //id = 0, priority_queue = {45,40}
replica1C.push(30); //id = 0, priority_queue = {45,40,30}
replica1C.push(30); //id = 0, priority_queue = {45,40,30,30}
handler1.addExternalReplica({replica1A,replica1B,replica1C}); //Add to handler1, handler1, fix conflict and merge
for (int i: handler1.queryPayloadVector()) cout << i; // Will print (45, 40, 35, 30, 30, 25, 20, 5)
```


<h3 id="GMAPA"> Grow-Only Map (Generic) </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A map where only <key,value> pairs are allowed to be added |   `-Keys have to be unique`  `-Does not support removing elements` `+Keys are sorted from least to greatest` | Photo collaborator application settings | Greater value wins |

| Name | Identifier | Supported Operations | Data types supported | 
|------|------------|----------|------------|
| Handler | `GMapSB` | `.insert(uint32_t replicaID, K key, T value)`, `.updateIncrease(uint32_t replicaID, K key, T value)`,  `fixlocalConflict(K key, T value)`, `.queryPayload()`, `.updateInternalPayload(GMapMetadata<K,T> metadata , bool externalCall = false)` ,`.queryId()`, `.queryPayload(K key)`,  `.queryPayloadwithID(K mapId, K key) `, `queryAllKeys()`, `.queryAllValues()`, `.fixSameKeyConflict(GMapMetadata<K,T>& metadata)`, `.addExternalReplica(std::vector<GMapMetadata<K,T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<GMapSB> handlers)` | `int`, `char` , `bool`, `string`, `double`  |
| Metadata | `GMapMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.queryPayloadValue(K key) ` , `.insert(K key, T value)`, `updateIncrease(K key, T increment)` , `.updateDecrease(K key, T decrement)` , `.queryPayload() const`, `.getKey(T value)`  | `int`, `char` ,  `bool`, `string`, `double` |

| Supported Operations (Handler) | Functionality | 
|----------|------------|
 `.insert(uint32_t replicaID, K key, T value)` | Insert <Key,Value> into metadata via ID |
 `.updateIncrease(uint32_t replicaID, K key, T value)` | Update the metadata <Key,Value> via ID |
 `.fixlocalConflict(K key, T value)`| Helper function used to fix conflicts |
 `.queryPayload()` |  Returns all the maps within the handler |
 `.updateInternalPayload(GMapMetadata<K,T> metadata , bool externalCall = false)` ,`.queryId()` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
 `.queryPayload(K key)` | Returns all the values that matches the key passed into handler |
 `.queryPayloadwithID(K mapId, K key) `| Return a specific map value via ID and key within handler |
 `.queryAllKeys()`| Returns all the keys within the handler |
 `.queryAllValues()` | Returns all the values within the handler |
 `.fixSameKeyConflict(GMapMetadata<K,T>& metadata)`| Main Function to resolve Conflicts | 
 `.addExternalReplica(std::vector<GMapMetadata<K,T>> external_replica_metadata)`| Add as many metadatas into the handler |
 `.updateLocalExternalPayload(std::vector<GMapSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers  |


| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryPayloadValue(K key) ` | Returns the value within the map given key |
`.insert(K key, T value)` | Insert <Key,Value> pair into the map |
`updateIncrease(K key, T increment)` | Increase the key by value of increment |
`.updateDecrease(K key, T decrement)` | Decrease the key by value of decrement |
`.queryPayload() const`| Returns the map |
`.getKey(T value)` | Given the value, return the first key that matches |
<h4> Example </h4>

```cpp
crdt::state::GMapSB<uint32_t, uint32_t> handler(1); //Represents Server 1
crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,1); // id = 0, map = { (10 : 1) }
crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);  //Conflict  id = 0, map = { (10 : 2) }
crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3); //Conflict id = 0, map = { (10 : 3) } 
//Merging Policy for Map --> Greater Value Wins
handler.addExternalReplica({replica1A, replica1B, replica1C }); //Add, fix and merge conflict
for (int i: handler.queryAllKeys()) cout << i; //Will print(10)
for (int i: handler.queryAllValues()) cout << i; //Will print(3)
```

<h3 id="GMAPB"> Grow-Only Map (String Focused) </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A map where only <key,value> pairs are allowed. |   `-Keys have to be unique` `+Keys do not have to be string` `-Value must be string`  `-Does not support other data structures except string` `+Keys are sorted from least to greatest` | Collaberated word document, where key = page number and value is the text on each page | Union of conflict strings |

| Name | Identifier | Supported Operations | Data types supported (Key) | Data types supported (Value) |  
|------|------------|----------|------------| ------------|
| Handler | `GMapSBString` |  `.fixlocalConflict(std::string StringA, std::string StringB, K key, int sysCall)`, `.updateInternalPayload(GMapMetadata<K,T> metadata, bool externalCall = false)`, `.queryId()`,  `.queryAllKeys()`, `.queryAllValues()`, `.queryPayloadwithID(K mapId, K key)` ,  `.fixSameKeyConflict(GMapMetadata<K,T>& metadata)`, `.addExternalReplica(std::vector<GMapMetadata<K,T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<GMapSBString> handlers)` | `int`, `char` , `bool`, `string`, `double`  | `string` |

```cpp
MetaData is the same as Generic Map
```


| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.fixlocalConflict(std::string StringA, std::string StringB, K key, int sysCall)` | Helper function to fix conflcits between strings in metadata |
`.updateInternalPayload(GMapMetadata<K,T> metadata, bool externalCall = false)` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`.queryId()` | Get the id of the handler |
`.queryAllKeys()` | Returns all the keys within the handler |
`.queryAllValues()` | Returns all the values within the handler |
`.queryPayloadwithID(K mapId, K key)` |  Return a specific map value via ID and key within handler |
`.fixSameKeyConflict(GMapMetadata<K,T>& metadata)` | Main Function to resolve Conflicts | 
`.addExternalReplica(std::vector<GMapMetadata<K,T>> external_replica_metadata)` |  Add as many metadatas into the handler |
`.updateLocalExternalPayload(std::vector<GMapSBString> handlers)` |  Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers  |


<h4> Example </h4>

```cpp
crdt::state::GMapSB<uint32_t, uint32_t> handler(1); //Represents Server 1
crdt::state::GMapMetadata<uint32_t, std::string> replica1A(0,10,"Hello"); // id = 0, map = { (10 : "Hello") }
crdt::state::GMapMetadata<uint32_t, std::string> replica1B(0,10,"HelloMelo"); // Conflict  id = 0, map = { (10 : "HelloMelo") }
crdt::state::GMapMetadata<uint32_t, std::string> replica1C(0,10,"Hello Hello"); // Conflict id = 0, map = { (10 : "Hello Hello") } 
//Merging Policy for Map --> Union to resolve conflict
handler.addExternalReplica({replica1A, replica1B, replica1C }); //Add, fix and merge conflict
for (int i: handler.queryAllKeys()) cout << i; //Will print(10)
for (int i: handler.queryAllValues()) cout << i; //Will print("Hello Hello HelloMelo")
```

<h3 id="String"> Mutable String </h3>

| What is it? | Capability | Practical UseCases | Merging Policy | 
|------|------------|----------|----------|
A mutable string where the text can be modified |   `+Support deletion of characters at any position` `+Support insertion of characters at any position` `+Ordering of characters in the string do not change` `-Does not always give the intended merge results that end-user wants` | Any collaborator text editor applications | Resolve conflict based on index of characters |

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `StringOB` | `.fixConflict(std::string conflictA, std::string conflictB)`, `.updateInternalPayload()`,  `queryId() `, `.queryPayload()`, `.queryPayloadwithID(uint32_t replicaID)` ,`.insert(uint32_t replicaID, int pos, std::string payload)`, `.clear(uint32_t replicaID)`, `.erase(uint32_t replicaID, int startIndex, int endIndex)`, `.setStringTo(uint32_t replicaID, std::string payload)` , `.addExternalReplica(std::vector<StringMetaData<T>> external_replica_metadata)` , `.updateLocalExternalPayload(std::vector<StringOB> handlers)` | `string` |
| Metadata | `StringMetaData` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.queryPayload()` , `.setPayload(std::string payload)`, `.insert(int pos, std::string payload)`, `.clear()` , `.erase(int startIndex, int endIndex)`  | `string`    |

| Supported Operations (Handler) | Functionality | 
|----------|------------|
 `.fixConflict(std::string conflictA, std::string conflictB)` | Helper function for handler to fix conflicts between metadata
 `.updateInternalPayload()` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
 `queryId() ` | Get the ID of the metadata | 
`.queryPayload()` | Returns the merged variant of all the strings in the handler |
 `.queryPayloadwithID(uint32_t replicaID)` | Get the string metadata given ID |
 `.insert(uint32_t replicaID, int pos, std::string payload)`| Insert text into the string metadata given position | 
 `.clear(uint32_t replicaID)` | Clear all the strings in the handler |
 `.erase(uint32_t replicaID, int startIndex, int endIndex)` | Erase text from the metadata in the handler given position |
 `.setStringTo(uint32_t replicaID, std::string payload)` | Update the metadata in handler to new string value |
 `.addExternalReplica(std::vector<StringMetaData<T>> external_replica_metadata)` | Add as many metadatas into the handler |
 `.updateLocalExternalPayload(std::vector<StringOB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers | 


| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Serialize the CRDT into a string |
`.serializeFile(std::string pathToFile)`  | Serialize the CRDT into a string and save it into a file |
`.deserialize(std::string s)` | Deserialize a string into CRDT object |
`.deserializeFile(std::string jsonString)` | Deserialize a string from a file into a CRDT object | 
`.queryId()` | Get the id of the String |
`.queryPayload()` | Return the string |
`.setPayload(std::string payload)` | Initialize the string to the intended value |
`.insert(int pos, std::string payload)` | Insert text into the string given position |
`.clear()` | Clear the entire string |
`.erase(int startIndex, int endIndex)` | Erase text in the sting given position |

<h4> Example </h4>

```cpp
crdt::operation::StringOB<std::string> handler1(1);
crdt::operation::StringMetaData<std::string> replica1A(5, "Hello    World"); 
//id = 5, string = "Hello    World"
crdt::operation::StringMetaData<std::string> replica1B(5, "Hello Sun      "); 
// Conflict id = 5, string = "Hello Sun      ")
handler1.addExternalReplica({replica1A,replica1B}); 
//Add to handler1, handler1, fix conflict and merge
cout << handler1.queryPayload(); //Will print ("Hello Sun World")
```





<h2 id="showcase"> 🖼 Showcase - TrelloRDT </h2>

<h2 id="UnitTesting">✅ CI/CD Integration - Catch2 </h2>
For our testing framework and Github workflow we are using Catch2. All our unit tests can be found in the directory:

`/tests/test.cpp`. If you notice any issues, please leave an Issue Request. 
If you want to contribute any new test cases, please add the test case in and create a PR.




<h2 id="Benchmark">🧪 Performance Analysis </h2>
Here is a showcase of our benchmark analysis. Our benchmark criteria was to measure how long it took for each CRDT to perform 100 merge and then divide the total time by 100 to get the average time for 1 merge operation. To better showcase the advantage of C++ has over other framework we decided to compare it with an existing Python libary: https://github.com/anshulahuja98/python3-crdt. Special Credits to Geetesh Gupta and Anshul Ahuja who are the rightful owners of this libary. Please go and check them out as well.

PC Specs for Benchmark:
https://www.userbenchmark.com/UserRun/6945534

| CRDT Type | Average merge Time - C++ (msec) | Average merge Time - Python (msec) |
|------|------------|----------|
| 2P2P-Graph  | 0.00708377 | N/A |
| G-Set | 0.00267279 | 0.00498058 |
| 2P-Set | 0.00435916 | 0.00735843 |
| G-Counter | 0.00150033 | 0.00642128 |
| Or-Set | 0.00748743 | 0.01379353 |
| PN-Counter | 0.00327923 | 0.00755435 |
| Multiset | 0.01003491 | N/A |
| *LWW-Multiset* | 0.00848169 | 0.00523463 |
| *Mutable String* | 0.00277035 | 0.00235195 |
| Vector | 0.00227803 | N/A |
| G-Map (Generic) | 0.00769062 | N/A |
| G-Map (String) | 0.19229 | N/A |
| Priority Queue | 0.00873966 | N/A |


*Lww-Multiset* -> In the python libary a LWW-Set was used while ours is a LWW-Multiset. The difference is a multiset allows duplicate elements while a set does not.

*Mutable String* -> The algorithm for merging was different compared to our one. Furthermore strings in C++ are mutable while in Python they are not. 


<h1 align="center">
  <img src=https://i.ibb.co/kXgDkwJ/Graph.jpg"></a>
</h1>

The raw data for the benchmark can be found under the folder:  ```/benchmark```

<h2 id="Contribute">🤝 Contribute </h2>
Have an data structure in mind such as a binary tree or 2D arrays and want to attempt to make it into a CRDT? No open-source libary
can grow without the help of the community and would love your contributions in helping to add more CRDTs to this libary. To add a new
CRDT to this libary, go to either: 
                   
```/src/operation_based``` 
or `/src/state_based`  and create your CRDT. Remember to add the CRDT in the 
`/src/CrdtObject.hpp`. Once you are done create some unit tests for it under `/tests/test.cpp` to validate your results. 
Then leave a PR and if everything looks good, it will get merged into the repository. 
We greatly apperciate anyone who contributes or leaves feedback. 

<h2 id="license"> 📝 License </h2>
MIT License

Copyright (c) 2022 [Tapasvi Patel, Rushab Roihan, Vishwa Gandhi, Winston Sun]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.



