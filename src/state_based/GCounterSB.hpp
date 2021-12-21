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
template<typename T=int32_t>
class GCounterMetadata : CrdtMetaData
{
    private:
    uint32_t id;
    T num_increments;
    public:
    
    GCounterMetadata(uint32_t id = 1) : CrdtMetaData(CrdtType::GCounterSBType)
    {
        this->id = id;
        this->num_increments = 0;
    }
    GCounterMetadata(uint32_t id, T num_increments) : CrdtMetaData(CrdtType::GCounterSBType)
    {
        this->id = id;
        this->num_increments = num_increments;
    }

    void setReplicaID(uint32_t id, T num_increments = 0)
    {
        this->id = id;
        this->num_increments = num_increments;
    }

    const T& getNumIncrements() const
    {
        return this->num_increments;
    }

    void setNumIncrements(T num_increments)
    {
        this->num_increments += num_increments;
    }
    void localMerge(T newPayload)
    {
        this->num_increments = newPayload;
    }

    ~GCounterMetadata()
    {
        ;
    }
};

/*
* template class for CRDT Gcounter
*/
template<typename T=int32_t>
class GCounterSB : CrdtObject<T>
{
private:
    uint32_t id;
    T num_increments;
protected:
    bool merge(std::vector<T> replica_ids)
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
    std::map<uint32_t,GCounterMetadata<T>> external_replica_metadata;

    //This function is just used for unit testing and testing locally
    //Do not use for merging over the server
    void localMerge() 
    {
        T maxVal;
        for (auto &replica: external_replica_metadata)
        {
            maxVal = std::max(maxVal, replica.second.getNumIncrements());
        }
        for (auto &replica: external_replica_metadata)
        {
            replica.second.localMerge(maxVal);
        }

    }

    GCounterSB(uint32_t id = 1)
    {
        
        this->id = id;
        this->num_increments = 0;
    }
    
    ~GCounterSB()
    {

    }

#ifdef BUILD_TESTING
    const T& query_id() const
    {
        return this->id;
    }

    const T& query_num_increments() const
    {
        return this->getNumIncrements();
    }

    const T& total_query_num_increments() const 
    {
        return this->getTotalNumIncrements();
    }
#endif
};

}   // namespace state
}   // namespace crdt

#endif  // __GCOUNTERSB_H__
