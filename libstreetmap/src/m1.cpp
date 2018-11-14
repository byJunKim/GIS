/* 
 * Copyright 2018 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "map_data.h"
#include <utility>

map_data* mapData;

//loads the map
//if successful, creates a data structure of a vector where each element
//is another vector. The indices of outermost vector are the intersection ids
//and indices of the inner vector are segments ids
//another data structure is a vector of street names
//where each index represents street id

bool load_map(std::string map_path) {
    bool load_successful = false;
//    bool load_success_OSM = false;
//
//    std::string osm_path = map_path.substr(0, map_path.length() - 12) + ".osm.bin";

    //the function returns false if failed loading the map
    load_successful = loadStreetsDatabaseBIN(map_path);

    //the function returns false if failed loading the map
//    load_success_OSM = loadOSMDatabaseBIN(osm_path);


    if (!load_successful /*|| !load_success_OSM */) {
        return false;
    }

    // load map_data structure (located in constructor)
    mapData = new map_data();

    return true;
}


//closes the map

void close_map() {
    // delete data and free memory inside map_data structure
    delete mapData;
    close_graphics();
    closeStreetDatabase();
//    closeOSMDatabase();
}


//if the intersection id is found, get the street segments connected to the
//intersection. Data from the pre-loaded vector

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {

    //check if intersection id exists
    if (intersection_id < getNumberOfIntersections()) {
        return mapData->getIntersectionToSegment(intersection_id);
    }

    // if the intersection isn't found
    //returns an empty vector (default constructor)
    std::vector<IntersectionIndex> empty;
    return (empty);
}


//gets the total number of streets, looks for the same street name,
//adds street IDs to a vector.
//returns a vector of size 0 if not found.

std::vector<unsigned> find_street_ids_from_name(std::string street_name) {
    // returns street IDs from map_data structure
    return mapData->getStreetNameToID(street_name);
}


//gets concatenated names (ex. Young & Bloor) of street at an intersection,
//and puts it in a vector9

std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {
    // vector to return, containing intersection names
    std::vector<std::string> intersectionStreetNames;

    // for loop iterating through the set data
    for (auto const & segment : mapData->getIntersectionToSegment(intersection_id)) {
        intersectionStreetNames.push_back(getStreetName(getStreetSegmentInfo(segment).streetID));
    }


    return intersectionStreetNames;
}


//checks segments connected to one intersection and see the ends of each segment
//are id1 and id2.

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {
    // check if intersection is the same
    if (intersection_id1 == intersection_id2) {
        return true;
    }

    // vectors of segments for each intersection
    auto segment1 = mapData->getIntersectionToSegment(intersection_id1);
    auto segment2 = mapData->getIntersectionToSegment(intersection_id2);
    unsigned numOfSegments = segment1.size();

    //check all segments connected to intersection 1
    //see if each of its end is id1 and id2
    for (unsigned i = 0; i < numOfSegments; i++) {
        auto info = getStreetSegmentInfo(segment1[i]);
        if (info.oneWay) {
            if ((info.from == intersection_id1 &&
                    info.to == intersection_id2)) {
                return true;
            }
        }else{
            if(std::find(segment2.begin(), segment2.end(), segment1[i]) != segment2.end()){
                return true;
            }
        }
    }
    return false;
}


//Returns all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections

I_array find_adjacent_intersections(unsigned intersection_id) {
    // making vector to return adjacent intersections
    I_array adjacentIntersections;
    // set to store values temporarily / check for duplication
    std::set<IntersectionIndex> storage;

    // for loop going through each street connected to intersection, using streetVector
    for (auto const & segment : mapData->getIntersectionToSegment(intersection_id)) {
        auto info = getStreetSegmentInfo(segment);

        // check if segment is one way
        if (info.oneWay) {
            if (intersection_id == info.from) {
                storage.insert(info.to);
            }
        }
            // check if intersection is destination or arrival
        else if (intersection_id == info.from) {
            storage.insert(info.to);
        }
        else if (intersection_id == info.to) {
            storage.insert(info.from);
        }

    }

    // put all storage values into return vector
    for (auto i : storage) {
        adjacentIntersections.push_back(i);
    }

    return adjacentIntersections;
}


//Returns all street segments for the given street

Seg_array find_street_street_segments(unsigned street_id) {
    return mapData->getStreetToSegment(street_id);
}


//Returns all intersections along the a given street

I_array find_all_street_intersections(unsigned street_id) {
    // vector to return, containing intersections
    I_array intersections;

    // set to store values temporarily
    auto storage = mapData->getStreetToIntersection(street_id);

    // put all storage values into return vector
    for (auto i : storage) {
        intersections.push_back(i);
    }

    return intersections;
}


//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
//However street names are not guaranteed to be unique, so more than 1 intersection id
//may exist

I_array find_intersection_ids_from_street_names(std::string street_name1,
        std::string street_name2) {
    // vector to return, containing intersections
    I_array intersections;

    // get street IDs from names
    auto ids1 = mapData->getStreetNameToID(street_name1);
    auto ids2 = mapData->getStreetNameToID(street_name2);

    // loop through IDs (incase of duplicate street names)
    for (auto id1 : ids1) {
        // obtain set of intersection for street
        auto intersections1 = mapData->getStreetToIntersection(id1);

        for (auto id2 : ids2) {
            // obtain set of intersection for street
            auto intersections2 = mapData->getStreetToIntersection(id2);

            // check whether intersections match from either set
            for (auto intersection1 : intersections1) {
                if (intersections2.find(intersection1) != intersections2.end()) {
                    intersections.push_back(intersection1);
                }
            }
        }
    }

    return intersections;
}


//Returns the distance between two coordinates in meters

double find_distance_between_two_points(LatLon point1, LatLon point2) {
    return mapData->findDistance(point1, point2);
}


//Returns the distance between two coordinates in meters
//Returns the length of the given street segment in meters

double find_street_segment_length(unsigned street_segment_id) {
    return mapData->getSegmentToLength(street_segment_id);
}

//Returns the length of the specified street in meters

double find_street_length(unsigned street_id) {
    double streetLength = 0;

    //get the streetSegments vector that contains all the segments belong to street id
    Seg_array streetSegments = mapData->getStreetToSegment(street_id);

    //loop through the streetSegments vector and access each value
    for (auto i : streetSegments) {
        //add all the values together
        streetLength += mapData->getSegmentToLength(i);
    }

    return streetLength;
}

//Returns the travel time to drive a street segment in seconds 
//(time = distance/speed_limit)

double find_street_segment_travel_time(unsigned street_segment_id) {
    return mapData->getSegmentToTime(street_segment_id);
}

//Returns the nearest point of interest to the given position

unsigned find_closest_point_of_interest(LatLon my_position) {
    return mapData->find_closest_point_of_interest_Rtree(my_position);
}

//Returns the the nearest intersection to the given position

unsigned find_closest_intersection(LatLon my_position) {
    return mapData->find_closest_intersection_Rtree(my_position);
}
