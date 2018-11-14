#ifndef MAP_DATA_H
#define MAP_DATA_H

#define SCREEN_AREA 0.330174
#define AREA_DIVISION_FACTOR 10
#define LENGTH_DIVISION_FACTOR 10
#define BILLION 1000000000

#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <vector>
#include <cmath>
#include <set>
#include "graphics_types.h"
#include <unordered_map>
#include "graphics.h"
#include <map>
#include "f_struct.h"
#include "s_struct.h"
#include "poi_struct.h"
#include <cfloat>
#include <algorithm>
#include <thread>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry.hpp>
#include <thread>
#include "I_node.h"
#include "path_info.h"
#include "m4.h"


#define SPEEDCONVERSION 0.277778
#define CLICKDISTANCE 1200000
#define inf 999999

// Vectors of indices for different components
typedef std::vector<IntersectionIndex> I_array;
typedef std::vector<StreetIndex> S_array;
typedef std::vector<StreetSegmentIndex> Seg_array;
typedef std::vector<POIIndex> P_array;


namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// returns a point in 3 dimension
typedef bg::model::point<double, 3, bg::cs::cartesian> point;

// returns a pair that contains a point and an unsigned index
typedef std::pair<point, unsigned> value;


// Data structure to hold map data, initialized at load_map
class map_data {
public:
    // constructor to load all data
    map_data();

    // returns vector of segment indices at intersection
    std::vector<StreetSegmentIndex> const getIntersectionToSegment(IntersectionIndex);

    // returns vector of segment indices of a street
    std::vector<StreetSegmentIndex> const getStreetToSegment(StreetIndex);

    // returns set of intersections of a street
    std::set<IntersectionIndex> getStreetToIntersection(StreetIndex);

    // returns street index from its name
    std::vector<StreetIndex> const getStreetNameToID(std::string);

    // checks whether POI exists and returns POI index from its name
    P_array const getPOINameToID(std::string);

    // returns lengths from street segment
    double getSegmentToLength(StreetSegmentIndex);

    // returns travel times from street segment;
    double getSegmentToTime(StreetSegmentIndex);

    static bool is_longer(const f_struct& object1, const f_struct& object2);

    std::vector<s_struct> getStreetSegment1();
    std::vector<s_struct> getStreetSegment2();
    std::vector<s_struct> getStreetSegment3();
    std::vector<s_struct> getStreetSegment4();
    std::vector<s_struct> getStreetSegment5();
    std::vector<s_struct> getHighwaySegment();

    double getStreetDistance(StreetIndex);
    
    // return distance between two points (helper)
    double findDistance(LatLon, LatLon);

    // returns closest POI (helper)
    unsigned find_closest_point_of_interest_Rtree(LatLon);

    // returns closest intersection (helper)
    unsigned find_closest_intersection_Rtree(LatLon);


    t_point LatLon_to_t_point(LatLon);
    LatLon t_point_to_LatLon(t_point);


    std::vector<f_struct> get_L1_Features();
    std::vector<f_struct> get_L2_Features();
    std::vector<f_struct> get_L3_Features();
    std::vector<f_struct> get_L4_Features();
    std::vector<f_struct> get_L5_Features();
    std::vector<s_struct> get_all_segments();

    // variables for search queries
    I_array find_intersection_array;
    std::vector<Seg_array> find_street_array;
    P_array find_poi_array;
    std::string query;
    I_array find_intersections_path;
    Seg_array path;
    std::unordered_multimap<IntersectionIndex, path_info> path_element; 
    std::vector<std::pair<IntersectionIndex, IntersectionIndex>> swappable_cases;
    
    //store the first search bar string
    std::vector<std::string> find_search_store;
    //store the second search bar string
    std::vector<std::string> find_search_store2;
    
    // variable for search query error
    std::vector<std::string> errors;
    std::vector<std::string> autoCompletion;
    
//    //auto completion
//    trie trieHead;
//    std::vector<std::string> autoComplete;
//    //insertIntoTrie(trieHead);
      
//        std::vector<std::string> temp = trieHead.autoComplete(user_map);
//        std::cout << "do u mean?"<< std::endl;
//        for(unsigned i = 0; i<temp.size(); i++){
//            std::cout << temp[i] << std::endl;
//        }
    
    std::string user_input_map_name;
    std::string map_name;
     
    // state variables used for search queries
    bool first;
    bool button_load;
    bool click;
    bool right_click;
    char user_ans = '\0';
    bool enterWarning;

    unsigned findNodeIndex(OSMID);
    unsigned findWayIndex(OSMID);

    bool draw_subway;
    std::vector<LatLon> get_nodes();
    std::vector<std::vector<LatLon>> get_waysOfNodes();

    std::vector<poi_struct> get_L1_POI();
    std::vector<poi_struct> get_L2_POI();
    std::vector<poi_struct> get_L3_POI();
    std::vector<poi_struct> get_L4_POI();
    std::vector<poi_struct> get_L5_POI();
    
    std::vector<I_info> graph;
    double inverse_max_speed;
    
    
    //temp variables for m4
    std::vector<DeliveryInfo> deliveriesTemp;
    std::vector<unsigned> depotsTemp;
    std::vector<unsigned> pathTemp;
    
    void reset_graph();

    
    
private:
    //helper functions to build data structures

    void DataStruct_IntersectionToSegment();
    
    void DataStruct_segmentDistanceAndTime();

    void DataStruct_StreetToSegmentAndStreetToIntersection();

    void DataStruct_rtreePOI();

    void DataStruct_all_segments();

    void DataStruct_StreetLayers();

    void DataStruct_POINameToID();

    void DataStruct_featureLoad();

    void DataStruct_sorted_features();

    void DataStruct_FeatureLayers();

    void DataStruct_POILayers();

    void DataStruct_layer1NodesAndWays();
    
    void DataStruct_layer1otherfeatures();

    void DataStruct_subway();
    
    void DataStruct_world_coordinates();
    
    void DataStruct_graph();
    

 
    // vectors of vectors to hold segment data for intersections and street
    std::vector<std::vector<StreetSegmentIndex>> intersectionToSegment;
    std::vector<std::vector<StreetSegmentIndex>> streetToSegment;

    //street segments
    //big streets = speed limit above 60km/h
    std::vector<s_struct> streetSegment1;
    std::vector<s_struct> streetSegment2;
    std::vector<s_struct> streetSegment3;
    std::vector<s_struct> streetSegment4;
    std::vector<s_struct> highwaySegment;
    std::vector<s_struct> all_segments;

    // vector of set holding intersection IDs with streetID as index
    std::vector<std::set<IntersectionIndex>> streetToIntersection;

    // hash table to hold ID to street name
    std::unordered_multimap<std::string, StreetIndex> streetNameToID;

    // vector holding lengths of street segments
    std::vector<double> segmentToLength;

    // vector holding travel times of street segments
    std::vector<double> segmentToTime;

    // hash table to hold POI name to ID
    std::unordered_multimap<std::string, POIIndex> POINameToID;

    // Rtree which stores a pair of POI and unsigned intersection index
    bgi::rtree< value, bgi::quadratic<16> > rtreePOI;

    // Rtree which stores a pair of intersection points and unsigned intersection index
    bgi::rtree< value, bgi::quadratic<16> > rtreeIntersec;

    double computeArea(std::vector<t_point>, unsigned) const;

    std::vector<std::pair<double, FeatureIndex>> sorted_features;

    std::vector<f_struct> L1_Features;
    std::vector<f_struct> L2_Features;
    std::vector<f_struct> L3_Features;
    std::vector<f_struct> L4_Features;
    std::vector<f_struct> L5_Features;

    std::vector<poi_struct> L1_POI;
    std::vector<poi_struct> L2_POI;
    std::vector<poi_struct> L3_POI;
    std::vector<poi_struct> L4_POI;
    std::vector<poi_struct> L5_POI;

    std::unordered_map<OSMID, unsigned> layer1Nodes;
    std::unordered_map<OSMID, unsigned> layer1Ways;

    std::vector<LatLon> nodes;
    std::vector<std::vector<LatLon>> waysOfNodes;
    std::unordered_multimap<OSMID, std::string> streetTypeToOSMID;
    

};

extern map_data* mapData;


#endif /* MAP_DATA_H */