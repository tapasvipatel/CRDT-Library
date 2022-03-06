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

#ifndef __GSetSB_H__
#define __GSetSB_H__

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
class GSetMetadata : CrdtMetaData
{
private:
    uint32_t id;
    std::set<T> payload;  // Set

public:
    GSetMetadata() : CrdtMetaData(CrdtType::GSetSBType)
    {
        ;
    }

    GSetMetadata(uint32_t id) : CrdtMetaData(CrdtType::GSetSBType)
    {
        this->id = id;
    }

    GSetMetadata(uint32_t id, T value) : CrdtMetaData(CrdtType::GSetSBType)
    {
        this->id = id;
        payload.insert(value);
    }

    GSetMetadata(uint32_t id, std::set<T> value) : CrdtMetaData(CrdtType::GSetSBType)
    {
        this->id = id;
        payload = value;
    }

    ~GSetMetadata()
    {
        ;
    }

    std::string serialize()
    {
        json j;
        j["id"] = this->id;
        json internal(this->payload);

        j["payload"] = internal;

        return j.dump();
    }

    void serializeFile(std::string pathToFile)
    {
        json j;
        j["id"] = this->id;
        json internal(this->payload);

        j["payload"] = internal;
        std::ofstream o(pathToFile);
        o << j << std::endl;
    }

    void deserializeFile(std::string jsonString)
    {
        std::ifstream i(jsonString);
        json j;
        i >> j;

        this->id = j["id"];

        for(json::iterator it = j["payload"].begin(); it != j["payload"].end(); ++it)
        {
            T value = *it;
            this->payload.insert(value);
        }
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    void setPayload(std::set<T> payload)
    {
        this->payload = payload;
    }

    std::set<T> queryPayload() 
    {
        return this->payload;
    }

    void insert(T value) 
    {
        this->payload.insert(value);
    }
    void insert(std::vector<T> values) 
    {
        for (auto value: values)
            this->payload.insert(value);
    }
};

/*
* template class for CRDT GSetSB
*/
template<typename T=int32_t>
class GSetSB : CrdtObject<T>
{
private:
    uint32_t id; // server id
    std::set<T> payload; 
    std::unordered_map<uint32_t,GSetMetadata<T>> replica_metadata; // sets on servers

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
    GSetSB()
    {
        ;
    }

    GSetSB(uint32_t id)
    {
        this->id = id;
    }

    ~GSetSB()
    {
        ;
    }
    void insert(uint32_t replicaID, T value)
    {
        auto findGSet = replica_metadata.find(replicaID);
        if (findGSet == replica_metadata.end()) return;
        findGSet->second.insert(value);
    }
    void insert(uint32_t replicaID, std::vector<T> values)
    {
        auto findGSet = replica_metadata.find(replicaID);
        if (findGSet == replica_metadata.end()) return;
        for (auto value: values) findGSet->second.insert(value);
    }

    bool updateInternalPayload()
    {
        std::set<T> curr;
        typename std::unordered_map<uint32_t,GSetMetadata<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryPayload();
            for (auto iter: temp_data)
            {
                curr.insert(iter);
            }
        }
        this->payload = curr;
        return true;
    }

    bool compare(GSetSB<T> handler, uint32_t setId) {
        return includes(this->replica_metadata[setId].queryPayload().begin(), this->replica_metadata[setId].queryPayload().end(), handler.replica_metadata[setId].queryPayload().begin(), handler.replica_metadata[setId].queryPayload().end());
    }

    bool compare_sets(std::set<T> set1, std::set<T> set2) {
        return includes(set1.begin(), set1.end(), set2.begin(), set2.end());
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::set<T> queryPayload() 
    {
        return this->payload;
    }

    std::set<T> queryPayloadwithID(uint32_t replicaID)
    {
        std::set<T> queryResult;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return queryResult;
        return findMS->second.queryPayload();
    }

    void addExternalReplica(std::vector<GSetMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            
            auto search = this->replica_metadata.find(metadata.queryId());
            if (search != this->replica_metadata.end()) {
                std::set<T> merged_set;
                std::set<T> setA = search->second.queryPayload();
                std::set<T> setB = metadata.queryPayload();
                std::set_union(setA.begin(),setA.end(),setB.begin(),setB.end(),std::inserter(merged_set,merged_set.begin()));
                metadata.setPayload(merged_set);
            }
            
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, GSetMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }

    void updateLocalExternalPayload(std::vector<GSetSB> handlers)
    {
        for (GSetSB handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                GSetMetadata<T> metadata = iter.second;
                addExternalReplica({metadata});  
            }
        }
    }
};

}   // namespace state
}   // namespace crdt

#endif  // __GSetSB_H__