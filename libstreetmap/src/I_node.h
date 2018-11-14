#ifndef I_NODE_H
#define I_NODE_H


#include <queue>
#include "map_data.h"
#define INF 99999999

class I_node {
public:
    IntersectionIndex id;
    double travel_time;
    double h;
    StreetSegmentIndex prev;
    
    I_node() : travel_time(INF), h(0) {}
};

struct I_info {
    std::vector<StreetSegmentIndex> segments;
    std::vector<IntersectionIndex> adjacent;
};

#endif /* I_NODE_H */

