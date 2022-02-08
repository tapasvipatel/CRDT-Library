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

#ifndef __CRDT_HANDLE__
#define __CRDT_HANDLE__

#include <vector>
#include <unordered_map>

#include "CrdtObject.hpp"
#include "Server.hpp"
#include "state_based/GCounterSB.hpp"

class CrdtHandle
{
private:
    Server server;
    std::unordered_map<uint32_t, crdt::state::GCounterSB<uint32_t>> global_gcountersb_list;

    //std::unordered_map<uint32_t, std::unordered_map<uint32_t, CrdtObject<uint32_t>>> global_crdt_list;
public:
    CrdtHandle();
    ~CrdtHandle();

    // API
    void initializeCrdtHandle(std::string ip_address, uint32_t port);
    void registerCrdt(crdt::state::GCounterSB<uint32_t>& crdt_object);
    void updateRegisteredCrdt(crdt::state::GCounterSB<uint32_t>& crdt_object);
    void removeRegisteredCrdt(crdt::state::GCounterSB<uint32_t>& crdt_object);
    void printRegisteredCrdtGCounterSB();
    void getExternalUpdates(crdt::state::GCounterSB<uint32_t>& crdt_object);
};

#endif  // __CRDT_HANDLE__
