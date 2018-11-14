

#include "trieNode.h"

void trieNode::add(char c){
    //if value exists
    if(!val.empty())
        children[c] = trieNode(val + c);
    else
        //make a string with c
        children[c] = trieNode(std::string(1,c));
}

//returns a vector of all the prefixes
std::vector<std::string> trieNode::prefixes(){
    std::vector<std::string> prefix;
    
    if(isCompleteWord == true)
        prefix.push_back(val);
    
    if(!children.empty()){
        //iterate through each node in children
        //std::map<char, trieNode>::iterator it;
        
        //call the prefixes recursively till it gets a complete word
        //for(it = children.begin(); it != children.end(); it++){
        for(auto&& it: children){
//            std::vector<std::string> temp; 
//            temp.insert(it.second.prefixes());
            auto&& temp = it.second.prefixes(); 
            
            
            //insert to the end of the prefix
            prefix.insert(prefix.end(), temp.begin(), temp.end());
        }
    }
    
    return prefix;
}
