#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;


class Node
{
public:
    
    Node(string name_,string value_);
    ~Node();
    
    string getNodeName() const;
    string getNodeValue() const;

    void setNodeValue(string);

    string getComponentType(Node*);
    
    void setParent(Node * parent);
    void printOutTree(Node *root);

    vector<Node*> getChildren() const;
    Node * getParent() const;
    
    Node * addChild(Node * child);


    
protected:
    
    string name;
    string value;
    
    Node * parent;
    vector<Node*> children;
    
private:

};



#endif






