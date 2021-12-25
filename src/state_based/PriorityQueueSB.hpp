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

#ifndef __PRIORITYQUEUESB_H__
#define __PRIORITYQUEUESB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"
#include <queue>

namespace crdt
{
namespace state
{
/*
* metadata template class for CRDT PQ
*/
template<typename T=int32_t>
class PriorityQueueMetadata : CrdtMetaData
{
    private:
    uint32_t id;
    std::priority_queue<T> payload;
    public:
    PriorityQueueMetadata(uint32_t id) : CrdtMetaData(CrdtType::PriorityQueueSBType)
    {
        this->id = id;
    }
    PriorityQueueMetadata(uint32_t id, T value) : CrdtMetaData(CrdtType::PriorityQueueSBType)
    {
        this->id = id;
        this->payload.push(value);
    }
    PriorityQueueMetadata(uint32_t id, std::priority_queue<T> payload) : CrdtMetaData(CrdtType::PriorityQueueSBType)
    {
        this->id = id;
        this->payload = payload;
    }
    ~PriorityQueueMetadata()
    {
        ;
    }
    const uint32_t& queryId() const
    {
        return this->id;
    }
    void push(T value) 
    {
        this->payload.push(value);
    }
    void setPayload(std::priority_queue<T> payload) 
    {
        this->payload = payload;
    }
    std::priority_queue<T> queryPayload()
    {
        return this->payload;
    }
};

/*
* template class for CRDT PQ
*/
template<typename T=int32_t>
class PriorityQueueSB : CrdtObject<T>
{
private:
    uint32_t id;
    std::priority_queue<T> payload;
    std::unordered_map<uint32_t,PriorityQueueMetadata<T>> replica_metadata;
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
    PriorityQueueSB(uint32_t id)
    {
        this->id = id;
    }
    ~PriorityQueueSB()
    {
        ;
    }
    bool updateInternalPayload()
    {
        std::priority_queue<T> curr;
        typename std::unordered_map<uint32_t,PriorityQueueMetadata<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryPayload();
            while (!temp_data.empty())
            {
                curr.push(temp_data.top());
                temp_data.pop();
            }
        }
        this->payload = curr;
        return true;
    }
    bool updateExternalPayload()
    {
        typename std::unordered_map<uint32_t,PriorityQueueMetadata<T>>::iterator metadata_it;
        metadata_it = this->replica_metadata.find(this->id);
        metadata_it->second.payload = this->payload;
        return true;
    }
#ifdef BUILD_TESTING
    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::priority_queue<T> queryPayload() 
    {
        return this->payload;
    }
    
    void addExternalReplica(std::vector<PriorityQueueMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto metadata_it = this->replica_metadata.find(metadata.queryId());
            if (metadata_it != this->replica_metadata.end())
            {
               std::priority_queue<T> fixConflict;
               auto pq1 = metadata_it->second.queryPayload();
               auto pq2 = metadata.queryPayload(); 
               while (!pq1.empty())
               {
                   fixConflict.push(pq1.top());
                   pq1.pop();
               }
               while (!pq2.empty())
               {
                   fixConflict.push(pq2.top());
                   pq2.pop();
               }
               metadata.setPayload(fixConflict);
            } 
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, PriorityQueueMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }
    void updateLocalExternalPayload(std::vector<PriorityQueueSB> handlers)
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
#endif // __PRIORITYQUEUESB_H__