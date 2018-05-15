#include "tree.h"

Node::Node(string _name,string _value)
{
    name = _name;
    value = _value;
    parent = NULL;
}

Node::~Node()
{
    for(vector<Node*>::iterator it = children.begin(); it < children.end(); it++)
    {
        if (*it)
        {
            delete * it;
        }
    }
    children.clear();
}


string Node::getNodeName() const
{
    return name;
}

string Node::getNodeValue() const
{
    return value;
}

void Node::setParent(Node * _parent)
{
    parent = _parent;
}

Node * Node::getParent() const
{
    return parent;
}

void Node::setNodeValue(string _value)
{
    value=_value;

}
Node * Node::addChild(Node * child)
{
    child->setParent(this);
    children.push_back(child);
    
    return child;
}


vector<Node*> Node::getChildren() const
{
    return children;
}



void Node::printOutTree(Node* root)
{
    vector<Node*> children = root->children;
        for(int i=0;i<children.size();i++){
            Node *temp = children[i];
            cout<<temp->name<<" :"<<temp->value<<" - parent:"<<temp->parent->getNodeName()<<endl;
            printOutTree(temp);
        }
    
}

/*
string Node::getComponentType(Node* root){

    vector<Node*> children = root->children;
    vector<Node*> childListTemp;
        for(int i=0;i<children.size();i++){
            Node *temp = children[i];
            if(temp->getNodeName() =="xpdl:component"){
                childListTemp = temp->getChildren();
                for(int j=0;j<childListTemp.size();j++){
                    if(childListTemp[i]->getNodeName()=="type"){
                        return  childListTemp[i]->getNodeValue();
                    }
                }
            }
        }
}
*/

