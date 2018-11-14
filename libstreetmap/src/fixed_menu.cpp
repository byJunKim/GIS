#include "fixed_menu.h"
#include "fontcache.h"



// global var to contain search bar data
extern search_bar_data s_data;
//bool draw_directions;

// draws fixed menu at top left corner of screen

void draw_fixed_menu() {
    // change coordinates to onscreen
    set_coordinate_system(GL_SCREEN);
        
    // draw menu components
    if(!s_data.pathChecked){
        draw_surface(s_data.menu, 37, 150); 
    }
    else{
        draw_surface(s_data.menu2, 37, 190); 
    }
        
    draw_compass();
    draw_search_bar();
    draw_subway_button();
    draw_result_text();
    

    // return coordinates to world
    set_coordinate_system(GL_WORLD);
}


// small compass drawing

void draw_compass() {
    draw_surface(s_data.compass, 145, 15);
}


// function to determine whether subway is drawn

void draw_subway_button() {
    draw_surface(s_data.subway_button, 40, 25);
}


// draws search bar and writes query

void draw_search_bar() {
    // drawing the outline and bar
    draw_surface(s_data.search, 37, 110);
    
    if(!s_data.mapChange){
        //draw the search bar enter
        setfontsize(7);
        setcolor(WHITE);
        drawtext(274,122,"|");
        setfontsize(12);
        setcolor(WHITE);
        drawtext(267,127,"←");
    }
    
    mapData->enterWarning = false;
    
    //checking if the user has inputed anything into the search bar
    int size = mapData->find_search_store.size();

    if (s_data.pathChecked) {
        
        //draw the second search bar
        draw_surface(s_data.search, 37, 150);
        
        //draw the second search bar enter
        setfontsize(7);
        setcolor(WHITE);
        drawtext(274,162,"|");
        setfontsize(12);
        setcolor(WHITE);
        drawtext(267,167,"←");
        
        //if the user hasnt entered any input
        if (!s_data.selected && !s_data.selected2) {

            //if intersection checkbox is checked
            if (s_data.intersectionChecked) {
                setfontsize(10);
                setcolor(WHITE);
                drawtext(160, 125, "Enter the first intersection");
                drawtext(160, 165, "Enter the second intersection");
            }
                //if poi checkbox is checked
            else if (s_data.poiChecked) {
                setfontsize(10);
                setcolor(WHITE);
                drawtext(160, 125, "Enter intersection");
                drawtext(160, 165, "Enter point of interest");
            }                //if nothing is checked
            else if (!s_data.intersectionChecked && !s_data.poiChecked) {
                setfontsize(10);
                setcolor(WHITE);
                drawtext(160, 125, "Indicate which type of path");
                drawtext(160, 165, "that you are looking for");
            }
            
        }            //if the user clicks on the first search bar entering the intersection
        else if (s_data.selected && !s_data.selected2) {
            if (s_data.intersectionChecked) {
                setfontsize(10);
                setcolor(WHITE);
                drawtext(160, 165, "Enter the second intersection");
                drawtext(160, 125, s_data.query + "|");
                mapData->enterWarning = true;
            }
            else if (s_data.poiChecked) {
                setfontsize(10);
                setcolor(WHITE);
                drawtext(160, 165, "Enter the poi");
                drawtext(160, 125, s_data.query + "|");
                mapData->enterWarning = true;
            }else {
                setfontsize(8);
                setcolor(WHITE);
                drawtext(155, 280, "Oh no!");
                drawtext(155, 295, "Please click on either poi or intersection");
                drawtext(155, 310, "for path finding!");
            }
        }            //if the user clicks on the second search bar
        else if (s_data.selected2) {
            setfontsize(10);
            setcolor(WHITE);
            //error checking to make sure there's something stored in the first search bar
            if (size > 0) {
                drawtext(160, 125, mapData->find_search_store[size - 1]);
            }
            drawtext(160, 165, s_data.query + "|");
            mapData->enterWarning = true;
        }

    } else if (s_data.mapChange) {
        // query printing
        if (s_data.selected) {
            setfontsize(10);
            setcolor(WHITE);
            drawtext(160, 125, s_data.query + "|");
            mapData->enterWarning = true;
        }// pre-filled text
        else if (!s_data.selected) {
            setfontsize(10);
            setcolor(RED);
            drawtext(160, 125, "Type in city_country to change the map!!!");
        }
    }
    else {
        // query printing
        if (s_data.selected) {
            setfontsize(10);
            setcolor(WHITE);
            drawtext(160, 125, s_data.query + "|");
            mapData->enterWarning = true;
        }// pre-filled text
        else if (!s_data.selected) {
            setfontsize(10);
            setcolor(WHITE);
            drawtext(160, 125, "Search Here");
        }
    }
}

void draw_check_boxes() {
    // change coordinates to onscreen
    set_coordinate_system(GL_SCREEN);

    //draw the outline and boxes
    //first box
    setcolor(t_color(125,167,217, 255));
    fillarc(47.5, 92.5, 7.5, 0, 360);

    //second box
    fillarc(100.5, 92.5, 7.5, 0, 360);
    
    //third box
    fillarc(193.5, 92.5, 7.5, 0, 360);
    
    //fourth box
    fillarc(239.5, 92.5, 7.5, 0, 360);
    
    //first box
    setcolor(t_color(161,202,241, 255));
    fillarc(47.5, 92.5, 5.5, 0, 360);

    //second box
    fillarc(100.5, 92.5, 5.5, 0, 360);
    
    //third box
    fillarc(193.5, 92.5, 5.5, 0, 360);
    
    //fourth box
    fillarc(239.5, 92.5, 5.5, 0, 360);

    setfontsize(10);
    setcolor(BLACK);
    drawtext(74, 92, "Path");
    drawtext(146, 92, "Intersection");
    drawtext(216, 92, "POI");
    drawtext(268, 92, "Street");
    
    setcolor(WHITE);
    if (s_data.pathChecked) {
        fillarc(47.5, 92.5, 5.5, 0, 360);
        s_data.streetChecked = false;
    }
    if (s_data.intersectionChecked) {
        fillarc(100.5, 92.5, 5.5, 0, 360);
    }
    else if (s_data.poiChecked) {
        fillarc(193.5, 92.5, 5.5, 0, 360);
    }
    else if (s_data.streetChecked) {
        fillarc(239.5, 92.5, 5.5, 0, 360);
    }


    // return coordinates to world
    set_coordinate_system(GL_WORLD);
}


// writing results of search from user

void draw_result_text() {
    // change coordinates to onscreen
    set_coordinate_system(GL_SCREEN);

    // writing search info
    setfontsize(8);
    setcolor(WHITE);

    if (mapData->find_intersection_array.size()) {
        drawtext(155, 200, std::to_string(mapData->find_intersection_array.size()) + " result(s) found for:");
        draw_query_text();

        drawtext(155, 310, "Intersections found at (x,y):");

        int ypos = 340;
        for (unsigned i = 0; i < mapData->find_intersection_array.size() && ypos < 895; ++i) {
            t_point position = mapData->LatLon_to_t_point(getIntersectionPosition(mapData->find_intersection_array[i]));
            drawtext(155, ypos, "(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            ypos += 15;
        }
        
        mapData->enterWarning = false;
    } else if (mapData->find_poi_array.size()) {
        drawtext(155, 210, std::to_string(mapData->find_poi_array.size()) + " result(s) found for:");
        draw_query_text();

        drawtext(155, 300, "Type: " + getPointOfInterestType(mapData->find_poi_array[0]));

        drawtext(155, 320, "Points found at (x,y):");


        int ypos = 350;
        for (unsigned i = 0; i < mapData->find_poi_array.size() && ypos < 875; ++i) {
            t_point position = mapData->LatLon_to_t_point(getPointOfInterestPosition(mapData->find_poi_array[i]));
            drawtext(155, ypos, "(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            ypos += 15;
        }
        
        mapData->enterWarning = false;
    } else if (mapData->find_street_array.size()) {
        drawtext(155, 240, std::to_string(mapData->find_street_array.size()) + " result(s) found for:");
        draw_query_text();
        
        mapData->enterWarning = false;
    }

    // This is a notification if map is zoomed into a single point
    if ((mapData->find_intersection_array.size() == 1 || mapData->find_poi_array.size() == 1) && !mapData->click) {
        drawtext(155, 280, "Press 'Clear' to reset the map.");
    }
    
            //error checking if user entered the same thing
    if (mapData->find_search_store2.size() > 0 && mapData->find_search_store.size() > 0 && mapData->find_intersections_path.size() == 2) {
        if (mapData->find_intersections_path[0] == mapData->find_intersections_path[1]) {
            mapData->errors.clear();
            mapData->errors.push_back("Sorry :(");
            mapData->errors.push_back("You entered the same thing");
            mapData->errors.push_back("Try again next time!");
        }
    }

    // If user enters error msg after he clicks one of the checkbox
    if (mapData->errors.size() && (s_data.intersectionChecked || s_data.pathChecked || s_data.poiChecked || s_data.streetChecked)) {
        int ypos = 340;
        for (unsigned i = 0; i < mapData->errors.size() && ypos < 895; ++i) {
            drawtext(155, ypos, mapData->errors[i]);
            ypos += 15;
        }
        
        mapData->enterWarning = false;
    }

    // If the user forgets to check any checkbox (just clicking on the search bar)
    if (mapData->errors.size() && (s_data.selected && !(s_data.intersectionChecked || s_data.pathChecked || s_data.poiChecked || s_data.streetChecked))) {
        int ypos = 340;
        for (unsigned i = 0; i < mapData->errors.size() && ypos < 895; ++i) {
            drawtext(155, ypos, mapData->errors[i]);
            ypos += 15;
        }
        
        mapData->enterWarning = false;
    }

    if (mapData->autoCompletion.size() > 1 && (mapData->find_poi_array.size() <= 0 && mapData->find_street_array.size() <= 0
            && mapData->find_intersection_array.size() <= 0 && mapData->find_intersections_path.size() <= 1)) {

        //suggest auto completion
        std::string qs = "Did you mean......?                    ";
        drawtext(155, 210, qs);

        int size = mapData->autoCompletion.size();
        int ypos = 235;

        for (int i = 0; i < size && i < 5 && ypos < 895; ++i) {
            //make sure the auto completion result is within the search bar
            if (mapData->autoCompletion[i].length() < MAX_STRING_LENGTH) {
                drawtext(155, ypos, mapData->autoCompletion[i]);
                ypos += 15;
            }
        }
        
        mapData->enterWarning = false;
    }

    if(mapData->enterWarning == true && mapData->path.size()==0){
        drawtext(155, 280, "Please press enter after finish typing :)");
    }
    
        
        // return coordinates to world
    set_coordinate_system(GL_WORLD);
}


// writing user query

void draw_query_text() {
    // checking length of search query to see if it fits box
    if (mapData->query.length() > MAX_STRING_LENGTH) {
        drawtext(155, 260, mapData->query.substr(0, MAX_STRING_LENGTH) + "-");
        drawtext(155, 275, mapData->query.substr(MAX_STRING_LENGTH, mapData->query.length() - MAX_STRING_LENGTH));
    } else {
        drawtext(155, 260, mapData->query);
    }
}



