/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   map_list.h
 * Author: kimbyeo4
 *
 * Created on March 20, 2018, 7:07 PM
 */

#ifndef MAP_LIST_H
#define MAP_LIST_H
#include "map_data.h"
#include "trie.h"
#include <string>
#include <utility>
#include <vector>

class map_list {
public:
    map_list();
    ~map_list();
    std::vector<std::pair<int, std::string>> map_names;
    trie trie_POI;
    trie trie_Street;
    trie trie_Intersection;
    void add_intersection_names();
    
private:
    void add_map_names();

};


// comparing function for I_nodes
class compare_I_node {
public:
    int operator() (const I_node a, const I_node b) const {
        return (a.travel_time + a.h) > (b.travel_time + b.h);
    }
};


// helper function to trace back path
Seg_array path_find (std::vector<I_node>& graph, I_node& end_node, unsigned intersect_id_start);


// helper dijkstra to find shortest path from intersection to a set of points
// also returns intersect_id_end (input as reference)
Seg_array intersection_dijkstra(const unsigned intersect_id_start, 
                                std::unordered_map<IntersectionIndex, bool>& endpoints,
                                const double turn_penalty,
                                IntersectionIndex& intersect_id_end);


// same function which takes in vector instead of multimap
Seg_array intersection_dijkstra(const unsigned intersect_id_start, 
                                I_array endpoints,
                                const double turn_penalty,
                                IntersectionIndex& intersect_id_end);




#endif /* MAP_LIST_H */

