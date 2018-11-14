#include <thread>
#include <queue>
#include <unordered_map>
#include "map_list.h"
#include "m1.h"
#include "m3.h"
#include "m2.h"
#include "I_node.h"


// Returns the time required to travel along the path specified, in seconds. 
// The path is given as a vector of street segment ids, and this function 
// can assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus the given turn_penalty (in seconds) per turn implied by the path. 
// A turn occurs when two consecutive street segments have different street IDs.
double compute_path_travel_time(const Seg_array& path, 
                                const double turn_penalty) {
    
   
    
    // variable to hold travel time
    double travel_time = 0;
    
    if (path.size() == 0) {
        return 0;
    }
    
    // loop through every segment inside path vector
    for (unsigned i = 0; i + 1 < path.size(); ++i) {
        travel_time += find_street_segment_travel_time(path[i]);
        
        // check for whether street changes -> turn penalty
        if (getStreetSegmentInfo(path[i]).streetID != getStreetSegmentInfo(path[i + 1]).streetID) {
            travel_time += turn_penalty;
        }
    }
    
    // add last street (disregarded in order to have turn penalty check)
    travel_time += find_street_segment_travel_time(path[path.size() - 1]);
    
    return travel_time;
}


// Returns a path (route) between the start intersection and the end
// intersection, if one exists. This routine should return the shortest path
// between the given intersections when the time penalty to turn (change
// street IDs) is given by turn_penalty (in seconds).
// If no path exists, this routine returns an empty (size == 0) vector.
// If more than one path exists, the path with the shortest travel time is
// returned. The path is returned as a vector of street segment ids; traversing
// these street segments, in the returned order, would take one from the start
// to the end intersection.
Seg_array find_path_between_intersections(const unsigned intersect_id_start,
        const unsigned intersect_id_end,
        const double turn_penalty) {
    // TODO: MAYBE ADD VISITED BOOL TO I_NODE
    std::vector<I_node> graph(getNumberOfIntersections());
    
    // check whether start = end
    if (intersect_id_start == intersect_id_end ||
            intersect_id_start >= getNumberOfIntersections() ||
            intersect_id_end >= getNumberOfIntersections()) {
        return Seg_array();
    }    
    
    // setup temp variables: visited checker and heap
    std::vector<bool> visited(getNumberOfIntersections(), false);
    std::priority_queue<I_node, std::vector<I_node>, compare_I_node> minheap;
    
    
    // initial distance = 0 and start node is visited
    visited[intersect_id_start] = true;
    
    graph[intersect_id_start].id = intersect_id_start;
    I_node current_node = graph[intersect_id_start];
    current_node.travel_time = 0;
    
    // initial loading from starting intersection
    for (unsigned i = 0; i < mapData->graph[current_node.id].adjacent.size(); ++i) {
        // setting up initial distances for intersections adjacent to starting point
        IntersectionIndex current_id = mapData->graph[current_node.id].adjacent[i];
        graph[current_id].id = current_id;
        double distance = find_street_segment_travel_time(mapData->graph[current_node.id].segments[i]);
        // check if less than existing distances (either INF or already set)
        if (distance < graph[mapData->graph[current_node.id].adjacent[i]].travel_time) {
            graph[current_id].travel_time = distance;
            graph[current_id].h =
                    find_distance_between_two_points(
                    getIntersectionPosition(mapData->graph[current_node.id].adjacent[i]),
                    getIntersectionPosition(intersect_id_end))
                    * mapData->inverse_max_speed;
            graph[current_id].prev = mapData->graph[current_node.id].segments[i];
            minheap.push(graph[current_id]);
        }
    }
    
    
//  check whether heap is empty or we found the destination
    while (!minheap.empty() && !visited[intersect_id_end]) {
        while (visited[current_node.id]) {
            if (!minheap.empty()) {
                current_node = minheap.top();
                minheap.pop();
            }
            
            else {
                return Seg_array();
            }
        }

        // mark the current node as visited
        visited[current_node.id] = true;  
        
        for (unsigned i = 0; i < mapData->graph[current_node.id].adjacent.size(); ++i) {
            // setting up initial distances for intersections adjacent to starting point
            
            if (!visited[mapData->graph[current_node.id].adjacent[i]]) {
                IntersectionIndex current_id = mapData->graph[current_node.id].adjacent[i];
                graph[current_id].id = current_id;
                double distance = find_street_segment_travel_time(mapData->graph[current_node.id].segments[i]) + current_node.travel_time;
                
                // check for turns
                if (getStreetSegmentInfo(mapData->graph[current_node.id].segments[i]).streetID != getStreetSegmentInfo(current_node.prev).streetID) {
                    distance += turn_penalty;
                }

                // check if less than existing distances (either INF or already set)
                if (distance < graph[current_id].travel_time) {
                    graph[current_id].travel_time = distance;
                    graph[current_id].h =
                            find_distance_between_two_points(
                            getIntersectionPosition(mapData->graph[current_node.id].adjacent[i]),
                            getIntersectionPosition(intersect_id_end))
                            * mapData->inverse_max_speed;
                    graph[current_id].prev = mapData->graph[current_node.id].segments[i];
                    minheap.push(graph[current_id]);
                }
            }
        }
    }
    
    // loop backwards to trace path if found
    if (visited[intersect_id_end]) {
        return path_find (graph, graph[intersect_id_end], intersect_id_start);
    }
    
    // else return empty path
    return Seg_array();
}


// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// The path will begin at the specified intersection, and end on the 
// intersection that is closest (in Euclidean distance) to the point of 
// interest.
// If no such path exists, returns an empty (size == 0) vector.
Seg_array find_path_to_point_of_interest(const unsigned intersect_id_start, 
                                               const std::string point_of_interest_name,
                                               const double turn_penalty) {
    
    // initializing array of potential POI destinations and hash table for checking
    P_array pois = mapData->getPOINameToID(point_of_interest_name);
    std::unordered_map<IntersectionIndex, bool> poi_check;
    
    // intersection to hold if there is only one potential POI
    IntersectionIndex single_dest;
    
    // store POI's closest intersections into hash table
    for (POIIndex i : pois) {
        single_dest = find_closest_intersection(getPointOfInterestPosition(i));
        poi_check.insert(std::make_pair(single_dest, true));
    }
    
    // run intersection path_finding if only one destination
    if (poi_check.size() == 1) {
        return find_path_between_intersections(intersect_id_start, single_dest, turn_penalty);
    }
    
    // containing uninitialized nodes with time = inf
    std::vector<I_node> graph(getNumberOfIntersections());
    
    // setup temp variables: visited checker and heap
    std::vector<bool> visited(getNumberOfIntersections(), false);
    std::priority_queue<I_node, std::vector<I_node>, compare_I_node> minheap;
    
    // initial distance = 0 and start node is visited
    visited[intersect_id_start] = true;
    bool found = false;
    
    // update graph for starting intersection
    graph[intersect_id_start].id = intersect_id_start;
    I_node current_node = graph[intersect_id_start];
    current_node.travel_time = 0;
    
    // initial loading from starting intersection
    for (unsigned i = 0; i < mapData->graph[current_node.id].adjacent.size(); ++i) {
        // setting up initial distances for intersections adjacent to starting point
        IntersectionIndex current_id = mapData->graph[current_node.id].adjacent[i];
        graph[current_id].id = current_id;
        double distance = find_street_segment_travel_time(mapData->graph[current_node.id].segments[i]);
        // check if less than existing distances (either INF or already set)
        if (distance < graph[mapData->graph[current_node.id].adjacent[i]].travel_time) {
            graph[current_id].travel_time = distance;
            graph[current_id].prev = mapData->graph[current_node.id].segments[i];
            minheap.push(graph[current_id]);
        }
    }
    
    
    //  check whether heap is empty or we found the destination
    while (!minheap.empty() && !found) {
        while (visited[current_node.id]) {
            if (!minheap.empty()) {
                current_node = minheap.top();
                minheap.pop();
            }
            
            else {
                continue;
            }
        }
        
        // check if any poi is found - if true, then it is the closest destination
        if (poi_check.find(current_node.id) != poi_check.end()) {
            found = true;
            continue;
        }


        // mark the current node as visited
        visited[current_node.id] = true;        
        
        for (unsigned i = 0; i < mapData->graph[current_node.id].adjacent.size(); ++i) {
            // setting up initial distances for intersections adjacent to starting point
            
            if (!visited[mapData->graph[current_node.id].adjacent[i]]) {
                IntersectionIndex current_id = mapData->graph[current_node.id].adjacent[i];
                graph[current_id].id = current_id;
                double distance = find_street_segment_travel_time(mapData->graph[current_node.id].segments[i]) + current_node.travel_time;
                
                // check for turns
                if (getStreetSegmentInfo(mapData->graph[current_node.id].segments[i]).streetID != getStreetSegmentInfo(current_node.prev).streetID) {
                    distance += turn_penalty;
                }

                // check if less than existing distances (either INF or already set)
                if (distance < graph[current_id].travel_time) {
                    graph[current_id].travel_time = distance;
                    graph[current_id].prev = mapData->graph[current_node.id].segments[i];
                    minheap.push(graph[current_id]);
                }
            }
        }
    }
    
    // loop backwards if found
    if (found) {
        return path_find (graph, current_node, intersect_id_start);
    }
    
    // else return empty array
    return Seg_array();
}


// path finding function to be used if path is found
Seg_array path_find (std::vector<I_node>& graph, I_node& end_node, unsigned intersect_id_start) {
    // find final node
    I_node node = end_node;
    Seg_array path;
    
    // trace back path using prev segment
    while (node.id != intersect_id_start) {
        path.insert(path.begin(), node.prev);
        if (node.id == getStreetSegmentInfo(node.prev).from) {
            node = graph[getStreetSegmentInfo(node.prev).to];
        }
        else {
            node = graph[getStreetSegmentInfo(node.prev).from];
        }
    }

    return path;
}
