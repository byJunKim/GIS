#include "map_data.h"
#include "LatLon.h"


extern double avg_latitude;
extern double maximum_latitude;
extern double minimum_latitude;
extern double maximum_longitude;
extern double minimum_longitude;
double world_area;
// constructor to load all data

map_data::map_data() {
    // setting up threads to load other variables
    // CHANGE THREADING FOR GOING THROUGH EVERY STREET SEGMENT (CHANGE IT TO DOING AFTER DRAW MAP)
    std::thread t0(&map_data::DataStruct_StreetToSegmentAndStreetToIntersection, this);
    std::thread t1(&map_data::DataStruct_all_segments, this);
    std::thread t2(&map_data::DataStruct_IntersectionToSegment, this);
    std::thread t3(&map_data::DataStruct_rtreePOI, this);
    std::thread t4(&map_data::DataStruct_POINameToID, this);
    std::thread t5(&map_data::DataStruct_segmentDistanceAndTime, this);
    std::thread t6(&map_data::DataStruct_featureLoad, this);
    std::thread t7(&map_data::DataStruct_POILayers, this);
    std::thread t8(&map_data::DataStruct_world_coordinates, this);


    // setting up state variables for searching
    first = true;
    button_load = true;
    click = false;
    draw_subway = false;

    // screen_area = get_visible_screen().area();
    // TODO - store into data structure:
    maximum_latitude = getIntersectionPosition(0).lat();
    minimum_latitude = maximum_latitude;
    maximum_longitude = getIntersectionPosition(0).lon();
    minimum_longitude = maximum_longitude;


    // joining threads
    t0.join();
    t1.join();
    std::thread ta(&map_data::DataStruct_StreetLayers, this);
    t2.join();
    std::thread tb(&map_data::DataStruct_graph, this);
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    ta.join();
    tb.join();
    t3.detach();

    
}



// returns vector of segment indices at intersection

Seg_array const map_data::getIntersectionToSegment(IntersectionIndex id) {
    return intersectionToSegment[id];
}


// returns vector of segment indices of a street

Seg_array const map_data::getStreetToSegment(StreetIndex id) {
    return streetToSegment[id];
}


// returns set of intersections of a street

std::set<IntersectionIndex> map_data::getStreetToIntersection(StreetIndex id) {
    return streetToIntersection[id];
}


// returns vectors to obtain street IDs (there may be more than one street w/ same name)

S_array const map_data::getStreetNameToID(std::string name) {
    S_array street_indices;
    auto range = streetNameToID.equal_range(name);

    for (auto it = range.first; it != range.second; ++it) {
        street_indices.push_back(it->second);
    }


    return street_indices;
}


// checks whether POI exists and returns POI index from its name

P_array const map_data::getPOINameToID(std::string name) {
    P_array poi_indices;

    auto range = POINameToID.equal_range(name);
    for (auto it = range.first; it != range.second; ++it) {
        poi_indices.push_back(it->second);
    }
    return poi_indices;
}


// returns lengths from street segment

double map_data::getSegmentToLength(StreetSegmentIndex id) {
    return segmentToLength[id];
}


// returns travel times from street segment;

double map_data::getSegmentToTime(StreetSegmentIndex id) {
    return segmentToTime[id];
}

std::vector<s_struct> map_data::getStreetSegment1() {
    return streetSegment1;
}

std::vector<s_struct> map_data::getStreetSegment2() {
    return streetSegment2;
}

std::vector<s_struct> map_data::getStreetSegment3() {
    return streetSegment3;
}

std::vector<s_struct> map_data::getStreetSegment4() {
    return streetSegment4;
}

std::vector<s_struct> map_data::getHighwaySegment() {
    return highwaySegment;
}

std::vector<s_struct> map_data::get_all_segments() {
    return all_segments;
}

std::vector<LatLon> map_data::get_nodes() {
    return nodes;
}

std::vector<std::vector<LatLon>> map_data::get_waysOfNodes() {
    return waysOfNodes;
}

// HELPER FUNCTIONS //

void map_data::DataStruct_IntersectionToSegment() {

    //each element of this vector is another vector of street segments
    //each index represents intersection id
    intersectionToSegment.resize(getNumberOfIntersections());

    // for loop to store segment data
    for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
        //the number of street segments at each intersection
        //sizing the inner vector which represents the number of segments
        intersectionToSegment[i].resize(getIntersectionStreetSegmentCount(i));


        for (unsigned j = 0; j < getIntersectionStreetSegmentCount(i); j++) {
            //each index of the inner vector becomes segments IDs
            intersectionToSegment[i][j] = getIntersectionStreetSegment(i, j);
        }
    }
}

void map_data::DataStruct_segmentDistanceAndTime() {
    segmentToLength.resize(getNumberOfStreetSegments());
    for (unsigned i = 0; i < getNumberOfStreetSegments(); ++i) {
        //obtaining number of curve points to calculate distances
        unsigned numOfCurvePts = getStreetSegmentInfo(i).curvePointCount;

        //getting the starting position and ending position for this street segment
        LatLon from = getIntersectionPosition(getStreetSegmentInfo(i).from);
        LatLon to = getIntersectionPosition(getStreetSegmentInfo(i).to);

        if (numOfCurvePts == 0)
            segmentToLength[i] = findDistance(from, to);

            // if there are curve points, need to sum each distance in between
        else {
            double length = 0;
            // curve point counter
            unsigned j = 0;
            length += findDistance(from, getStreetSegmentCurvePoint(i, j));

            // for loop to sum the distances between curve points
            for (; j < numOfCurvePts - 1; ++j) {
                LatLon c1 = getStreetSegmentCurvePoint(i, j);
                LatLon c2 = getStreetSegmentCurvePoint(i, j + 1);
                length += findDistance(c1, c2);
            }

            length += findDistance(getStreetSegmentCurvePoint(i, j), to);
            segmentToLength[i] = length;
        }
    }


    // stores travel times for each street segment
    segmentToTime.resize(getNumberOfStreetSegments());
    for (unsigned i = 0; i < getNumberOfStreetSegments(); ++i) {
        segmentToTime[i] = segmentToLength[i] / (SPEEDCONVERSION * (getStreetSegmentInfo(i).speedLimit));
    }
}

void map_data::DataStruct_StreetToSegmentAndStreetToIntersection() {

    // resize to get number of streets
    streetToSegment.resize(getNumberOfStreets());
    streetToIntersection.resize(getNumberOfStreets());

    //stores street segments for each street id
    for (unsigned i = 0; i < getNumberOfStreetSegments(); ++i) {
        // inserts street names with street segment IDs as pairs
        StreetSegmentIndex id = getStreetSegmentInfo(i).streetID;
        streetToSegment[id].push_back(i);


        // insert intersections of a street into set (prevents duplicates)
        streetToIntersection[id].insert(getStreetSegmentInfo(i).to);
        streetToIntersection[id].insert(getStreetSegmentInfo(i).from);
    }


    // stores street IDs with street name keys
    for (unsigned i = 0; i < getNumberOfStreets(); ++i) {
        streetNameToID.insert(std::make_pair(getStreetName(i), i));
    }


}

void map_data::DataStruct_rtreePOI() {

    //load r tree for find the closet POI function
    point ptemp;
    for (POIIndex i = 0; i < getNumberOfPointsOfInterest(); ++i) {
        LatLon temp = getPointOfInterestPosition(i);
        double cosine_lat = cos(DEG_TO_RAD * temp.lat());
        double x = EARTH_RADIUS_IN_METERS * cosine_lat * cos(DEG_TO_RAD * temp.lon());
        double y = EARTH_RADIUS_IN_METERS * cosine_lat * sin(DEG_TO_RAD * temp.lon());
        double z = EARTH_RADIUS_IN_METERS * sin(DEG_TO_RAD * temp.lat());
        ptemp.set<0>(x);
        ptemp.set<1>(y);
        ptemp.set<2>(z);
        rtreePOI.insert(std::make_pair(ptemp, i));
    }


    //load r tree for find the closet intersection function
    for (POIIndex i = 0; i < getNumberOfIntersections(); ++i) {
        LatLon temp = getIntersectionPosition(i);
        double cosine_lat = cos(DEG_TO_RAD * temp.lat());
        double x = EARTH_RADIUS_IN_METERS * cosine_lat * cos(DEG_TO_RAD * temp.lon());
        double y = EARTH_RADIUS_IN_METERS * cosine_lat * sin(DEG_TO_RAD * temp.lon());
        double z = EARTH_RADIUS_IN_METERS * sin(DEG_TO_RAD * temp.lat());
        ptemp.set<0>(x);
        ptemp.set<1>(y);
        ptemp.set<2>(z);
        rtreeIntersec.insert(std::make_pair(ptemp, i));
    }

}

void map_data::DataStruct_POINameToID() {

    //hash table for POI name -> Index
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); ++i) {
        POINameToID.insert(std::make_pair(getPointOfInterestName(i), i));
    }

}

//filling up each object in all_segments vector

void map_data::DataStruct_all_segments() {
    // keep track of maximum speed
    float maxSpeed = 0;

    //filtering out information using the number of intersections in a street
    //and unknown streets
    s_struct segment;
    for (unsigned i = 0; i < getNumberOfStreetSegments(); ++i) {
        maxSpeed = std::max(maxSpeed, getStreetSegmentInfo(i).speedLimit);

        segment.index = i;

        if (getStreetSegmentInfo(i).speedLimit > 80) {
            segment.width = 6;
        } else if (getStreetSegmentInfo(i).speedLimit > 60) {
            segment.width = 5;
        } else if (getStreetSegmentInfo(i).speedLimit > 40) {
            segment.width = 4;
        } else if (getStreetSegmentInfo(i).speedLimit > 20) {
            segment.width = 3;
        } else {
            segment.width = 2;
        }

        t_point from, to;
        from = LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(i).from));
        to = LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(i).to));

        if (from.x < to.x) {
            if (from.y < to.y) {
                segment.position = t_bound_box(from.x, from.y, to.x, to.y);
                segment.angle = atan2((to.y - from.y), (to.x - from.x));
                segment.direction = 1;
            } else { //from.y > to.y
                segment.position = t_bound_box(from.x, to.y, to.x, from.y);
                segment.angle = atan2((from.y - to.y), (to.x - from.x));
                segment.direction = 4;
            }
        } else { //from.x > to.x
            if (from.y < to.y) {
                segment.position = t_bound_box(to.x, from.y, from.x, to.y);
                segment.angle = atan2((to.y - from.y), (from.x - to.x));
                segment.direction = 2;
            } else { // from.y > to.y
                segment.position = t_bound_box(to.x, to.y, from.x, from.y);
                segment.angle = atan2((from.y - to.y), (from.x - to.x));
                segment.direction = 3;
            }


        }

        all_segments.push_back(segment);
        
    }
        
    inverse_max_speed = 1 / maxSpeed;
        }

void map_data::DataStruct_StreetLayers() {

    for (unsigned i = 0; i < getNumberOfStreets(); i++) {


        if (getStreetSegmentInfo(getStreetToSegment(i)[0]).speedLimit > 80) {
            for (unsigned j = 0; j < getStreetToSegment(i).size(); ++j) {

                highwaySegment.push_back(all_segments[getStreetToSegment(i)[j]]);
            }
        } else {

            if (getStreetToIntersection(i).size() > 40 && getStreetName(i) != "<unknown>") {
                for (unsigned j = 0; j < getStreetToSegment(i).size(); ++j) {
                    if (getStreetSegmentInfo(getStreetToSegment(i)[j]).speedLimit > 60) {
                        streetSegment1.push_back(all_segments[getStreetToSegment(i)[j]]);
                    }
                }
            }
            if (getStreetToIntersection(i).size() > 20 && getStreetName(i) != "<unknown>") {
                for (unsigned j = 0; j < getStreetToSegment(i).size(); ++j) {
                    if (getStreetSegmentInfo(getStreetToSegment(i)[j]).speedLimit > 40) {
                        streetSegment2.push_back(all_segments[getStreetToSegment(i)[j]]);
                    }
                }
            }

            if (getStreetToIntersection(i).size() > 10 && getStreetName(i) != "<unknown>") {
                for (unsigned j = 0; j < getStreetToSegment(i).size(); ++j) {
                    if (getStreetSegmentInfo(getStreetToSegment(i)[j]).speedLimit > 20) {
                        streetSegment3.push_back(all_segments[getStreetToSegment(i)[j]]);
                    }
                }
            }

            if (getStreetToIntersection(i).size() > 5 && getStreetName(i) != "<unknown>") {
                for (unsigned j = 0; j < getStreetToSegment(i).size(); ++j) {
                    if (getStreetSegmentInfo(getStreetToSegment(i)[j]).speedLimit > 10) {
                        streetSegment4.push_back(all_segments[getStreetToSegment(i)[j]]);
                }
                }
            }

        }
    }
}

void map_data::DataStruct_featureLoad() {
    DataStruct_sorted_features();
    DataStruct_FeatureLayers();
}

void map_data::DataStruct_sorted_features() {

    std::vector<t_point> featurePoint;
    //calculate the area of each feature and sort by the area in the vector
    for (FeatureIndex i = 0; i < getNumberOfFeatures(); ++i) {
        //filtering out unknown features
        if (getFeatureName(i) != "<unknown>") {
            double area = 0;
            featurePoint.resize(getFeaturePointCount(i));
            for (unsigned j = 0; j < getFeaturePointCount(i); ++j) {
                featurePoint[j] = LatLon_to_t_point(getFeaturePoint(i, j));
            }
            area = computeArea(featurePoint, getFeaturePointCount(i));


            sorted_features.push_back(std::make_pair(area, i));
        }
    }

    //sorting by areas
    std::sort(sorted_features.begin(), sorted_features.end());
    //reverse the vector -> bigger areas in lower indices
    std::reverse(sorted_features.begin(), sorted_features.end());

}

void map_data::DataStruct_FeatureLayers() {

    //two vectors to separate open & closed features
    std::vector<f_struct> openFeatures;
    std::vector<f_struct> closedFeatures;


    for (auto it = sorted_features.begin(); it != sorted_features.end(); ++it) {
        //filling the object up and putting in open & closed feature vectors
        f_struct object;
        object.area = it->first;
        object.length = 0;
        object.index = it->second;
        if (getFeatureType(it->second) == Lake || getFeatureType(it->second) == River
                || getFeatureType(it->second) == Stream) {
            object.color = t_color(153, 204, 255);
        } else if (getFeatureType(it->second) == Island || getFeatureType(it->second) == Beach) {
            object.color = t_color(240, 240, 230);
        } else if (getFeatureType(it->second) == Building) {
            object.color = t_color(220, 220, 210);
        } else if (getFeatureType(it->second) == Golfcourse || getFeatureType(it->second) == Greenspace) {
            object.color = t_color(0, 204, 102);
        } else if (getFeatureType(it->second) == Park) {
            object.color = t_color(204, 255, 153);
        } else {
            object.color = t_color(240, 240, 230);
        }
        //filling up the vector in the f_struct
        object.feature_point_count = getFeaturePointCount(it->second);
        for (unsigned j = 0; j < getFeaturePointCount(it->second); ++j) {
            object.feature_point.push_back(LatLon_to_t_point(getFeaturePoint(it->second, j)));
        }
        //distinguishing open & closed features
        if (getFeaturePoint(it->second, 0).lat() == getFeaturePoint(it->second, getFeaturePointCount(it->second) - 1).lat()
                && getFeaturePoint(it->second, 0).lon() == getFeaturePoint(it->second, getFeaturePointCount(it->second) - 1).lon()) {
            object.open = false;
            closedFeatures.push_back(object);
        } else {
            double length_ = 0;
            for (int k = 0; k + 1 < object.feature_point_count; ++k) {
                length_ += sqrt(pow((object.feature_point[k + 1].y - object.feature_point[k].y), 2)
                        + pow((object.feature_point[k + 1].x - object.feature_point[k].x), 2));
            }
            object.length = length_;
            object.open = true;
            openFeatures.push_back(object);
        }
    }

    int counter = 0;
    std::sort(openFeatures.begin(), openFeatures.end(), is_longer);
    LatLon top_right(maximum_latitude, maximum_longitude);
    LatLon bottom_left(minimum_latitude, minimum_longitude);
    //pushing open features first to the sorted features
    //this helps print open features first
    //all sorted features are from biggest areas to smallest areas
    for (unsigned i = 0; i < openFeatures.size(); ++i) {


        if (openFeatures[i].length > sqrt(world_area) / (2 * LENGTH_DIVISION_FACTOR)) {
            L1_Features.push_back(openFeatures[i]);
        }
        if (openFeatures[i].length > sqrt(world_area) / (20 * LENGTH_DIVISION_FACTOR)) {
            L2_Features.push_back(openFeatures[i]);
        }
        if (openFeatures[i].length > sqrt(world_area) / (100 * LENGTH_DIVISION_FACTOR)) {
            L3_Features.push_back(openFeatures[i]);
        }
        if (openFeatures[i].length > sqrt(world_area) / (1000 * LENGTH_DIVISION_FACTOR)) {
            L4_Features.push_back(openFeatures[i]);
        }
        L5_Features.push_back(openFeatures[i]);

    }

    for (unsigned i = 0; i < closedFeatures.size(); ++i) {
        if (closedFeatures[i].area == 0) counter++;

        if (closedFeatures[i].area > (world_area) / (pow(AREA_DIVISION_FACTOR, 4))) {
            L1_Features.push_back(closedFeatures[i]);
        }
        if (closedFeatures[i].area > (world_area) / (pow(AREA_DIVISION_FACTOR, 5))) {
            L2_Features.push_back(closedFeatures[i]);
        }
        if (closedFeatures[i].area > (world_area) / (pow(AREA_DIVISION_FACTOR, 6))) {
            L3_Features.push_back(closedFeatures[i]);
        }
        if (closedFeatures[i].area > (world_area) / (pow(AREA_DIVISION_FACTOR, 7))) {
            L4_Features.push_back(closedFeatures[i]);
        }
        L5_Features.push_back(closedFeatures[i]);

    }

}

void map_data::DataStruct_POILayers() {

    //for loop going through all the poi locations and draw the star pic
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); ++i) {
        poi_struct object;
        //get the location of poi position
        LatLon location = getPointOfInterestPosition(i);
        object.name = getPointOfInterestName(i);
        std::string typeName = getPointOfInterestType(i);
        object.typeName = typeName;
        //change latlon to tpoint so that we can plot them
        object.location = mapData->LatLon_to_t_point(location);

        if (typeName == "car_rental" || typeName == "old_car_rental")
            object.typeName = "car_rental";

        if (typeName == "kindergarten" || typeName == "college" || typeName == "school" ||
                typeName == "music_lessons" || typeName == "childcare" ||
                typeName == "driving_school" || typeName == "tutor" || typeName == "driving_school")
            object.typeName = "school";

        if (typeName == "doctors" || typeName == "pharmacy" || typeName == "dentist" || typeName == "hospital" || typeName == "clinic")
            object.typeName = "hospital";

        if (typeName == "museum" || typeName == "park") {
            L1_POI.push_back(object);
        } else if (object.typeName == "car_rental" || typeName == "theatre" ||
                typeName == "university") {
            L2_POI.push_back(object);
        } else if (typeName == "fuel" || typeName == "place_of_worship" ||
                object.typeName == "school") {
            L3_POI.push_back(object);
        } else if (typeName == "fast_food" || typeName == "restaurant" || typeName == "atm" ||
                typeName == "cafe" || typeName == "hotel" || typeName == "library") {
            L4_POI.push_back(object);
        } else {
            L5_POI.push_back(object);
        }

    }

}

//void map_data::DataStruct_layer1NodesAndWays() {
//
//
//    //hash table for OSMNodes OSMID to node index
//    for (unsigned i = 0; i < getNumberOfNodes(); i++) {
//        const OSMNode* temp = getNodeByIndex(i);
//        layer1Nodes.insert(std::make_pair(temp->id(), i));
//    }
//
//    //hash table for OSMWays OSMID to way index
//    for (unsigned i = 0; i < getNumberOfWays(); i++) {
//        const OSMWay* temp = getWayByIndex(i);
//        layer1Ways.insert(std::make_pair(temp->id(), i));
//    }
//
//
//}
//
//void map_data::DataStruct_subway() {
//
//    //initialize nodes vector and waysOfNodes
//    for (unsigned i = 0; i < getNumberOfRelations(); ++i) {
//        const OSMRelation* temp = getRelationByIndex(i);
//
//        for (unsigned j = 0; j < getTagCount(temp); ++j) {
//            if (getTagPair(temp, j).second == "subway") {
//                std::vector<OSMRelation::Member> tempMember = temp->members();
//
//                for (unsigned k = 0; k < tempMember.size(); ++k) {
//                    //node
//                    if (tempMember[k].tid.type() == 1) {
//                        unsigned nodeIndex = findNodeIndex(tempMember[k].tid);
//                        nodes.push_back(getNodeByIndex(nodeIndex)->coords());
//                    }//way
//                    else if (tempMember[k].tid.type() == 2) {
//                        unsigned wayIndex = findWayIndex(tempMember[k].tid);
//                        std::vector<OSMID> wayToNodes = getWayByIndex(wayIndex)->ndrefs();
//
//
//                        std::vector<LatLon> temp2;
//                        for (unsigned l = 0; l < wayToNodes.size(); ++l) {
//                            unsigned nodeIndex2 = findNodeIndex(wayToNodes[l]);
//                            temp2.push_back(getNodeByIndex(nodeIndex2)->coords());
//                        }
//
//                        waysOfNodes.push_back(temp2);
//                    }
//                }
//            }
//        }
//    }
//
//
//}

void map_data::DataStruct_world_coordinates() {
    // Store intersection data into intersection data structure
    // Also loop through every position to obtain largest and smallest LatLon coordinates
    for (unsigned i = 0; i < getNumberOfIntersections(); ++i) {
        // Comparing with current position to see which one is smaller
        maximum_longitude = std::max(maximum_longitude, getIntersectionPosition(i).lon());
        minimum_longitude = std::min(minimum_longitude, getIntersectionPosition(i).lon());
        maximum_latitude = std::max(maximum_latitude, getIntersectionPosition(i).lat());
        minimum_latitude = std::min(minimum_latitude, getIntersectionPosition(i).lat());
    }
    //calculating average latitude
    avg_latitude = 0.5 * (maximum_latitude + minimum_latitude);



    world_area = (maximum_latitude - minimum_latitude)*(maximum_longitude - minimum_longitude);
}


// TODO : OPTIMIZE BY PUTTING INTO INTERSECTINO TO SEGMENT LOADING

void map_data::DataStruct_graph() {

    for (unsigned i = 0; i < getNumberOfIntersections(); ++i) {
        I_info temp;

        for (StreetSegmentIndex seg : intersectionToSegment[i]) {
            if (getStreetSegmentInfo(seg).oneWay) {
                if (getStreetSegmentInfo(seg).from == i) {
                    temp.segments.push_back(seg);
                    temp.adjacent.push_back(getStreetSegmentInfo(seg).to);
                }
            } else {
                temp.segments.push_back(seg);
                if (getStreetSegmentInfo(seg).to == i) {
                    temp.adjacent.push_back(getStreetSegmentInfo(seg).from);
                } else {
                    temp.adjacent.push_back(getStreetSegmentInfo(seg).to);
                }
            }
        }

        graph.push_back(temp);
    }
}


// return distance between two points (helper)

double map_data::findDistance(LatLon point1, LatLon point2) {
    // calculating average latitude between two points    
    double latAvg = cos(((point1.lat() + point2.lat()) * DEG_TO_RAD) * 0.5);

    // finding (x) in (x,y) for method to calculate distance found in
    // milestone1 pdf instructions
    double xCoord1 = point1.lon() * DEG_TO_RAD * latAvg;
    double xCoord2 = point2.lon() * DEG_TO_RAD * latAvg;

    // returning distance double
    return EARTH_RADIUS_IN_METERS * sqrt(pow((point2.lat() - point1.lat()) * DEG_TO_RAD, 2) + pow(xCoord2 - xCoord1, 2));
}


// returns closest POI (helper)

unsigned map_data::find_closest_point_of_interest_Rtree(LatLon my_position) {
    std::vector<value> temp;
    double x, y, z;

    x = EARTH_RADIUS_IN_METERS * cos(DEG_TO_RAD * my_position.lat()) * cos(DEG_TO_RAD * my_position.lon());
    y = EARTH_RADIUS_IN_METERS * cos(DEG_TO_RAD * my_position.lat()) * sin(DEG_TO_RAD * my_position.lon());
    z = EARTH_RADIUS_IN_METERS * sin(DEG_TO_RAD * my_position.lat());

    rtreePOI.query(bgi::nearest(point(x, y, z), 1), std::back_inserter(temp));

    return temp[0].second;
}


// returns closest intersection (helper)

unsigned map_data::find_closest_intersection_Rtree(LatLon my_position) {
    std::vector<value> temp;
    double x, y, z;

    x = EARTH_RADIUS_IN_METERS * cos(DEG_TO_RAD * my_position.lat()) * cos(DEG_TO_RAD * my_position.lon());
    y = EARTH_RADIUS_IN_METERS * cos(DEG_TO_RAD * my_position.lat()) * sin(DEG_TO_RAD * my_position.lon());
    z = EARTH_RADIUS_IN_METERS * sin(DEG_TO_RAD * my_position.lat());

    rtreeIntersec.query(bgi::nearest(point(x, y, z), 1), std::back_inserter(temp));

    return temp[0].second;
}

double map_data::computeArea(std::vector<t_point> vertices, unsigned nPoint) const {
    double area = 0; // Accumulates area in the loop
    int j = nPoint - 1; // The last vertex is the 'previous' one to the first

    for (unsigned i = 0; i < nPoint; i++) {
        area = area + (vertices[j].x + vertices[i].x) * (vertices[j].y - vertices[i].y);
        j = i; //j is previous vertex to i
    }
    return abs(area / 2);
}


// making x and y coordinates from formula given in milestone 1

t_point map_data::LatLon_to_t_point(LatLon point1) {
    float X = point1.lon() * cos(avg_latitude * DEG_TO_RAD);
    float Y = point1.lat();

    return t_point(X, Y);
}

LatLon map_data::t_point_to_LatLon(t_point point1) {
    float lat = point1.y;
    float lon = point1.x / cos(avg_latitude * DEG_TO_RAD);

    return LatLon(lat, lon);
}

std::vector<f_struct> map_data::get_L1_Features() {
    return L1_Features;
}

std::vector<f_struct> map_data::get_L2_Features() {
    return L2_Features;
}

std::vector<f_struct> map_data::get_L3_Features() {
    return L3_Features;
}

std::vector<f_struct> map_data::get_L4_Features() {
    return L4_Features;
}

std::vector<f_struct> map_data::get_L5_Features() {
    return L5_Features;
}

unsigned map_data::findNodeIndex(OSMID id) {
    std::unordered_map<OSMID, unsigned>::const_iterator it = layer1Nodes.find(id);
    if (it != layer1Nodes.end())
        return it->second;
    else
        return 0;
}

unsigned map_data::findWayIndex(OSMID id) {
    std::unordered_map<OSMID, unsigned>::const_iterator it = layer1Ways.find(id);
    if (it != layer1Ways.end())
        return it->second;
    else
        return 0;
}

bool map_data::is_longer(const f_struct& object1, const f_struct& object2) {
    return (object1.length > object2.length);
}

std::vector<poi_struct> map_data::get_L1_POI() {
    return L1_POI;
}

std::vector<poi_struct> map_data::get_L2_POI() {
    return L2_POI;
}

std::vector<poi_struct> map_data::get_L3_POI() {
    return L3_POI;
}

std::vector<poi_struct> map_data::get_L4_POI() {
    return L4_POI;
}

std::vector<poi_struct> map_data::get_L5_POI() {
    return L5_POI;
}

