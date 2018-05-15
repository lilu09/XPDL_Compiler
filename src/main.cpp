#include "xmlParser.h"
#include <string>
#include <vector>
using namespace std;

int main(int argc, char**argv){

    //add export sth to ~/.bash_profile
    string xmlPath;
    string xsdPath;
    string sysFile = argv[1];
    char* xpdl_path;

    xpdl_path = getenv ("XPDL_HOME");
    
    if (xpdl_path!=NULL)
    {
        xmlPath = string(xpdl_path)+"/src/xml";
        xsdPath = string(xpdl_path)+"/src/xml/xpdl_schema.xsd";
        //xsdPath = string(xpdl_path)+"/src/xml/xp";
        Parser my_parser(xmlPath,xsdPath,sysFile,xpdl_path);
        my_parser.loadXmlFile(sysFile);

    }

    //Parser my_parser(xmlPath,xsdPath,sysFile);
    //my_parser.loadXmlFile(sysFile);


    //XPDL_Compiler my_compiler(xmlPath,xsdPath,sysFile);
    //my_compiler.loadXmlFile();
    //my_compiler.construct_ir();
    //my_compiler.code_gen();


    return 0;
    
}
