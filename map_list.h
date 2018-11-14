/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   map_list.h
 * Author: kimbyeo4
 *
 * Created on March 20, 2018, 7:07 PM
 */

#ifndef MAP_LIST_H
#define MAP_LIST_H

class map_list {
public:
    map_list();
    map_list(const map_list& orig);
    virtual ~map_list();
    std::vector<std::pair<int, std::string>> map_names;

};

#endif /* MAP_LIST_H */

