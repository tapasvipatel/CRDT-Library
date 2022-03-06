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

#ifndef __ORSetSB_H__
#define __ORSetSB_H__

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
class ORSetMetadata : CrdtMetaData
{
private:
    uint32_t id;
    std::vector<T> payload;  // Vector

public:
    ORSetMetadata() : CrdtMetaData(CrdtType::ORSetSBType)
    {
        ;
    }

    ORSetMetadata(uint32_t id) : CrdtMetaData(CrdtType::ORSetSBType)
    {
        this->id = id;
    }

    ORSetMetadata(uint32_t id, T value) : CrdtMetaData(CrdtType::ORSetSBType)
    {
        this->id = id;
        payload.push_back(value);
    }

    ORSetMetadata(uint32_t id, std::vector<T> value) : CrdtMetaData(CrdtType::ORSetSBType)
    {
        this->id = id;
        payload = value;
    }

    ~ORSetMetadata()
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
            int32_t value = *it;
            this->payload.push_back(value);
        }
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

    std::set<T> queryORSet() 
    {
        std::set<T> ORSet(this->payload.begin(), this->payload.end());
        return ORSet;
    }

    void insert(T value) 
    {
        this->payload.push_back(value);
    }
    void insert(std::vector<T> values)
    {
        for (auto value: values)
        {
            this->payload.push_back(value);
        }
    }

    void remove(T value)
    {
        typename std::vector<T>::iterator position = std::find(this->payload.begin(), this->payload.end(), value);
        if (position != this->payload.end()) this->payload.erase(position);
    }
};

/*
* template class for CRDT ORSetSB
*/
template<typename T=int32_t>
class ORSetSB : CrdtObject<T>
{
private:
    uint32_t id; // server id
    std::vector<T> payload; 
    std::map<uint32_t,ORSetMetadata<T>> replica_metadata; // vectors on servers

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
    ORSetSB(uint32_t id)
    {
        this->id = id;
    }

    ~ORSetSB()
    {
        ;
    }
    void insert(uint32_t replicaID, T value) 
    {
        auto findOrSet = replica_metadata.find(replicaID);
        if (findOrSet  == replica_metadata.end()) return;
        findOrSet->second.insert(value);
        updateInternalPayload();
    }
    void insert(uint32_t replicaID, std::vector<T> values) 
    {
        auto findOrSet = replica_metadata.find(replicaID);
        if (findOrSet  == replica_metadata.end()) return;
        findOrSet->second.insert(values);
        updateInternalPayload();
    }

    std::vector<T> fixLocalConflict(std::vector<T> vector1, std::vector<T> vector2)
    {
        std::vector<T> fixConflict;
        std::unordered_map<T,uint32_t> freq; //We use this for set union
        for (T n1 : vector1) {
            freq[n1]++;
            fixConflict.push_back(n1);
        }
        for (T n2 : vector2) {
            if (freq[n2] == 0) {
                fixConflict.push_back(n2);
            }
            else {
                freq[n2]--;
            }
        }
        sort(fixConflict.begin(), fixConflict.end());
        return fixConflict;
    }

    bool updateInternalPayload()
    {
        std::vector<T> curr;
        typename std::map<uint32_t,ORSetMetadata<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryPayload();
            for (auto iter: temp_data)
            {
                curr.push_back(iter);
            }
        }
        sort(curr.begin(),curr.end());
        this->payload = curr;
        return true;
    }

    bool compare(ORSetSB<T> handler, uint32_t vectorId) {
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

    std::set<T> queryORSet() 
    {
        std::set<T> ORSet(this->payload.begin(), this->payload.end());
        return ORSet;
    }

    std::vector<T> queryPayloadwithID(uint32_t replicaID)
    {
        std::vector<T> queryResult;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return queryResult;
        return findMS->second.queryPayload();
    }

    std::set<T> queryORSetwithID(uint32_t replicaID) 
    {
        std::set<T> queryResult;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return queryResult;
        return findMS->second.queryORSet();
    }

    void addExternalReplica(std::vector<ORSetMetadata<T>> external_replica_metadata, std::unordered_map<uint32_t,std::vector<T>> removed_items = {})
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto search = this->replica_metadata.find(metadata.queryId());
            if (search != this->replica_metadata.end()) {
                std::vector<T> vectorA = search->second.queryPayload();
                auto iter = removed_items.find(metadata.queryId());
                if (iter != removed_items.end()) {
                    for (T value : iter->second) {
                        typename std::vector<T>::iterator position = std::find(vectorA.begin(), vectorA.end(), value);
                        if (position != vectorA.end()) vectorA.erase(position);
                    }
                }
                std::vector<T> vectorB = metadata.queryPayload();
                metadata.setPayload(fixLocalConflict(vectorA,vectorB));
            }
            
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, ORSetMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }

    void updateLocalExternalPayload(std::vector<ORSetSB> handlers,  std::unordered_map<uint32_t,std::vector<T>> removed_items = {})
    {
        for (ORSetSB handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                ORSetMetadata<T> metadata = iter.second;
                auto it = removed_items.find(metadata.queryId());
                if (it != removed_items.end()) {
                    auto temp = metadata.queryPayload();
                    for (T value : it->second) {
                        typename std::vector<T>::iterator position = std::find(temp.begin(), temp.end(), value);
                        if (position != temp.end()) temp.erase(position);
                    }
                    metadata.setPayload(temp);
                }
                addExternalReplica({metadata}, removed_items);  
            }
        }
    }

#endif
};

}   // namespace state
}   // namespace crdt

#endif  // __ORSetSB_H__