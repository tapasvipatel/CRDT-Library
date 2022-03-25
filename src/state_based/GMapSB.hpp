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

#ifndef __GMAPSB_H__
#define __GMAPSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"

namespace crdt
{
namespace state
{
/*
* metadata template class for CRDT GMap
*/
template<typename K, typename T = uint32_t>
class GMapMetadata : CrdtMetaData
{
private:
    uint32_t id;
    std::map<K,T> payload;
public:
    GMapMetadata() : CrdtMetaData(CrdtType::GMapSBType)
    {
        ;
    }

    GMapMetadata(uint32_t id) : CrdtMetaData(CrdtType::GMapSBType)
    {
        this->id = id;
    }
    GMapMetadata(uint32_t id, K key, T value) : CrdtMetaData(CrdtType::GMapSBType)
    {
        this->id = id;
        this->payload[key] = value;
    }
    GMapMetadata(uint32_t id, std::map<K,T> payload)
    {
        this->id = id;
        this->payload = payload;
    }
    ~GMapMetadata()
    {
        ;
    }

    std::string serialize()
    {
        json j;
        j["id"] = this->id;
        json internal;
        
        for(auto value : this->payload)
        {
            internal[std::to_string(value.first)] = std::to_string(value.second);
        }

        j["payload"] = internal.dump();

        return j.dump();
    }

    void serializeFile(std::string pathToFile)
    {
        json j;
        j["id"] = this->id;
        json internal;
        
        for(auto value : this->payload)
        {
            //json internalPayload(value.second);
            internal[std::to_string(value.first)] = value.second;
        }

        j["payload"] = internal.dump();
        std::ofstream o(pathToFile);
        o << j << std::endl;
    }

    void deserialize(std::string s)
    {
        json j = json::parse(s);
        this->id = j["id"];

        std::string payload_string = j["payload"];
        json internal = json::parse(payload_string);

        for(json::iterator it = internal.begin(); it != internal.end(); ++it)
        {
            std::string value = it.value();
            value.erase(remove(value.begin(), value.end(), '"'), value.end());
            this->payload[std::stoi(it.key())] = std::stoi(value);
        }
    }

    void deserializeFile(std::string jsonString)
    {
        std::ifstream i(jsonString);
        json j;
        i >> j;

        this->id = j["id"];

        std::string payload_string = j["payload"];
        json internal = json::parse(payload_string);

        for(json::iterator it = internal.begin(); it != internal.end(); ++it)
        {
            std::string value = it.value();
            value.erase(remove(value.begin(), value.end(), '"'), value.end());
            this->payload[std::stoi(it.key())] = std::stoi(value);
        }
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }
    const T& queryPayloadValue(K key) const
    {
        return this->payload[key];
    }

    void insert(K key, T value) 
    {
        this->payload[key]=value;
    }

    void updateIncrease(K key, T increment)
    {
        this->payload[key]+=increment;
    }

    void updateDecrease(K key, T decrement)
    {
        this->payload[key]-=decrement;
    }

    const std::map<K,T> queryPayload() const
    {
        return this->payload;
    }

    const K& getKey(T value)
    {
        for (auto it: this->payload)
        {
            T _value = it.second;
            if (_value == value)
                return it.first;
        }
    }


};

template<typename K = uint32_t, typename T = uint32_t>
class GMapSB : CrdtObject<T>
{
    uint32_t id; // Represents the server id
    //unnecessary but makes it so much easier to acess the replicas for each GMap in local Server
    //key reprents the id of the replica
    std::unordered_map<uint32_t,std::map<K,T>> payload; 

    std::unordered_map<uint32_t,GMapMetadata<K,T>> replica_metadata;
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
    std::map<K,T> totalPayload;
    GMapSB()
    {
        ;
    }

    GMapSB(uint32_t id)
    {
        this->id = id;
    }
    ~GMapSB()
    {
        ;
    }

    void insert(uint32_t replicaID, K key, T value)
    {
        auto findGMap = replica_metadata.find(replicaID);
        if (findGMap == replica_metadata.end()) return;
        findGMap->second.insert(key,value);
    }
    void updateIncrease(uint32_t replicaID, K key, T value)
    {
        auto findGMap = replica_metadata.find(replicaID);
        if (findGMap == replica_metadata.end()) return;
        findGMap->second.updateIncrease(key,value);
    }

    void fixlocalConflict(K key, T value)
    {
        if (this->totalPayload.find(key) == this->totalPayload.end()) 
        {
            this->totalPayload.insert({key,value});
        } 
        else 
        {
            this->totalPayload[key] = std::max(this->totalPayload[key],value);
        }
    }
    bool updateInternalPayload(GMapMetadata<K,T> metadata , bool externalCall = false)
    {
        if (externalCall == false)
            this->payload[metadata.queryId()] = metadata.queryPayload();
        typename std::unordered_map<uint32_t,GMapMetadata<K,T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto tempData = metadata_it->second.queryPayload();
            for (auto iter: tempData)
            {
                fixlocalConflict(iter.first,iter.second);  
            }
        }
        for (auto iter: metadata.queryPayload())
        {
            fixlocalConflict(iter.first,iter.second);     
        }
        return true;
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    T queryPayload(K key)
    {
        return this->totalPayload[key];
    }

    T queryPayloadwithID(K mapId, K key) 
    {
        T val = T();
        auto metadata_it = this->payload[mapId].find(key);
        if (metadata_it != this->payload[mapId].end())
        {
            return metadata_it->second;
        }
        return val;
    }
    

    std::vector<K> queryAllKeys()
    {
        std::vector<K> queryResults;
        for (auto iter: this->totalPayload)
        {
            auto key = iter.first;
            queryResults.push_back(key);
        }
        return queryResults;
    }

    std::vector<T> queryAllValues()
    {
        std::vector<T> queryResults;
        for (auto iter: this->totalPayload)
        {
            auto value = iter.second;
            queryResults.push_back(value);
        }
        return queryResults;
    }

    std::map<K,T> getTotalPayload()
    {
        return this->totalPayload;
    }

   void fixSameKeyConflict(GMapMetadata<K,T>& metadata)
   {
       for (auto &iter: metadata.queryPayload())
       {
           auto key = iter.first;
           auto value = iter.second;
           auto metadata_it = this->payload[metadata.queryId()].find(key);
           if (metadata_it == this->payload[metadata.queryId()].end()) continue;
           metadata_it->second = std::max(value,metadata_it->second);
           metadata.insert(key,metadata_it->second);
        }
   }
   

    void addExternalReplica(std::vector<GMapMetadata<K,T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, GMapMetadata<K,T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
            fixSameKeyConflict(metadata);
            updateInternalPayload(metadata);
        }
    }
    void updateLocalExternalPayload(std::vector<GMapSB> handlers)
    {
        for (auto& handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                auto metadata = iter.second;
                updateInternalPayload(metadata, true);
                auto metadata_it = this->payload.find(metadata.queryId());
                if (metadata_it != this->payload.end()) 
                {
                    for (auto &serverPayload: metadata.queryPayload())
                    {
                        auto key = serverPayload.first;
                        auto value = serverPayload.second;
                        auto metadata_update = this->payload[metadata.queryId()].find(key);
                        if (metadata_update == this->payload[metadata.queryId()].end()) {
                            this->payload[metadata.queryId()].insert({key,value});
                        } else {
                            metadata_update->second = std::max(value,metadata_update->second);
                        }
                    }
                } else 
                {
                    addExternalReplica({metadata});  
                }
            }
        }
       
    }

};

template<typename K, typename T = std::string>
class GMapSBString : CrdtObject<T>
{
    uint32_t id; // Represents the server id
    std::map<K,T> totalPayload;
    std::unordered_map<uint32_t,std::map<K,T>> payload; 
    std::unordered_map<uint32_t,GMapMetadata<K,T>> replica_metadata;
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
    GMapSBString()
    {
        ;
    }

    GMapSBString(uint32_t id)
    {
        this->id = id;
    }
    ~GMapSBString()
    {
        ;
    }

    void setID(uint32_t id)
    {
        this->id = id;
    }

    std::string fixlocalConflict(std::string StringA, std::string StringB, K key, int sysCall)
    {
        std::multiset<std::string> mergeStringA;
        std::multiset<std::string> mergeStringB;
        std::multiset<std::string> mergeStringAns;
        std::istringstream streamA(StringA);
        std::istringstream streamB(StringB);
        std::string mergeString = "";
        while (streamA >> mergeString) mergeStringA.insert(mergeString);
        while (streamB >> mergeString) mergeStringB.insert(mergeString);
        std::set_union(mergeStringA.begin(),mergeStringA.end(),mergeStringB.begin(),
        mergeStringB.end(),inserter(mergeStringAns,mergeStringAns.begin()));
        mergeString = "";
        for (std::string curr: mergeStringAns)
        {
            mergeString+=curr + " ";
        }
        mergeString.pop_back();
        if (sysCall == 1)
            this->totalPayload[key] = mergeString; 
        return mergeString;
    }

    bool updateInternalPayload(GMapMetadata<K,T> metadata, bool externalCall = false)
    {
        if (externalCall == false)
            this->payload[metadata.queryId()] = metadata.queryPayload();
        typename std::unordered_map<uint32_t,GMapMetadata<K,T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto tempData = metadata_it->second.queryPayload();
            for (auto iter: tempData)
            {
                auto key = iter.first;
                auto value = iter.second;
                if (this->totalPayload.find(key) == this->totalPayload.end()) 
                {
                    this->totalPayload.insert({key,value});
                } 
                else 
                {
                    fixlocalConflict(this->totalPayload[key],value,key,1);
                }
            }
        }
        for (auto iter: metadata.queryPayload())
        {
            auto key = iter.first;
            auto value = iter.second;
            if (this->totalPayload.find(key) == this->totalPayload.end()) {
                this->totalPayload.insert({key,value});
            } else {
                fixlocalConflict(this->totalPayload[key],value,key,1) ;
            }   
        }
        return true;
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::vector<K> queryAllKeys()
    {
        std::vector<K> queryResults;
        for (auto iter: this->totalPayload)
        {
            auto key = iter.first;
            queryResults.push_back(key);
        }
        return queryResults;
    }

    std::vector<T> queryAllValues()
    {
        std::vector<T> queryResults;
        for (auto iter: this->totalPayload)
        {
            auto value = iter.second;
            queryResults.push_back(value);
        }
        return queryResults;
    }


    T queryPayloadwithID(K mapId, K key) 
    {
        T val = T();
        auto metadata_it = this->payload[mapId].find(key);
        if (metadata_it != this->payload[mapId].end())
        {
            return metadata_it->second;
        }
        return val;
    }

    T queryPayload(K key)
    {
        return this->totalPayload[key];
    }

    std::map<K,T> getTotalPayload()
    {
        return this->totalPayload;
    } 


   void fixSameKeyConflict(GMapMetadata<K,T>& metadata)
   {
        for (auto &iter: metadata.queryPayload())
        {
           auto key = iter.first;
           auto value = iter.second;
           auto metadata_it = this->payload[metadata.queryId()].find(key);
           if (metadata_it == this->payload[metadata.queryId()].end()) continue;
           metadata_it->second = fixlocalConflict(metadata_it->second,value,-1,-1);
           metadata.insert(key,metadata_it->second);   
        }
   }
   

    void addExternalReplica(std::vector<GMapMetadata<K,T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, GMapMetadata<K,T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
            fixSameKeyConflict(metadata);
            updateInternalPayload(metadata);
        }
    }
    void updateLocalExternalPayload(std::vector<GMapSBString> handlers)
    {
        for (auto handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                auto metadata = iter.second;
                updateInternalPayload(metadata, true);
                auto metadata_it = this->payload.find(metadata.queryId());
                if (metadata_it != this->payload.end()) 
                {
                    for (auto &serverPayload: metadata.queryPayload())
                    {
                        auto key = serverPayload.first;
                        auto value = serverPayload.second;
                        auto metadata_update = this->payload[metadata.queryId()].find(key);
                        if (metadata_update == this->payload[metadata.queryId()].end()) 
                        {
                            this->payload[metadata.queryId()].insert({key,fixlocalConflict(value,"",-1,-1)});
                        } 
                        else 
                        {
                            metadata_update->second = fixlocalConflict(metadata_update->second,value,-1,-1);
                        }
                    }
                } 
                else 
                {
                    addExternalReplica({metadata});  
                }
            }
        }
    }

};


}   // namespace state
}   // namespace crdt

#endif  // __GMAPSB_H__