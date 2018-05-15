#include "xmlParser.h"


Parser::Parser(string xmlFilePath_, string xsdFilePath_ ,string rootFile_, string xpdl_path_):xmlFilePath(xmlFilePath_), xsdFilePath(xsdFilePath_), rootFile(rootFile_), xpdl_path(xpdl_path_)
{
    //xmlFilePath = xmlFilePath_ ;
    //xsdFilePath = xsdFilePath_ ;
    //rootFile = rootFile_;
}

Parser::~Parser(){
	//TODO: delete trees
    
}

int Parser::getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {

        if ( !strcmp( dirp->d_name, "."  )) continue;
        if ( !strcmp( dirp->d_name, ".." )) continue;
        if ( dirp->d_name[0] == '.' ) continue;
        if(strstr( dirp->d_name, ".xml" ))
        {
            files.push_back(string(dirp->d_name));
        }
    }
    closedir(dp);
    return 0;
}

bool Parser::bash_exe (string& cmd,vector<string>& out ) {
    FILE*           fp;
    const int       SIZEBUF = 1234;
    char            buf [SIZEBUF];
    out = std::vector<std::string> ();
    if ((fp = popen(cmd.c_str (), "r")) == NULL) {
        return false;
    }
    std::string  cur_string = "";
    while (fgets(buf, sizeof (buf), fp)) {
        cur_string += buf;
    }
    out.push_back (cur_string.substr (0, cur_string.size () - 1));
    pclose(fp);
    return true;
}

string Parser::microbenchmarkExe(string target, string path, string fileName)
{
    string expr="g++ "+path+"/"+"temp.cpp" +" -o " +target+".out";
    
    vector<std::string> output;
    bash_exe(expr, output);

    expr="./"+target+".out";
    bash_exe(expr, output);
    string result = output[0];
    
    expr="rm "+target+".out "+path+"/temp.cpp";
    bash_exe(expr, output);

    return result;
}

void Parser::microbenchmarkGen(string path, string fileName)
{

    string input;
    string userFile=path+"/"+fileName;

    ofstream m_file;
    ifstream fin(userFile,ios::in);



    m_file.open(path+"/temp.cpp",ios::out | ios::app | ios::binary);

    if(m_file.is_open()){

        m_file<<"#include <iostream>\n #include<stdlib.h>\n #include<string.h>\n#include <time.h>\nusing namespace std;\nint main(){\n\tclock_t time0 = clock();\n";

        while(getline(fin,input))
        {
            m_file<<input<<"\n";
        }
        fin.close();
        m_file<<"\n\tclock_t time1 = clock();\n\tcout << 100/((double)(time1 - time0) / CLOCKS_PER_SEC) << endl;\n}\n";
    
    }
    m_file.close();

}


void Parser::microbenchmark(Node* node)
{
    Node* parent = node -> getParent();
    Node* targetNode = NULL;
    //find the microbenchmark node
    if(checkIfChild("xpdl:microbenchmark",parent))
    {
	    //get it
        targetNode = getTargetNode(parent,"xpdl:microbenchmark");

	//check target, path and file exist, later add compiler, cflag, ldflag
        if(checkIfChild("target",targetNode) && checkIfChild("path",targetNode)&& checkIfChild("file",targetNode))
        {
            Node* tempTarget = getTargetNode(targetNode,"target");
            Node* tempPath = getTargetNode(targetNode,"path");
            Node* tempFile= getTargetNode(targetNode,"file");

	    //relate questionmark to the right microbenchmark
            if(tempTarget->getNodeValue()==node->getNodeName())
            {
                string target = tempTarget->getNodeValue();
                string path = xpdl_path+"/src"+tempPath->getNodeValue();
                string fileName = tempFile->getNodeValue();
                string result;

                microbenchmarkGen(path,fileName);
                result = microbenchmarkExe(target,path,"temp.cpp");

                node->setNodeValue(result);
            }
        }
    }

    
}

void Parser::treeTravel(Node* root)
{
    vector<Node*> children = root->getChildren();
        for(int i=0;i<children.size();i++){
            Node *temp = children[i];
            if(temp->getNodeValue()=="?")
            {
                microbenchmark(temp);
            }

            treeTravel(temp);
        }
}


void Parser::checkFilesStatus(string xmlFile, string xsdFilePath)throw( std::runtime_error ){
    
    
    struct stat xmlFileStatus;
    struct stat xsdFileStatus;

    errno = 0;
    
    if(stat(xmlFile.c_str(), &xmlFileStatus) == -1) // ==0 ok; ==-1 error
    {
        if( errno == ENOENT )      // errno declared by include file errno.h
            throw ( std::runtime_error("xml: Path file_name does not exist, or path is an empty string.") );
        else if( errno == ENOTDIR )
            throw ( std::runtime_error("xml: A component of the path is not a directory."));
        else if( errno == ELOOP )
            throw ( std::runtime_error("xml: Too many symbolic links encountered while traversing the path."));
        else if( errno == EACCES )
            throw ( std::runtime_error("xml: Permission denied."));
        else if( errno == ENAMETOOLONG )
            throw ( std::runtime_error("xml: File can not be read\n"));
    }
    else if(stat(xsdFilePath.c_str(), &xsdFileStatus) == -1)
    {
        if( errno == ENOENT )      // errno declared by include file errno.h
            throw ( std::runtime_error("schema: Path file_name does not exist, or path is an empty string.") );
        else if( errno == ENOTDIR )
            throw ( std::runtime_error("schema: A component of the path is not a directory."));
        else if( errno == ELOOP )
            throw ( std::runtime_error("schema: Too many symbolic links encountered while traversing the path."));
        else if( errno == EACCES )
            throw ( std::runtime_error("schema: Permission denied."));
        else if( errno == ENAMETOOLONG )
            throw ( std::runtime_error("schema: File can not be read\n"));
    }
    
}

string Parser::removeStrBlank(string str)
{

    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    str.erase(remove(str.begin(), str.end(), '\n'), str.end());

    return str;
}

void Parser::getTheAttributes(DOMNode *attribute, Node* treeNode)
{
    DOMNamedNodeMap *temp = attribute->getAttributes();


    if(temp){
        for(int m=0;m<(int)temp->getLength();m++){

            DOMNode* tempItem = temp ->item(m);
            char *attrname = XMLString::transcode(tempItem->getNodeName());
            char *attrvalue = XMLString::transcode(tempItem->getTextContent());

            Node* treeNodeTemp = new Node(attrname,attrvalue);
            treeNode->addChild(treeNodeTemp);

            //cout << " "<<attrname<<":" << attrvalue<< endl;
            XMLString::release(&attrname);
            XMLString::release(&attrvalue);
        }
    }   

}

void Parser::transferTreeStructure(DOMNode *parentTag, Node* treeNode)
{
    DOMNodeList* childListTemp;
    DOMNode* childTemp;
    childListTemp =parentTag->getChildNodes();

    for(int i=1;i<childListTemp->getLength();i++){

        childTemp = childListTemp->item(i);
        char *childname = XMLString::transcode(childTemp->getNodeName());

        Node *treeNodeTemp=NULL;

	    if(childTemp->hasChildNodes()){

            char *childValue = XMLString::transcode(childTemp->getFirstChild()->getNodeValue());
            string childValueStr = childValue;

            //if(childValueStr.length()!=1 && childValueStr != "\n" && DOMNode::TEXT_NODE == childTemp->getFirstChild()->getNodeType()){
            if((childTemp->getNodeType() != DOMNode::TEXT_NODE) && (childTemp->getNodeType() != DOMNode::COMMENT_NODE)){
                treeNodeTemp = new Node(removeStrBlank(childname),removeStrBlank(childValue));
                treeNode->addChild(treeNodeTemp);
                XMLString::release( &childname );
                XMLString::release( &childValue);            
            }else{
                treeNodeTemp = new Node(removeStrBlank(childname),"non_value");

            }
            getTheAttributes(childTemp,treeNodeTemp);

            transferTreeStructure(childTemp,treeNodeTemp);

        }else{

            if((childTemp->getNodeType() != DOMNode::TEXT_NODE) && (childTemp->getNodeType() != DOMNode::COMMENT_NODE)){

                char *childname = XMLString::transcode(childTemp->getNodeName());
                char *childValue = XMLString::transcode(childTemp->getTextContent());

                Node * treeNodeTemp = new Node(removeStrBlank(childname),removeStrBlank(childValue));
                treeNode->addChild(treeNodeTemp);

                //cout << " "<<childname<<":" << childValue<< endl;
                XMLString::release( &childname );
                XMLString::release( &childValue);

                getTheAttributes(childTemp,treeNodeTemp);
            }

        }
    }
    
    
}

bool Parser::checkIfChild(string nodeName,Node* node)
{
  vector<Node*> childList;
  childList= node->getChildren();

  for (int i = 0; i <childList.size(); i++)
  {
    Node* temp = childList[i];
    if(temp->getNodeName()==nodeName){

      return true;
    }
  }
  return false;
}


Node* Parser::getTargetNode(Node* targetNode,string targetName)
{
  vector<Node*>childList= targetNode->getChildren();
  bool found;

  for(int i=0;i<childList.size();i++){
    Node* temp = childList[i];
    if(temp->getNodeName() == targetName){
      found=true;
      return temp;
    }
  }

  found = false;

}



void Parser::childInsert(Node*target,Node*sub)
{   
    vector<Node*> childList = sub->getChildren();

    for(int i=0;i<childList.size();i++)
    {   
        if(childList[i]->getNodeName()!="type" && childList[i]->getNodeName()!="name")
        {
            target->addChild(childList[i]);
        }
    }

}




void Parser::codeGenerate(Node* genRoot, string type,Generator &m_generator){


    if(type=="xpdl:cpu"){
        m_generator.cpuClassGen(genRoot,type);
        m_generator.endOfClass();
    }
    else if(type=="xpdl:gpu"){
        m_generator.gpuClassGen(genRoot,type);
        m_generator.endOfClass();
    }
    else if(type=="xpdl:memory"){
        m_generator.memClassGen(genRoot,type);
        m_generator.endOfClass();
    }
    else if(type=="xpdl:interconnect"){
        m_generator.interconnectClassGen(genRoot,type);
        m_generator.endOfClass();
    }
    else if(type=="xpdl:software"){
        m_generator.swClassGen(genRoot,type);
        m_generator.endOfClassSW();

    }
    else if(type=="xpdl:library"){
        m_generator.swClassGen(genRoot,type);
        m_generator.endOfClassSW();

    }
    else if(type=="xpdl:os"){
        m_generator.swClassGen(genRoot,type);
        m_generator.endOfClassSW();

    }

}



void Parser::codeGenerateSetUp()
{
	//build a generator based on IR
    Generator m_generator(platformLocalRoot);
    	
    m_generator.setUpHeader();

    //m_generator.insertNamespace();
    vector<Node*> platformChildList = platformLocalRoot->getChildren();

    for(int i=0;i<platformChildList.size();i++)
    {
        Node* temp=platformChildList[i];
        //cout<<temp->getNodeName()<<endl;
	
	//software, os, or library
            if(temp->getChildren().size()!=0 && temp->getNodeName()!="xpdl:component"&&(temp->getNodeName()=="xpdl:software" || temp->getNodeName()=="xpdl:os" ||temp->getNodeName()=="xpdl:library") )
            {
                //Node* tempRoot = new Node(temp->getNodeName(),"");
                //tempRoot->addChild(temp);
                codeGenerate(temp,temp->getNodeName(),m_generator);
            }        
    }


    m_generator.swDefGen(); //empty and empty_all generation
    
    m_generator.insertNamespace();

    for(int i=0;i<platformChildList.size();i++)
    {
        Node* temp=platformChildList[i];
	//anything but not software, os, or library
            if(temp->getChildren().size()!=0 &&temp->getNodeName()!="xpdl:component"&& temp->getNodeName()!="xpdl:software"&&temp->getNodeName()!="xpdl:os" &&temp->getNodeName()!="xpdl:library"){
                codeGenerate(temp,temp->getNodeName(),m_generator);
            }        
    }


    m_generator.platformClassGen(); //system struct generation
    m_generator.endOfClass();
    m_generator.runtime_worker_gen();
    m_generator.swTemplateGen(); //software retrieve method
    m_generator.deviceTemplateGen(); //template method get
    


}
/*
    parseXmlFile parse the XML files with type-name pair
*/

Node* Parser::parseXmlFile(string xmlFile, vector<string> argList)
{
    string toParse=rootFile;

    if(xmlFile!=rootFile)
    {
        toParse = xmlFilePath+"/"+xmlFile;
    }
    else
    {
        toParse = rootFile;
    }
    

    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch (XMLException& e)
    {
        char* message = XMLString::transcode(e.getMessage());
        cout << "Initialization error:"<< message << ""<<endl;
        XMLString::release(&message);
    }
    
    XercesDOMParser* parser = new XercesDOMParser;
    ParserErrorHandler* errHandler = (ParserErrorHandler*) new HandlerBase();

    vector<DOMElement*>rootList;
    //parser->setExternalNoNamespaceSchemaLocation("xpdl_schema.xsd");
       
    if (parser->loadGrammar(xsdFilePath.c_str(), Grammar::SchemaGrammarType)==NULL) {
        cerr<<"[Error.] Could not load the schema file: "<<xsdFilePath<<endl;
    }
    else{
        //cout<<"[info.] "<<"Loading "<<xsdFile<<" Success !"<<endl;
        
            ParserErrorHandler validatorErrorHandler;
            parser->setErrorHandler(&validatorErrorHandler);
            parser->setDoNamespaces(true);
            parser->setDoSchema(true);
            parser->setValidationConstraintFatal(true);
            parser->setValidationSchemaFullChecking(true);
            parser->setValidationScheme(XercesDOMParser::Val_Auto);
            parser->setDoNamespaces(true);
            //parser->setExternalSchemaLocation(xsdFilePath.c_str());
            //parser->setValidationSchemaFullChecking(true);
            //parser->setExternalSchemaLocation(xsdFilePath.c_str());

            parser->parse(toParse.c_str());

            if(parser->getErrorCount()==0){

                //cout<<"[info.] "<<"Start parsing "<<xmlFile<<" "<<xsdFile<<endl;
                DOMDocument* xmlDoc = parser->getDocument();
                DOMElement* elementRoot = xmlDoc->getDocumentElement();

                //rootList.push_back(elementRoot);
                //DOMNodeList* childListTemp;
                //childListTemp =rootList->getChildNodes();

                char *eleName = XMLString::transcode(elementRoot->getNodeName());
                Node* localRoot = new Node(eleName,"non_value");

                if(localRoot !=NULL){
                    transferTreeStructure(elementRoot,localRoot);
                }

                delete parser;
                delete errHandler;
                
                try
                {
                    XMLPlatformUtils::Terminate();
                }
                catch (XMLException& e)
                {
                    char* message = XMLString::transcode(e.getMessage());
                    //cout << "Terminating error:"<< message << ""<<endl;
                    XMLString::release(&message);
                    
                }
                
                if(xmlFile !=rootFile)
                {
                    Node* mbNode = NULL;

                    if(checkIfChild("xpdl:microbenchmark",localRoot))
                    {
                        mbNode = getTargetNode(localRoot,"xpdl:microbenchmark");
                    }

                    vector<Node*> childList = localRoot->getChildren();
                    for(int i=0;i<childList.size();i++)
                    {
                        if(checkIfChild("type",childList[i]))
                        {
                            if(mbNode!=NULL)
                            {
                                childList[i]->addChild(mbNode);
                            }
                            
                            Node* typeNode = getTargetNode(childList[i],"type");

                            for(int j=0;j<argList.size();j++)
                            {
                                if(argList[j]==typeNode->getNodeValue()+".xml")
                                {
                                    Node* insertTarget = childList[i];
                                    Node* temp = parseXmlFile(argList[j],argList);
                                    vector<Node*> tempChildList = temp->getChildren();
                                    //--
                                    for(int k=0;k<tempChildList.size();k++)
                                    {
                                        if(checkIfChild("name",tempChildList[k]))
                                        {
                                            Node* nameNode = getTargetNode(tempChildList[k],"name");
                                            if(typeNode->getNodeValue() == nameNode->getNodeValue())
                                            {
                                                childInsert(childList[i],tempChildList[k]);
                                                break;
                                            }
                                        }

                                    }
                                }
                            }
                        }

                    }
                }


                /*cout<<"...................................................................."<<endl;
                cout<<"[In parseXmlFile()] Parsing: "<<xmlFile<<" and "<<xsdFilePath <<" Success !"<<endl;
                cout<<"...................................................................."<<endl;
                //localRoot->printOutTree(localRoot);*/

                return localRoot;
                //cout<<"[info.] "<<"Parsing "<<xmlFile<<" "<<xsdFile<<" Success!"<<endl;
                //cout<<"\n";
            }
            else{
                cout<<"[Error.] "<<"Parsing "<<xmlFile<<" "<<xsdFilePath<<"Fail"<<endl;
              //  cout<<"\n";
            }

        }

}

/*
loadXmlFile() loads the wholeMachine.xml and search for the type-name pair 
and link them together
*/
void Parser::loadXmlFile(string sysFile)
{
	//TODO: make the for loop into function()
	//TODO: think to recursive whole in the begining

    vector<string> xmlFileList = vector<string>();
    string dir = string(xmlFilePath);
    getdir(dir,xmlFileList);

    //TODO: rename platformLocalRoot to ir_root
    platformLocalRoot = parseXmlFile(sysFile,xmlFileList);

    vector<Node*> childList = platformLocalRoot->getChildren();
    for(int i=0;i<childList.size();i++)
    {
	    //parse + model_linker

	    //check if "type" exist
        if(checkIfChild("type",childList[i]))
        {
		//if exist, get it
            Node* typeNode = getTargetNode(childList[i],"type");

	    //a for loop to get the target xml file
            for(int j=0;j<xmlFileList.size();j++)
            {
		    //add a postfix ".xml"
                if(xmlFileList[j]==typeNode->getNodeValue()+".xml")
                {   
			//parse it, enabling recursion only here
                    Node* temp = parseXmlFile(xmlFileList[j],xmlFileList);
		        //get all children
                    vector<Node*> tempChildList = temp->getChildren();
                    //--
                    for(int k=0;k<tempChildList.size();k++)
                    {
                        if(checkIfChild("name",tempChildList[k]))
                        {
                            Node* nameNode = getTargetNode(tempChildList[k],"name");
                            if(typeNode->getNodeValue() == nameNode->getNodeValue())
                            {
                                childInsert(childList[i],tempChildList[k]);
                                break;
                            }
                        }

                    }
                }
            }
        }
    }

    //treeTravel(platformLocalRoot);
    /*cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    cout<<"Whole IR "<<endl;
    cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;*/
    //platformLocalRoot->printOutTree(platformLocalRoot);
    //TODO:change to microbenchmarkExe()
    treeTravel(platformLocalRoot);
    codeGenerateSetUp();


    


}
