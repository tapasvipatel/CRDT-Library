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
namespace operation
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
    void setPayload(std::string payload) 
    {
        this->payload = payload;
    }
    void insert(int pos, std::string payload)
    {
        if (pos >= this->payload.size())
        {
            this->payload.resize(pos+1);
        }
        this->payload.insert(pos,payload);
    }
    void clear()
    {
        this->payload = "";
    }
    void erase(int startIndex, int endIndex)
    {
        if (startIndex >= this->payload.size() || startIndex < 0) return;
        if (endIndex >= this->pyalod.size() || endIndex < 0) return;
        this->payload.erase(startIndex,endIndex);
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
        if (conflictA.size() > conflictB.size())
            return fixConflict(conflictB,conflictA);
        std::string mergedString = "";
        std::map<std::pair<char,int>,int> m1;
        std::map<std::pair<char,int>,int> m2;
        std::unordered_map<char,int> m3;
        std::unordered_map<char,int> m4;
        for (int i = 0 ; i < conflictA.size(); i++) 
        {
            m1[{conflictA[i],i}]++;
            m3[conflictA[i]]++;
        }
        for (int i = 0 ; i < conflictB.size(); i++)
        {
            m2[{conflictB[i],i}]++;
            m4[conflictB[i]]++;
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
                m2[{c,a}]--;
                m4[c]--;
            }
            else if (c == 255)
            {
                mergedString+=b;
                m1[{b,a}]--;
                m3[b]--;
            }
            else if (b == c)
            {
                mergedString+=b;
                m1[{b,a}]--;
                m2[{b,a}]--;
                m3[b]--;
                m4[b]--;
            }
            else
            {
                if (m1[{c,a}] <= 0 && m2[{c,a}] >= 1) 
                { //found only within the second sting
                    mergedString+=c; 
                    m2[{c,a}]--;
                    m3[c]--;
                    m4[c]--;
                }
                else if (m2[{b,a}] <= 0 && m1[{b,a}] >= 1) 
                { //only found within the first string
                    mergedString+=b;
                    m1[{b,a}]--;
                    m3[b]--;
                    m4[b]--;
                }
            }
        }
        for (auto iter: vis) {
            auto a = std::get<0>(iter);
            auto b = std::get<1>(iter);
            auto c = std::get<2>(iter);
            if (m1[{b,a}] >= 1 && b != c && m3[b] >= 1) {
                m1[{b,a}]--;
                m3[b]--;
                std::string temp = "";
                temp+=b;
                mergedString.insert(a+1,temp);
            } else if (m2[{c,a}] >= 1 && b != c && m4[c] >= 1) {
                m2[{c,a}]--;
                m4[c]--;
                std::string temp = "";
                temp+=c;
                mergedString.insert(a+1,temp);
            }
        }
        while (!mergedString.empty() && mergedString[0] == ' ') mergedString.erase(mergedString.begin());
        while (!mergedString.empty() && mergedString.back() == ' ') mergedString.pop_back();
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
        if (findString == replica_metadata.end()) return "";
        return findString->second.queryPayload();
    }

    void insert(uint32_t replicaID, int pos, std::string payload)
    {
        auto findString = replica_metadata.find(replicaID);
        if (findString == replica_metadata.end()) return;
        findString->second.insert(pos,payload);
        updateInternalPayload();
    }
    void clear(uint32_t replicaID)
    {
        auto findString = replica_metadata.find(replicaID);
        if (findString == replica_metadata.end()) return;
        findString->second.clear();
        updateInternalPayload();
    }
    void erase(uint32_t replicaID, int startIndex, int endIndex)
    {
        auto findString = replica_metadata.find(replicaID);
        if (findString == replica_metadata.end()) return;
        findString->second.erase(startIndex,endIndex);
        updateInternalPayload();
    }
    void setStringTo(uint32_t replicaID, std::string payload)
    {
        auto findString = replica_metadata.find(replicaID);
        if (findString == replica_metadata.end()) return;
        findString->second.setPayload(payload);
        updateInternalPayload();
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


