/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   f_struct.h
 * Author: kimbyeo4
 *
 * Created on February 21, 2018, 7:05 PM
 */

#ifndef F_STRUCT_H
#define F_STRUCT_H
#include "map_data.h"

struct f_struct {
    std::vector<t_point> feature_point;
    int feature_point_count;
    FeatureIndex index;
    t_color color;
    double area; // for closed features
    bool open;
    double length; // for open feature

};



#endif /* F_STRUCT_H */

