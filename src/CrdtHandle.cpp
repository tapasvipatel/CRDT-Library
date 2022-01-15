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

#include <iostream>
#include <vector>
#include <unordered_map>

#include "CrdtHandle.hpp"
#include "state_based/GCounterSB.hpp"

/*******************************************************************************************
CrdtHandle
*******************************************************************************************/
CrdtHandle::CrdtHandle()
{
    this->server = Server();
}

CrdtHandle::~CrdtHandle()
{
    ;
}

/*******************************************************************************************
API
*******************************************************************************************/
void CrdtHandle::initializeCrdtHandle(std::string ip_address, uint32_t port)
{
    this->server.ip_address = ip_address;
    this->server.port = port;
}

void CrdtHandle::registerCrdt(crdt::state::GCounterSB<uint32_t>& crdt_object)
{
    std::unordered_map<uint32_t, crdt::state::GCounterSB<uint32_t>>::iterator global_gcountersb_list_it;
    global_gcountersb_list_it = this->global_gcountersb_list.find(crdt_object.inst_id);

    if(global_gcountersb_list_it == this->global_gcountersb_list.end())
    {
        this->global_gcountersb_list.insert(std::pair<uint32_t, crdt::state::GCounterSB<uint32_t>>(crdt_object.inst_id, crdt_object));
    }
    else
    {
        global_gcountersb_list_it->second = crdt_object;
    }
}

void CrdtHandle::updateRegisteredCrdt(crdt::state::GCounterSB<uint32_t>& crdt_object)
{
    std::unordered_map<uint32_t, crdt::state::GCounterSB<uint32_t>>::iterator global_gcountersb_list_it;
    global_gcountersb_list_it = this->global_gcountersb_list.find(crdt_object.inst_id);

    if(global_gcountersb_list_it == this->global_gcountersb_list.end())
    {
        this->global_gcountersb_list.insert(std::pair<uint32_t, crdt::state::GCounterSB<uint32_t>>(crdt_object.inst_id, crdt_object));
    }
    else
    {
        global_gcountersb_list_it->second = crdt_object;
    }
}

void CrdtHandle::removeRegisteredCrdt(crdt::state::GCounterSB<uint32_t>& crdt_object)
{
    std::unordered_map<uint32_t, crdt::state::GCounterSB<uint32_t>>::iterator global_gcountersb_list_it;
    global_gcountersb_list_it = this->global_gcountersb_list.find(crdt_object.inst_id);

    if(global_gcountersb_list_it != this->global_gcountersb_list.end())
    {
        this->global_gcountersb_list.erase(crdt_object.inst_id);
    }
}

void CrdtHandle::printRegisteredCrdtGCounterSB()
{
    std::cout << std::endl;
    std::cout << "GCOUNTERSB DATA " << std::endl;

    std::unordered_map<uint32_t, crdt::state::GCounterSB<uint32_t>>::iterator global_gcountersb_list_it;

    for(global_gcountersb_list_it = this->global_gcountersb_list.begin(); global_gcountersb_list_it != this->global_gcountersb_list.end(); global_gcountersb_list_it++)
    {
        std::cout << "INST ID: " << global_gcountersb_list_it->first << " INST VALUE: " << global_gcountersb_list_it->second.payload << std::endl;
    }
    std::cout << std::endl;
}
