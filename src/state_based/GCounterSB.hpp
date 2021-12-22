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
#include <unordered_map>
#include <vector>
#include <iterator>


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
    
    void merge(T payload)
    {
        this->payload = max(this->payload, payload);
    }

    const T& queryId() const
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
};

/*
* template class for CRDT Gcounter
*/
template<typename T=int32_t>
class GCounterSB : CrdtObject<T>
{
private:
    uint32_t id;
    T payload;
    std::unordered_map<uint32_t,GCounterMetadata<T>> replica_metadata;

protected:
    bool merge(std::vector<uint32_t> replica_ids)
    {
        return false;
    }
    bool serialize(std::string& buffer)
    {
        return false;
    }

    bool deserialize(std::string& buffer)
    {
        return false;
    }

    bool exportDB()
    {
        return false;
    }

    bool importDB()
    {
        return false;
    }

public:
    GCounterSB(uint32_t id, T payload)
    {
        this->id = id;
        this->payload = payload;
        this->replica_metadata.insert(std::pair<uint32_t, GCounterMetadata<T>>(this->id, GCounterMetadata<T>(this->id, this->payload)));
    }
    
    ~GCounterSB()
    {
        ;
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

#ifdef BUILD_TESTING
    const T& queryId() const
    {
        return this->id;
    }

    const T& queryPayload() const
    {
        return this->payload;
    }

    void setPayLoad(T payload)
    {
        this->payload = payload;
    }

    void addExternalReplica(std::vector<GCounterMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, GCounterMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        
    }
    void updateLocalExternalPayload(std::vector<GCounterSB> handlers)
    {
        T maxPayload = T();
        for (auto handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                auto metadata = iter.second;
                auto replica = this->replica_metadata.insert(std::pair<uint32_t, GCounterMetadata<T>>(metadata.queryId(), metadata));
                if (replica.second)
                {
                    maxPayload += std::max(metadata.queryPayload(),replica.first->second.queryPayload());
                } else
                {
                    maxPayload += metadata.queryPayload();
                }
            }
        }
        setPayLoad(maxPayload);
    }
#endif
};

}   // namespace state
}   // namespace crdt

#endif  // __GCOUNTERSB_H__
