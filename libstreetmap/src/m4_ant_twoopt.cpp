#include <thread>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <ratio>
#include "map_list.h"
#include "m3.h"
#include "m4.h"
#include <time.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h> 
#include <cstdlib>

#define ALPHA 1
#define BETA 3
#define RHO 0.1
#define Qo 0.99

#define ANTNUM 5





typedef std::pair<IntersectionIndex, IntersectionIndex> pairInt;

struct hash {

    template <class c1, class c2>
    std::size_t operator()(const std::pair<c1, c2> &pair)const {
        return std::hash<c1>{}
        (

                pair.first) ^ std::hash<c2> {
        }
        (pair.second);
    }
};

std::unordered_map<pairInt, double, hash> ph_table;
std::vector<std::pair<IntersectionIndex, IntersectionIndex>> best_intersection_path;
double inverse_best_cost;
double initPH;
double EPSILON;


Seg_array two_opt(double turn_penalty, double original_time, std::vector<IntersectionIndex> path_intersections, Seg_array original_path, std::unordered_multimap<IntersectionIndex, path_info> p_info,
       std::unordered_multimap<IntersectionIndex, IntersectionIndex> pickup_dropoff, const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots);

bool swappable(std::vector<unsigned> path_intersections, std::unordered_multimap<IntersectionIndex, path_info> p_info);

Seg_array nearest_neighbor(const std::vector<unsigned>& depots,
        const float turn_penalty,
        const std::unordered_multimap<IntersectionIndex, IntersectionIndex>& pd_pair,
        const std::unordered_map<IntersectionIndex, bool>& legal,
        std::vector<IntersectionIndex>& nn_intersections, const std::vector<DeliveryInfo>& deliveries);


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
    Seg_array nn_path = nearest_neighbor(depots, turn_penalty, pickup_dropoff, legal_points, nn_intersections, deliveries);
    double nn_time = compute_path_travel_time(nn_path, turn_penalty);
    initPH = ANTNUM / nn_time;
    EPSILON = 1 / (deliveries.size() * nn_time);

    //    std::cout << "NN-time : " << nn_time << std::endl;

    //    for (unsigned i = 0; i + 1 < nn_intersections.size(); ++i) {
    //        double ph_val = 500 * initPH;
    //        ph_table.insert(std::make_pair(std::make_pair(nn_intersections[i], nn_intersections[i + 1]), ph_val));
    //    }



    // create random # gen. using seed -> uniformly distributed from 0 to 1
    std::random_device rand_d;
    std::mt19937 generator(rand_d());
    std::uniform_real_distribution<> dist(0, 1.0);

    //plug this in as q: dist(generator)

    std::vector<Seg_array> best_segment_path;
    double best_time = INF;



    auto t_end = t_start;
    auto time = std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
    unsigned x = 0;

    while (time.count() < 3) {
        //for (unsigned x = 0; x < GENERATIONS; ++x) {
        //        std::cout << "Generation : " << x << std::endl;

        bool best_changed = false;


        for (unsigned i = 0; i < ANTNUM; ++i) {
            //            std::cout << "Ant : " << i;

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
                current_segment_path.push_back(find_path_between_intersections(current_point, next_point, turn_penalty));

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

            //            std::cout << " Cost : " << current_cost;
            //            std::cout << " Best : " << best_time << std::endl;

            // checking wall clock here:
            t_end = std::chrono::steady_clock::now();
            time = std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);

            //            std::cout << "Time used : " << time.count() << std::endl;

            if (time.count() > 2) {
                break;
            }

        }

        // UPDATE BEST HERE
        if (best_changed) {
            //            best_path.clear();
            //
            //            inverse_best_cost = 1 / best_time;
            //
            //            for (auto i : best_intersection_path) {
            //                best_path.insert(std::make_pair(i, true));
            //            }
        }

        globalUpdate();


        t_end = std::chrono::steady_clock::now();
        time = std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
        x++;
    }

    if (nn_time < best_time) {
        return nn_path;
    }


    IntersectionIndex starting_point = best_intersection_path[0].first;
    Seg_array best_depot_path;
    double min_depot_cost = INF;

    for (IntersectionIndex i : depots) {
        Seg_array current_path = find_path_between_intersections(i, starting_point, turn_penalty);
        double current_cost = compute_path_travel_time(current_path, turn_penalty);


        if (i == starting_point) {
            best_depot_path = Seg_array();
            break;
        } else if (current_path.size() == 0) {
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

    return path;

}

Seg_array nearest_neighbor(const std::vector<unsigned>& depots,
        const float turn_penalty,
        const std::unordered_multimap<IntersectionIndex, IntersectionIndex>& pd_pair,
        const std::unordered_map<IntersectionIndex, bool>& legal,
        std::vector<IntersectionIndex>& nn_intersections, const std::vector<DeliveryInfo>& deliveries) {

    // path to return
    Seg_array path;
    Seg_array sub_path;
    std::vector<Seg_array> delivery_paths;



    //for two opt
    std::unordered_multimap<IntersectionIndex, path_info> p_info;
    path_info p_info_temp;


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

    auto iter = pd_pair.equal_range(start_pickup);
    while (iter.first != iter.second) {
        p_info_temp.dropoff_point.push_back(iter.first->second);
        iter.first++;
    }
    p_info_temp.pickup = true;
    p_info_temp.pickup_point = start_pickup;
    p_info_temp.index = path.size() - 1;

    p_info.insert(std::make_pair(start_pickup, p_info_temp));
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
        sub_path = intersection_dijkstra(current_point, legal_points, turn_penalty, next_point);
        path.insert(path.end(), sub_path.begin(), sub_path.end());
        // mark our next point as visited by removing from legal_points table
        legal_points.erase(next_point);

        p_info_temp.index = path.size() - 1;

        // add the corresponding dropoff point to legal_points table
        auto range = pickup_dropoff.equal_range(next_point);
        for (auto it = range.first; it != range.second; ++it) {
            legal_points.insert(std::make_pair(it->second, true));
        }
        // switch to next point
        current_point = next_point;
        nn_intersections.push_back(current_point);

        //store data to p_info hash table
        auto it0 = pd_pair.equal_range(current_point);
        if (it0.first != it0.second) {
            p_info_temp.pickup = true;
            p_info_temp.pickup_point = current_point;

            while (it0.first != it0.second) {
                p_info_temp.dropoff_point.push_back(it0.first->second);
                it0.first++;
            }
        }


        p_info.insert(std::make_pair(current_point, p_info_temp));


    }

    // at this point, current_point = the last point
    // now we need to find destination depot
    sub_path = intersection_dijkstra(current_point, depots, turn_penalty, current_point);
    path.insert(path.end(), sub_path.begin(), sub_path.end());

    path = two_opt(turn_penalty, compute_path_travel_time(path, turn_penalty), nn_intersections, path, p_info,
            pickup_dropoff, deliveries, depots);

    //    for (Seg_array i : delivery_paths) {
    //        path.insert(std::end(path), std::begin(i), std::end(i));
    //    }
    //

    return path;
}


// local update to ph

void localUpdate(IntersectionIndex i, IntersectionIndex j) {
    double ph_val = (1 - EPSILON) * getPheromone(i, j) + (EPSILON * initPH);

    if (ph_table.find(std::make_pair(i, j)) == ph_table.end()) {
        // initializing ph value here
        ph_table.insert(std::make_pair(std::make_pair(i, j), ph_val));
    } else {
        // algo for updating ph value
        ph_table.find(std::make_pair(i, j))->second = ph_val;
    }

    return;
}


// returns ph values for subpaths

double getPheromone(IntersectionIndex i, IntersectionIndex j) {
    if (ph_table.find(std::make_pair(i, j)) == ph_table.end()) {
        // initial ph value
        return initPH;
    }

    return ph_table.find(std::make_pair(i, j))->second;
}


// global update to ph

void globalUpdate() {
    for (auto it = ph_table.begin(); it != ph_table.end(); ++it) {
        for (unsigned i = 0; i < best_intersection_path.size(); ++i) {
            if (it->first == best_intersection_path[i]) {
                it->second = (1 - RHO) * it->second + RHO * (inverse_best_cost);
            }
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
    } else {
        j = rand_prop_rule(i, legal_points);
    }

    localUpdate(i, j);
    return j;
}

Seg_array two_opt(double turn_penalty, double original_time, std::vector<IntersectionIndex> path_intersections, Seg_array original_path, std::unordered_multimap<IntersectionIndex, path_info> p_info,
       std::unordered_multimap<IntersectionIndex, IntersectionIndex> pickup_dropoff, const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots) {

    
    bool swapped = false;
    Seg_array best_path;
    unsigned count = 0;
    
    
    if(path_intersections.size() > 5){

    Seg_array new_path;
    Seg_array path_copy;
    Seg_array sub_path;
    std::vector<IntersectionIndex> path_intersections_copy;
    IntersectionIndex intersection_temp;
    int rand1 = 0;
    int rand2 = 0;
    int temp = 0;
    
    //initialize random number generator

//    std::vector<IntersectionIndex> sub_vec1;
//    std::vector<IntersectionIndex> sub_vec2;
//    std::vector<IntersectionIndex> sub_vec3;

    
    int random_num = 0;
    double min_time_diff = 0;
    double time__ = 0;
    time_t time_passed = 0;
    
    
    path_intersections_copy = path_intersections;

    srand(time(NULL));

    time_t start_time = clock();
    
    while (time_passed < 15) {

        rand1 = (rand() % (path_intersections_copy.size()-2)) + 1;

        rand2 = (rand() % (path_intersections_copy.size()-2)) + 1 ;


        

        if (abs(rand1 - rand2) > 2) {
            
            
            
            if (rand1 > rand2) {
                temp = rand1;
                rand1 = rand2;
                rand2 = temp;
            }

            intersection_temp = path_intersections_copy[rand1];
            path_intersections_copy[rand1] = path_intersections_copy[rand2];
            path_intersections_copy[rand2] = intersection_temp;
            
            
            
//            sub_vec1.insert(sub_vec1.end(), path_intersections.begin(), path_intersections.begin() + rand1);
//            sub_vec2.insert(sub_vec2.end(), path_intersections.begin() + rand1,
//                    path_intersections.begin() + rand2);
//
//            sub_vec3.insert(sub_vec3.end(), path_intersections.begin() + rand2, path_intersections.end());
//            

//
//                path_intersections_copy.insert(path_copy.end(), sub_vec3.begin(), sub_vec3.end());
//                path_intersections_copy.insert(path_copy.end(), sub_vec2.begin(), sub_vec2.end());
//                path_intersections_copy.insert(path_copy.end(), sub_vec1.begin(), sub_vec1.end());

            
//            std::cout << "mark4 " << std::endl;
//            
//            std::cout << "size of unchanged: " << path_intersections.size() << std::endl;
//            std::cout << "size of changed: " << path_copy.size() << std::endl;

            if (swappable(path_intersections_copy, p_info)) {

                std::cout << "LEGAL!" << std::endl;
                
                
                
                // store legal_points
                std::unordered_map<IntersectionIndex, bool> legal_points;

                // initializing first points
                for (DeliveryInfo i : deliveries) {
                    pickup_dropoff.insert(std::make_pair(i.pickUp, i.dropOff));
                    legal_points.insert(std::make_pair(i.pickUp, true));
                }



                for (DeliveryInfo i : deliveries) {
                    pickup_dropoff.insert(std::make_pair(i.pickUp, i.dropOff));
                    legal_points.insert(std::make_pair(i.pickUp, true));
                }

                double init_path_time = INF;
                for (IntersectionIndex i : depots) {
                    IntersectionIndex start;
                    Seg_array first_path = intersection_dijkstra(i, legal_points, turn_penalty, start);

                    if (i == start) {
                        init_path_time = 0;
//                        start_pickup = start;
                        break;
                    }

                    if (first_path.empty()) {
                        continue;
                    }

                    double time = compute_path_travel_time(first_path, turn_penalty);

                    if (time < init_path_time) {
                        init_path_time = time;
                        sub_path = first_path; //stores path from the depot to the first pick up point (closest)
//                        start_pickup = start;
                    }
                }


                
                new_path.insert(new_path.end(), sub_path.begin(), sub_path.end()); 
                                
                
                
                
                
//                
//                //copy from depo to the first pickup point                
//                new_path.insert(new_path.end(), original_path.begin(), original_path.begin() + p_info.find(path_intersections_copy[0])->second.index + 1);


                //creating a new path
                for (unsigned h = 0; h < path_copy.size() - 1; ++h) {
                    sub_path = find_path_between_intersections(path_intersections_copy[h], path_intersections_copy[h + 1], turn_penalty);
                    new_path.insert(new_path.end(), sub_path.begin(), sub_path.end());
                    sub_path.clear();
                }

                
                
                sub_path = find_path_between_intersections(path_intersections_copy[path_intersections_copy.size() - 2],
                        path_intersections_copy[path_intersections_copy.size() - 1], turn_penalty);
                new_path.insert(new_path.end(), sub_path.begin(), sub_path.end());

//                //copy from the last intersection to the end depot;
//                new_path.insert(new_path.end(), original_path.begin() +
//                        p_info.find(path_intersections_copy[path_intersections_copy.size() - 1])->second.index + 1,
//                        original_path.end());
                IntersectionIndex dummy;
                sub_path = intersection_dijkstra(path_copy[path_copy.size()-1], depots, turn_penalty, dummy);
                new_path.insert(new_path.end(), sub_path.begin(), sub_path.end());
                


                time__ = compute_path_travel_time(new_path, turn_penalty);
                if ((time__ - original_time) < min_time_diff) {
                    min_time_diff = time__ - original_time;
                    best_path = new_path;
                    swapped = true;
                    count++;
                }
            }
//            path_intersections = path_copy;
//            path_intersections_copy.clear();
            new_path.clear();
            sub_path.clear();
//            sub_vec1.clear();
//            sub_vec2.clear();
//            sub_vec3.clear();
            

        }

        time_passed = (clock() - start_time) / ((double) CLOCKS_PER_SEC);
//        std::cout << "time passed!!!!!!!!!!!!!!!! " << time_passed << std::endl;

    }
    }
    std::cout << "@@@@@@@@ swapped: " << count << " times" << std::endl;
    if (swapped) {
        return best_path;
    } else {
        return original_path;
    }

}

bool swappable(std::vector<unsigned> path_intersections, std::unordered_multimap<IntersectionIndex, path_info> p_info) {

    //the vector to be reversed is given
    //if there are pickup and any of corresponding dropoffs, cannot be swapped

    std::unordered_multimap<IntersectionIndex, path_info> new_hash;
    path_info pathInfo;

    std::vector<IntersectionIndex> dropoffs;



    for (unsigned i = 0; i < path_intersections.size(); ++i) {

        if (p_info.find(path_intersections[i])->second.pickup) {

            pathInfo.pickup = true;
            pathInfo.pickup_point = path_intersections[i];
            pathInfo.order = i;

            dropoffs = p_info.find(path_intersections[i])->second.dropoff_point;
            for (unsigned j = 0; j < dropoffs.size(); ++j) {
                pathInfo.dropoff_point.push_back(dropoffs[j]);

                new_hash.insert(std::make_pair(path_intersections[i], pathInfo));
            }
        } 
//        else {
//
//            pathInfo.pickup = false;
//            pathInfo.dropoff_point.push_back(path_intersections[i]);
//            pathInfo.order = i;
//            new_hash.insert(std::make_pair(path_intersections[i], pathInfo));
//
//        }

    }

    for (unsigned i = 0; i < path_intersections.size(); ++i) {

        if (new_hash.find(path_intersections[i])->second.pickup) {
            dropoffs = new_hash.find(path_intersections[i])->second.dropoff_point;
            for (unsigned j = 0; j < dropoffs.size(); ++j) {
                if (new_hash.find(path_intersections[i])->second.order >
                        new_hash.find(dropoffs[j])->second.order) {
                    return false;
                }
            }

        }

    }
    std::cout << "@@LEGAL@@" << std::endl;

    return true;

}
