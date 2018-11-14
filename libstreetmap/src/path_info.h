/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   path_info.h
 * Author: kimbyeo4
 *
 * Created on April 2, 2018, 8:07 PM
 */

#ifndef PATH_INFO_H
#define PATH_INFO_H
#include "map_data.h"

struct path_info {
    bool pickup = false;
    IntersectionIndex pickup_point;
    std::vector<IntersectionIndex> dropoff_point; //multiple dropoffs can correspond to one pickup
    StreetSegmentIndex index;
    unsigned order=INF;
};





#endif /* PATH_INFO_H */

