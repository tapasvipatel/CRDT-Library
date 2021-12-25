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
#include <sstream> 
namespace crdt
{
namespace state
{
/*
* metadata template class for CRDT counter
*/
template<typename K, typename T = std::string>
class GMapMetadata : CrdtMetaData
{
    private:
    uint32_t id;
    std::map<K,T> payload;
    public:
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

template<typename K, typename T = uint32_t>
class GMapSB : CrdtObject<T>
{
    uint32_t id; // Represents the server id

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
    GMapSB(uint32_t id)
    {
        this->id = id;
    }
    ~GMapSB()
    {
        ;
    }
    bool updateInternalPayload(GMapMetadata<K,T> metadata)
    {
        this->payload[metadata.queryId()] = metadata.queryPayload();
        return true;
    }

#ifdef BUILD_TESTING
    const uint32_t& queryId() const
    {
        return this->id;
    }

    T queryPayload(K mapId, K key) 
    {
        T val = T();
        auto metadata_it = this->payload[mapId].find(key);
        if (metadata_it != this->payload[mapId].end())
        {
            return metadata_it->second;
        }
        return val;
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
    
        for (auto handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                auto metadata = iter.second;
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
#endif

};

template<typename K, typename T = std::string>
class GMapSBString : CrdtObject<T>
{
    uint32_t id; // Represents the server id

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
    GMapSBString(uint32_t id)
    {
        this->id = id;
    }
    ~GMapSBString()
    {
        ;
    }
    bool updateInternalPayload(GMapMetadata<K,T> metadata)
    {
        this->payload[metadata.queryId()] = metadata.queryPayload();
        return true;
    }

#ifdef BUILD_TESTING
    const uint32_t& queryId() const
    {
        return this->id;
    }

    T queryPayload(K mapId, K key) 
    {
        T val = T();
        auto metadata_it = this->payload[mapId].find(key);
        if (metadata_it != this->payload[mapId].end())
        {
            return metadata_it->second;
        }
        return val;
    }

   void fixSameKeyConflict(GMapMetadata<K,T>& metadata)
   {
       for (auto &iter: metadata.queryPayload())
       {
           auto key = iter.first;
           auto value = iter.second;
           auto metadata_it = this->payload[metadata.queryId()].find(key);
           if (metadata_it == this->payload[metadata.queryId()].end()) continue;
         
            std::set<std::string> mergeStringSet;
            std::istringstream streamA(metadata_it->second);
            std::istringstream streamB(value);
            std::string mergeString = "";
            while (streamA >> mergeString) mergeStringSet.insert(mergeString);
            while (streamB >> mergeString) mergeStringSet.insert(mergeString);;
            mergeString = "";
            for (std::string curr: mergeStringSet)
            {
                mergeString+=curr + " ";
            }
            mergeString.pop_back();
            metadata_it->second = mergeString;
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
                auto metadata_it = this->payload.find(metadata.queryId());
                if (metadata_it != this->payload.end()) 
                {
                    for (auto &serverPayload: metadata.queryPayload())
                    {
                        auto key = serverPayload.first;
                        auto value = serverPayload.second;
                        auto metadata_update = this->payload[metadata.queryId()].find(key);
                        if (metadata_update == this->payload[metadata.queryId()].end()) {
                            std::set<std::string> mergeStringSet;
                            std::istringstream streamB(value);
                            std::string mergeString = "";
                            while (streamB >> mergeString) mergeStringSet.insert(mergeString);
                            mergeString = "";
                            for (std::string curr: mergeStringSet)
                            {
                                mergeString+=curr + " ";
                            }
                            mergeString.pop_back();
                            this->payload[metadata.queryId()].insert({key,mergeString});
                        } else {
                            std::set<std::string> mergeStringSet;
                            std::istringstream streamA(metadata_update->second);
                            std::istringstream streamB(value);
                            std::string mergeString = "";
                            while (streamA >> mergeString) mergeStringSet.insert(mergeString);
                            while (streamB >> mergeString) mergeStringSet.insert(mergeString);
                            mergeString = "";
                            for (std::string curr: mergeStringSet)
                            {
                                mergeString+=curr + " ";
                            }
                            mergeString.pop_back();
                            metadata_update->second = mergeString;
                        }
                    }
                } else 
                {
                    addExternalReplica({metadata});  
                }
            }
        }
    }
#endif

};


}   // namespace state
}   // namespace crdt

#endif  // __GMAPSB_H__