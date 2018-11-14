#include <thread>
#include <queue>
#include <unordered_map>
#include <algorithm> 
#include <cstdlib>
#include <ctime>
#include "map_list.h"
#include "m3.h"
#include "m2.h"
#include "m4.h"

//// generates path from start point using greedy algo
//Seg_array path_gen(const IntersectionIndex intersect_id_start,
//                    const std::unordered_multimap<IntersectionIndex, IntersectionIndex>& pd_pair,
//                    const std::unordered_map<IntersectionIndex, bool>& legal_points,
//                    const std::vector<unsigned>& depots,
//                    const double turn_penalty);
//
//double calculate_points_distance_helper(IntersectionIndex i, IntersectionIndex j);
//
//
//double calculate_path_distance_helper(const I_array path) ;
//
//
//Seg_array double_insertion2(const std::vector<DeliveryInfo>& deliveries,
//        const std::vector<unsigned>& depots,
//        const float turn_penalty, const float alpha);
//
////void rand_path_gen(const std::unordered_multimap<IntersectionIndex, IntersectionIndex>& pd_pair,
////                                        const I_array& legal, 
////                                        const std::vector<unsigned>& depots,
////                                        std::pair<I_array, double>& score);
////
////
////double calculate_intersection_score(const I_array& path);
////
////
////Seg_array i_array_to_path(const I_array& intersection_path, const double turn_penalty);
//
//
////Seg_array traveling_courier(const std::vector<DeliveryInfo>& deliveries, 
////                                        const std::vector<unsigned>& depots, 
////                                        const float turn_penalty) {
////    
////    // path to return
//////    Seg_array path;
//////    std::vector<Seg_array> delivery_paths;
////    
////    // use to check for corresponding dropoff point after pickup
////    std::unordered_multimap<IntersectionIndex, IntersectionIndex> pickup_dropoff;
////    
////    // store legal_points
////    std::unordered_map<IntersectionIndex, bool> legal_points;
////    
////    
////    // initializing first points
////    for (DeliveryInfo i : deliveries) {
////        pickup_dropoff.insert(std::make_pair(i.pickUp, i.dropOff));
////        legal_points.insert(std::make_pair(i.pickUp, true));
////    }
////    
////    
////    // keep track of best path
////    Seg_array best_path;
////    double min_distance = INF;
////    
////    
////    for (IntersectionIndex i : depots) {
////        // call path_gen here for each depot
////        Seg_array current_path = path_gen(i, pickup_dropoff, legal_points, depots, turn_penalty);
////        
////        if (current_path.size() == 0) {
////            continue;
////        }
////        
////        double current_distance = compute_path_travel_time(current_path, turn_penalty);
////        
////        if (current_distance < min_distance) {
////            best_path = current_path;
////        }
////    }
////    
////    return best_path;
////}
//
///*
//// should take in a start point, legal points, and pickup-dropoff pairs to develop greedy path
//Seg_array path_gen(const IntersectionIndex intersect_id_start,
//                    const std::unordered_multimap<IntersectionIndex, IntersectionIndex>& pd_pair,
//                    const std::unordered_map<IntersectionIndex, bool>& legal_points,
//                    const std::vector<unsigned>& depots,
//                    const double turn_penalty) {
//    
//    std::unordered_map<IntersectionIndex, bool> legal = legal_points;
//    
//    Seg_array path;
//    
//    IntersectionIndex current_point = intersect_id_start;
//    IntersectionIndex next_point;
//    
//    if (legal.find(intersect_id_start) != legal.end()) {
//        for (auto it = pd_pair.equal_range(intersect_id_start).first; it != pd_pair.equal_range(intersect_id_start).second; ++it) {
//            legal.insert(std::make_pair(it->second, true));
//        }
//    }
//    
//    else if (intersection_dijkstra(intersect_id_start, legal, turn_penalty, next_point).size() == 0) {
//        return Seg_array();
//    }
//    
//    while (legal.size() != 0) {
//        Seg_array current_subpath = intersection_dijkstra(current_point, legal, turn_penalty, next_point);
//        
//        legal.erase(next_point);
//        
//        for (auto it = pd_pair.equal_range(next_point).first; it != pd_pair.equal_range(next_point).second; ++it) {
//            legal.insert(std::make_pair(it->second, true));
//        }
//        
//        current_point = next_point;
//        path.insert(std::end(path), std::begin(current_subpath), std::end(current_subpath));
//    }
//    
//    Seg_array last_path = intersection_dijkstra(current_point, depots, turn_penalty, current_point);
//    
//    path.insert(std::end(path), std::begin(last_path), std::end(last_path));
//    return path;
//}
//

// helper dijkstra to find shortest path from intersection to a set of points
// also returns intersect_id_end (input as reference)
Seg_array intersection_dijkstra(const unsigned intersect_id_start, 
                                std::unordered_map<IntersectionIndex, bool>& endpoints,
                                const double turn_penalty,
                                IntersectionIndex& intersect_id_end) {
    
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
        
        // check if any intersection is found - if true, then it is the closest destination
        if (endpoints.find(current_node.id) != endpoints.end()) {
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
        intersect_id_end = current_node.id;
        return path_find(graph, current_node, intersect_id_start);
    }
    
    // else return empty array
    return Seg_array();
}


// same function which takes in vector instead of multimap
Seg_array intersection_dijkstra(const unsigned intersect_id_start, 
                                I_array endpoints,
                                const double turn_penalty,
                                IntersectionIndex& intersect_id_end) {
    
    // initializing potential destinations and hash table for checking
    std::unordered_map<IntersectionIndex, bool> intersections_check;
    
    // closest intersections into hash table
    for (IntersectionIndex i : endpoints) {
        intersections_check.insert(std::make_pair(i, true));
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
        
        // check if any intersection is found - if true, then it is the closest destination
        if (intersections_check.find(current_node.id) != intersections_check.end()) {
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
        intersect_id_end = current_node.id;
        return path_find(graph, current_node, intersect_id_start);
    }
    
    // else return empty array
    return Seg_array();
}

//*/
//
//Seg_array traveling_courier(const std::vector<DeliveryInfo>& deliveries,
//        const std::vector<unsigned>& depots,
//        const float turn_penalty) {
//    
//    float alpha = 1.0;
//    
//    return double_insertion2(deliveries, depots, turn_penalty, alpha);
//}
//
//Seg_array double_insertion2(const std::vector<DeliveryInfo>& deliveries,
//        const std::vector<unsigned>& depots,
//        const float turn_penalty, const float alpha){   
//    
//    // intersection-ordered path (path to plug into a*)
//    I_array path;
//
//    //step 0: initialization
//    IntersectionIndex largest_pickup;
//    IntersectionIndex largest_dropoff;
//    double largest_cost = INF;
//    for (auto i : deliveries) {
//        double current_cost = compute_path_travel_time(find_path_between_intersections(i.pickUp, i.dropOff, turn_penalty), turn_penalty);
//        
//        if (current_cost < largest_cost) {
//            largest_cost = current_cost;
//            largest_pickup = i.pickUp;
//            largest_dropoff = i.dropOff;
//        }
//    }
//    
//    
//    
//    
//    //add the first pair of pickup and dropoff to the path to start with
//    path.push_back(largest_pickup);
//    path.push_back(largest_dropoff);
//    
//    unsigned i, j;
//    
//    //going through all the deliveries
//    for(unsigned x = 0; x < deliveries.size(); x++){
//        
//        //get each pair of delivery and pickup
//        i = deliveries[x].pickUp;
//        j = deliveries[x].dropOff;
//        
//        if (i == largest_pickup) {
//            continue;
//        }
//        
//        //find their combined minimum weighted insertion cost 
//        //find the corresponding position in the current subtour H
//        
//        //(k,l) & (s,t) are two consecutive vertices
//        
//        
//        double   min_cost1 = INF;
//        double   min_cost2 = INF;
//        
//        unsigned position1, position2i, position2j;
//        
//        //1st case: i & j are inserted btw k and l
//        for(unsigned k = 0; k + 1 < path.size(); k++){
//            unsigned l = k + 1;
//            
//            double new_cost = 0;
//           
//            //calculate min_cost1 based on the formula
//            //min{alphi * cost(k,i) + cost(i,j) + (2-alpha) * cost(j,l) - cost(k,l)}
//            new_cost = alpha * calculate_points_distance_helper(path[k], i);
//            new_cost += calculate_points_distance_helper(i, j);
//            new_cost += (2 - alpha) * (calculate_points_distance_helper(j, path[l]));
//            new_cost -= calculate_points_distance_helper(path[k], path[l]);
//            
//            if(new_cost < min_cost1){
//                min_cost1 = new_cost;
//                position1 = k;
//            }
//        }
//        
//        
//        //2nd case: i is inserted btw k and l
//        //          j is inserted btw s and t
//        for(unsigned k = 0; k + 1 <  path.size(); k++){
//            unsigned l = k + 1;
//            
//            for(unsigned s = l + 1; s + 1 < path.size(); s++){
//                unsigned t = s + 1;
//                
//                double new_cost = 0;
//                
//                //calculate min_cost2 based on the formula
//                //min{alpha * (cost(k,i) + cost(i,l) - cost(k,l)) 
//                //      + (2 - alpha) * (cost(s,j) + cost(j, t) - cost(s,t)}
//                
//                new_cost = alpha * (calculate_points_distance_helper(path[k], i) 
//                             + calculate_points_distance_helper(i, path[l])
//                             - calculate_points_distance_helper(path[k], path[l]));
//                new_cost += (2 - alpha) * (calculate_points_distance_helper(path[s], j) 
//                                            + calculate_points_distance_helper(j, path[t])
//                                            - calculate_points_distance_helper(path[s], path[t]));
//                
//                if(new_cost < min_cost2){
//                    min_cost2 = new_cost;
//                    position2i = k;
//                    position2j = s;
//                }
//            }
//        }
//        
//        //take the min{min_cost1, min_cost2}
//        
//        //if its better to insert this pair together
//        if(min_cost1 <= min_cost2){
//            path.insert(path.begin() + position1 + 1, i);
//            path.insert(path.begin() + position1 + 2, j);
//        }
//        else if(min_cost1 > min_cost2){
//            path.insert(path.begin() + position2i + 1, i);
//            path.insert(path.begin() + position2j + 1, j);
//        }
//        
//    }
//
//    //find the lowest cost depots to connect to first and last point
//    
//    //start and end with any depot first, will change later
//    path.insert(path.begin(), depots[0]);
//    path.insert(path.end(), depots[0]);
//    
//    Seg_array seg_path;
//    
//    //plug into m3 a* 
//    for(unsigned x = 0; x + 1 < path.size(); x ++){
//        Seg_array temp = find_path_between_intersections(path[x], path[x+1], turn_penalty);
//        seg_path.insert(seg_path.end(), temp.begin(), temp.end());
//    }
//    
//    return seg_path;
//    
//}
//
//double calculate_path_distance_helper(const I_array path) {
//    double distance = 0;
//
//    //for loop going through all the points in the path and calculating the total distance in the array
//    for (unsigned i = 0; i + 1 < path.size(); i++) {
//        distance += calculate_points_distance_helper(path[i], path[i + 1]);
//    }
//
//    return distance;
//}
//
//double calculate_points_distance_helper(IntersectionIndex i, IntersectionIndex j) {
//    LatLon start, end;
//
//    start = getIntersectionPosition(i);
//    end = getIntersectionPosition(j);
//
//    if (i == j) {
//        return 0;
//    }
//
//    return find_distance_between_two_points(start, end);
//}
