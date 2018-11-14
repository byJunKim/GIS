#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <cairo.h>
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "StreetsDatabaseAPI.h"
//#include "OSMDatabaseAPI.h"
#include "graphics.h"
#include "map_data.h"
#include "Feature.h"
#include "surfaces.h"
#include "math.h"
#include "fixed_menu.h"
#include "map_list.h"

#define SEARCH_POINT_OFFSET 0.0005
#define SEARCH_PATH_OFFSET 0.008
#define INSTRUCTION_X 500//-57.532253
#define INSTRUCTION_Y 65 //43.843880
#define PI_IN_DEGREE 180

// HELPER FUNCTIONS AND VARIABLES //
search_bar_data s_data;
void search_point();
map_list mapList;

// state variables and containers

void mouse_click(float x, float y, t_event_buttonPressed event);
void keyboard_press(char c, int keysym);
void path_clicked();
void two_intersections_path();
void intersection_to_poi_path();
void intersection_clicked();
void poi_clicked();
void street_clicked();
void find_point(void (*draw_screen_ptr) (void));
void find_intersection(void (*draw_screen_ptr) (void));
I_array find_intersection_for_path();
void find_street(void (*draw_screen_ptr) (void));
void find_poi(void (*draw_screen_ptr) (void));
void clear_point(void (*draw_screen_ptr) (void));
void clear_arrays();
void change_map(void (*draw_screen_ptr) (void));
void help(void (*draw_screen_ptr) (void));

void draw_screen();
void draw_streets_and_features();
void draw_segments(unsigned num_of_segments);
void draw_segments(std::vector<StreetSegmentIndex> segmentsToDraw);
void draw_segments(std::vector<s_struct> segmentsToDraw);
void draw_PointOfInterest();
void draw_surfaces(std::vector<poi_struct> surfaces_to_draw);
void draw_firstLayerSurfaces(std::string typeName, t_point pt, std::string name);
void draw_secondLayerSurfaces(std::string typeName, t_point pt, std::string name);
void draw_thirdLayerSurfaces(std::string typeName, t_point temp, std::string name);
void draw_fourthLayerSurfaces(std::string typeName, t_point pt, std::string name);
void draw_fifthLayerSurfaces(std::string typeName, t_point pt, std::string name);
void draw_features(std::vector<f_struct> features_to_draw);
void draw_find_points();
void zoom_find_points(t_point);
void zoom_find_path(t_point, t_point);
void draw_text(std::vector<s_struct> segments_to_draw);
void draw_subwayLines();
void draw_path(std::vector<unsigned> path);
void display_directions();
char get_turning_direction(double angle1, double angle2, int current_dir, int next_dir);


//m4 helper
void draw_intersections(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const Seg_array segmentsToDraw);


std::string map_path;
std::vector<StreetSegmentIndex> streetSegments;

//These variables are global to be used in set_visible_world
double maximum_latitude;
double minimum_latitude;
double maximum_longitude;
double minimum_longitude;
double avg_latitude;
double initial_area;
bool help_pressed;
bool draw_directions;
bool wait_for_answer;
unsigned layer;
// calls the functions to set up map window

void draw_map() {
    mapData->right_click = false;
    s_data.mapChange = false;
    help_pressed = false;
    draw_directions = false;
    wait_for_answer = false;

    // Setting background color and window name
    init_graphics("City Map", t_color(240, 240, 230));

    // Setting the max min coordinates of map to max min XY coordinates
    //The first two parameters are the bottom left corner, last two are the top right corner
    set_visible_world(
            minimum_longitude * cos(avg_latitude * DEG_TO_RAD),
            minimum_latitude,
            maximum_longitude * cos(avg_latitude * DEG_TO_RAD),
            maximum_latitude
            );

    if (mapData->button_load) {
        // buttons for extra features
        //create_button("Window", "Find", find_point);
        //create_button("Find", "Clear", clear_point);
        create_button("Window", "Clear", clear_point);
        create_button("Clear", "Change Map", change_map);
        create_button("Change Map", "Help", help);
        mapData->button_load = false;
    }

    //add all the names into tire after loading the map
    mapList.add_intersection_names();

    initial_area = get_visible_world().area();

    // enable keyboard input
    set_keypress_input(true);

    // The event_loop will call 4 functions repeatedly in a loop
    event_loop(mouse_click, nullptr, keyboard_press, draw_screen);

    //    close_graphics();
    std::cout << "got out" << std::endl;
    return;

}


// main drawing function for screen

void draw_screen() {

    if (!s_data.mapChange) {
        clearscreen();

        draw_streets_and_features();

        draw_PointOfInterest();

//        if (mapData->draw_subway) {
//
//            draw_subwayLines();
//
//        }

        draw_find_points();

        draw_check_boxes();

        draw_fixed_menu();

        if (draw_directions) {
            set_coordinate_system(GL_SCREEN);
            display_directions();
            set_coordinate_system(GL_WORLD);
        }
        
        draw_intersections(mapData->deliveriesTemp, mapData->depotsTemp, mapData -> pathTemp);


        copy_off_screen_buffer_to_screen();

        set_drawing_buffer(OFF_SCREEN);

    }
}


/*
20 km/hr - public parks (e.g. Exhibition Place and High Park)30 km/hr - streets with traffic calming devices (i.e. speed humps) installed

40 km/hr -, local streets adjacent schools and parks, and in neighbourhood communities

50 km/hr - most local streets - this is the default speed limit in the City in the absence of signage (Note: Signs are posted at all entry points to the City from major roadways, to advise to this effect)

60 km/hr - busy collector and arterial roadways

70/80 km/hr - controlled access arterials (i.e. busy arterial roads accessing or egressing highways like portions of Hwy 27 and the W.R. Allen Expressway)

90 km/hr - controlled access highways/expressways (e.g. most of F.G. Gardiner Expressway and Don Valley Parkway)

100 km/hr - the maximum speed limit that can be imposed by the City (e.g. westernmost portion of F.G. Gardiner Expressway)
 */


//draws filtered streets with lines depending on the current zoom level

void draw_streets_and_features() {

    //screen area never changes
    //world area changes depending on user's zoom level
    layer = 0;

    //Only draws big streets that have more than 50 intersections
    //only drawing big features that have areas > 0.5
    if (!LOD_area_test(initial_area / (AREA_DIVISION_FACTOR / 2))) {
        layer = 1;
        draw_features(mapData->get_L1_Features());
        setcolor_by_name("white");
        draw_segments(mapData->getStreetSegment2());
        setcolor_by_name("orange");
        draw_segments(mapData->getHighwaySegment());
        setcolor_by_name("black");
        //only drawing small streets that have more than 20 intersections
        //only drawing big and medium features that have areas > 0.2
    } else if (LOD_area_test(initial_area / (AREA_DIVISION_FACTOR / 2)) && !LOD_area_test(initial_area / (10 * (AREA_DIVISION_FACTOR / 2)))) {
        layer = 2;
        draw_features(mapData->get_L2_Features());
        setcolor_by_name("white");
        draw_segments(mapData->getStreetSegment2());
        setcolor_by_name("orange");
        draw_segments(mapData->getHighwaySegment());
        setcolor_by_name("black");

        setfontsize(12);
        draw_text(mapData->getHighwaySegment());
        //drawing all the street segments
        //drawing all features
    } else if (LOD_area_test(initial_area / (10 * (AREA_DIVISION_FACTOR / 2))) && !LOD_area_test(initial_area / (100 * (AREA_DIVISION_FACTOR / 2)))) {
        layer = 3;
        draw_features(mapData->get_L3_Features());
        setcolor_by_name("white");
        draw_segments(mapData->getStreetSegment3());
        setcolor_by_name("orange");
        draw_segments(mapData->getHighwaySegment());
        setcolor_by_name("black");
        setfontsize(11);
        draw_text(mapData->getHighwaySegment());
        draw_text(mapData->getStreetSegment1());
        //drawing all the street segments
        //drawing all features
    } else if (LOD_area_test(initial_area / (100 * 5)) && !LOD_area_test(initial_area / (1000 * (AREA_DIVISION_FACTOR / 2)))) {
        layer = 4;
        draw_features(mapData->get_L4_Features());
        setcolor_by_name("white");
        draw_segments(mapData->getStreetSegment4());
        setcolor_by_name("orange");
        draw_segments(mapData->getHighwaySegment());
        setcolor_by_name("black");
        setfontsize(10);
        draw_text(mapData->getStreetSegment3());
        draw_text(mapData->getHighwaySegment());
        //drawing all the street segments
        //drawing all features
    } else if (LOD_area_test(initial_area / (1000 * (AREA_DIVISION_FACTOR / 2) && !LOD_area_test(initial_area / (10000 * (AREA_DIVISION_FACTOR / 2)))))) {
        layer = 5;
        draw_features(mapData->get_L5_Features());
        setcolor_by_name("white");
        draw_segments(mapData->get_all_segments());
        setcolor_by_name("orange");
        draw_segments(mapData->getHighwaySegment());
        setcolor_by_name("black");
        setfontsize(10);
        draw_text(mapData->get_all_segments());
    } else { // this layer is to print street names more frequently
        layer = 6;
        draw_features(mapData->get_L5_Features());
        setcolor_by_name("white");
        draw_segments(mapData->get_all_segments());
        setcolor_by_name("orange");
        draw_segments(mapData->getHighwaySegment());
        setcolor_by_name("black");
        setfontsize(10);
        draw_text(mapData->get_all_segments());
    }
}


//draws only the streets contained in the pass-in vector

void draw_segments(std::vector<StreetSegmentIndex> segmentsToDraw) {
    //LatLon points for start and end points and inbetween
    LatLon from, to, c1, c2;
    //two t_points for from and to in cartesian
    t_point coordinateFrom, coordinateTo;
    unsigned j = 0;


    //outer for loop to go through all segments in the vector to be drawn
    for (unsigned i = 0; i < segmentsToDraw.size(); ++i) {
        from = getIntersectionPosition(getStreetSegmentInfo(segmentsToDraw[i]).from);
        to = getIntersectionPosition(getStreetSegmentInfo(segmentsToDraw[i]).to);

        coordinateFrom = mapData->LatLon_to_t_point(from);
        coordinateTo = mapData->LatLon_to_t_point(to);

        //no curve point
        if (getStreetSegmentInfo(segmentsToDraw[i]).curvePointCount == 0) {
            drawline(coordinateFrom, coordinateTo);
        } else {
            j = 0;
            drawline(coordinateFrom, mapData->LatLon_to_t_point(getStreetSegmentCurvePoint(segmentsToDraw[i], j)));

            // inner for loop to draw lines between curve points
            for (; j < getStreetSegmentInfo(segmentsToDraw[i]).curvePointCount - 1; ++j) {
                c1 = getStreetSegmentCurvePoint(segmentsToDraw[i], j);
                c2 = getStreetSegmentCurvePoint(segmentsToDraw[i], j + 1);
                drawline(mapData->LatLon_to_t_point(c1), mapData->LatLon_to_t_point(c2));
            }
            //last piece of a segment
            c2 = getStreetSegmentCurvePoint(segmentsToDraw[i], j);
            drawline(mapData->LatLon_to_t_point(c2), coordinateTo);
        }
    }
}



//draws only the streets contained in the pass-in vector

void draw_segments(std::vector<s_struct> segmentsToDraw) {

    //LatLon points for start and end points and inbetween
    LatLon from, to, c1, c2;
    //two t_points for from and to in cartesian
    t_point coordinateFrom, coordinateTo;
    unsigned numOfCurvePts = 0;
    unsigned j = 0;


    //outer for loop to go through all segments in the vector to be drawn
    for (unsigned i = 0; i < segmentsToDraw.size(); ++i) {
        numOfCurvePts = getStreetSegmentInfo(segmentsToDraw[i].index).curvePointCount;

        from = getIntersectionPosition(getStreetSegmentInfo(segmentsToDraw[i].index).from);
        to = getIntersectionPosition(getStreetSegmentInfo(segmentsToDraw[i].index).to);

        coordinateFrom = mapData->LatLon_to_t_point(from);
        coordinateTo = mapData->LatLon_to_t_point(to);

        //no curve point

        setlinewidth(segmentsToDraw[i].width);

        if (numOfCurvePts == 0) {
            drawline(coordinateFrom, coordinateTo);
        } else {
            j = 0;
            drawline(coordinateFrom, mapData->LatLon_to_t_point(getStreetSegmentCurvePoint(segmentsToDraw[i].index, j)));

            // inner for loop to draw lines between curve points
            for (; j < numOfCurvePts - 1; ++j) {
                c1 = getStreetSegmentCurvePoint(segmentsToDraw[i].index, j);
                c2 = getStreetSegmentCurvePoint(segmentsToDraw[i].index, j + 1);
                drawline(mapData->LatLon_to_t_point(c1), mapData->LatLon_to_t_point(c2));
            }
            //last piece of a segment
            c2 = getStreetSegmentCurvePoint(segmentsToDraw[i].index, j);
            drawline(mapData->LatLon_to_t_point(c2), coordinateTo);
        }
    }
}


//draws features in the passed in vector

void draw_features(std::vector<f_struct> features_to_draw) {

    //adjusting line width according to the cuurent zoom level
    if (layer == 1) {
        setlinewidth(4);
    } else if (layer == 2) {
        setlinewidth(3);
    } else if (layer == 3) {
        setlinewidth(2);
    } else {
        setlinewidth(1);
    }



    //using sorted features and data from data structure
    for (FeatureIndex i = 0; i < features_to_draw.size(); ++i) {
        setcolor(features_to_draw[i].color);
        //draw lines for open features
        if (features_to_draw[i].open) {
            for (int j = 0; j < features_to_draw[i].feature_point_count - 1; ++j) {
                drawline(features_to_draw[i].feature_point[j],
                        features_to_draw[i].feature_point[j + 1]);
            }
        } else {
            //draw polygon for closed features
            fillpoly(&(features_to_draw[i].feature_point[0]), features_to_draw[i].feature_point_count);
        }
    }
}



// function for mouse clicks on map

void mouse_click(float x, float y, t_event_buttonPressed event) {
    // if user left clicks
    if (event.button == 1) {
        //user clicked on the search bar
        if (x > scrn_to_world(t_point(40, 140)).x && x < scrn_to_world(t_point(280, 110)).x && y > scrn_to_world(t_point(40, 140)).y && y < scrn_to_world(t_point(280, 110)).y) {
            std::cout << "selected first search bar" << std::endl;
            s_data.selected = true;
            s_data.selected2 = false;

            set_drawing_buffer(ON_SCREEN);

            draw_fixed_menu();

            draw_find_points();

            set_drawing_buffer(OFF_SCREEN);

            mapData->find_search_store2.clear();

            return;

            //user clicked on the second search bar
        } else if (x > scrn_to_world(t_point(40, 180)).x && x < scrn_to_world(t_point(280, 150)).x && y > scrn_to_world(t_point(40, 180)).y && y < scrn_to_world(t_point(280, 150)).y) {
            std::cout << "selected second search bar" << std::endl;
            s_data.selected2 = true;
            s_data.selected = false;

            set_drawing_buffer(ON_SCREEN);

            draw_fixed_menu();

            draw_find_points();

            set_drawing_buffer(OFF_SCREEN);

            return;

            //user clicked on the subway button
        } else if (x > scrn_to_world(t_point(40, 45)).x && x < scrn_to_world(t_point(100, 25)).x && y > scrn_to_world(t_point(40, 45)).y && y < scrn_to_world(t_point(100, 25)).y) {
            mapData->draw_subway = !mapData->draw_subway;
            std::cout << mapData->draw_subway << std::endl;
            //user clicked on path check box
        } else if (x > scrn_to_world(t_point(40, 100)).x && x < scrn_to_world(t_point(55, 85)).x && y > scrn_to_world(t_point(40, 100)).y && y < scrn_to_world(t_point(55, 85)).y) {
            std::cout << "checked path check box" << std::endl;
            s_data.pathChecked = true;
            clear_arrays();

            //user clicked on intersection check box    
        } else if (x > scrn_to_world(t_point(93, 100)).x && x < scrn_to_world(t_point(108, 85)).x && y > scrn_to_world(t_point(93, 100)).y && y < scrn_to_world(t_point(108, 85)).y) {
            std::cout << "checked intersection check box" << std::endl;
            s_data.intersectionChecked = true;
            s_data.poiChecked = false;
            clear_arrays();

            //user clicked on poi check box    
        } else if (x > scrn_to_world(t_point(186, 100)).x && x < scrn_to_world(t_point(201, 85)).x && y > scrn_to_world(t_point(186, 100)).y && y < scrn_to_world(t_point(201, 85)).y) {
            std::cout << "checked POI check box" << std::endl;
            s_data.poiChecked = true;
            s_data.intersectionChecked = false;
            clear_arrays();

            //user clicked on street check box    
        } else if (x > scrn_to_world(t_point(232, 100)).x && x < scrn_to_world(t_point(247, 85)).x && y > scrn_to_world(t_point(232, 100)).y && y < scrn_to_world(t_point(247, 85)).y) {
            std::cout << "checked street check box" << std::endl;
            s_data.streetChecked = true;
            clear_arrays();

        } else {
            s_data.selected = false;
            s_data.selected2 = false;
            s_data.pathChecked = false;
            s_data.intersectionChecked = false;
            s_data.poiChecked = false;
            s_data.streetChecked = false;


            //clear path for right click
            mapData->right_click = false;
            draw_directions = false;

            // clear last search results
            clear_arrays();

            s_data.query = "";

            // convert position to LatLon on map
            t_point tpoint = t_point(x, y);
            LatLon llpoint = mapData->t_point_to_LatLon(tpoint);

            // obtain closest POI and Intersection
            POIIndex poi = find_closest_point_of_interest(llpoint);
            IntersectionIndex intersection = find_closest_intersection(llpoint);

            // convert positions to onscreen distances
            double poi_pos = find_distance_between_two_points(mapData->t_point_to_LatLon(world_to_scrn(mapData->LatLon_to_t_point(llpoint))), mapData->t_point_to_LatLon(world_to_scrn(mapData->LatLon_to_t_point(getPointOfInterestPosition(poi)))));
            double intersection_pos = find_distance_between_two_points(mapData->t_point_to_LatLon(world_to_scrn(mapData->LatLon_to_t_point(llpoint))), mapData->t_point_to_LatLon(world_to_scrn(mapData->LatLon_to_t_point(getIntersectionPosition(intersection)))));

            // compare onscreen distances to see whether they should be highlighted or not
            // afterwards, adds points to global array to be highlighted and drawn
            if (poi_pos < intersection_pos && poi_pos < CLICKDISTANCE) {
                mapData->click = true;
                mapData->query = getPointOfInterestName(poi);
                std::cout << mapData->query << std::endl;
                mapData->find_poi_array.clear();
                mapData->find_intersection_array.clear();
                mapData->find_poi_array.push_back(poi);

                set_drawing_buffer(ON_SCREEN);

                draw_find_points();

                draw_fixed_menu();

                set_drawing_buffer(OFF_SCREEN);

            } else if (poi_pos > intersection_pos && intersection_pos < CLICKDISTANCE) {
                mapData->click = true;
                mapData->query = getIntersectionName(intersection);
                std::cout << mapData->query << std::endl;
                mapData->find_poi_array.clear();
                mapData->find_intersection_array.clear();
                mapData->find_intersection_array.push_back(intersection);

                set_drawing_buffer(ON_SCREEN);

                draw_find_points();

                draw_fixed_menu();

                set_drawing_buffer(OFF_SCREEN);
            }
        }
    } else if (event.button == 3) { //right click

        // clear last search results
        mapData->find_intersection_array.clear();
        mapData->find_street_array.clear();
        mapData->find_poi_array.clear();
        mapData->errors.clear();
        mapData->autoCompletion.clear();
        mapData->find_search_store.clear();
        mapData->find_search_store2.clear();

        if (mapData->find_intersections_path.size() == 2) {
            mapData->find_intersections_path.clear();
        }


        s_data.query = "";

        // convert position to LatLon on map
        t_point tpoint = t_point(x, y);
        LatLon llpoint = mapData->t_point_to_LatLon(tpoint);

        // obtain closest POI and Intersection
        IntersectionIndex intersection = find_closest_intersection(llpoint);

        // convert positions to onscreen distances
        double intersection_pos = find_distance_between_two_points(mapData->t_point_to_LatLon(world_to_scrn(mapData->LatLon_to_t_point(llpoint))), mapData->t_point_to_LatLon(world_to_scrn(mapData->LatLon_to_t_point(getIntersectionPosition(intersection)))));

        // compare onscreen distances to see whether they should be highlighted or not
        // afterwards, adds points to global array to be highlighted and drawn
        if (mapData->find_intersections_path.size() == 0) {
            if (intersection_pos < CLICKDISTANCE) {
                mapData->right_click = true;
                mapData->find_intersections_path.push_back(intersection);
                //
                set_drawing_buffer(ON_SCREEN);

                draw_find_points();

                draw_fixed_menu();

                set_drawing_buffer(OFF_SCREEN);
            }
        } else {
            if (intersection_pos < CLICKDISTANCE) {
                mapData->find_intersections_path.push_back(intersection);

                set_drawing_buffer(ON_SCREEN);

                draw_find_points();

                draw_fixed_menu();

                set_drawing_buffer(OFF_SCREEN);
            }
        }
    }


    draw_screen();


}


// function for keyboard input on search

void keyboard_press(char c, int keysym) {
    // function to handle keyboard press event, the ASCII character is returned
    // along with an extended code (keysym) on X11 to represent non-ASCII
    // characters like the arrow keys.

    if (s_data.selected || s_data.selected2) {
        switch (c) {
            default:
                break;
            case 'a':
                s_data.query += c;
                break;
            case 'b':
                s_data.query += c;
                break;
            case 'c':
                s_data.query += c;
                break;
            case 'd':
                s_data.query += c;
                break;
            case 'e':
                s_data.query += c;
                break;
            case 'f':
                s_data.query += c;
                break;
            case 'g':
                s_data.query += c;
                break;
            case 'h':
                s_data.query += c;
                break;
            case 'i':
                s_data.query += c;
                break;
            case 'j':
                s_data.query += c;
                break;
            case 'k':
                s_data.query += c;
                break;
            case 'l':
                s_data.query += c;
                break;
            case 'm':
                s_data.query += c;
                break;
            case 'n':
                s_data.query += c;
                break;
            case 'o':
                s_data.query += c;
                break;
            case 'p':
                s_data.query += c;
                break;
            case 'q':
                s_data.query += c;
                break;
            case 'r':
                s_data.query += c;
                break;
            case 's':
                s_data.query += c;
                break;
            case 't':
                s_data.query += c;
                break;
            case 'u':
                s_data.query += c;
                break;
            case 'v':
                s_data.query += c;
                break;
            case 'w':
                s_data.query += c;
                break;
            case 'x':
                s_data.query += c;
                break;
            case 'y':
                s_data.query += c;
                break;
            case 'z':
                s_data.query += c;
                break;
            case 'A':
                s_data.query += c;
                break;
            case 'B':
                s_data.query += c;
                break;
            case 'C':
                s_data.query += c;
                break;
            case 'D':
                s_data.query += c;
                break;
            case 'E':
                s_data.query += c;
                break;
            case 'F':
                s_data.query += c;
                break;
            case 'G':
                s_data.query += c;
                break;
            case 'H':
                s_data.query += c;
                break;
            case 'I':
                s_data.query += c;
                break;
            case 'J':
                s_data.query += c;
                break;
            case 'K':
                s_data.query += c;
                break;
            case 'L':
                s_data.query += c;
                break;
            case 'M':
                s_data.query += c;
                break;
            case 'N':
                s_data.query += c;
                break;
            case 'O':
                s_data.query += c;
                break;
            case 'P':
                s_data.query += c;
                break;
            case 'Q':
                s_data.query += c;
                break;
            case 'R':
                s_data.query += c;
                break;
            case 'S':
                s_data.query += c;
                break;
            case 'T':
                s_data.query += c;
                break;
            case 'U':
                s_data.query += c;
                break;
            case 'V':
                s_data.query += c;
                break;
            case 'W':
                s_data.query += c;
                break;
            case 'X':
                s_data.query += c;
                break;
            case 'Y':
                s_data.query += c;
                break;
            case 'Z':
                s_data.query += c;
                break;
            case '0':
                s_data.query += c;
                break;
            case '1':
                s_data.query += c;
                break;
            case '2':
                s_data.query += c;
                break;
            case '3':
                s_data.query += c;
                break;
            case '4':
                s_data.query += c;
                break;
            case '5':
                s_data.query += c;
                break;
            case '6':
                s_data.query += c;
                break;
            case '7':
                s_data.query += c;
                break;
            case '8':
                s_data.query += c;
                break;
            case '9':
                s_data.query += c;
                break;
            case '&':
                s_data.query += c;
                break;
            case '<':
                s_data.query += c;
                break;
            case '>':
                s_data.query += c;
                break;
            case '\'':
                s_data.query += c;
                break;
            case '_':
                s_data.query += c;
                break;
            case '-':
                s_data.query += c;
                break;
        }

        switch (keysym) {
            default:
                break;
            case XK_BackSpace:
                s_data.query = s_data.query.substr(0, s_data.query.length() - 1);
                break;
            case XK_Delete:
                s_data.query = s_data.query.substr(0, s_data.query.length() - 1);
                break;
            case XK_Escape:
                s_data.query = "";
                break;
            case XK_space:
                s_data.query += " ";
                break;
            case XK_minus:
                s_data.query += "-";
                break;
            case 65293:
                // clear last search results
                mapData->find_intersection_array.clear();
                mapData->find_street_array.clear();
                mapData->find_poi_array.clear();
                mapData->right_click = false;
                mapData->autoCompletion.clear();

                mapData->query = s_data.query;
                if (!s_data.mapChange) {
                    //checking which user query it is
                    if (s_data.pathChecked) {
                        path_clicked();
                        mapData->enterWarning = false;
                    } else if (s_data.intersectionChecked && !s_data.pathChecked) {
                        intersection_clicked();
                        mapData->enterWarning = false;
                    } else if (s_data.poiChecked && !s_data.pathChecked) {
                        poi_clicked();
                        mapData->enterWarning = false;
                    } else if (s_data.streetChecked) {
                        street_clicked();
                        mapData->enterWarning = false;
                    } else {
                        //if user forgets to check which type of search they want to do
                        mapData->errors.clear();
                        mapData->errors.push_back("Oh no!");
                        mapData->errors.push_back("You forgot to tell us which type of search");
                        mapData->errors.push_back("you are looking for!");
                        mapData->errors.push_back("Please check the box on the top");
                        mapData->errors.push_back("and search again :)");
                    }
                } else {
                    change_map(draw_screen);
                }
                break;
        }
    }


    set_drawing_buffer(ON_SCREEN);

    draw_result_text();

    draw_fixed_menu();

    draw_find_points();

    set_drawing_buffer(OFF_SCREEN);
}

//a function to check user query for the path inputs

void path_clicked() {
    if (s_data.intersectionChecked) {
        two_intersections_path();
    } else if (s_data.poiChecked) {
        intersection_to_poi_path();
    }

}

void intersection_to_poi_path() {
    mapData->find_poi_array.clear();
    mapData->autoCompletion.clear();
    //parse the first search bar ---- intersection
    if (s_data.selected) {
        //check for path query results
        std::string delimiter = " & ";
        std::string temp = s_data.query;
        std::string street1, street2;

        //for auto completion
        mapData->find_search_store.push_back(s_data.query);
        mapData->autoCompletion = mapList.trie_Intersection.autoComplete(temp);

        //user inputs invalid command
        if (temp.find(delimiter) == std::string::npos || temp.size() <= 0) {
            mapData->errors.clear();
            mapData->errors.push_back("Incorrect format! Please follow this format:");
            mapData->errors.push_back("Yonge Street & College Street");
            mapData->errors.push_back("Please try again :) ");
        } else {
            mapData->errors.clear();
            //parse the user input to two street names by using string functions substr and erase
            street1 = temp.substr(0, temp.find(delimiter));
            temp.erase(0, temp.find(delimiter) + delimiter.length());
            street2 = temp;

            //get the intersection id from 2 street names
            I_array tempIntersections = find_intersection_ids_from_street_names(street1, street2);

            //push the intersection to the global intersection path array 
            if (street1.length() > 0 && street2.length() > 0 && tempIntersections.size() > 0) {
                mapData->find_intersections_path.push_back(tempIntersections[0]);
            }

            //clear the query data to prepare for the next input (second search bar)
            if (mapData->find_search_store2.size() == 0) {
                s_data.query.clear();
            }

            if (tempIntersections.size() <= 0 && mapData->find_search_store2.size() != 0) {
                mapData->errors.clear();
                mapData->errors.push_back("Can not find result :(");
                mapData->errors.push_back("Please follow this format:");
                mapData->errors.push_back("Yonge Street & College Street");
            }

            tempIntersections.clear();
        }
    }

    if (s_data.selected2) {
        std::string temp2 = s_data.query;

        //for auto completion
        mapData->autoCompletion = mapList.trie_POI.autoComplete(temp2);

        mapData->find_search_store2.push_back(s_data.query);

        P_array temp = mapData->getPOINameToID(mapData->query);

        mapData->errors.clear();

        if (temp.size() <= 0) {
            mapData->errors.push_back("No results found for this poi :(");
            mapData->errors.push_back("Please check if the input is correct");
        } else {
            mapData->find_intersections_path.push_back(temp[0]);
        }

        temp.clear();
    }
}

void two_intersections_path() {

    //check for path query results
    std::string delimiter = " & ";
    std::string temp = s_data.query;
    std::string street1, street2;
    mapData->autoCompletion.clear();
    
        //error checking if user entered the same thing
    if (mapData->find_search_store2.size() > 0 && mapData->find_search_store.size() > 0 && mapData->find_intersections_path.size() == 2) {
        if (mapData->find_intersections_path[0] == mapData->find_intersections_path[1]) {
            return;
        }
    }

    //error checking for path finding -- intersection
    if (s_data.selected) {
        mapData->find_search_store.push_back(s_data.query);
        mapData->autoCompletion = mapList.trie_Intersection.autoComplete(temp);
    }

    if (s_data.selected2) {
        mapData->find_search_store2.push_back(s_data.query);
        mapData->autoCompletion = mapList.trie_Intersection.autoComplete(temp);
    }

    //user inputs invalid command
    if (temp.find(delimiter) == std::string::npos || temp.size() <= 0) {
        mapData->errors.clear();
        mapData->errors.push_back("Incorrect format! Please follow this format:");
        mapData->errors.push_back("Yonge Street & College Street");
        mapData->errors.push_back("Please try again :) ");
    } else {
        mapData->errors.clear();
        //parse the user input to two street names by using string functions substr and erase
        street1 = temp.substr(0, temp.find(delimiter));
        temp.erase(0, temp.find(delimiter) + delimiter.length());
        street2 = temp;

        //get the intersection id from 2 street names
        I_array tempIntersections = find_intersection_ids_from_street_names(street1, street2);

        //push the intersection to the global intersection path array 
        if (street1.length() > 0 && street2.length() > 0 && tempIntersections.size() > 0) {
            mapData->find_intersections_path.push_back(tempIntersections[0]);
            std::cout << mapData->find_intersections_path[0] << std::endl;
            std::cout << mapData->find_intersections_path.size() << std::endl;
        }

        //clear the query data to prepare for the next input (second search bar)
        if (mapData->find_search_store2.size() == 0) {
            s_data.query.clear();
        }

        if (tempIntersections.size() <= 0 && mapData->find_search_store2.size() != 0) {
            mapData->errors.clear();
            mapData->errors.push_back("Can not find result :(");
            mapData->errors.push_back("Please follow this format:");
            mapData->errors.push_back("Yonge Street & College Street");
        }

        tempIntersections.clear();
    }

}

void poi_clicked() {
    std::string temp = mapData->query;
    mapData->autoCompletion = mapList.trie_POI.autoComplete(temp);

    mapData->find_poi_array = mapData->getPOINameToID(mapData->query);
    mapData->errors.clear();

    if (mapData->find_poi_array.size() <= 0) {
        mapData->errors.push_back("No results found for this poi :(");
        mapData->errors.push_back("Please check if the input is correct");
    }
}

void street_clicked() {
    std::string temp = mapData->query;
    mapData->autoCompletion = mapList.trie_Street.autoComplete(temp);

    S_array streets = mapData->getStreetNameToID(mapData->query);
    mapData->errors.clear();
    if (streets.size() > 0) {
        // put segments of found street into global array for later drawing
        for (StreetIndex i : streets) {
            mapData->find_street_array.push_back(find_street_street_segments(i));
        }
    } else {
        mapData->errors.push_back("No results found for this street :(");
        mapData->errors.push_back("Please check if the input is correct");
    }

}

void intersection_clicked() {
    //check for path query results
    std::string delimiter = " & ";
    std::string temp = s_data.query;
    std::string street1, street2;

    mapData->errors.clear();

    mapData->autoCompletion = mapList.trie_Intersection.autoComplete(temp);

    //user inputs invalid command
    if (temp.find(delimiter) == std::string::npos || temp.size() <= 0) {
        mapData->errors.clear();
        mapData->errors.push_back("Incorrect format! Please follow this format:");
        mapData->errors.push_back("Yonge Street & College Street");
        mapData->errors.push_back("Please try again :) ");
    } else {
        //parse the user input to two street names by using string functions substr and erase
        street1 = temp.substr(0, temp.find(delimiter));
        temp.erase(0, temp.find(delimiter) + delimiter.length());
        street2 = temp;

        //get the intersection id from 2 street names
        I_array tempIntersections = find_intersection_ids_from_street_names(street1, street2);

        //push the intersection to the global intersection path array 
        if (street1.length() > 0 && street2.length() > 0 && tempIntersections.size() > 0) {
            mapData->find_intersection_array.push_back(tempIntersections[0]);
        }

        if (tempIntersections.size() <= 0) {
            mapData->errors.clear();
            mapData->errors.push_back("Can not find result :(");
            mapData->errors.push_back("Please follow this format:");
            mapData->errors.push_back("Yonge Street & College Street");
        }

        tempIntersections.clear();
    }
}

void draw_find_points() {
    setcolor(BLACK);

    // checks each of the global arrays for which results to highlight
    for (IntersectionIndex i : mapData->find_intersection_array) {
        // position variables
        LatLon llpoint = getIntersectionPosition(i);
        t_point tpoint = mapData->LatLon_to_t_point(llpoint);

        // zooms into point if only one is found
        if (mapData->find_intersection_array.size() == 1 && !mapData->click) zoom_find_points(tpoint);

        // draw pin
        draw_surface(pin, tpoint.x - 0.0000125, tpoint.y + 0.00003);

    }

    if (mapData->right_click) {
        // position variables
        LatLon RCpoint = getIntersectionPosition(mapData->find_intersections_path[0]);
        t_point tpoint1 = mapData->LatLon_to_t_point(RCpoint);

        // draw pin
        draw_surface(bluePin, tpoint1.x - 0.0000125, tpoint1.y + 0.00003);

    }

    for (POIIndex i : mapData->find_poi_array) {
        LatLon llpoint = getPointOfInterestPosition(i);
        t_point tpoint = mapData->LatLon_to_t_point(llpoint);
        if (mapData->find_poi_array.size() == 1 && !mapData->click) zoom_find_points(tpoint);
        draw_surface(pin, tpoint.x - 0.0000125, tpoint.y + 0.00003);
    }

    for (S_array i : mapData->find_street_array) {
        draw_segments(i);
    }

    //draw the paths + error chekcing for the same intersection
    if (mapData -> find_intersections_path.size() == 2 && (mapData -> find_intersections_path[0] != mapData -> find_intersections_path[1])) {
        if (s_data.intersectionChecked) {
            mapData->path = find_path_between_intersections(mapData -> find_intersections_path[0], mapData -> find_intersections_path[1], 10);
            draw_path(mapData->path);
        } else if (s_data.poiChecked) {
            std::string temp = getPointOfInterestName(mapData -> find_intersections_path[1]);
            mapData->path = find_path_to_point_of_interest(mapData -> find_intersections_path[0], temp, 10);
            draw_path(mapData->path);
        } else {
            mapData->path = find_path_between_intersections(mapData -> find_intersections_path[0], mapData -> find_intersections_path[1], 10);
            draw_path(mapData->path);
        }
    }

}


// zooms user interface into found point

void zoom_find_points(t_point search_point) {
    set_visible_world(search_point.x - SEARCH_POINT_OFFSET, search_point.y - SEARCH_POINT_OFFSET, search_point.x + SEARCH_POINT_OFFSET, search_point.y + SEARCH_POINT_OFFSET);
}

// zooms user interface into the path

void zoom_find_path(t_point start, t_point end) {
    t_point bottomLeft;
    t_point topRight;

    // setting up screen coordinates based on whether start/end points are greater/smaller
    if (start.x > end.x) {
        topRight.x = start.x;
        bottomLeft.x = end.x;
    } else {
        topRight.x = end.x;
        bottomLeft.x = start.x;
    }

    if (start.y > end.y) {
        topRight.y = start.y;
        bottomLeft.y = end.y;
    } else {
        topRight.y = end.y;
        bottomLeft.y = start.y;
    }

    set_visible_world(bottomLeft.x - SEARCH_PATH_OFFSET, bottomLeft.y - SEARCH_PATH_OFFSET, topRight.x + SEARCH_PATH_OFFSET, topRight.y + SEARCH_PATH_OFFSET);
}

// draws text for streets and oneWay symbols

void draw_text(std::vector<s_struct> segments_to_draw) {

    double angle_ = 0;
    unsigned interval = 0;

    if (layer == 2) {
        interval = 50;
    } else if (layer == 3) {
        interval = 30;
    } else if (layer == 4) {
        interval = 15;
    } else { //layer 5;
        interval = 1;
    }



    for (unsigned i = 0; i < segments_to_draw.size(); i = i + interval) {

        //if the one way sign will be printed, interval becomes 2
        //this is to avoid overlapping between one way signs and street name labels
        if (layer == 5 && getStreetSegmentInfo(segments_to_draw[i].index).oneWay) {
            interval = 2;
        }

        if (getStreetName(getStreetSegmentInfo(segments_to_draw[i].index).streetID) != "<unknown>" &&
                getStreetSegmentInfo(segments_to_draw[i].index).curvePointCount == 0) {
            //label the text in the way the they are in the correct orientations.
            if (segments_to_draw[i].direction == 1 || segments_to_draw[i].direction == 3) {
                angle_ = (segments_to_draw[i].angle)*(180 / PI);
                settextrotation(angle_);
            } else {
                angle_ = -(segments_to_draw[i].angle)*(180 / PI);
                settextrotation(angle_);
            }
            drawtext_in(segments_to_draw[i].position, getStreetName(getStreetSegmentInfo(segments_to_draw[i].index).streetID), FLT_MAX);
            //setting the rotation angle back to zero
            settextrotation(0);
        }


        //draw arrow only in the last zoom level
        //street labels will be printed with interval 2 and arrow will be drawn in between the
        //street names
        if (i + 1 < segments_to_draw.size()) {
            if (layer == 4 || layer == 5 || layer == 6) {
                if (getStreetSegmentInfo(segments_to_draw[i + 1].index).oneWay == true
                        && i + 1 < segments_to_draw.size()) {
                    std::string arrow = "→   →";
                    //grey
                    setcolor(t_color(66, 66, 66, 255));
                    //setting the rotation angle
                    switch (segments_to_draw[i + 1].direction) {
                        case 1: angle_ = (segments_to_draw[i + 1].angle)*(180 / PI);
                            break;
                        case 2: angle_ = 180 - (segments_to_draw[i + 1].angle)*(180 / PI);
                            break;
                        case 3: angle_ = 180 + (segments_to_draw[i + 1].angle)*(180 / PI);
                            break;
                        case 4: angle_ = 360 - (segments_to_draw[i + 1].angle)*(180 / PI);
                            break;
                        default:
                            break;
                    }
                    settextrotation(angle_);
                    drawtext_in(segments_to_draw[i + 1].position, arrow, FLT_MAX);
                    //setting the rotation angle back to zero
                    settextrotation(0);
                }
            }
        }
        }
    }


// clear all the arrays which store user search input
// we need this for check boxes once user checks one of the checkbox
// all the previous stored errors and query are cleared

void clear_arrays() {
    mapData->find_intersection_array.clear();
    mapData->find_street_array.clear();
    mapData->find_poi_array.clear();
    mapData->find_intersections_path.clear();
    mapData->find_search_store.clear();
    mapData->find_search_store2.clear();
    mapData->errors.clear();
    mapData->path.clear();
    mapData->autoCompletion.clear();

    s_data.query = "";
}


// clears points found through user search

void clear_point(void (*draw_screen_ptr) (void)) {
    draw_directions = false;
    mapData->right_click = false;
    mapData->query = "";
    help_pressed = false;
    s_data.pathChecked = false;
    s_data.intersectionChecked = false;
    s_data.poiChecked = false;
    s_data.streetChecked = false;
    s_data.query = "";
    mapData->user_input_map_name = "";
    mapData->enterWarning = false;

    clear_arrays();

    set_visible_world(
            minimum_longitude * cos(avg_latitude * DEG_TO_RAD),
            minimum_latitude,
            maximum_longitude * cos(avg_latitude * DEG_TO_RAD),
            maximum_latitude
            );
    draw_screen_ptr();
}


// change map button function

void change_map(void (*draw_screen_ptr) (void)) {
    // suppress warning
    (void) draw_screen_ptr;

    // setting global map change marker to true
    s_data.mapChange = true;

    clearscreen();

    draw_streets_and_features();

    draw_PointOfInterest();

    draw_find_points();

    draw_check_boxes();

    draw_fixed_menu();

    copy_off_screen_buffer_to_screen();

    set_drawing_buffer(OFF_SCREEN);

    std::pair<int, std::string> *best_guess;
    s_data.mapChange = true;
    std::string user_map;
    std::string temp;


    if (wait_for_answer) {
        mapData->user_ans = s_data.query[0];
        s_data.query.clear();
    } else {
        mapData->user_ans = '\0';
        mapData->user_input_map_name = s_data.query;
        s_data.query.clear();
    }

    user_map = mapData->user_input_map_name;

    bool done_map_switch = false;

    if (!mapData->user_input_map_name.empty() || mapData->user_ans != '\0') {

        for (unsigned i = 0; i < user_map.length(); ++i) {

            if (user_map[i] < 97) {
                //convert all the capitals to lower cases
                user_map[i] = user_map[i] + 32;
            }
        }

        std::string::const_iterator a_it = user_map.begin();
        std::string::const_iterator a_end = user_map.end();
        for (unsigned i = 0; i < mapList.map_names.size(); i++) {
            a_it = user_map.begin();
            int difference = 0;
            std::string::const_iterator b_it = mapList.map_names[i].second.begin();
            std::string::const_iterator b_end = mapList.map_names[i].second.end();
            while (a_it != a_end && b_it != b_end) {
                if (*a_it != *b_it) {
                    difference++;
                }
                a_it++;
                b_it++;
            }
            mapList.map_names[i].first = difference;
        }

        std::sort(mapList.map_names.begin(), mapList.map_names.end());
        best_guess = &(mapList.map_names[0]);
        auto ptr = best_guess;
        int error = best_guess->first;


        while (ptr->first == error) {
            if (ptr->second.length() < best_guess->second.length()) {
                best_guess = ptr;
            }
            ptr++;
        }

        best_guess->first = best_guess->first + abs(best_guess->second.length() - user_map.length());

        std::cout << "loading " << best_guess->second << std::endl;
            map_path = "/cad2/ece297s/public/maps/" + best_guess->second + ".streets.bin";

            done_map_switch = true;

    }

    if (done_map_switch) {
        std::cout << "Closing map\n";
        destroy_button("Clear");
        destroy_button("Change Map");
        destroy_button("Help");

        close_map();
        mapData = NULL;
        std::cout << "Successfully load the map" << std::endl;
        load_map(map_path);
        draw_map();

    }

}

void help(void (*draw_screen_ptr) (void)) {
    // suppress warning
    (void) draw_screen_ptr;

    // automatically draw the surface without buffer for user responsiveness
    set_drawing_buffer(ON_SCREEN);

    set_coordinate_system(GL_SCREEN);

    // obtaining screen coordinates for instruction page
    t_point screenPosition(INSTRUCTION_X, INSTRUCTION_Y);

    // drawing instruction picture
    draw_surface(instruction, screenPosition.x, screenPosition.y);

    set_coordinate_system(GL_WORLD);

    set_drawing_buffer(OFF_SCREEN);

    return;
}



// loops through POI data and 

void draw_PointOfInterest() {

    setcolor(10, 60, 145, 255);
    setfontsize(8);
    settextrotation(0);

    if (!LOD_area_test(initial_area / (15 * AREA_DIVISION_FACTOR))) {
        layer = 1;
        draw_surfaces(mapData->get_L1_POI());
    } else if (LOD_area_test(initial_area / (15 * AREA_DIVISION_FACTOR)) && !LOD_area_test(initial_area / (500 * AREA_DIVISION_FACTOR))) {
        layer = 2;
        draw_surfaces(mapData->get_L2_POI());
        layer = 1;
        draw_surfaces(mapData->get_L1_POI());
    } else if (LOD_area_test(initial_area / (500 * AREA_DIVISION_FACTOR)) && !LOD_area_test(initial_area / (4000 * AREA_DIVISION_FACTOR))) {//&& !LOD_area_test(initial_area / 8000)) {
        layer = 3;
        draw_surfaces(mapData->get_L3_POI());
        layer = 2;
        draw_surfaces(mapData->get_L2_POI());
        layer = 1;
        draw_surfaces(mapData->get_L1_POI());
    } else if (LOD_area_test(initial_area / (4000 * AREA_DIVISION_FACTOR)) && !LOD_area_test(initial_area / (6000 * AREA_DIVISION_FACTOR))) {
        layer = 4;
        draw_surfaces(mapData->get_L4_POI());
        layer = 3;
        draw_surfaces(mapData->get_L3_POI());
        layer = 2;
        draw_surfaces(mapData->get_L2_POI());
        layer = 1;
        draw_surfaces(mapData->get_L1_POI());
    } else if (LOD_area_test(initial_area / (6000 * AREA_DIVISION_FACTOR))) {
        layer = 5;
        draw_surfaces(mapData->get_L5_POI());
        layer = 4;
        draw_surfaces(mapData->get_L4_POI());
        layer = 3;
        draw_surfaces(mapData->get_L3_POI());
        layer = 2;
        draw_surfaces(mapData->get_L2_POI());
        layer = 1;
        draw_surfaces(mapData->get_L1_POI());
    }

}

void draw_surfaces(std::vector<poi_struct> surfaces_to_draw) {
    double bound;
    float y;
    for (POIIndex i = 0; i < surfaces_to_draw.size(); ++i) {
        t_point temp = surfaces_to_draw[i].location;
        if (layer == 1) {
            y = temp.y + 0.0005;
            bound = 0.1;
        } else if (layer == 2) {
            y = temp.y + 0.00005;
            bound = 0.1;
        } else {
            y = temp.y + 0.000005;
            bound = 0.001;
        }

        t_point tempToLeft(temp.x, y);
        std::string typeName = surfaces_to_draw[i].typeName;
        std::string name = surfaces_to_draw[i].name;
        if (typeName == "fast_food") {
            draw_surface(fast_food, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "restaurant") {
            draw_surface(restaurant, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "atm") {
            draw_surface(atm, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "cafe") {
            draw_surface(cafe, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "hotel") {
            draw_surface(hotel, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "library") {
            draw_surface(library, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "bank") {
            draw_surface(bank, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "school") {
            draw_surface(school, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "hospital") {
            draw_surface(hospital, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "fuel") {
            draw_surface(fuel, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "car_rental") {
            draw_surface(car_rental, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "place_of_worship") {
            draw_surface(place_of_worship, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "park") {
            draw_surface(park, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "museum") {
            draw_surface(museum, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else if (typeName == "theatre") {
            draw_surface(theatre, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        } else {
            draw_surface(star, temp.x, temp.y);
            drawtext(tempToLeft, name, bound, bound);
        }
    }
}

/*To draw the proper links between subway stations, we would have to search through all
the OSM relations to find the one that represented the subway line, and then we could use
OSMRelation::members() to get the (ordered) vector of OSMNodes that form the subway
line.*/

//void draw_subwayLines() {
//
//    t_point from, to;
//
//    //vector to store subway nodes
//    std::vector<LatLon> nodesTemp = mapData->get_nodes();
//    //vector to store ways connected to each node
//    std::vector<std::vector < LatLon>> waysOfNodesTemp = mapData->get_waysOfNodes();
//
//    set_coordinate_system(GL_WORLD);
//
//
//    LatLon location;
//    t_point tlocation;
//
//    for (unsigned i = 0; i < getNumberOfNodes(); ++i) {
//        const OSMNode* temp = getNodeByIndex(i);
//
//        for (unsigned j = 0; j < getTagCount(temp); ++j) {
//            std::string key, val;
//            std::tie(key, val) = getTagPair(temp, j);
//
//            if (key == "railway" && val == "subway_entrance") {
//                location = temp->coords();
//                tlocation = mapData->LatLon_to_t_point(location);
//                draw_surface(subway, tlocation.x, tlocation.y);
//                std::cout << tlocation.x << "\t" << tlocation.y << "\n";
//            }
//        }
//    }
//}


//implementation for m3

void draw_path(std::vector<unsigned> path) {
    
    mapData->errors.clear();
    mapData->enterWarning = false;
    
    draw_directions = true;

    LatLon startLL, endLL;
    t_point start, end;

    if (path.size() > 0) {

        //path contains all the street segment id
        startLL = getIntersectionPosition(getStreetSegmentInfo(path[0]).from);
        start = mapData->LatLon_to_t_point(startLL);

        if (getStreetSegmentInfo(path[path.size() - 2]).to == getStreetSegmentInfo(path[path.size() - 1]).from ||
                getStreetSegmentInfo(path[path.size() - 2]).from == getStreetSegmentInfo(path[path.size() - 1]).from) {
            endLL = getIntersectionPosition(getStreetSegmentInfo(path[path.size() - 1]).to);
        }
        else {
            endLL = getIntersectionPosition(getStreetSegmentInfo(path[path.size() - 1]).from);
        }

        end = mapData->LatLon_to_t_point(endLL);
    } else {
        start = mapData->LatLon_to_t_point(getIntersectionPosition(mapData->find_intersections_path[0]));
        end = start;
    }

    draw_segments(path);

    if (!s_data.poiChecked) {
        // position variables
        LatLon RCpoint = getIntersectionPosition(mapData->find_intersections_path[1]);
        t_point tpoint1 = mapData->LatLon_to_t_point(RCpoint);

        // draw pin
        draw_surface(pin, tpoint1.x - 0.0000125, tpoint1.y + 0.00003);
    } else {
        setcolor(t_color(0, 168, 219, 255));
        fillarc(start.x, start.y, 0.0002, 0, 360);
        setcolor(t_color(0, 204, 104, 255));
        fillarc(end.x, end.y, 0.0002, 0, 360);
        setcolor(WHITE);
        fillarc(start.x, start.y, 0.0001, 0, 360);
        fillarc(end.x, end.y, 0.0001, 0, 360);
    } 
        
    zoom_find_path(start, end);


    

}

void display_directions() {
    mapData->errors.clear();
    mapData->enterWarning = false;
    double angle1 = 0;
    double angle2 = 0;
    char turning_dir = '\0';
    unsigned counter = 1; //counter used to lower the print
    if (mapData->path.size() == 0) {
        drawtext(155, 200 + 50 * (counter), "You clicked the same intersection!");
        return;
    }

    unsigned start_id = mapData->find_intersections_path[0];
    double distance = 0;
    if (draw_directions) {
        unsigned streetID = getStreetSegmentInfo(mapData->find_intersections_path[0]).streetID;
        t_point from_, to_;
        bool first_instruction = false;

        unsigned current_dir = 0;
        unsigned next_dir = 0;

        std::string street_name_after_turning;



        for (unsigned i = 0; i < mapData->path.size(); ++i) {

            distance += find_street_segment_length(mapData->path[i]);

            //every time from & to update, start id also updates to the next to point
            from_ = mapData->LatLon_to_t_point(getIntersectionPosition(start_id));
            if (getStreetSegmentInfo(mapData->path[i]).from == start_id) {
                to_ = mapData->LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(mapData->path[i]).to));
                start_id = getStreetSegmentInfo(mapData->path[i]).to;
            } else {
                to_ = mapData->LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(mapData->path[i]).from));
                start_id = getStreetSegmentInfo(mapData->path[i]).from;
            }


            //converting to screen coordinate
            from_ = world_to_scrn(from_);
            to_ = world_to_scrn(to_);

            //setting up the current direction
            if (from_.x < to_.x) {
                if (from_.y < to_.y) {
                    current_dir = 1;
                } else if (from_.y > to_.y) {
                    current_dir = 4;
                } else {
                    current_dir = 5; //moving towards right
                }
            } else if (from_.x > to_.x) {
                if (from_.y < to_.y) {
                    current_dir = 2;
                } else if (from_.y > to_.y) {
                    current_dir = 3;
                } else {
                    current_dir = 6; //moving left
                }
            } else {
                if (from_.y < to_.y) {
                    current_dir = 7; //moving upward
                } else {
                    current_dir = 8; //moving downward
                }
            }


            //In the last iteration, break after getting current direction
            if (i == mapData->path.size() - 1) {
                break;
            }

            //looking at the next street segment
            from_ = mapData->LatLon_to_t_point(getIntersectionPosition(start_id));
            if (getStreetSegmentInfo(mapData->path[i + 1]).from == start_id) {
                to_ = mapData->LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(mapData->path[i + 1]).to));
            } else {
                to_ = mapData->LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(mapData->path[i + 1]).from));

            }

            from_ = world_to_scrn(from_);
            to_ = world_to_scrn(to_);

            //setting up the next travel direction
            if (from_.x < to_.x) {
                if (from_.y < to_.y) {
                    next_dir = 1;
                } else if (from_.y > to_.y) { //from.y > to.y
                    next_dir = 4;
                } else {
                    next_dir = 5; //moving towards right
                }
            } else if (from_.x > to_.x) { //from.x > to.x
                if (from_.y < to_.y) {
                    next_dir = 2;
                } else if (from_.y > to_.y) {
                    next_dir = 3;
                } else {
                    next_dir = 6; //moving towards left
                }
            } else { //moving vertically
                if (from_.y < to_.y) {
                    next_dir = 7; //moving upward
                } else {
                    next_dir = 8; //moving downward
                }
            }


            //angle calculation


            if (getStreetSegmentInfo(mapData->path[i + 1]).streetID != streetID) {
                angle1 = (mapData->get_all_segments()[mapData->path[i]]).angle;
                angle2 = (mapData->get_all_segments()[mapData->path[i + 1]]).angle;

                //calculating angles depending on the current and next directions
                //if the angle is greater than 45 degree, consider it as a turn

                turning_dir = get_turning_direction(angle1, angle2, current_dir, next_dir);


                if (!first_instruction) {

                    street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                    drawtext(155, 200 + 50 * (counter), "↑");
                    drawtext(155, 200 + 50 * (counter) + 15, "Head toward " + street_name_after_turning);
                    counter++;
                    first_instruction = true;
                    if ((current_dir == 5 && (next_dir == 7 || next_dir == 2)) || (current_dir == 6 && (next_dir == 8 || next_dir == 4)) ||
                            (current_dir == 7 && (next_dir == 6 || next_dir == 3)) || (current_dir == 8 && (next_dir == 5 || next_dir == 1))) {
                        drawtext(155, 200 + 50 * (counter), "←←");
                        drawtext(155, 200 + 50 * (counter) + 15, "Hard left turn onto " + street_name_after_turning);
                        if (distance < 1000) {
                            drawtext(155, 200 + 50 * (counter) + 15, std::to_string(distance) + " m");
                        } else {
                            drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                        }
                        counter++;
                        distance = 0;
                        streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                    } else if ((turning_dir == 'r') || ((current_dir == 5 && (next_dir == 8 || next_dir == 3)) || (current_dir == 6 && (next_dir == 7 || next_dir == 1)) ||
                            (current_dir == 7 && (next_dir == 5 || next_dir == 4)) || (current_dir == 8 && (next_dir == 6 || next_dir == 2)))) {
                        drawtext(155, 200 + 50 * (counter), "→→");
                        drawtext(155, 200 + 50 * (counter) + 15, "Hard right turn onto " + street_name_after_turning);
                        if (distance < 1000) {
                            drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                        } else {
                            drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                        }
                        counter++;
                        distance = 0;
                        streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                    } else {
                        if (turning_dir == 'L') {
                            street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                            drawtext(155, 200 + 50 * (counter), "←");
                            drawtext(155, 200 + 50 * (counter) + 15, "Turn left onto " + street_name_after_turning);
                            if (distance < 1000) {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                            } else {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                            }
                            counter++;
                            distance = 0;
                            streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                        } else if (turning_dir == 'R') {
                            street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                            drawtext(155, 200 + 50 * (counter), "→");
                            drawtext(155, 200 + 50 * (counter) + 15, "Turn right onto " + street_name_after_turning);
                            if (distance < 1000) {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                            } else {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                            }
                            counter++;
                            distance = 0;
                            streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                        } else {
                            street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                            drawtext(155, 200 + 50 * (counter), "↑");
                            drawtext(155, 200 + 50 * (counter) + 15, "Continue onto " + street_name_after_turning);
                            if (distance < 1000) {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                            } else {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                            }
                            counter++;
                            distance = 0;
                            streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                        }
                    }
                } else {

                    if ((turning_dir == 'l') || ((current_dir == 5 && (next_dir == 7 || next_dir == 2)) || (current_dir == 6 && (next_dir == 8 || next_dir == 4)) ||
                            (current_dir == 7 && (next_dir == 6 || next_dir == 3)) || (current_dir == 8 && (next_dir == 5 || next_dir == 1)))) {
                        street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                        drawtext(155, 200 + 50 * (counter), "←←");
                        drawtext(155, 200 + 50 * (counter) + 15, "Hard left turn degree onto " + street_name_after_turning);
                        if (distance < 1000) {
                            drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                        } else {
                            drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                        }
                        counter++;
                        distance = 0;
                        streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                    } else if ((turning_dir == 'r') || ((current_dir == 5 && (next_dir == 8 || next_dir == 3)) || (current_dir == 6 && (next_dir == 7 || next_dir == 1)) ||
                            (current_dir == 7 && (next_dir == 5 || next_dir == 4)) || (current_dir == 8 && (next_dir == 6 || next_dir == 2)))) {
                        street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                        drawtext(155, 200 + 50 * (counter), "→→");
                        drawtext(155, 200 + 50 * (counter) + 15, "Hard right turn onto " + street_name_after_turning);
                        if (distance < 1000) {
                            drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                        } else {
                            drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                        }
                        counter++;
                        distance = 0;
                        streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                    } else {
                        if (turning_dir == 'L') {
                            street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                            drawtext(155, 200 + 50 * (counter), "←");
                            drawtext(155, 200 + 50 * (counter) + 15, "Turn left onto " + street_name_after_turning);
                            if (distance < 1000) {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                            } else {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                            }
                            counter++;
                            distance = 0;
                            streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                        } else if (turning_dir == 'R') {
                            street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                            drawtext(155, 200 + 50 * (counter), "→");
                            drawtext(155, 200 + 50 * (counter) + 15, "Turn right onto " + street_name_after_turning);
                            if (distance < 1000) {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                            } else {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                            }
                            counter++;
                            distance = 0;
                            streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                        } else {
                            street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[i + 1]).streetID);
                            drawtext(155, 200 + 50 * (counter), "↑");
                            drawtext(155, 200 + 50 * (counter) + 15, "Continue onto " + street_name_after_turning);
                            if (distance < 1000) {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                            } else {
                                drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                            }
                            counter++;
                            distance = 0;
                            streetID = getStreetSegmentInfo(mapData->path[i + 1]).streetID;
                        }
                    }
                }
            }
        }

        from_ = mapData->LatLon_to_t_point(getIntersectionPosition(start_id));
        if (getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).from == start_id) {
            to_ = mapData->LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).to));
            //                start_id = getStreetSegmentInfo(mapData->find_intersections_path[i+1]).to;
        } else {
            to_ = mapData->LatLon_to_t_point(getIntersectionPosition(getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).from));
            //                start_id = getStreetSegmentInfo(mapData->find_intersections_path[i+1]).from;
        }

        from_ = world_to_scrn(from_);
        to_ = world_to_scrn(to_);

        //setting up the next travel direction
        if (from_.x < to_.x) {
            if (from_.y < to_.y) {
                next_dir = 1;
            } else if (from_.y > to_.y) { //from.y > to.y
                next_dir = 4;
            } else {
                next_dir = 5; //moving towards right
            }
        } else if (from_.x > to_.x) { //from.x > to.x
            if (from_.y < to_.y) {
                next_dir = 2;
            } else if (from_.y > to_.y) {
                next_dir = 3;
            } else {
                next_dir = 6; //moving towards left
            }
        } else { //moving vertically
            if (from_.y < to_.y) {
                next_dir = 7; //moving upward
            } else {
                next_dir = 8; //moving downward
            }
        }

        turning_dir = get_turning_direction(angle1, angle2, current_dir, next_dir);

        if (getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID != streetID) {
            if ((turning_dir == 'l') || ((current_dir == 5 && (next_dir == 7 || next_dir == 2)) || (current_dir == 6 && (next_dir == 8 || next_dir == 4)) ||
                    (current_dir == 7 && (next_dir == 6 || next_dir == 3)) || (current_dir == 8 && (next_dir == 5 || next_dir == 1)))) {
                street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID);
                drawtext(155, 200 + 50 * (counter), "←←");
                drawtext(155, 200 + 50 * (counter) + 15, "Hard left turn degree onto " + street_name_after_turning);
                if (distance < 1000) {
                    drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                } else {
                    drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                }
                counter++;
                distance = 0;
                streetID = getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID;
            } else if ((turning_dir == 'r') || ((current_dir == 5 && (next_dir == 8 || next_dir == 3)) || (current_dir == 6 && (next_dir == 7 || next_dir == 1)) ||
                    (current_dir == 7 && (next_dir == 5 || next_dir == 4)) || (current_dir == 8 && (next_dir == 6 || next_dir == 2)))) {
                street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID);
                drawtext(155, 200 + 50 * (counter), "→→");
                drawtext(155, 200 + 50 * (counter) + 15, "Hard right turn onto " + street_name_after_turning);
                if (distance < 1000) {
                    drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                } else {
                    drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                }
                counter++;
                distance = 0;
                streetID = getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID;
            } else {

                if (turning_dir == 'L') {
                    street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID);
                    drawtext(155, 200 + 50 * (counter), "←");
                    drawtext(155, 200 + 50 * (counter) + 15, "Turn left onto " + street_name_after_turning);
                    if (distance < 1000) {
                        drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                    } else {
                        drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                    }
                    counter++;
                    distance = 0;
                    streetID = getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID;
                } else if (turning_dir == 'R') {
                    street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID);
                    drawtext(155, 200 + 50 * (counter), "→");
                    drawtext(155, 200 + 50 * (counter) + 15, "Turn right onto " + street_name_after_turning);
                    if (distance < 1000) {
                        drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                    } else {
                        drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                    }
                    counter++;
                    distance = 0;
                    streetID = getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID;
                } else {
                    street_name_after_turning = getStreetName(getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID);
                    drawtext(155, 200 + 50 * (counter), "↑");
                    drawtext(155, 200 + 50 * (counter) + 15, "Continue onto " + street_name_after_turning);
                    if (distance < 1000) {
                        drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance) + " m");
                    } else {
                        drawtext(155, 200 + 50 * (counter) + 30, std::to_string(distance / 1000) + " km");
                    }
                    counter++;
                    distance = 0;
                    streetID = getStreetSegmentInfo(mapData->path[mapData->path.size() - 1]).streetID;
                }
            }
        }
    }

}
char get_turning_direction(double angle1, double angle2, int current_dir, int next_dir) {

    char turning_dir = '\0';
    double turning_angle = 0;
    turning_angle = angle2 - angle1;
    if (current_dir == 1 && next_dir == 3) {
        if (turning_angle > 0) {
            turning_dir = 'r'; //hard right
        } else {
            turning_dir = 'l'; //hard left
        }
    } else if (current_dir == 3 && next_dir == 1) {
        if (turning_angle < 0) {
            turning_dir = 'l';
        } else {
            turning_dir = 'r';
        }
    } else if (current_dir == 2 && next_dir == 4) {
        if (turning_angle < 0) {
            turning_dir = 'r';
        } else {
            turning_dir = 'l';
        }
    } else if (current_dir == 4 && next_dir == 2) {
        if (turning_angle < 0) {
            turning_dir = 'r';
        } else {
            turning_dir = 'l';
        }
    } else {

        if (current_dir == 1 && (next_dir == 1 || next_dir == 7)) {
            turning_angle = angle2 - angle1;
            if (turning_angle > PI / 4) {
                turning_dir = 'L'; //left
            }
        }
        if (current_dir == 1 && (next_dir == 1 || next_dir == 5)) {
            turning_angle = angle2 - angle1;
            if (turning_angle < -PI / 4) {
                turning_dir = 'R';
            }
        }
        if (current_dir == 2 && (next_dir == 2 || next_dir == 6)) {
            turning_angle = angle2 - angle1;
            if (turning_angle < -PI / 4) {
                turning_dir = 'L';
            }
        }
        if (current_dir == 2 && (next_dir == 2 || next_dir == 7)) {
            turning_angle = angle2 - angle1;
            if (turning_angle > PI / 4) {
                turning_dir = 'R';
            }
        }
        if (current_dir == 3 && (next_dir == 3 || next_dir == 8)) {
            turning_angle = angle2 - angle1;
            if (turning_angle > PI / 4) {
                turning_dir = 'L';
            }
        }
        if (current_dir == 3 && (next_dir == 3 || next_dir == 6)) {
            turning_angle = angle2 - angle1;
            if (turning_angle < -PI / 4) {
                turning_dir = 'R';
            }
        }
        if (current_dir == 4 && (next_dir == 4 || next_dir == 8)) {
            turning_angle = angle2 - angle1;
            if (turning_angle > PI / 4) {
                turning_dir = 'R';
            }
        }
        if (current_dir == 4 && (next_dir == 4 || next_dir == 5)) {
            turning_angle = angle2 - angle1;
            if (turning_angle < -PI / 4) {
                turning_dir = 'L';
            }
        } else {
            turning_angle = ((PI / 2) - angle1) + ((PI / 2) - angle2);
            //            turning_angle = angle1 + angle2;
            if (turning_angle > PI / 4) {
                if (current_dir == 1 && next_dir == 2) {
                    turning_dir = 'L';
                } else if (current_dir == 2 && next_dir == 1) {
                    turning_dir = 'R';
                } else if (current_dir == 4 && next_dir == 3) {
                    turning_dir = 'R';
                } else if (current_dir == 3 && next_dir == 4) {
                    turning_dir = 'L';
                }
            }
            turning_angle = angle1 + angle2;
            //            turning_angle = ((PI / 2) - angle1) + ((PI / 2) - angle2);
            if (turning_angle > PI / 4) {
                if (current_dir == 1 && next_dir == 4) {
                    if (turning_angle > PI / 2) {
                        turning_dir = 'r';
                    } else {
                        turning_dir = 'R';
                    }
                } else if (current_dir == 2 && next_dir == 3) {
                    if (turning_angle > PI / 2) {
                        turning_dir = 'l';
                    } else {
                        turning_dir = 'L';
                    }
                } else if (current_dir == 4 && next_dir == 1) {
                    if (turning_angle > PI / 2) {
                        turning_dir = 'l';
                    } else {
                        turning_dir = 'L';
                    }
                } else if (current_dir == 3 && next_dir == 2) {
                    if (turning_angle > PI / 2) {
                        turning_dir = 'r';
                    } else {
                        turning_dir = 'R';
                    }
                }
            } else {
                if ((current_dir == 5 && next_dir == 1) && (angle2 > PI / 4)) {
                    turning_dir = 'L';
                } else if ((current_dir == 5 && next_dir == 4) && (angle2 > PI / 4)) {
                    turning_dir = 'R';
                } else if ((current_dir == 6 && next_dir == 2) && (angle2 > PI / 4)) {
                    turning_dir = 'R';
                } else if ((current_dir == 6 && next_dir == 3) && (angle2 > PI / 4)) {
                    turning_dir = 'L';
                } else if ((current_dir == 7 && next_dir == 1) && (angle2 > PI / 4)) {
                    turning_dir = 'R';
                } else if ((current_dir == 7 && next_dir == 2) && (angle2 > PI / 4)) {
                    turning_dir = 'L';
                } else if ((current_dir == 8 && next_dir == 3) && (angle2 > PI / 4)) {
                    turning_dir = 'R';
                } else if ((current_dir == 8 && next_dir == 4) && (angle2 > PI / 4)) {
                    turning_dir = 'L';
                }
            }

        }
    }
    return turning_dir;

}


//m4 helper
void draw_intersections(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const Seg_array segmentsToDraw){
    
    // draw all the deliveries
    for (DeliveryInfo i : deliveries) {
        //draw all the pickups black circle
        t_point pickup = mapData->LatLon_to_t_point(getIntersectionPosition(i.pickUp));
        draw_surface(startS, pickup.x, pickup.y);
     
        //draw all the dropoffs   red circle
        t_point dropOff = mapData->LatLon_to_t_point(getIntersectionPosition(i.dropOff));
        draw_surface(circle, dropOff.x, dropOff.y);
    }
    
    for(unsigned i = 0; i < depots.size(); i++){
        //draw all the depots black circle
        t_point depot = mapData->LatLon_to_t_point(getIntersectionPosition(depots[i]));
        draw_surface(bluePin, depot.x, depot.y);
    }
    
    //LatLon points for start and end points and inbetween
    LatLon from, to, c1, c2;
    //two t_points for from and to in cartesian
    t_point coordinateFrom, coordinateTo;
    unsigned j = 0;


    //outer for loop to go through all segments in the vector to be drawn
    for (unsigned i = 0; i < segmentsToDraw.size(); ++i) {
        from = getIntersectionPosition(getStreetSegmentInfo(segmentsToDraw[i]).from);
        to = getIntersectionPosition(getStreetSegmentInfo(segmentsToDraw[i]).to);

        coordinateFrom = mapData->LatLon_to_t_point(from);
        coordinateTo = mapData->LatLon_to_t_point(to);

        //no curve point
        if (getStreetSegmentInfo(segmentsToDraw[i]).curvePointCount == 0) {
            setcolor_by_name("blue");
            drawline(coordinateFrom, coordinateTo);
        } else {
            j = 0;
            setcolor_by_name("blue");
            drawline(coordinateFrom, mapData->LatLon_to_t_point(getStreetSegmentCurvePoint(segmentsToDraw[i], j)));

            // inner for loop to draw lines between curve points
            for (; j < getStreetSegmentInfo(segmentsToDraw[i]).curvePointCount - 1; ++j) {
                c1 = getStreetSegmentCurvePoint(segmentsToDraw[i], j);
                c2 = getStreetSegmentCurvePoint(segmentsToDraw[i], j + 1);
                setcolor_by_name("blue");
                drawline(mapData->LatLon_to_t_point(c1), mapData->LatLon_to_t_point(c2));
            }
            //last piece of a segment
            c2 = getStreetSegmentCurvePoint(segmentsToDraw[i], j);
            setcolor_by_name("blue");
            drawline(mapData->LatLon_to_t_point(c2), coordinateTo);
        }
    }
}