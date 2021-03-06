/*
    Copyright (c) 2021 [Tapasvi Patel, Rushab Roihan, Vishwa Gandhi, Winston Sun]

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __CRDTOBJECT__
#define __CRDTOBJECT__

#include <string>
#include <chrono>
#include <vector>
#include <set>
#include <algorithm>
#include <sstream> 

#include <ostream>
#include <iostream>
#include <fstream>

#include "json.hpp"
using json = nlohmann::json;

enum CrdtType
{
    CounterOBType,
    GCounterOBType,
    GCounterSBType,
    PNCounterSBType,
    GSetSBType,
    TwoPSetSBType,
    ORSetSBType,
    VectorSBType,
    GMapSBType,
    PriorityQueueSBType,
    MultiSetSBType,
    LWWMultiSetSBType,
    StringOBType,
    TwoPTwoPGraphSBType
};

class CrdtMetaData
{
private:
    CrdtType crdt_type;
public:
    CrdtMetaData(CrdtType crdt_type);
    ~CrdtMetaData();
};

template<typename T>
class CrdtObject
{
private:
    std::chrono::time_point<std::chrono::system_clock> date_last_modified;
protected:
    virtual bool merge(std::vector<uint32_t> replica_ids) = 0;
    virtual bool serialize(std::string& buffer) = 0;
    virtual bool deserialize(std::string& buffer) = 0;
    virtual bool exportDB(std::string file) = 0;
    virtual bool importDB() = 0;
public:    
    CrdtObject()
    {
        this->date_last_modified = std::chrono::system_clock::now();
    }

    ~CrdtObject()
    {
        ;
    }
};

#endif  // __CRDTOBJECT__
