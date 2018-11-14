/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   map_list.cpp
 * Author: kimbyeo4
 * 
 * Created on March 20, 2018, 7:07 PM
 */

#include "map_list.h"

map_list::map_list() {
    
    map_names.push_back(std::make_pair(100, "beijing_china"));
    map_names.push_back(std::make_pair(100, "cairo_egypt"));
    map_names.push_back(std::make_pair(100, "cape-town_south-africa"));
    map_names.push_back(std::make_pair(100, "golden-horsehoe_canada"));
    map_names.push_back(std::make_pair(100, "hamilton_canada"));
    map_names.push_back(std::make_pair(100, "hong-kong_china"));
    map_names.push_back(std::make_pair(100, "iceland"));
    map_names.push_back(std::make_pair(100, "interlaken_switzerland"));
    map_names.push_back(std::make_pair(100, "london_england"));
    map_names.push_back(std::make_pair(100, "moscow_russia"));
    map_names.push_back(std::make_pair(100, "new-delhi_india"));
    map_names.push_back(std::make_pair(100, "new-york_usa"));
    map_names.push_back(std::make_pair(100, "rio-de-janeiro_brazil"));
    map_names.push_back(std::make_pair(100, "saint-helena"));
    map_names.push_back(std::make_pair(100, "singapore"));
    map_names.push_back(std::make_pair(100, "sydney_australia"));
    map_names.push_back(std::make_pair(100, "tehran_iran"));
    map_names.push_back(std::make_pair(100, "tokyo_japan"));
    map_names.push_back(std::make_pair(100, "toronto_canada"));
    
}

map_list::map_list(const map_list& orig) {
}

map_list::~map_list() {
}

