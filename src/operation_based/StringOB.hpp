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

#ifndef __STRINGOB_H__
#define __STRINGOB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"
using Triplet = std::tuple<int,char,char>;
namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT String
*/
template<typename T=std::string>
class StringMetaData: CrdtMetaData
{
private:
    uint32_t id;
    std::string payload;
public:
    StringMetaData() : CrdtMetaData(CrdtType::StringOBType)
    {
        ;
    }
    StringMetaData(uint32_t id) : CrdtMetaData(CrdtType::StringOBType)
    {
        this->id = id;
        this->payload = "";
    }
    StringMetaData(uint32_t id, std::string payload) : CrdtMetaData(CrdtType::StringOBType)
    {
        this->id = id;
        this->payload = payload;
    }
    ~StringMetaData()
    {
        ;
    }
    const uint32_t& queryId() const
    {
        return this->id;
    }    
    const std::string& queryPayload() const 
    {
        return this->payload;
    }
    void setPayload(std::string payload) {
        this->payload = payload;
    }
};

/*
* template class for CRDT String
*/
template<typename T=std::string>
class StringOB: CrdtObject<T>
{
private:
    uint32_t id;
    std::string payload;
    std::unordered_map<uint32_t, StringMetaData<T>> replica_metadata;
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
    StringOB(uint32_t id) 
    {
        this->id = id;
    }

    std::string fixConflict(std::string conflictA, std::string conflictB) 
    {
        std::string mergedString = "";
        std::unordered_map<char,std::pair<int,int>> m1;
        std::unordered_map<char,std::pair<int,int>> m2;
        for (int i = 0 ; i < conflictA.size(); i++) 
        {
            m1[conflictA[i]].first++;
            m1[conflictA[i]].second = i;
        }
        for (int i = 0 ; i < conflictB.size(); i++)
        {
            m2[conflictB[i]].first++;
            m2[conflictB[i]].second = i;
        }
        int i = 0;
        int j = 0;
        
        std::set<Triplet> vis;
        while (i < conflictA.size() && j < conflictB.size())
        {
            vis.insert({i,conflictA[i],conflictB[j]});
            i++;
            j++;
        }
        while (i < conflictA.size())
        {
            vis.insert({i,conflictA[i],255});
            i++;
        }
        while (j < conflictB.size())
        {
            vis.insert({j,255,conflictB[j]});
            j++;
        }
        for (auto iter: vis) {
            auto a = std::get<0>(iter);
            auto b = std::get<1>(iter);
            auto c = std::get<2>(iter);
            if (b == 255)
            {
                mergedString+=c;
                m2[c].first--;
            }
            else if (c == 255)
            {
                mergedString+=b;
                m1[b].first--;
            }
            else if (b == c)
            {
                mergedString+=b;
                m1[b].first--;
                m2[c].first--;
            }
            else
            {
                if (m1[c].first <= 0 && m2[c].first >= 1)
                {
                    mergedString+=c;
                    m2[c].first--;
                }
                else if (m1[c].first >= 1 && m2[c].first >= 1)
                {
                    mergedString+=c;
                    m1[c].first--;
                    m2[c].first--;
                }
                else if (m2[b].first <= 0 && m1[b].first >= 1)
                {
                    mergedString+=b;
                    m1[b].first--;
                }
                else if (m1[b].first >= 1 && m2[b].first >= 1)
                {
                    mergedString+=b;
                    m1[b].first--;
                    m2[b].first--;
                } else {
                    mergedString+=b;
                }
            }
        }
        for (auto i: m1)
        {
            auto iter = i.second;
            auto freq = iter.first;
            auto index = iter.second;
            if (freq <= 0) continue;
            while(freq--)
            {
                std::string temp = "";
                temp+=i.first;
                mergedString.insert(index+1,temp);
            }
        }
        for (auto i: m2)
        {
            auto iter = i.second;
            auto freq = iter.first;
            auto index = iter.second;
            if (freq <= 0) continue;
            while(freq--)
            {
                std::string temp = "";
                temp+=i.first;
                mergedString.insert(index+1,temp);
            }
        }
        return mergedString;
    }
    bool updateInternalPayload()
    {
        std::string curr;

        typename std::unordered_map<uint32_t,StringMetaData<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            curr += metadata_it->second.queryPayload();
            curr += " ";
        }
        curr.pop_back();
        this->payload = curr;
        return true;
    }

    bool updateExternalPayload()
    {
        typename std::unordered_map<uint32_t,StringMetaData<T>>::iterator metadata_it;
        metadata_it = this->replica_metadata.find(this->id);
        metadata_it->second.payload = this->payload;
        return true;
    }
#ifdef BUILD_TESTING
    
    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::string queryPayload()
    {
        return this->payload;
    }

    std::string queryPayloadwithID(uint32_t replicaID)
    {
        std::string queryResult;
        auto findString = replica_metadata.find(replicaID);
        return findString->second.queryPayload();
    }

    

    void addExternalReplica(std::vector<StringMetaData<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            auto search = this->replica_metadata.find(metadata.queryId());
            if (search != this->replica_metadata.end())
            {
                std::string StringA = search->second.queryPayload();
                std::string StringB = metadata.queryPayload();
                metadata.setPayload(fixConflict(StringA, StringB));
            }
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, StringMetaData<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }
    void updateLocalExternalPayload(std::vector<StringOB> handlers)
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
#endif  
};
} // namespace state
} // namespace crdt
#endif


