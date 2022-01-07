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
#ifndef __LWWMULTISETSB_H__
#define __LWWMULTISETSB_H__
#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"

namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT LWW(Last Write Wins) Multiset
*/

template<typename T=int32_t>
class LWWMultiSetMetadata : CrdtMetaData
{
    private:
    uint32_t id;
    long long int currentTime;
    std::map<uint32_t, std::multiset<T>> payload;
    std::map<uint32_t, std::multiset<T>> tombstone; 
    public:
    LWWMultiSetMetadata(uint32_t id) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
        this->currentTime = 0;
    }
    LWWMultiSetMetadata(uint32_t id, T value) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
        this->currentTime = 0;
        this->payload[0].insert(value);

    }
    LWWMultiSetMetadata(uint32_t id, T value, long long int timestamp) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
        this->currentTime = timestamp;
        this->payload[timestamp].insert(value);
    }
    LWWMultiSetMetadata(uint32_t id, std::multiset<T> payload) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
        this->payload[0] = payload;
    }
    LWWMultiSetMetadata(uint32_t id, std::multiset<T> payload, long long int timestamp) : CrdtMetaData(CrdtType::MultiSetSBType)
    {
        this->id = id;
        this->currentTime = timestamp;
        this->payload[timestamp] = payload;
    }
    ~LWWMultiSetMetadata()
    {
        ;
    }
    const uint32_t& queryId() const
    {
        return this->id;
    }
    const long long int& queryTime() const
    {
        return this->currentTime;
    }
    void insert(long long int timestamp, T value)
    {
        if (timestamp > currentTime) 
        {
            this->payload[timestamp] = this->payload[currentTime];
        }
        currentTime = std::max(timestamp,currentTime);
        if (timestamp != currentTime) return;
        this->payload[timestamp].insert(value);
    }
    void insert(long long int timestamp, std::vector<T> value)
    {   
        if (timestamp > currentTime) 
        {
            this->payload[timestamp] = this->payload[currentTime];
        }
        currentTime = std::max(timestamp,currentTime);
        if (timestamp != currentTime) return;
        for (auto &iter:value) {
            this->payload[timestamp].insert(iter);
        }
    }
    void remove(long long int timestamp, T value)
    {

        currentTime = std::max(timestamp,currentTime);
        if (currentTime != timestamp) return;
        this->tombstone[timestamp].insert(value);
        if (this->payload.count(timestamp)) {
            std::multiset<T> updateMS;
            std::unordered_map<T,int> freq;
            for (auto i: this->tombstone[timestamp]) {
                freq[i]++;
            }
            for (auto i: this->payload[timestamp]) {
                if (freq[i] <= 0) {
                    updateMS.insert(i);
                }
                freq[i]--;
            }
            this->payload[timestamp] = updateMS;
        }
    }
    void remove(long long int timestamp, std::vector<T> value)
    {
        currentTime = std::max(timestamp,currentTime);
        if (currentTime != timestamp) return;
        for (auto &iter:value) {
            this->tombstone[timestamp].insert(iter);
        }
        if (this->payload.count(timestamp)) {
            std::multiset<T> updateMS;
            std::unordered_map<T,int> freq;
            for (auto i: this->tombstone[timestamp]) {
                freq[i]++;
            }
            for (auto i: this->payload[timestamp]) {
                if (freq[i] <= 0) {
                    updateMS.insert(i);
                }
                freq[i]--;
            }
            this->payload[timestamp] = updateMS;
        }
    }
    std::multiset<T> queryPayload() {
        // std::cout << "The current time is" << currentTime << std::endl;
        return this->payload[currentTime];
    }
    std::multiset<T> queryTombstone(int flag = -1) {
        // // for (auto i: this->tombstone[currentTime]) {
        // //     std::cout << i << std::endl;
        // // }
        // if (flag == 3) {
        //     std::cout << currentTime << " ";
        //     for (auto [_,i]: this->tombstone) {
        //         for (auto k: i) {
        //             std::cout << k << std::endl;
        //         }
            
        //     }
        // }
        return this->tombstone[currentTime];
    }
    void setPayload(std::multiset<T> payload, uint32_t timestamp)
    {
        this->payload[timestamp] = payload;
    }
    void setTombstone(std::multiset<T> tombstone, uint32_t timestamp)
    {
        this->tombstone[timestamp] = tombstone;
    }
};

/*
* template class for CRDT LWW Mutliset
*/
template<typename T=int32_t>
class LWWMultiSetSB : CrdtObject<T> 
{
private:
    uint32_t id;
    std::multiset<T> payload;
    std::multiset<T> tombstone;
    std::unordered_map<uint32_t,LWWMultiSetMetadata<T>> replica_metadata;
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
    LWWMultiSetSB(uint32_t id)
    {
        this->id = id;
    }
    ~LWWMultiSetSB()
    {
        ;
    }
    bool updateInternalPayload()
    {
        std::multiset<T> curr;
        std::multiset<T> curr_ts;
        typename std::unordered_map<uint32_t,LWWMultiSetMetadata<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryPayload();
            for (auto iter: temp_data)
            {
                curr.insert(iter);
            }
            
            auto temp_data_ts = metadata_it->second.queryTombstone(3);
           // std::cout << "SIZE IS: " << temp_data_ts.size();
            for (auto iter_ts: temp_data_ts)
            {
                std::cout << iter_ts << std::endl;
                curr_ts.insert(iter_ts);
            }
        }
        this->payload = curr;
        this->tombstone = curr_ts;
        return true;
    }
    bool updateExternalPayload()
    {
        typename std::unordered_map<uint32_t,LWWMultiSetMetadata<T>>::iterator metadata_it;
        metadata_it = this->replica_metadata.find(this->id);
        metadata_it->second.payload = this->payload;
        return true;
    }
#ifdef BUILD_TESTING
    const uint32_t& queryId() const
    {
        return this->id;
    }
    std::multiset<T> queryPayload()
    {
        return this->payload;
    }
    std::multiset<T> queryTombstone() 
    {
        return this->tombstone;
    }
    std::multiset<T> queryLWWMultiSet()
    {
        std::multiset<T> queryResult;
        std::unordered_map<T,int> inTombStone;
        for (auto const &e: this->tombstone) {
            inTombStone[e]++;
        }
        // std::cout << "THE SIZE: = " << this->tombstone.size() << " ";
        // for (auto[_,value]: inTombStone) {
        //     std::cout << value << std::endl;
        // }
        for (auto const &e: this->payload) {
            if (inTombStone[e] <= 0)
                queryResult.insert(e);
            inTombStone[e]--;
        }
        return queryResult;
    }
    
   
    std::multiset<T> queryTwoPSetwithID(uint32_t replicaID)
    {
        std::multiset<T> queryPayload, queryResult, queryTombstone;
        auto findMS = replica_metadata.find(replicaID);
        if (findMS == replica_metadata.end()) return queryPayload;
        queryPayload = findMS->second.queryPayload();
        queryTombstone = findMS->second.queryTombstone();
        std::unordered_map<T,int> inTombStone;
        for (auto const &e: queryTombstone) {
            inTombStone[e]++;
        }
        for (auto const &e: queryPayload) {
            if (inTombStone[e] <= 0)
                queryResult.insert(e);
            inTombStone[e]--;
        }
        return queryResult;
    }
    void addExternalReplica(std::vector<LWWMultiSetMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            
            auto search = this->replica_metadata.find(metadata.queryId());
            if (search != this->replica_metadata.end()) {
                std::multiset<T> merged_set;
                std::multiset<T> setA = search->second.queryPayload();
                std::multiset<T> setB = metadata.queryPayload();
                long long int  setATime = search->second.queryTime();
                long long int  setBTime = metadata.queryTime();
                long long int  merged_setTime = std::max(setATime,setBTime);
                //std::set_union(setA.begin(),setA.end(),setB.begin(),setB.end(),std::inserter(merged_set,merged_set.begin()));
                if (setATime == merged_setTime) {
                    metadata.setPayload(setA,merged_setTime);
                } else {
                    metadata.setPayload(setB,merged_setTime);
                }
                std::multiset<T> merged_ts;
                std::multiset<T> tsA = search->second.queryTombstone();
                std::multiset<T> tsB = metadata.queryTombstone();
                if (setATime == merged_setTime) {
                    // std::cout << merged_setTime;
                    metadata.setTombstone(tsA,merged_setTime);
                } else {
                    //  std::cout << "B";
                    metadata.setTombstone(tsB,merged_setTime);
                }
            }
    
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, LWWMultiSetMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }
    void updateLocalExternalPayload(std::vector<LWWMultiSetSB> handlers)
    {
        for (LWWMultiSetSB handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                LWWMultiSetMetadata<T> metadata = iter.second;
                addExternalReplica({metadata});  
            }
        }
    }
#endif
};
}   // namespace state
}   // namespace crdt
#endif // __LWWMULTISETSB_H__
