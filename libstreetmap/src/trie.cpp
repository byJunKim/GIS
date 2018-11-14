

#include "trie.h"

void trie::insert(std::string& word) {
    trieNode* cur = &head;

    //for loop insert every char of the string
    for (unsigned i = 0; i < word.length(); i++) {
        //get the character
        const char c = word[i];

        //create a new node if theres no path
        if (cur->children.find(c) == cur->children.end())
            cur->add(c);
        //increment cur
        cur = &cur->children[c];
    }

    cur->isCompleteWord = true;
}

std::string trie::find(std::string& word){
    trieNode* cur = &head;
    
    std::string temp = "";
    
    //iterate through the trie tree till we find the end
    for(unsigned i = 0; i < word.length(); i++){ 
        //get the character
        const char c = word[i];

        //if we cant find the word
        if(cur->children.find(c) == cur->children.end())
            return temp;
    }
    
    return cur->val;
}


std::vector<std::string> trie::autoComplete(std::string& incompleteWord){
    trieNode* cur = &head;
    
    std::vector<std::string> temp;
    
    //iterate throu the trie tree till we find the end
    for(unsigned i = 0; i < incompleteWord.length(); i++){ 
        //get the character
        const char c = incompleteWord[i];
        
        //if we reach the end
        if(cur->children.find(c) == cur->children.end())
            return temp;
        else
            cur = &(cur->children[c]);
    }
    
    return cur->prefixes();
    
}
