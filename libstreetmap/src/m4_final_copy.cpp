#include <thread>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <ratio>
#include "StreetsDatabaseAPI.h"
#include "map_list.h"
#include "m3.h"
#include "m4.h"

#define ALPHA 1
#define BETA 3
#define RHO 0.1
#define Qo 0.99

#define ANTNUM 10
#define MAX_SAME_GEN 10

typedef std::pair<IntersectionIndex, IntersectionIndex> pairInt;

struct pair_hash_function{
    template <class c1, class c2>
    std::size_t operator() (const std::pair<c1, c2> &pair)const{
        return std::hash<c1>{}(pair.first) ^ std::hash<c2>{}(pair.second);
    }
};

std::unordered_map<pairInt, double, pair_hash_function> ph_table;
std::unordered_map<pairInt, bool, pair_hash_function> best_path;

double inverse_best_cost;
double initPH;
double EPSILON;

Seg_array nearest_neighbor(const std::vector<unsigned>& depots, 
                            const float turn_penalty,
                            const std::unordered_multimap<IntersectionIndex, IntersectionIndex>& pd_pair,
                            const std::unordered_map<IntersectionIndex, bool>& legal,
                            std::vector<IntersectionIndex>& nn_intersections);


// local update to ph
void localUpdate(IntersectionIndex i, IntersectionIndex j);


// returns ph values for subpaths
double getPheromone(IntersectionIndex i, IntersectionIndex j);


// global update to ph
void globalUpdate();


IntersectionIndex rand_prop_rule(IntersectionIndex i,
        const std::unordered_map<IntersectionIndex, bool>& legal_points);


// ant travels path i to the intersection j (returned in this func)
IntersectionIndex tour_construction(IntersectionIndex i, double q, const std::unordered_map<IntersectionIndex, bool>& legal_points);


Seg_array traveling_courier(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    // start wall clock here:
    auto t_start = std::chrono::steady_clock::now();
    
    
    // use to check for corresponding dropoff point after pickup
    std::unordered_multimap<IntersectionIndex, IntersectionIndex> pickup_dropoff;
    
    // store legal_points
    std::unordered_map<IntersectionIndex, bool> legal_points;
    
    // initializing first points
    for (DeliveryInfo i : deliveries) {
        pickup_dropoff.insert(std::make_pair(i.pickUp, i.dropOff));
        legal_points.insert(std::make_pair(i.pickUp, true));
    }
    
    std::vector<IntersectionIndex> nn_intersections;
    
    // calculating initial PH and epsilon values using NN-greedy algo
    Seg_array nn_path = nearest_neighbor(depots, turn_penalty, pickup_dropoff, legal_points, nn_intersections);
    double nn_time = compute_path_travel_time(nn_path, turn_penalty);
    initPH = ANTNUM / nn_time;
    EPSILON = 1 / (deliveries.size() * nn_time);
    
    std::cout << "NN-time : " << nn_time << std::endl;
    
    
    // create random # gen. using seed -> uniformly distributed from 0 to 1
    std::random_device rand_d;
    std::mt19937 generator(rand_d());
    std::uniform_real_distribution<> dist(0, 1.0);
    
    //plug this in as q: dist(generator)
    
    std::vector<std::pair<IntersectionIndex, IntersectionIndex>> best_intersection_path;
    std::vector<Seg_array> best_segment_path;
    double best_time = INF;
    
    
    
    auto t_end = t_start;
    auto time = std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
    unsigned x = 0;
    int same_generation_count = 0;
    
    while (time.count() < 26.9) {
        std::cout << "Generation : " << x << std::endl;
        
        bool best_changed = false;
        
        
        for (unsigned i = 0; i < ANTNUM; ++i) {
            std::cout << "Ant : " << i;
            
            std::unordered_map<IntersectionIndex, bool> ant_points = legal_points;
            std::vector<std::pair<IntersectionIndex, IntersectionIndex>> current_intersection_path;
            std::vector<Seg_array> current_segment_path;
            double current_cost = 0;
            
            IntersectionIndex current_point;
            IntersectionIndex next_point;
            
            int rand_index = rand() % ant_points.size();
            int index = 0;
            
            for (auto it = ant_points.begin(); it != ant_points.end() && index <= rand_index; ++it) {
                current_point = it->first;
                index++;
            }
            
            ant_points.erase(current_point);
            
            for (auto it = pickup_dropoff.equal_range(current_point).first; it != pickup_dropoff.equal_range(current_point).second; ++it) {
                ant_points.insert(std::make_pair(it->second, true));
            }
            
            while (ant_points.size() != 0) {
                next_point = tour_construction(current_point, dist(generator), ant_points);
                
                current_intersection_path.push_back(std::make_pair(current_point, next_point));
                current_segment_path.push_back(find_path_between_intersections(current_point, next_point,turn_penalty));
                
                ant_points.erase(next_point);
                
                for (auto it = pickup_dropoff.equal_range(next_point).first; it != pickup_dropoff.equal_range(next_point).second; ++it) {
                    ant_points.insert(std::make_pair(it->second, true));
                }
                
                current_point = next_point;
            }
            
            for (Seg_array p : current_segment_path) {
                current_cost += compute_path_travel_time(p, turn_penalty);
            }
            
            if (current_cost < best_time) {
                best_changed = true;
                best_time = current_cost;
                best_intersection_path = current_intersection_path;
                best_segment_path = current_segment_path;
            }
            
            std::cout << " Cost : " << current_cost;
            std::cout << " Best : " << best_time << std::endl;
            
            // checking wall clock here:
            t_end = std::chrono::steady_clock::now();
            time = std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
            
            std::cout << "Time used : " << time.count() << std::endl;
            
            if (time.count() > 26.9) {
                break;
            }
            
        }
        
        // UPDATE BEST HERE
        if (best_changed) {
            same_generation_count = 0;
            best_path.clear();

            inverse_best_cost = 1 / best_time;

            for (auto i : best_intersection_path) {
                best_path.insert(std::make_pair(i, true));
            }
        }
        
        else {
            same_generation_count++;
            
            if (same_generation_count >= MAX_SAME_GEN) {
                break;
            }
            
        }

        globalUpdate();
        
        
        t_end = std::chrono::steady_clock::now();
        time = std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
        x++;
    }
    
    
//    while (time.count() < 28.5) {
//        auto current_intersection_path = best_intersection_path;
//        std::vector<Seg_array> current_segment_path;
//        double current_cost = 0;
//        
//        int rand_index0 = rand() % best_intersection_path.size();
//        int rand_index1 = rand_index0;
//        
//        while (rand_index0 == rand_index1) {
//            rand_index1 = rand() % best_intersection_path.size();
//        }
//        
//        current_intersection_path[rand_index0] = best_intersection_path[rand_index1];
//        current_intersection_path[rand_index1] = best_intersection_path[rand_index0];
//        
//        for (auto i : current_intersection_path) {
//            Seg_array sub_path = find_path_between_intersections(i.first, i.second, turn_penalty);
//            current_segment_path.push_back(sub_path);
//            current_cost += compute_path_travel_time(sub_path, turn_penalty);
//        }
//        
//        if (current_cost < best_time) {
//            std::cout << "Improvement found : " << current_cost << std::endl;
//            best_time = current_cost;
//            best_intersection_path = current_intersection_path;
//            best_segment_path = current_segment_path;
//        }
//        
//        t_end = std::chrono::steady_clock::now();
//        time = std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
//    }
    
    
    IntersectionIndex starting_point = best_intersection_path[0].first;
    Seg_array best_depot_path;
    double min_depot_cost = INF;
    
    for (IntersectionIndex i : depots) {
        Seg_array current_path = find_path_between_intersections(i, starting_point, turn_penalty);
        double current_cost = compute_path_travel_time(current_path, turn_penalty);
        
        
        if (i == starting_point) {
            best_depot_path = Seg_array();
            break;
        }
        
        else if (current_path.size() == 0) {
            continue;
        }
        
        if (current_cost < min_depot_cost) {
            min_depot_cost = current_cost;
            best_depot_path = current_path;
        }
    }
    
    IntersectionIndex temp;
    
    best_segment_path.insert(best_segment_path.begin(), best_depot_path);
    
    best_segment_path.push_back(intersection_dijkstra(best_intersection_path[best_intersection_path.size() - 1].second, 
                                depots, turn_penalty, temp));
    
    Seg_array path;
    
    for (Seg_array i : best_segment_path) {
        path.insert(std::end(path), std::begin(i), std::end(i));
    }
    
    best_time = compute_path_travel_time(path, turn_penalty);
    
    if (nn_time < best_time) {
        return nn_path;
    }
    
    return path;
    
}

Seg_array nearest_neighbor(const std::vector<unsigned>& depots, 
                            const float turn_penalty,
                            const std::unordered_multimap<IntersectionIndex, IntersectionIndex>& pd_pair,
                            const std::unordered_map<IntersectionIndex, bool>& legal,
                            std::vector<IntersectionIndex>& nn_intersections) {
    
    // path to return
    Seg_array path;
    std::vector<Seg_array> delivery_paths;
    
    std::unordered_multimap<IntersectionIndex, IntersectionIndex> pickup_dropoff = pd_pair;
    std::unordered_map<IntersectionIndex, bool> legal_points = legal;
    
    // store first start_pickup point
    IntersectionIndex start_pickup;
    
    // find shortest path from any depot to any pickup - this will be the first part of our path
    double init_path_time = INF;
    for (IntersectionIndex i : depots) {
        IntersectionIndex start;
        Seg_array first_path = intersection_dijkstra(i, legal_points, turn_penalty, start);
        
        if (i == start) {
            init_path_time = 0;
            start_pickup = start;
            break;
        }
        
        if (first_path.empty()) {
            continue;
        }
        
        double time = compute_path_travel_time(first_path, turn_penalty);
        
        if (time < init_path_time) {
            init_path_time = time;
            path = first_path; //stores path from the depot to the first pick up point (closest)
            start_pickup = start;
        }
    }
    
    // remove the first pickup point we reached from our legal_points table
    legal_points.erase(start_pickup);
    
    // add the corresponding dropoff point to legal_points table
    for (auto it = pickup_dropoff.equal_range(start_pickup).first; it != pickup_dropoff.equal_range(start_pickup).second; ++it) {
        legal_points.insert(std::make_pair(it->second, true));
    }
    
    // clear and add intersections to global data structure
    IntersectionIndex current_point = start_pickup;
    nn_intersections.push_back(current_point);
    
    while (legal_points.size() != 0) {
        // find shortest path to next legal point from current point
        IntersectionIndex next_point;
        delivery_paths.push_back(intersection_dijkstra(current_point, legal_points, turn_penalty, next_point));
        
        // mark our next point as visited by removing from legal_points table
        legal_points.erase(next_point);
        
        // add the corresponding dropoff point to legal_points table
        auto range = pickup_dropoff.equal_range(next_point);
        for (auto it = range.first; it != range.second; ++it) {
            legal_points.insert(std::make_pair(it->second, true));
        }
        
        // switch to next point
        current_point = next_point;
        nn_intersections.push_back(current_point);
    }
    
    // at this point, current_point = the last point
    // now we need to find destination depot
    delivery_paths.push_back(intersection_dijkstra(current_point, depots, turn_penalty, current_point));
    
    for (Seg_array i : delivery_paths) {
        path.insert(std::end(path), std::begin(i), std::end(i));
    }
    
    
    return path;
}


// local update to ph
void localUpdate(IntersectionIndex i, IntersectionIndex j) {
    double ph_val = (1 - EPSILON) * getPheromone(i,j) + (EPSILON * initPH);
    
    if (ph_table.find(std::make_pair(i,j)) == ph_table.end()) {
        // initializing ph value here
        ph_table.insert(std::make_pair(std::make_pair(i,j), ph_val));
    }
    
    else {
        // algo for updating ph value
        ph_table.find(std::make_pair(i,j))->second = ph_val;
    }
    
    return;
}


// returns ph values for subpaths
double getPheromone(IntersectionIndex i, IntersectionIndex j) {
    if (ph_table.find(std::make_pair(i,j)) == ph_table.end()) {
        // initial ph value
        return initPH;
    }
    
    return ph_table.find(std::make_pair(i,j))->second;
}


// global update to ph
void globalUpdate() {
    for (auto it = ph_table.begin(); it != ph_table.end(); ++it) {
        if (best_path.find(it->first) != best_path.end()) {
            it->second = (1 - RHO) * it->second + RHO * (inverse_best_cost);
        }
    }
    
    return;
}

IntersectionIndex rand_prop_rule(IntersectionIndex i, const std::unordered_map<IntersectionIndex, bool>& legal_points) {
    // keep track of next node (i -> j)
    IntersectionIndex j = 0;
    
    // used to store probabilities for each legal point from i
    std::unordered_map<IntersectionIndex, double> intersection_prob; 
    
    // total sum of probabilities
    double sum_chance = 0;
    
    for (auto it = legal_points.begin(); it != legal_points.end(); ++it) {
        double current_prob = pow(getPheromone(i, it->first), ALPHA) * pow((1 / find_distance_between_two_points(getIntersectionPosition(i), getIntersectionPosition(it->first))), BETA);
        sum_chance += current_prob;
        intersection_prob.insert(std::make_pair(it->first, current_prob));
    }
    
    
    std::random_device rand;
    std::mt19937 generator(rand());
    std::uniform_real_distribution<> dist(0, sum_chance);
    
    double random = dist(generator);
    
    
    double choice = 0;
    
    for (auto it = legal_points.begin(); it != legal_points.end() && choice < random; ++it) {
        j = it->first;
        choice += intersection_prob.find(j)->second;
    }
    
    return j;
}


// ant travels path i to the intersection j (returned in this func)
IntersectionIndex tour_construction(IntersectionIndex i, double q, const std::unordered_map<IntersectionIndex, bool>& legal_points) {
    // keep track of chosen path based on greedy + probability
    IntersectionIndex j = 0;
    
    if (q <= Qo) {
        double best_cost = 0;

        for (auto it = legal_points.begin(); it != legal_points.end(); ++it) {
            double current_distance = find_distance_between_two_points(getIntersectionPosition(i), getIntersectionPosition(it->first));
            double current_cost = pow(getPheromone(i, it->first), ALPHA) * pow((1 / current_distance), BETA);

            if (current_cost > best_cost) {
                j = it->first;
                best_cost = current_cost;
            }
        }
    }

    else {
        j = rand_prop_rule(i, legal_points);
    }
    
    localUpdate(i, j);
    return j;
}

