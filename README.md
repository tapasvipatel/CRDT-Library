<h1 align="center">
  <br>
  <a href="https://github.com/tapasvipatel/CRDT-Library"><img src="https://i.ibb.co/xMSp6wj/Merge-Logo.png"></a>
  
  MergeRDT
</h1>
<h3 align=center> A CRDT libary supporting 12 composite data structures written in C++ framework</h3>

------------------

![](https://img.shields.io/badge/Version-Version%201.0-green) ![](https://img.shields.io/badge/Language-C%2B%2B%2011-brightgreen) ![](https://img.shields.io/badge/Building-Passing-green) ![](https://img.shields.io/badge/Framework-SFML%2C%20TGUI-blue) ![](https://img.shields.io/badge/Pipeline-Catch2-orange) ![](https://img.shields.io/badge/Operating%20System-Ubuntu%2020.04-yellow) 

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
This libary currently supports a combined of 12 CRDTs.

<h3> Counters </h3>

| Name | Identifier | Supported Operations | Data types supported |
|------|------------|----------|------------|
| Grow Only Counter| `gcounter` | `.inc()` | int |
| Positive-Negative-Counter | `pncounter` |   `.inc()`,`.dec()` | int |
| State-Based Counter | `scounter` |   `.inc()`,`.dec()` | int |

<h2 id="setup"> ⚙ Set Up </h2>

<h2 id="guide"> 🐕‍🦺 Guide for CRDT data types</h2>

<h2 id="showcase"> 🖼 Showcase - TrelloRDT </h2>

<h2 id="UnitTesting">✅ CI/CD Integration - Catch2 </h2>

<h2 id="license"> 📝 License </h2>




