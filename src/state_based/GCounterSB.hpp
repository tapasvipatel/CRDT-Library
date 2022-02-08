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

#ifndef __GCOUNTERSB_H__
#define __GCOUNTERSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"

namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT counter
*/
template<typename T=int32_t>
class GCounterMetadata : CrdtMetaData
{
private:
    uint32_t id;
    T payload;

public:
    GCounterMetadata(uint32_t id) : CrdtMetaData(CrdtType::GCounterSBType)
    {
        this->id = id;
        this->payload = 0;
    }

    GCounterMetadata(uint32_t id, T payload) : CrdtMetaData(CrdtType::GCounterSBType)
    {
        this->id = id;
        this->payload = payload;
    }

    ~GCounterMetadata()
    {
        ;
    }
    
    const uint32_t& queryId() const
    {
        return this->id;
    }

    const T& queryPayload() const
    {
        return this->payload;
    }

    void updatePayload(T payload)
    {
        this->payload += payload;
    }

    void setPayload(T payload)
    {
        this->payload = payload;
    }
    
};

/*
* template class for CRDT Gcounter
*/
template<typename T=int32_t>
class GCounterSB : public CrdtObject<T>
{
private:
    uint32_t id;
    std::unordered_map<uint32_t,GCounterMetadata<T>> replica_metadata;

public:
    T payload;

    GCounterSB(uint32_t id)
    {
        this->id = id;
        this->payload = T();
        this->crdt_type = GCounterSBType;
        this->inst_id = id;
    }
    
    ~GCounterSB()
    {
        ;
    }
    
    void updatePayload(uint32_t replicaID, T payload)
    {
        auto findGCounter = replica_metadata.find(replicaID);
        if (findGCounter == replica_metadata.end()) return;
        findGCounter->second.updatePayload(payload);
    }

    bool updateInternalPayload()
    {
        T curr = T();
        typename std::unordered_map<uint32_t,GCounterMetadata<T>>::iterator metadata_it;

        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            curr += metadata_it->second.queryPayload();
        }

        this->payload = curr;
        return true;
    }

    bool updateExternalPayload()
    {
        typename std::unordered_map<uint32_t,GCounterMetadata<T>>::iterator metadata_it;
        metadata_it = this->replica_metadata.find(this->id);
        metadata_it->second.payload = this->payload;
        return true;
    }

    std::string serialize()
    {
        std::string serialized_string = "";

        serialized_string += std::to_string(this->id) + ",";
        serialized_string += std::to_string(this->payload) + ",";
        
        return serialized_string;
    }

    void deserialize(std::string s)
    {
        // split string and get elements
        std::vector<int> elements;
        uint32_t extern_id;
        uint32_t extern_payload;
        
        std::string token;
        size_t pos = 0;
        
        while((pos = s.find(",")) != std::string::npos)
        {
            token = s.substr(0, pos);
            elements.push_back(std::stoi(token));
            s.erase(0, pos + 1);
        }

        extern_id = elements[0];
        extern_payload = elements[1];
    }

#ifdef BUILD_TESTING
    const uint32_t& queryId() const
    {
        return this->id;
    }

    const T& queryPayload() const
    {
        return this->payload;
    }

    T queryPayloadwithID(uint32_t replicaID) 
    {
        T queryResult = T();
        auto findPQ = replica_metadata.find(replicaID);
        if (findPQ == replica_metadata.end()) return queryResult;
        return findPQ->second.queryPayload();
    }

    void addExternalReplica(std::vector<GCounterMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto metadata_it = this->replica_metadata.find(metadata.queryId());
            if (metadata_it != this->replica_metadata.end()) //Found a conflict
            {
               metadata.setPayload(std::max(metadata_it->second.queryPayload(), metadata.queryPayload()));
            } 
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, GCounterMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }
    void updateLocalExternalPayload(std::vector<GCounterSB> handlers)
    {
        
        for (auto handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                auto metadata = iter.second;
                addExternalReplica({metadata});
            }
        }
        
    }
#endif
};

}   // namespace state
}   // namespace crdt

#endif  // __GCOUNTERSB_H__
