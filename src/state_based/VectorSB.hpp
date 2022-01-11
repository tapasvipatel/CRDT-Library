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

#ifndef __VectorSB_H__
#define __VectorSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"


namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT set
*/
template<typename T=int32_t>
class VectorMetadata : CrdtMetaData
{
private:
    uint32_t id;
    std::vector<T> payload;  // Vector

public:
    VectorMetadata() : CrdtMetaData(CrdtType::VectorSBType)
    {
        ;
    }

    VectorMetadata(uint32_t id) : CrdtMetaData(CrdtType::VectorSBType)
    {
        this->id = id;
    }

    VectorMetadata(uint32_t id, T value) : CrdtMetaData(CrdtType::VectorSBType)
    {
        this->id = id;
        this->payload.push_back(value);
    }

    VectorMetadata(uint32_t id, std::vector<T> value) : CrdtMetaData(CrdtType::VectorSBType)
    {
        this->id = id;
        this->payload = value;
    }

    ~VectorMetadata()
    {
        ;
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    void setPayload(std::vector<T> payload)
    {
        this->payload = payload;
    }

    std::vector<T> queryPayload() 
    {
        return this->payload;
    }

    void push_back(T value) 
    {
        this->payload.push_back(value);
    }

    void push_back(std::vector<T> v) 
    {
        for (auto val: v) this->payload.push_back(val);
    }
};

/*
* template class for CRDT VectorSB
*/
template<typename T=int32_t>
class VectorSB : CrdtObject<T>
{
private:
    uint32_t id; // server id
    std::vector<T> payload; 
    std::unordered_map<uint32_t,VectorMetadata<T>> replica_metadata; // vectors on servers

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
    VectorSB(uint32_t id)
    {
        this->id = id;
    }

    ~VectorSB()
    {
        ;
    }
    void push_back(uint32_t replicaID, T value) 
    {
        auto findVector = replica_metadata.find(replicaID);
        if (findVector == replica_metadata.end()) return;
        findVector->second.push_back(value);
    }

    void push_back(uint32_t replicaID,std::vector<T> v) 
    {
        auto findVector = replica_metadata.find(replicaID);
        if (findVector == replica_metadata.end()) return;
        findVector->second.push_back(v);
    }

    std::vector<T> fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)
    {
        std::vector<T> merged_vector;
        std::merge(vector1.begin(), vector1.end(), vector2.begin(), vector2.end(), std::inserter(merged_vector, merged_vector.begin()));
        std::unordered_map<T,uint32_t> freq, freq2; //We use this for set union
        for (T n1 : vector1) {
            freq[n1]++;
        }
        for (T n2 : vector2) {
            if (freq[n2] != 0) {
                freq[n2]--;
                freq2[n2]++;
                typename std::vector<T>::iterator position = std::find(merged_vector.begin(), merged_vector.end(), n2);
                if (position != merged_vector.end()) merged_vector.erase(position);
            }
        }
        
        return merged_vector;
    }

    bool updateInternalPayload()
    {
        std::vector<T> curr;
        typename std::unordered_map<uint32_t,VectorMetadata<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryPayload();
            std::vector<T> merged_vector;
            std::merge(curr.begin(), curr.end(), temp_data.begin(), temp_data.end(), std::inserter(merged_vector, merged_vector.begin()));
            curr = merged_vector;
        }
        this->payload = curr;
        return true;
    }

    bool compare(VectorSB<T> handler, uint32_t vectorId) {
        return includes(this->replica_metadata[vectorId].queryPayload().begin(), this->replica_metadata[vectorId].queryPayload().end(), handler.replica_metadata[vectorId].queryPayload().begin(), handler.replica_metadata[vectorId].queryPayload().end());
    }

    bool compare_vectors(std::vector<T> vector1, std::vector<T> vector2) {
        return includes(vector1.begin(), vector1.end(), vector2.begin(), vector2.end());
    }

#ifdef BUILD_TESTING
    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::vector<T> queryPayload() 
    {
        return this->payload;
    }

    std::vector<T> queryPayloadwithID(uint32_t replicaID)
    {
        std::vector<T> queryResult;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return queryResult;
        return findMS->second.queryPayload();
    }

    void addExternalReplica(std::vector<VectorMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            
            auto search = this->replica_metadata.find(metadata.queryId());
            if (search != this->replica_metadata.end()) {
                std::vector<T> vectorA = search->second.queryPayload();
                std::vector<T> vectorB = metadata.queryPayload();
                metadata.setPayload(fixLocalConflict(vectorA,vectorB));
            }
            
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, VectorMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }

    void updateLocalExternalPayload(std::vector<VectorSB> handlers)
    {
        for (VectorSB handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                VectorMetadata<T> metadata = iter.second;
                addExternalReplica({metadata});  
            }
        }
    }

#endif
};

}   // namespace state
}   // namespace crdt

#endif  // __VectorSB_H__