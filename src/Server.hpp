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

#ifndef __SERVER__
#define __SERVER__

#include <string>
#include <unordered_map>
#include <queue>

#include "CrdtObject.hpp"

enum Request
{
    Identity,   // send identity of remote connection
    Insert, // insert new replica into distributed system
    Remove, // remove replica from distributed system
    Get,    // get updates for an existing crdt
    Drop    // drop an existing crdt
}

enum DataType
{
    uint8_t,
    int8_t,
    uint16_t,
    int16_t
    uint32_t,
    int32_t,
    uint64_t,
    int64_t,
    char,
    bool,
    double
}

class ExternalReplicaMetadata
{
private:
    uint32_t external_replica_id;
    queue<CrdtMetadata> previous_queue;
    queue<CrdtMetadata> ready_queue;
public:
    ExternalReplicaMetadata();
    ~ExternalReplicaMetadata();
};

class Server
{
private:
    std::string ip_address;
    uint32_t port;
    unordered_map<string, uint32_t> lookup_table;
    unordered_map<uint32_t, ExternalReplicaMetadata> replica_table;
public:
    Server();
    Server(std::string ip_address, uint32_t port);
    ~Server();
};

#endif  // __SERVER__
