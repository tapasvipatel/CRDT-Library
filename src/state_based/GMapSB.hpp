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
* metadata template class for CRDT counter
*/
template<typename K = int32_t, typename T=int32_t>
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
    const T& queryId() const
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

template<typename K = int32_t, typename T=int32_t>
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
    const T& queryId() const
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

   

    void addExternalReplica(std::vector<GMapMetadata<K,T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, GMapMetadata<K,T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
            updateInternalPayload(metadata);
        }
    }
    void updateLocalExternalPayload(std::vector<GMapSB> handlers)
    {
    
        for (auto handler: handlers)
        {
            for (auto &iter: handler.replica_metadata)
            {
                ;
            }
        }
       
    }
#endif

};


}   // namespace state
}   // namespace crdt

#endif  // __GMAPSB_H__