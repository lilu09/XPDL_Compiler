
#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
//geting file status
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>


#include "codeGenerator.hpp"
using namespace xercesc;
//using namespace std;


class ParserErrorHandler : public ErrorHandler
{
private:
    void reportParseException(const SAXParseException& ex)
    {
        char* msg = XMLString::transcode(ex.getMessage());
        fprintf(stderr, "at line %llu column %llu, %s\n",
                ex.getLineNumber(), ex.getColumnNumber(), msg);
        XMLString::release(&msg);
    }
    
public:
    void warning(const SAXParseException& ex)
    {
        reportParseException(ex);
    }
    
    void error(const SAXParseException& ex)
    {
        reportParseException(ex);
    }
    
    void fatalError(const SAXParseException& ex)
    {
        reportParseException(ex);
    }
    
    void resetErrors()
    {
    }
};


class Parser{
    
public:
    Parser(string xmlFilePath_, string xsdFilePath_ ,string rootFile_, string xpdl_path_);
    ~Parser();
    
    void loadXmlFile(string);

    
private:

    Node* platformLocalRoot;
    string xmlFilePath;
    string xsdFilePath;
    string rootFile;
    string xpdl_path;

    Node* parseXmlFile(string,vector<string>);
   
    //vector<string> sortArglist(vector<string>);
    
    /**
     *  \brief Check if the XML file passed, XML module and XSD file exists.
     *  
     *  
     */
    void checkFilesStatus(string,string)throw( std::runtime_error);

    /**
     *  \brief remove the redundant tree node from DOMTreeNode by Xerces parser, quite
     *  important!
     *  
     *  
     */
    string removeStrBlank(string);

    /**
     *  \brief Transform the special node type called AttributeTypeNode from Xerces parser.
     *  
     *  
     */
    void getTheAttributes(DOMNode*, Node*);
    /**
     *  \brief Recursive transform from DOM to XPDL tree structure
     *  
     *  
     */
    void transferTreeStructure(DOMNode *, Node*);

    /**
     *  \brief Check if children exists
     *  
     *  
     */
    bool checkIfChild(string,Node*);
    /**
     *  \brief retrieve node reference by name
     *  
     *  
     */
    Node* getTargetNode(Node*,string);
    /**
     *  \brief Insert a child to a father node.
     *  TODO: should be modelLinker
     *  
     *  
     */
    void childInsert(Node*,Node*);
    
    //void modelLinker(vector<Node*>);

    void codeGenerateSetUp();
    
    /**
     * Generate code for a tree
     * \param Reference to a tree
     * \param Component type such as "memory", "cpu", "gpu" etc
     * \param the object that perform the code generation
     * 
     */
    void codeGenerate(Node*,string,Generator&);

    /**
     *  \brief Check if there is a quesion mark in the whole IR
     *  
     *  
     */
    void treeTravel(Node*);


    /**
     *  \brief Setup macrobenchmark generation
     *  TODO: rename the function
     *  
     */
    void microbenchmark(Node*);
    /**
     *  \brief generate macrobenchmark code
     *  
     *  
     */
    void microbenchmarkGen(string,string);
    /**
     *  \brief execution of the generated macrobenchmark
     *  
     *  
     */
    string microbenchmarkExe(string, string,string);
    
    /**
     *  \brief use bash to execute macrobenchmark
     *  
     *  
     */
    bool bash_exe(string&, vector<string>&);

    /**
     *  \brief Get all XML files by a give path
     *  
     *  
     */
    int getdir (string, vector<string>&);

   
};




#endif
