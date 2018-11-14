
#ifndef TRIENODE_H
#define TRIENODE_H

#include <map>
#include <string>
#include <vector>

class trieNode{
public:
    std::map<char, trieNode> children;
    bool isCompleteWord;
    std::string val;
    
    //constructor for trieNode
    trieNode(const std::string v){
        val = v;
        isCompleteWord = false;
    }
    
    trieNode(){
        val = "";
        isCompleteWord = false;
    }
    
    void add(char);
    std::vector<std::string> prefixes();
};





#endif /* TRIENODE_H */

