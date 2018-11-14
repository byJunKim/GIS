#ifndef FIXED_MENU_H
#define FIXED_MENU_H


#include "graphics.h"
#include "map_data.h"
#include "m3.h"
#include "map_list.h"


#define MAX_STRING_LENGTH 35


// struct holding search data
typedef struct search_bar_data {
    bool selected;
    bool selected2;
    bool pathChecked;
    bool intersectionChecked;
    bool poiChecked;
    bool streetChecked;
    bool mapChange;
    int search_for; // 1 = intersection, 2 = street, 3 = POI
    std::string query;

    // for menu
    Surface menu = load_png_from_file("libstreetmap/resources/menu.png");
    Surface menu2 = load_png_from_file("libstreetmap/resources/smaller-menu.png");
    Surface search = load_png_from_file("libstreetmap/resources/search-bar.png");
    Surface subway_button = load_png_from_file("libstreetmap/resources/subway-button.png");
    Surface compass = load_png_from_file("libstreetmap/resources/compass.png");
    search_bar_data() {
        query = "";
    }
} search_bar_data;


// helper functions to draw the menu
void draw_fixed_menu();
void draw_fixed_background();
void draw_compass();
void draw_subway_button();
void draw_check_boxes();
void draw_search_bar();
void draw_result_text();
void draw_query_text();


#endif /* FIXED_MENU_H */

