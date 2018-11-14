/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   s_struct.h
 * Author: kimbyeo4
 *
 * Created on February 23, 2018, 9:14 PM
 */

#ifndef S_STRUCT_H
#define S_STRUCT_H
#include "map_data.h"

struct s_struct {
    t_bound_box position;
    StreetSegmentIndex index;
    unsigned direction; //1 means the first quadrant
    double angle;  //with respect to the x axis
    unsigned width;
    double length;
    std::string streetType;
    std::string streetColour;
    unsigned streetThickness;
    
    
} ;

#endif /* S_STRUCT_H */

