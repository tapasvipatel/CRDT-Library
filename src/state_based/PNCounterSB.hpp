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

#ifndef __PNCOUNTERSB_H__
#define __PNCOUNTERSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"

namespace crdt
{
namespace state
{
/*
* metadata template class for CRDT PNCounter
*/
template<typename T=int32_t>
class PNCounterMetadata : CrdtMetaData
{
private:
public:
    uint32_t id;
    T positivePayload;
    T negativePayload;
    T totalPayload;

    PNCounterMetadata() : CrdtMetaData(CrdtType::PNCounterSBType)
    {
        ;
    }
    PNCounterMetadata(uint32_t id) : CrdtMetaData(CrdtType::PNCounterSBType)
    {
        this->id = id;
        this->positivePayload = 0;
        this->negativePayload = 0;
    }
    PNCounterMetadata(uint32_t id, T positivePayload) : CrdtMetaData(CrdtType::PNCounterSBType)
    {
        this->id = id;
        this->positivePayload = positivePayload;
        this->totalPayload = positivePayload;
        this->negativePayload = 0;
    }
    ~PNCounterMetadata()
    {
        ;
    }

    std::string serialize()
    {
        json j;
        j["id"] = this->id;
        j["positivePayload"] = this->positivePayload;
        j["negativePayload"] = this->negativePayload;
        j["totalPayload"] = this->totalPayload;

        return j.dump();
    }

    void serializeFile(std::string pathToFile)
    {
        json j;
        j["id"] = this->id;
        j["positivePayload"] = this->positivePayload;
        j["negativePayload"] = this->negativePayload;
        j["totalPayload"] = this->totalPayload;
        std::ofstream o(pathToFile);
        o << j << std::endl;
    }

    void deserializeFile(std::string jsonString)
    {
        std::ifstream i(jsonString);
        json j;
        i >> j;

        this->id = j["id"];
        this->positivePayload = j["positivePayload"];
        this->negativePayload = j["negativePayload"];
        this->totalPayload = j["totalPayload"];
    }
    
    const uint32_t& queryId() const
    {
        return this->id;
    }

    const T& queryPayloadT() const
    {
        return this->totalPayload;
    }

    const T& queryPayloadP() const
    {
        return this->positivePayload;
    }
    const T& queryPayloadN() const
    {
        return this->negativePayload;
    }

    void increasePayload(T positivePayload)
    {
        this->positivePayload += positivePayload;
        this->totalPayload = this->positivePayload - this->negativePayload;
    }

    void decreasePayload(T negativePayload)
    {
        this->negativePayload += negativePayload;
        this->totalPayload = this->positivePayload - this->negativePayload;
    }
    void setPayloadT(T payload)
    {
        this->totalPayload = payload;
    }
    void setPayloadP(T payload)
    {
        this->positivePayload = payload;
        this->totalPayload = this->positivePayload - this->negativePayload;
    }
    void setPayloadN(T payload)
    {
        this->negativePayload = payload;
        this->totalPayload = this->positivePayload - this->negativePayload;
    }

};


/*
* template class for CRDT PNCounter
*/
template<typename T=int32_t>
class PNCounterSB : CrdtObject<T>
{
private:
    T payload;
    std::unordered_map<uint32_t,PNCounterMetadata<T>> replica_metadata;
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
    PNCounterSB()
    {
        ;
    }

    PNCounterSB(uint32_t id)
    {
        this->id = id;
        this->payload = T();
    }
    ~PNCounterSB()
    {
        ;
    }
    void increasePayload(uint32_t replicaID, T payload)
    {
        auto findPNCounter = replica_metadata.find(replicaID);
        if (findPNCounter == replica_metadata.end()) return;
        findPNCounter->second.increasePayload(payload);
    }
    void decreasePayload(uint32_t replicaID, T payload)
    {
        auto findPNCounter = replica_metadata.find(replicaID);
        if (findPNCounter == replica_metadata.end()) return;
        findPNCounter->second.decreasePayload(payload);
    }
    bool updateInternalPayload()
    {
        T curr = T();
        
        typename std::unordered_map<uint32_t,PNCounterMetadata<T>>::iterator metadata_it;

        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            curr += metadata_it->second.queryPayloadT();
        }

        this->payload = curr;
        return true;
    }
    bool updateExternalPayload()
    {
        typename std::unordered_map<uint32_t,PNCounterMetadata<T>>::iterator metadata_it;
        metadata_it = this->replica_metadata.find(this->id);
        metadata_it->second.payload = this->payload;
        return true;
    }
    std::string serializeObject()
    {
        json j;

        for(auto replicaMetadata : this->replica_metadata)
        {
            j[std::to_string(replicaMetadata.first)] = replicaMetadata.second.serialize();
        }

        return j.dump();
    }

    const uint32_t& queryId() const
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

    T queryPayloadwithID(uint32_t replicaID) 
    {
        T queryResult = T();
        auto findPQ = replica_metadata.find(replicaID);
        if (findPQ == replica_metadata.end()) return queryResult;
        return findPQ->second.queryPayloadT();
    }


    void addExternalReplica(std::vector<PNCounterMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto metadata_it = this->replica_metadata.find(metadata.queryId());
            if (metadata_it != this->replica_metadata.end()) //Fixes the Conflict
            {
               auto metadata_it = this->replica_metadata.find(metadata.queryId());
               metadata.setPayloadP(std::max(metadata_it->second.queryPayloadP(), metadata.queryPayloadP()));
               metadata.setPayloadN(std::max(metadata_it->second.queryPayloadN(), metadata.queryPayloadN()));
            } 
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, PNCounterMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }
    void updateLocalExternalPayload(std::vector<PNCounterSB> handlers)
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

#endif  // __PNCOUNTERSB_H__
