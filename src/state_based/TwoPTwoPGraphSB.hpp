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

#ifndef __TwoPTwoPGraphSB_H__
#define __TwoPTwoPGraphSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"
#include <unordered_map>
#include <vector>
#include <iterator>


namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT set
*/
template<typename T=int32_t>
class TwoPTwoPGraphMetadata : CrdtMetaData
{
private:
    std::set<T> vertices;
    std::set<T> vertices_tombstone;
    std::set<std::pair<T, T>> edges;
    std::set<std::pair<T, T>> edges_tombstone;

public:
    uint32_t id;
    TwoPTwoPGraphMetadata() : CrdtMetaData(CrdtType::TwoPTwoPGraphSBType)
    {
        ;
    }

    TwoPTwoPGraphMetadata(uint32_t id) : CrdtMetaData(CrdtType::TwoPTwoPGraphSBType)
    {
        this->id = id;
    }

    ~TwoPTwoPGraphMetadata()
    {
        ;
    }

    std::string serialize()
    {
        json j;
        j["id"] = this->id;
        json internalVertices(this->vertices);
        j["vertices"] = internalVertices;
        json internalVerticesTombstone(this->vertices_tombstone);
        j["vertices_tombstone"] = internalVerticesTombstone;

        json internalEdges(this->edges);
        j["edges"] = internalEdges;
        json internalEdgesTombstone(this->edges_tombstone);
        j["edges_tombstone"] = internalEdgesTombstone;

        return j.dump();
    }

    void serializeFile(std::string pathToFile)
    {
        json j;
        j["id"] = this->id;
        json internalVertices(this->vertices);
        j["vertices"] = internalVertices;
        json internalVerticesTombstone(this->vertices_tombstone);
        j["vertices_tombstone"] = internalVerticesTombstone;

        json internalEdges(this->edges);
        j["edges"] = internalEdges;
        json internalEdgesTombstone(this->edges_tombstone);
        j["edges_tombstone"] = internalEdgesTombstone;

        std::ofstream o(pathToFile);
        o << j << std::endl;
    }

    void deserialize(std::string s)
    {
        json j = json::parse(s);
        this->id = j["id"];

        for(json::iterator it = j["vertices"].begin(); it != j["vertices"].end(); ++it)
        {
            T value = *it;
            this->vertices.insert(value);
        }

        for(json::iterator it = j["vertices_tombstone"].begin(); it != j["vertices_tombstone"].end(); ++it)
        {
            T value = *it;
            this->vertices_tombstone.insert(value);
        }

        for(json::iterator it = j["edges"].begin(); it != j["edges"].end(); ++it)
        {
            T value = *it;
            this->edges.insert(value);
        }

        for(json::iterator it = j["edges_tombstone"].begin(); it != j["edges_tombstone"].end(); ++it)
        {
            T value = *it;
            this->edges_tombstone.insert(value);
        }
    }

    void deserializeFile(std::string jsonString)
    {
        std::ifstream i(jsonString);
        json j;
        i >> j;

        this->id = j["id"];

        for(json::iterator it = j["vertices"].begin(); it != j["vertices"].end(); ++it)
        {
            T value = *it;
            this->vertices.insert(value);
        }

        for(json::iterator it = j["vertices_tombstone"].begin(); it != j["vertices_tombstone"].end(); ++it)
        {
            T value = *it;
            this->vertices_tombstone.insert(value);
        }

        for(json::iterator it = j["edges"].begin(); it != j["edges"].end(); ++it)
        {
            T value = *it;
            this->edges.insert(value);
        }

        for(json::iterator it = j["edges_tombstone"].begin(); it != j["edges_tombstone"].end(); ++it)
        {
            T value = *it;
            this->edges_tombstone.insert(value);
        }
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    void setVertices(std::set<T> vertices)
    {
        this->vertices = vertices;
    }

    void setVerticesTombstone(std::set<T> vertices_tombstone)
    {
        this->vertices_tombstone = vertices_tombstone;
    }

    void setEdges(std::set<T> edges)
    {
        this->edges = edges;
    }

    void setEdgesTombstone(std::set<T> edges_tombstone)
    {
        this->edges_tombstone = edges_tombstone;
    }

    std::set<T> queryVertices() 
    {
        return this->vertices;
    }

    std::set<T> queryVerticesTombstone() 
    {
        return this->vertices_tombstone;
    }

    std::set<T> queryEdges() 
    {
        return this->edges;
    }

    std::set<T> queryEdgesTombstone() 
    {
        return this->edges_tombstone;
    }

    void insertVertice(T value) 
    {
        this->vertices.insert(value);
    }

    void insertEdge(std::pair<T, T> value)
    {
        this.edges.insert(value);
    }

    void insertVertices(std::vector<T> values) 
    {
        for (auto value: values)
            this->vertices.insert(value);
    }

    void insertEdges(std::vector<T> values) 
    {
        for (auto value: values)
            this->edges.insert(value);
    }

    void removeVertice(T value) 
    {
        this->vertices_tombstone.insert(value);
    }

    void removeEdge(std::pair<T, T> value)
    {
        this->edges_tombstone.insert(value);
    }
};

/*
* template class for CRDT TwoPTwoPGraphSB
*/

template<typename T=int32_t>
class TwoPTwoPGraphSB : CrdtObject<T>
{
private:
    std::set<T> vertices;
    std::set<T> vertices_tombstone;
    std::set<std::pair<T, T>> edges;
    std::set<std::pair<T, T>> edges_tombstone;
    std::unordered_map<uint32_t,TwoPTwoPGraphMetadata<T>> replica_metadata; // sets on servers

public:
    uint32_t id; // server id
    TwoPTwoPGraphSB()
    {
        ;
    }

    TwoPTwoPGraphSB(uint32_t id)
    {
        this->id = id;
    }

    ~TwoPTwoPGraphSB()
    {
        ;
    }

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

    bool updateInternalPayload()
    {
        std::set<T> curr_vertices;
        std::set<T> curr_vertices_ts;
        typename std::unordered_map<uint32_t,TwoPTwoPGraphMetadata<T>>::iterator metadata_it;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryVertices();
            for (auto iter: temp_data)
            {
                curr_vertices.insert(iter);
            }
            auto temp_data_ts = metadata_it->second.queryVerticesTombstone();
            for (auto iter_ts: temp_data_ts)
            {
                curr_vertices_ts.insert(iter_ts);
            }
        }

        this->vertices = curr_vertices;
        this->vertices_tombstone = curr_vertices_ts;

        std::set<T> curr_edges;
        std::set<T> curr_edges_ts;
        for(metadata_it = this->replica_metadata.begin(); metadata_it != this->replica_metadata.end(); metadata_it++)
        {
            auto temp_data = metadata_it->second.queryEdges();
            for (auto iter: temp_data)
            {
                curr_edges.insert(iter);
            }
            auto temp_data_ts = metadata_it->second.queryEdgesTombstone();
            for (auto iter_ts: temp_data_ts)
            {
                curr_edges_ts.insert(iter_ts);
            }
        }

        this->edges = curr_edges;
        this->edges_tombstone = curr_edges_ts;

        return true;
    }

    const uint32_t& queryId() const
    {
        return this->id;
    }

    std::set<T> queryVertices() 
    {
        return this->vertices;
    }

    std::set<T> queryVerticesTombstone() 
    {
        return this->vertices_tombstone;
    }

    std::set<T> queryEdges() 
    {
        return this->edges;
    }

    std::set<T> queryEdgesTombstone() 
    {
        return this->edges_tombstone;
    }

    bool lookupVertice(T e) {
        if (this->vertices.count(e) && !this->vertices_tombstone.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }

    void addExternalReplica(std::vector<TwoPTwoPGraphMetadata<T>> external_replica_metadata)
    {
        for (auto &metadata: external_replica_metadata)
        {
            
            auto search = this->replica_metadata.find(metadata.queryId());
            if (search != this->replica_metadata.end()) {
                std::set<T> merged_set;
                std::set<T> setA = search->second.queryVertices();
                std::set<T> setB = metadata.queryVertices();
                std::set_union(setA.begin(),setA.end(),setB.begin(),setB.end(),std::inserter(merged_set,merged_set.begin()));
                metadata.setVertices(merged_set);

                std::set<T> merged_ts;
                std::set<T> tsA = search->second.queryVerticesTombstone();
                std::set<T> tsB = metadata.queryVerticesTombstone();
                std::set_union(tsA.begin(),tsA.end(),tsB.begin(),tsB.end(),std::inserter(merged_ts,merged_ts.begin()));
                metadata.setVerticesTombstone(merged_ts);

                std::set<T> merged_set_edges;
                std::set<T> setAedges = search->second.queryEdges();
                std::set<T> setBedges = metadata.queryEdges();
                std::set_union(setAedges.begin(),setAedges.end(),setBedges.begin(),setBedges.end(),std::inserter(merged_set_edges,merged_set_edges.begin()));
                metadata.setEdges(merged_set_edges);

                std::set<T> merged_ts_edges;
                std::set<T> tsA_edges = search->second.queryEdgesTombstone();
                std::set<T> tsB_edges = metadata.queryEdgesTombstone();
                std::set_union(tsA_edges.begin(),tsA_edges.end(),tsB_edges.begin(),tsB_edges.end(),std::inserter(merged_ts_edges,merged_ts_edges.begin()));
                metadata.setEdgesTombstone(merged_ts_edges);
            }
            
            auto replica = this->replica_metadata.insert(std::pair<uint32_t, TwoPTwoPGraphMetadata<T>>(metadata.queryId(), metadata));
            if (!replica.second) replica.first->second = metadata;
        }
        updateInternalPayload();
    }
};

}   // namespace state
}   // namespace crdt

#endif  // __TwoPTwoPGraphSB_H__
