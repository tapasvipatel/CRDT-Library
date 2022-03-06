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

#ifndef __TwoPSetSB_H__
#define __TwoPSetSB_H__

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
* metadata template class for CRDT set
*/
template<typename T=int32_t>
class TwoPSetMetadata : CrdtMetaData
{
private:
    std::set<T> payload;  // Set
    std::set<T> tombstone;  // Removed Set

public:
    uint32_t id;
    TwoPSetMetadata() : CrdtMetaData(CrdtType::TwoPSetSBType)
    {
        ;
    }

    TwoPSetMetadata(uint32_t id) : CrdtMetaData(CrdtType::TwoPSetSBType)
    {
        this->id = id;
    }

    TwoPSetMetadata(uint32_t id, T value) : CrdtMetaData(CrdtType::TwoPSetSBType)
    {
        this->id = id;
        payload.insert(value);
    }

    TwoPSetMetadata(uint32_t id, std::set<T> value) : CrdtMetaData(CrdtType::TwoPSetSBType)
    {
        this->id = id;
        payload = value;
    }

    ~TwoPSetMetadata()
    {
        ;
    }

    std::string serialize()
    {
        json j;
        j["id"] = this->id;
        json internalPayload(this->payload);
        j["payload"] = internalPayload;
        json internalTombstone(this->tombstone);
        j["tombstome"] = internalTombstone;

        return j.dump();
    }

    void serializeFile(std::string pathToFile)
    {
        json j;
        j["id"] = this->id;
        json internalPayload(this->payload);
        j["payload"] = internalPayload;
        json internalTombstone(this->tombstone);
        j["tombstone"] = internalTombstone;
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

        for(json::iterator it = j["tombstone"].begin(); it != j["tombstone"].end(); ++it)
        {
            T value = *it;
            this->tombstone.insert(value);
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

    void setTombstone(std::set<T> tombstone)
    {
        this->tombstone = tombstone;
    }

    std::set<T> queryPayload() 
    {
        return this->payload;
    }

    std::set<T> queryTombstone() 
    {
        return this->tombstone;
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

    void remove(T value) 
    {
        this->tombstone.insert(value);
    }
};

/*
* template class for CRDT TwoPSetSB
*/
template<typename T=int32_t>
class TwoPSetSB : CrdtObject<T>
{
private:
    std::set<T> payload;
    std::set<T> tombstone; 
    std::unordered_map<uint32_t,TwoPSetMetadata<T>> replica_metadata; // sets on servers

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
    uint32_t id; // server id
    TwoPSetSB()
    {
        ;
    }

    TwoPSetSB(uint32_t id)
    {
        this->id = id;
    }

    ~TwoPSetSB()
    {
        ;
    }

    void insert(uint32_t replicaID, T value) 
    {
        auto findTwoPSet = replica_metadata.find(replicaID);
        if (findTwoPSet == replica_metadata.end()) return;
        findTwoPSet->second.insert(payload);
    }
    void insert(uint32_t replicaID, std::vector<T> values) 
    {
        auto findTwoPSet = replica_metadata.find(replicaID);
        if (findTwoPSet == replica_metadata.end()) return;
        for (auto value: values)
            this->payload.insert(value);
    }



    bool updateInternalPayload()
    {
        std::set<T> curr;
        std::set<T> curr_ts;
        typename std::unordered_map<uint32_t,TwoPSetMetadata<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryPayload();
            for (auto iter: temp_data)
            {
                curr.insert(iter);
            }
            auto temp_data_ts = metadata_it->second.queryTombstone();
            for (auto iter_ts: temp_data_ts)
            {
                curr_ts.insert(iter_ts);
            }
        }
        this->payload = curr;
        this->tombstone = curr_ts;
        return true;
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::set<T> queryPayload() 
    {
        return this->payload;
    }

    std::set<T> queryTombstone() 
    {
        return this->tombstone;
    }

    std::set<T> queryTwoPSet()
    {
        std::set<T> queryResult;
        for (auto const &e: this->payload) {
            if (lookup(e)) {
                queryResult.insert(e);
            }
        }
        return queryResult;
    }

    bool lookup(T e) {
        if (this->payload.count(e) && !this->tombstone.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }

    bool lookupwithID(T e, uint32_t replicaID) {
        std::set<T> queryPayload, queryTombstone;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return false;
        queryPayload = findMS->second.queryPayload();
        queryTombstone = findMS->second.queryTombstone();
        if (queryPayload.count(e) && !queryTombstone.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }

    std::set<T> queryTwoPSetwithID(uint32_t replicaID)
    {
        std::set<T> queryPayload, queryResult;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return queryPayload;
        queryPayload = findMS->second.queryPayload();
        for (auto const &e: queryPayload) {
            if (lookupwithID(e, replicaID)) {
                queryResult.insert(e);
            }
        }
        return queryResult;
    }

    void addExternalReplica(std::vector<TwoPSetMetadata<T>> external_replica_metadata)
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

                std::set<T> merged_ts;
                std::set<T> tsA = search->second.queryTombstone();
                std::set<T> tsB = metadata.queryTombstone();
                std::set_union(tsA.begin(),tsA.end(),tsB.begin(),tsB.end(),std::inserter(merged_ts,merged_ts.begin()));
                metadata.setTombstone(merged_ts);
            }
            
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, TwoPSetMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }

    void updateLocalExternalPayload(std::vector<TwoPSetSB> handlers)
    {
        for (TwoPSetSB handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                TwoPSetMetadata<T> metadata = iter.second;
                addExternalReplica({metadata});  
            }
        }
    }
};

}   // namespace state
}   // namespace crdt

#endif  // __TwoPSetSB_H__