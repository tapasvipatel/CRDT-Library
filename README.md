<h1 align="center">
  <br>
  <a href="https://github.com/tapasvipatel/CRDT-Library"><img src="https://i.ibb.co/xMSp6wj/Merge-Logo.png"></a>
  
  MergeRDT
</h1>
<h3 align=center> A CRDT libary supporting 12 composite data structures written in C++ framework</h3>

------------------

![](https://img.shields.io/badge/Version-Version%201.0-green) ![](https://img.shields.io/badge/Language-C%2B%2B%2011-brightgreen) ![](https://img.shields.io/badge/Build-Passing-green) ![](https://img.shields.io/badge/Framework-SFML%2C%20TGUI-blue) ![](https://img.shields.io/badge/Pipeline-Catch2-orange) ![](https://img.shields.io/badge/Operating%20System-Ubuntu%2020.04-yellow) 

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
This libary currently supports a combined of 12 CRDTs. We have also created a Trello-clone that utilizes different CRDTs in this libary
for anyone wanted to create an application using CRDTs in C++ enviroment. 


<h2 id="setup"> ⚙ Set Up </h2>
This libary has only been validated on Linux OS, and support for Windows/MAC will be provided at a later time. To set up 

<h2 id="guide"> 🐕‍🦺 Guide for CRDT data types</h2>
Each CRDT class has a handler and a metadata class. On each of your server, you create the handler once. To actually instatiate the CRDT data structure use the metadata. To add
the CRDT to the server, add the metadata to the handler. 

<h3> Grow Only Counter </h3>

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
 `.serialize()`  | Tas Explain |
`.serializeFile(std::string pathToFile)`  | Tas Explain |
`.deserialize(std::string s)` | Tas Explain |
`.deserializeFile(std::string jsonString)` | Tas Explain | 
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

<h3> Positive Negative Counter </h3>

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `PNCounterSB` | `.increasePayload(uint32_t replicaID, T payload)`, `.decreasePayload(uint32_t replicaID, T payload)`, `.updateInternalPayload()`, `.serializeObject())`, `.queryId()`, `.queryPayload()`, `.queryPayloadwithID(uint32_t replicaID)`, `.addExternalReplica(std::vector<PNCounterMetadata<T>> external_replica_metadata)`, `.updateLocalExternalPayload(std::vector<PNCounterSB> handlers)` | `int`, `char`,  `bool`, `double` |
| Metadata | `GCounterMetadata` |   `.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.queryPayloadT()`,`.queryPayloadP()`, `.queryPayloadN()`, `.increasePayload(T positivePayload)`, `.decreasePayload(T negativePayload)`, `.setPayloadT(T payload)` , `.setPayloadP(T payload)` , `.setPayloadN(T payload)` | `int`, `char`,  `bool`, `double`  |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.increasePayload(uint32_t replicaID, T payload)` | Increases metadata by the payload using the ID of the metadata |
`.decreasePayload(uint32_t replicaID, T payload)` |  Decreases metadata by the payload using the ID of the metadata |
`.updateInternalPayload()` | Merges all the CRDTs that it contains. Equivalent to doing a localMerge |
`.serializeObject())` | Tas Explain | 
`.queryPayload()` | Get the payload of all the metadatas |
`.queryPayloadwithID(uint32_t replicaID)` | Gets the payload of the metadata using ID | 
`.addExternalReplica(std::vector<PNCounterMetadata<T>> external_replica_metadata)` | Add as many metadatas as you want to the handler. 
`.updateLocalExternalPayload(std::vector<PNCounterSB> handlers)` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Tas Explain |
`.serializeFile(std::string pathToFile)`  | Tas Explain |
`.deserialize(std::string s)` | Tas Explain |
`.deserializeFile(std::string jsonString)` | Tas Explain | 
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

<h3> Operation Grow Counter</h3>

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

<h3> Grow-Only Set</h3>

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
`.serialize()`  | Tas Explain |
`.serializeFile(std::string pathToFile)`  | Tas Explain |
`.deserialize(std::string s)` | Tas Explain |
`.deserializeFile(std::string jsonString)` | Tas Explain | 
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

<h3> Or Set</h3>

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Handler | `ORSetSB` | `.insert(uint32_t replicaID, T value)`, `.insert(uint32_t replicaID, std::vector<T> values)`, `.fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)`, `.updateInternalPayload()`, `.compare(ORSetSB<T> handler, uint32_t vectorId) `, `.compare_vectors(std::vector<T> vector1, std::vector<T> vector2)`, `.queryId()` , `.queryPayload()`, `.queryORSet()` , `.queryPayloadwithID(uint32_t replicaID)`, `.queryORSetwithID(uint32_t replicaID)` , `.addExternalReplica(std::vector<ORSetMetadata<T>> external_replica_metadata, std::unordered_map<uint32_t,std::vector<T>> removed_items = {})`, `.updateLocalExternalPayload(std::vector<ORSetSB> handlers,  std::unordered_map<uint32_t,std::vector<T>> removed_items = {})`  | `int`, `char` ,  `bool`, `string`, `double`  |
| Metadata | `ORSetMetadata` |`.serialize()`,`.serializeFile(std::string pathToFile)`, `.deserialize(std::string s)`, `.deserializeFile(std::string jsonString)`, `.queryId()`, `.setPayload(std::set<T> payload)`,`queryPayload()`, `.queryORSet() `,  `.insert(T value) ` , `.insert(std::vector<T> values)` , `.remove(T value)`  | `int`, `char` ,  `bool`, `string`, `double`   |


| Supported Operations (Handler) | Functionality | 
|----------|------------|
`.insert(uint32_t replicaID, T value)` | Add an element into the metadata by using the ID via the handler |
`.insert(uint32_t replicaID, std::vector<T> values)` |  Add multiple element into the metadata by using the ID via the handler |
`.fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)` | Helper function to fix conflicts within handler |
 `.compare(ORSetSB<T> handler, uint32_t vectorId) ` | Checks if metadata is found within the handler | 
`.compare_sets(std::set<T> set1, std::set<T> set2)` | Compare two sets to see if they are a match |
`.queryPayload()` | Gets the payload of the metadata using ID | 
`.queryORSet()` | Winston Explain | 
`.queryPayloadwithID(uint32_t replicaID)`| Get the metadata inside the handler via ID  |
`.queryORSetwithID(uint32_t replicaID)` | Winston Explain |
`.addExternalReplica(std::vector<ORSetMetadata<T>> external_replica_metadata, std::unordered_map<uint32_t,std::vector<T>> removed_items = {})` |  Add as many metadatas into the handler  |
`.updateLocalExternalPayload(std::vector<ORSetSB> handlers,  std::unordered_map<uint32_t,std::vector<T>> removed_items = {})` | Fetches all the other handlers and does a merge. Equivalent of doing merge between multiple servers |
 

| Supported Operations (Metadata) | Functionality | 
|----------|------------|
`.serialize()`  | Tas Explain |
`.serializeFile(std::string pathToFile)`  | Tas Explain |
`.deserialize(std::string s)` | Tas Explain |
`.deserializeFile(std::string jsonString)` | Tas Explain | 
`.queryId()` | Get the ID of the metadata |
`.setPayload(std::set<T> payload)` | Set the payload |
`.queryORSet() ` | Winston Explain |
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



<h2 id="showcase"> 🖼 Showcase - TrelloRDT </h2>

<h2 id="UnitTesting">✅ CI/CD Integration - Catch2 </h2>

<h2 id="license"> 📝 License </h2>




