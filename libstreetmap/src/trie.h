#ifndef TRIE_H
#define TRIE_H


#include <map>
#include <string>
#include <vector>
#include "trieNode.h"

class trie {
public:
    trieNode head;
    
    //constructor
    trie() : head(){}

    //public functions
    void insert(std::string& word);
    std::string find(std::string& word);
    std::vector<std::string> autoComplete(std::string & prefix);
};


#endif /* TRIE_H */

