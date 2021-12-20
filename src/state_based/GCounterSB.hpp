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

#ifndef __GCOUNTERSB_H__
#define __GCOUNTERSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"
#include <map>
#include <vector>


namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT counter
*/
template<typename K=int32_t, typename V=int32_t>
class GCounterMetadata : CrdtMetaData
{
    private:
    K id;
    public:
    
    GCounterMetadata(K id) : CrdtMetaData(CrdtType::GCounterOBType)
    {
        this->id = id;
    }
    ~GCounterMetadata()
    {
        ;
    }

    // void merge(std::vector<K> replica_ids) 
    // {
    //     for (auto replica_id : replica_ids) {
    //         for (auto j: this->internal_replica_metadata)  {
    //             this->internal_replica_metadata [j.first] = std::max(j.second, 
    //             this->internal_replica_metadata [replica_id]);
    //         }
    //     }
    // }

};

/*
* template class for CRDT Gcounter
*/
template<typename K=int32_t, typename V=int32_t>
class GCounterSB : CrdtObject<K,V>
{
private:
    K id;
    
protected:
    bool merge(std::vector<K> replica_ids)
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

    const V& getNumIncrements() const
    {
        return this->internal_replica_metadata[this->id];
    }

    const V& getTotalNumIncrements() const
    {
        V totalSum = 0;
        for (auto &i: this->internal_replica_metadata) {
            totalSum+=i.second;
        }
        return totalSum;
    }

public:
    std::map<K,V> internal_replica_metadata;
    GCounterSB(K id = 1)
    {
        this->id = id;
    }
    V getTotalNumIncrements()
    {
        V totalSum = 0;
        for (auto &i: internal_replica_metadata) {
            totalSum+=i.second;
        }
        return totalSum;
    }
    bool join(std::vector<GCounterSB<K,V>> replica_ids)
    {
        for (auto replica_id : replica_ids) {
            for (auto replica: replica_id.internal_replica_metadata)  {
                
                internal_replica_metadata[replica.first] = std::max(replica.second, 
                 internal_replica_metadata[replica.first]);
            }
        }
        return true;
    }

    void setNumIncrements(V num_increments)
    {
        this->internal_replica_metadata[this->id]+=num_increments;
    }

    ~GCounterSB()
    {

    }

#ifdef BUILD_TESTING
    const K& query_id() const
    {
        return this->id;
    }

    const V& query_num_increments() const
    {
        return this->getNumIncrements();
    }

    const V& total_query_num_increments() const 
    {
        return this->getTotalNumIncrements();
    }
#endif
};

}   // namespace state
}   // namespace crdt

#endif  // __GCOUNTERSB_H__
