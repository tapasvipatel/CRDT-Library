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

#ifndef __MULTISETSB_H__
#define __MULTISETSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"


namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT Multiset
*/
template<typename T=int32_t>
class MultiSetMetadata : CrdtMetaData
{
private:
    std::multiset<T> payload;
public:
    uint32_t id;
    MultiSetMetadata() : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        ;
    }

    MultiSetMetadata(uint32_t id) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
    }
    MultiSetMetadata(uint32_t id, T value) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
        this->payload.insert(value);
    }
    MultiSetMetadata(uint32_t id, std::multiset<T> payload) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
        this->payload = payload;
    }
    ~MultiSetMetadata()
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

    void deserialize(std::string s)
    {
        json j = json::parse(s);
        this->id = j["id"];

        for(json::iterator it = j["payload"].begin(); it != j["payload"].end(); ++it)
        {
            T value = *it;
            this->payload.insert(value);
        }
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
    void insert(T value)
    {
        this->payload.insert(value);
    }
    void insert(std::vector<T> value)
    {
        for (auto iter:value)
        {
            this->payload.insert(iter);
        }
    }
    void setPayload(std::multiset<T> payload)
    {
        this->payload = payload;
    }
    std::multiset<T> queryPayload()
    {
        return this->payload;
    }
};

/*
* template class for CRDT Mutliset
*/
template<typename T=int32_t>
class MultiSetSB : CrdtObject<T> 
{
private:
    std::multiset<T> payload;
    std::unordered_map<uint32_t,MultiSetMetadata<T>> replica_metadata;
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
    uint32_t id;
    MultiSetSB()
    {
        ;
    }

    MultiSetSB(uint32_t id)
    {
        this->id = id;
    }
    ~MultiSetSB()
    {
        ;
    }
    std::multiset<T> fixlocalConflict(std::multiset<T> multisetA, std::multiset<T> multisetB)
    {
        std::multiset<T> fixConflict;
        std::set_union(multisetA.begin(),multisetA.end(),multisetB.begin(),multisetB.end(),std::inserter(fixConflict,fixConflict.begin()));
        return fixConflict;
    }
    void insert(uint32_t replicaID, T value) 
    {
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return;
        findMS->second.insert(value);
        updateInternalPayload();
    }
    void insert(uint32_t replicaID, std::vector<T> value) 
    {
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return;
        findMS->second.insert(value);
        updateInternalPayload();
    }

    bool updateInternalPayload()
    {
        std::multiset<T> curr;
        typename std::unordered_map<uint32_t,MultiSetMetadata<T>>::iterator metadata_it;
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
     bool updateExternalPayload()
    {
        typename std::unordered_map<uint32_t,MultiSetMetadata<T>>::iterator metadata_it;
        metadata_it = this->replica_metadata.find(this->id);
        metadata_it->second.payload = this->payload;
        return true;
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::multiset<T> queryPayload()
    {
        return this->payload;
    }

    std::multiset<T> queryPayloadwithID(uint32_t replicaID)
    {
        std::multiset<T> queryResult;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return queryResult;
        return findMS->second.queryPayload();
    }
    void addExternalReplica(std::vector<MultiSetMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto metadata_it = this->replica_metadata.find(metadata.queryId());
            if (metadata_it != this->replica_metadata.end())
            {
               metadata.setPayload(fixlocalConflict(metadata_it->second.queryPayload(), metadata.queryPayload()));
            } 
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, MultiSetMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }
    void updateLocalExternalPayload(std::vector<MultiSetSB> handlers)
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
};


}   // namespace state
}   // namespace crdt
#endif // __MULTISETSB_H__