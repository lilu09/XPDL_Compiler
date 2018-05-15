#include "codeGenerator.hpp"
#include <assert.h>


Generator::Generator(Node* root_){

	root = root_;

  deviceMap.insert(make_pair("cpu",0));
  deviceMap.insert(make_pair("memory",0));
  deviceMap.insert(make_pair("gpu",0));
  deviceMap.insert(make_pair("interconnect",0));

}

Generator::~Generator(){

}

vector<Node*> Generator::getNodesAddress(string nodeName)
{
  vector<Node*> ptrList;
  getNodesAddressRec(nodeName,localRoot,ptrList);

  return ptrList;
}

vector<Node*> Generator::getNodesAddressRec(string nodeName,Node* node,vector<Node*>& ptrList)
{
  vector<Node*> childList = node->getChildren();
    for(int i=0;i<childList.size();i++){
      Node *temp = childList[i];
      if (temp->getNodeName() == nodeName)
      {
        ptrList.push_back(temp);
        getNodesAddressRec(nodeName,temp,ptrList);
      }
      else
      {
        getNodesAddressRec(nodeName,temp,ptrList);
      }
  }
  return ptrList;

}


bool Generator::checkIfChild(string nodeName,Node* node)
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

Node* Generator::getTargetNode(Node* targetNode,string targetName)
{
  

  vector<Node*>childList= targetNode->getChildren();
  
  if(targetNode->getNodeName()==targetName){
    childList.push_back(targetNode);
  }

  bool found;

    try{

      for(int i=0;i<childList.size();i++){
        Node* temp = childList[i];
        if(temp->getNodeName() == targetName){
          found=true;
          return temp;
        }
      }

      found = false;


      if(found==false)
      {
        throw found;
      }
  }
  catch(...){

    cerr<<"Couldn't find the target node : "<<targetName<<"in getTargetNode()"<<endl;
  }

}

string Generator::removeXpdInStr(string xpdlString)
{
  if(xpdlString=="xpdl:memory"){
    return"memory";

  }else if(xpdlString=="xpdl:cpu"){
    return"cpu";

  }else if(xpdlString=="xpdl:gpu"){
    return "gpu";

  }else if(xpdlString=="xpdl:interconnect"){
    return "interconnect";

  }

}

void Generator::writeToFile(string funcType,string funcName,string result)
{

  for (int i=0; i<funcName.length(); ++i){
    funcName[i]=tolower(funcName[i]);
  }

  if(result !="?" && result != " "){
    m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
    if(m_file.is_open()){
      if(funcType=="string"){
        m_file<<"\t\t"<< "static constexpr char* "<<funcName<<"=(char*)\""<<result<<"\";\n\n";
      }
      else if(funcType=="double" || funcType=="float"){
        m_file<<"\t\t"<< "static constexpr "<<funcType<<" "<<funcName<<"="<<result<<";\n\n";
      }
      else{
        m_file<<"\t\t"<< "static constexpr "<<funcType<<" "<<funcName<<"="<<result<<";\n\n";
      }
      m_file.close();
    }
  }
}

void Generator::writeToFileMacro(string header, string name, string result){
	m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
	if(m_file.is_open()){
		m_file<<"\t\t"<< header <<" "<< name <<" "<< result <<"\n\n";
	}
}


void Generator::writeToFileSW(string funcType,string funcName,string result)
{
  for (int i=0; i<funcName.length(); ++i){
    funcName[i]=tolower(funcName[i]);
  }
  if(result !="?" && result != " "){
    m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
    if(m_file.is_open()){
      if(funcType=="string"){
        m_file<<"\t"<< "static constexpr char* "<<funcName<<"=(char*)\""<<result<<"\";\n\n";
      }
      else if(funcType=="double"){
        m_file<<"\t"<< "static constexpr "<<funcType<<" "<<funcName<<"="<<result<<";\n\n";
      }
      else{
        m_file<<"\t"<< "static const "<<funcType<<" "<<funcName<<"="<<result<<";\n\n";
      }
      m_file.close();
    }
  }
}


/**
 * 
 * \param targetParent the parent contains targetNode
 * \param targetNode the final target we want to find
 * \param genFucType the data type of targetNode
 * \param genFuncName static variable name
 */
void Generator::codeGenerator(string targetParent,string targetNode,string genFucType,string genFuncName)
{
  vector<Node*> nodeList;
  string parent = targetParent;
  
  string funcType = genFucType;
  string funcName = genFuncName;
  
  //find the parent
  if(localRoot->getNodeName()==parent)
  {
    nodeList.push_back(localRoot);
  }
  else
  {
	  //find in all IR, only a string is given
    nodeList=getNodesAddress(parent);
  }

  try{

    if(nodeList.size()==0)
    {
      throw nodeList.size();
    }

    //find the final targetNode
    for(int i=0;i<nodeList.size();i++){
      if(checkIfChild(targetNode,nodeList[i])){
	      //search only from the node given
        Node* target = getTargetNode(nodeList[i],targetNode);
        string funcRes = target->getNodeValue();
        writeToFile(funcType,funcName,funcRes);
      }
    }
  }
  catch(...){
    cerr<<"Couldn't find the node : "<<targetParent<<"in finction codeGenerator()"<<endl;
  }

}

//string Generator::get_info(Node* matched_sub_tree, string target_parent_name, string target_child_name){
	////cout<<matched_sub_tree->getNodeName()<<endl;
	

	//Node* target = getTargetNode(matched_sub_tree,target_parent_name);
	//target = getTargetNode(target,target_child_name);
	//return target->getNodeValue();

//}

//string Generator::get_info(Node* matched_sub_tree, string target_parent_name, string target_child_name){
	////cout<<matched_sub_tree->getNodeName()<<endl;
	
	//Node* target=matched_sub_tree;

	//target = getTargetNode(target,target_parent_name);
	//target = getTargetNode(target,target_child_name);
	//return target->getNodeValue();

//}

string Generator::get_info(Node* matched_sub_tree, const vector<string>& paths){
	
	Node* target=matched_sub_tree;

	//va_list args;
	//va_start(args, paths);

	for(auto const &current_path_tag:paths)
		target = getTargetNode(target,current_path_tag);
	//target = getTargetNode(target,target_child_name);
	return target->getNodeValue();

}


void Generator::codeGeneratorWithName(string targetParent,string targetNode,string targetName,string genFucType,string genFuncName, string preNamed)
{
  vector<Node*> nodeList;
  string parent = targetParent;

  if(localRoot->getNodeName()==parent)
  {
    nodeList.push_back(localRoot);
  }
  else
  {
    nodeList=getNodesAddress(parent);
  }
  string funcType = genFucType;
  string funcName;
  string funcRes;

  assert(nodeList.size() != 0);


  try{

    if(nodeList.size()==0)
    {
      throw nodeList.size();
    }
    for(int i=0;i<nodeList.size();i++){
      if(checkIfChild(targetName,nodeList[i])){
        Node* targetNameLocal = getTargetNode(nodeList[i],targetName);

        assert(targetNameLocal != NULL);

        Node* targetValue = getTargetNode(nodeList[i],targetNode);

        assert(targetValue != NULL);

        funcName = targetNameLocal->getNodeValue()+"_"+genFuncName;

        /*if(preNamed==""){
          funcName = targetNameLocal->getNodeValue()+"_"+genFuncName;
        }else{
          funcName = targetNameLocal->getNodeValue()+"_"+genFuncName;
        }*/
        if(targetParent=="xpdl:software"||targetParent=="xpdl:os"||targetParent=="xpdl:library"){
          funcRes = targetValue->getNodeValue();
          writeToFileSW(funcType,funcName,funcRes);
        }else{
          funcRes = targetValue->getNodeValue();
          writeToFile(funcType,funcName,funcRes);
        }
      }
    }
  }
  catch(...){
    cerr<<"Couldn't find the node : "<<targetParent<<"in codeGeneratorWithName()"<<endl;
  }
}

 //--------------------------Class generator--------------------------------

void Generator::setUpHeader()
{
  ifstream my_file("xpdl.hpp");
  if(my_file){
   remove("xpdl.hpp");
  }

  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  if(m_file.is_open()){
    m_file<<"#include <iostream>\n#include <string>\n#include <boost/mpl/vector.hpp>\n#include <boost/mpl/at.hpp>\n#include <boost/mpl/map.hpp>\n#include <boost/mpl/set.hpp>\n\n\nusing namespace std;\nnamespace mpl=boost::mpl;\n";
    m_file.close();
  } 



}

string Generator::getClassName(Node* node,string targetParent)
{
  vector<Node*> nodeList;
  string parent = targetParent;
  string className;
  
  if(node->getNodeName()==targetParent)
  {
    nodeList.push_back(node);
  }
  //nodeList=getNodesAddress(parent);

  try{

    if(nodeList.size()==0)
    {
      throw nodeList.size();
    }

    for(int i=0;i<nodeList.size();i++){
      if(checkIfChild("id",nodeList[i])){
        Node* target = getTargetNode(nodeList[i],"id");
        className= target->getNodeValue();
        break;
      }    
    }

    return className;
  }
  catch(...){
    cerr<<"Couldn't find the node : "<<targetParent<<endl;
  }



}

void Generator::insertNamespace(){

  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
   if(m_file.is_open())
     m_file<<"namespace xpdl\n{";
     m_file.close();


}


void Generator::setUpClass(string className){

  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  if(m_file.is_open())
    m_file<<"\n\tstruct "<<className<<"\n\t{\n"<<"\n";
    m_file.close();

}

void Generator::setUpClassSW(string className){

  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  if(m_file.is_open())
    m_file<<"\nstruct "<<className<<"\n{\n"<<"\n";
    m_file.close();

}


void Generator::endOfClass(){
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  
  if(m_file.is_open()){
    m_file<<"\n\t};\n\n";
    m_file.close();
  }

}


void Generator::endOfClassSW(){
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  
  if(m_file.is_open()){
    m_file<<"\n};\n";
    m_file.close();
  }

}

void Generator::deviceTemplateGen()
{
  map<string,int>::iterator it;
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  if(m_file.is_open())
  {
    m_file<<"\n\ttemplate <class PUs, int index>\n";
    m_file<<"\t\tstruct get : mpl::at_c<PUs,index>::type {};\n\n";
  }
  m_file<<"}";
  m_file.close();

}

void Generator::swSetGen()
{
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  if(libList.size()>0){
    if(m_file.is_open())
    {
      m_file<<"\t\ttypedef mpl::set<";
      for(int i =0;i<libList.size()-1;i++){
        m_file<<libList[i]<<", ";
      }
      m_file<<libList[libList.size()-1]<<"> libraries;\n";
    }
  }
  if(swList.size()>0){
    if(m_file.is_open())
    {
      m_file<<"\t\ttypedef mpl::set<";
      for(int i =0;i<swList.size()-1;i++){
        m_file<<swList[i]<<", ";
      }
      m_file<<swList[swList.size()-1]<<"> softwares;\n";
    }
  }

  if(osList.size()>0){
    if(m_file.is_open())
    {
      m_file<<"\t\ttypedef mpl::set<";
      for(int i =0;i<osList.size()-1;i++){
        m_file<<osList[i]<<", ";
      }
      m_file<<osList[osList.size()-1]<<"> oss;\n";
    }
  }

  m_file.close();

}

unsigned long long Generator::get_MB(const string& size, const string& unit){
	unsigned long long sizell=stoll(size); 
	if(unit=="MiB")
		return sizell;
	else if(unit=="KiB")
		return sizell/1024;
	else if(unit=="GiB")
		return sizell*1024;
	else if(unit=="TiB")
		return sizell*1024*1024;
	else
		assert(0);
}

void Generator::runtime_worker_gen()
{
    m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
    if(m_file.is_open())
    {
      m_file<<"\t\tconst int total_workers=";
      unsigned int total_workers=deviceMap["cpu"]+deviceMap["gpu"];
      m_file<<total_workers<<";"<<endl;
      m_file<<"\t\tconst std::string unit=";
      m_file<<'"';
      m_file<<"MiB";
      m_file<<'"';
      m_file<<";"<<endl;
      //compute name
      m_file<<"\t\tconst std::string worker_name[total_workers]{";

      vector<Node*> platformChildList = root->getChildren();
      unsigned int number_count=0;

      for(int i=0;i<platformChildList.size();i++) {
	      Node* temp=platformChildList[i];
	      if(temp->getChildren().size()!=0 && temp->getNodeName()=="xpdl:cpu"){
		      m_file<<'"';
		      m_file<<get_info(temp, {"id"});
		      m_file<<'"';
		      if(++number_count!=total_workers)
			      m_file<<",";
	      }
	      if(temp->getChildren().size()!=0 && temp->getNodeName()=="xpdl:gpu"){
		      m_file<<'"';
		      m_file<<get_info(temp, {"id"});
		      m_file<<'"';
		      if(++number_count!=total_workers)
			      m_file<<",";
	      }
      }


      m_file<<"};"<<endl;


      //compute size
      m_file<<"\t\tint memory_size[total_workers]{";

      //insert each worker size
      
      string memory_size_in_XML, unit;
      



      //compute total cpu memory
      unsigned long long cpu_memory=0;
      for(int i=0;i<platformChildList.size();i++) {
	      Node* temp=platformChildList[i];
	      if(temp->getChildren().size()!=0 && temp->getNodeName()=="xpdl:memory"){
		      memory_size_in_XML=get_info(temp, {"xpdl:memory","size"} );
		      unit=get_info(temp, {"xpdl:memory","unit"} );
		      cpu_memory+=get_MB(memory_size_in_XML, unit);
	      }
      }

      //write memory for each worker
      number_count=0;
      for(int i=0;i<platformChildList.size();i++) {

	      Node* temp=platformChildList[i];
	      if(temp->getChildren().size()!=0 && temp->getNodeName()=="xpdl:cpu"){
		      m_file<<cpu_memory;
		      if(++number_count!=total_workers)
			      m_file<<",";
	      }
	      if(temp->getChildren().size()!=0 && temp->getNodeName()=="xpdl:gpu"){
		      memory_size_in_XML=get_info(temp, {"xpdl:memory","size"} );
		      unit=get_info(temp, {"xpdl:memory","unit"} );
		      m_file<<get_MB(memory_size_in_XML, unit);
		      if(++number_count!=total_workers)
			      m_file<<",";
	      }
      }

      m_file<<"};"<<endl;
    }
}

void Generator::vecGen(){
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  map<string,int>::iterator it;


  for(it = deviceMap.begin();it != deviceMap.end(); ++it){
    if(it->second !=0 && it->first !="software"){
      if(m_file.is_open()){
        m_file<<"\t\ttypedef mpl::vector<";
        for(int i=1;i<=it->second;i++){
          if(i != it->second){
            m_file<<it->first<<"_"<<i<<", ";
          }
          else{

            if(it->first=="memory"){
              m_file<<it->first<<"_"<<i<<"> memories;\n";
            }else{
              m_file<<it->first<<"_"<<i<<"> "<<it->first<<"s;\n";
            }
          }
        }
      }
    }

    
}
m_file.close();

}


void Generator::swDefGen()
{
  m_file<<"\n\n";
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);

  if((swList.size()>0 || osList.size()>0 || libList.size()>0)&& m_file.is_open()){
      m_file<<"#define XDPL_PREPAPRE_SW_QUERY(sw_name) struct sw_name\n";

  }
  m_file<<"\n\n";
  m_file.close();

}

void Generator::swTemplateGen()
{
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);

  if((libList.size()>0||swList.size()>0||osList.size()>0) && m_file.is_open()){
      m_file<<"\ttemplate <class Software_Set, class A_Software>\n";
      m_file<<"\t\tstruct is_installed : mpl::has_key<Software_Set, A_Software>::type {};\n\n";

      m_file<<"\ttemplate <class Software_Set, class A_Software>\n";
      m_file<<"\t\tstruct get_lib_anyway : mpl::at<Software_Set,A_Software>::type {\n";
      m_file<<"\t\t\ttypedef get_lib_anyway type;\n\t\t};\n";



      m_file<<"\ttemplate<class Software_Set, class A_Software>\n";
      m_file<<"\t\tstruct get_lib : mpl::eval_if< mpl::int_< xpdl::is_installed<Software_Set,A_Software>::value>,\n";
      m_file<<"\t\t\tget_lib_anyway<Software_Set,A_Software>, xpdl_empty>::type {\n\t\t\ttypedef get_lib type;\n\t\t};\n"; 



     }

  m_file.close();

}


void Generator::emptyClassGen(){
  setUpClassSW("xpdl_empty_all");
  
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  if(m_file.is_open()){
    m_file<<"\tstatic constexpr char* xpdl_id=(char *)\"xpdl_empty\";\n";
    m_file.close();
  } 
  endOfClassSW();

  setUpClassSW("xpdl_empty");
  m_file.open("xpdl.hpp",ios::out | ios::app | ios::binary);
  if(m_file.is_open()){
    m_file<<"\ttypedef xpdl_empty_all type;\n";
    m_file.close();
  } 
  endOfClassSW();


}

void Generator::cpuClassGen(Node* tempRoot,string xpdlNode)
{

  localRoot = tempRoot;

  xpdlNode=removeXpdInStr(xpdlNode);
  deviceMap[xpdlNode]++;


  //string className = getClassName(tempRoot ,"xpdl:"+xpdlNode);

  setUpClass("cpu_"+to_string(deviceMap[xpdlNode]));

  getCpuId();
  getNumOfCores();
  //getCpuName();
  getCpuGroupNum();
  getCpuCoresFreq();
  getCacheSize();
  getPowerModelType();

}

void Generator::memClassGen(Node* tempRoot,string xpdlNode)
{
  localRoot = tempRoot;
  xpdlNode=removeXpdInStr(xpdlNode);
  deviceMap[xpdlNode]++;
  //xpdlNode=removeXpdInStr(xpdlNode);
  //string className = getClassName(tempRoot ,"xpdl:"+xpdlNode);
  setUpClass("memory_"+to_string(deviceMap[xpdlNode]));

  getMemId();
  getMemSize();
  //getMemType();
  getMemBandwith();
  getMemStaticPwr();

}

void Generator::gpuClassGen(Node* tempRoot,string xpdlNode)
{
  localRoot = tempRoot;
  xpdlNode=removeXpdInStr(xpdlNode); //remove xpdl:: in string
  deviceMap[xpdlNode]++;    //increase the count in deviceMap for this kind of hardware
  setUpClass("gpu_"+to_string(deviceMap[xpdlNode]));  //generate class header
  getGpuId();
  getGpuNumOfCores();
  getGpuProgrammingModel();
  getComputeCapability();
  getGpuCoreFreq();
  getMaxDimGrid();
  getMaxDimThreadBlock();
  getGpuMemSize();
  getGpuCacheSize();
  getGpuGroupNum();
  getGpuCompiler();


}

void Generator::interconnectClassGen(Node* tempRoot,string xpdlNode)
{
  localRoot = tempRoot;
  xpdlNode=removeXpdInStr(xpdlNode);
  deviceMap[xpdlNode]++;
  //string className = getClassName(tempRoot ,"xpdl:"+xpdlNode);
  setUpClass("interconnect_"+to_string(deviceMap[xpdlNode]));

  getInterconnectId();

  getChannelMaxBw();
  getChannelEnergyPerByte();

}

void Generator::swClassGen(Node* tempRoot,string xpdlNode)
{
  string className;
  localRoot = tempRoot;

  Node* versionNode = getTargetNode(tempRoot,"version");
  string version = versionNode->getNodeValue();
  Node* idNode = getTargetNode(tempRoot,"id");
  string id = idNode->getNodeValue();
  replace( version.begin(), version.end(), '.', '_');
  string newId = id+"_"+version;
  idNode->setNodeValue(newId);
  //deviceMap[xpdlNode]++;

  if(swClassGenCount==0){
    emptyClassGen();
  }


  string localRootName = localRoot->getNodeName();

  if(localRootName =="xpdl:library"){
    className = getClassName(localRoot,"xpdl:library");
  }
  if(localRootName =="xpdl:os"){
    className = getClassName(localRoot,"xpdl:os");
  }
  if(localRootName =="xpdl:software"){
    className = getClassName(localRoot,"xpdl:software");
  }
  //string className = getClassName(tempRoot ,"xpdl:"+xpdlNode);
  setUpClassSW(className);

  if(localRootName =="xpdl:library"){

    swMap.insert(make_pair(className,"library"));
    libList.push_back(className);
    getLibPath();
    getLibId();  
  }
  if(localRootName =="xpdl:os"){
    swMap.insert(make_pair(className,"os"));
    osList.push_back(className);
    getOSPath();
    getOSId();
  }
  if(localRootName =="xpdl:software"){
    swMap.insert(make_pair(className,"sw"));
    swList.push_back(className);
    getSoftwarePath();
    getSoftwareId();
  }
  

   swClassGenCount++;


}

void Generator::platformClassGen()
{
  setUpClass("system");
  getPlatformDevNum();
  vecGen();
  swSetGen();




}

//----------API implemantation start from here------------------------

void Generator::getCpuId()
{
  //type
  codeGenerator("xpdl:cpu","id","string","xpdl_id");
}

/*void Generator::getCpuName()
{
  //type
  codeGenerator("xpdl:cpu","type","string","xpdl_type");
}*/

void Generator::getNumOfCores()
{

  codeGenerator("xpdl:cpu","num_of_cores","int","num_of_cores");
  codeGenerator("xpdl:cpu","num_of_threads","int","num_of_hardware_threads");

}


void Generator::getCpuGroupNum()
{
  codeGeneratorWithName("xpdl:group","quantity","prefix","int","num","");
}

void Generator::getCpuCoresFreq()
{
  codeGenerator("xpdl:core","frequency","double","core_frequency");
  codeGenerator("xpdl:core","unit","string","core_frequency_unit");
}

void Generator::getCacheSize()
{
  codeGeneratorWithName("xpdl:cache","size","name","int","cache_size","cpu");
  codeGeneratorWithName("xpdl:cache","unit","name","string","cache_size_unit","cpu");
}

void Generator::getPowerModelType()
{
  codeGenerator("xpdl:power_model","type","string","get_power_model_type");
}

//------------------- Memory API ---------------------------

void Generator::getMemId()
{
  codeGenerator("xpdl:memory","id","string","xpdl_id");

}

void Generator::getMemSize()
{
  codeGenerator("xpdl:memory","size","int","size");
  codeGenerator("xpdl:memory","unit","string","size_unit");
}

void Generator::getMemType()
{
  codeGenerator("xpdl:memory","type","string","type");
}

void Generator::getMemStaticPwr()
{
  codeGenerator("xpdl:memory","static_power","double","static_power");
  codeGenerator("xpdl:memory","static_power_unit","string","static_power_unit");
}

void Generator::getMemBandwith()
{
  codeGenerator("xpdl:memory","bandwith","double","bandwith");
}


//----------------------- GPU API ------------------------------------

void Generator::getGpuId()
{

  codeGenerator("xpdl:gpu","id","string","xpdl_id");
}

void Generator::getGpuName()
{
  codeGenerator("xpdl:gpu","id","string","xpdl_id");
}

void Generator::getGpuNumOfCores()
{

  codeGenerator("xpdl:gpu","num_of_cores","int","num_of_cores");


}

void Generator::getGpuProgrammingModel()
{
  codeGenerator("xpdl:gpu","programming_model","string","programming_model");
}

void Generator::getComputeCapability()
{ 
  codeGenerator("xpdl:gpu","compute_capability","double","compute_capability");
}

void Generator::getGpuCoreFreq()
{
  codeGenerator("xpdl:core","frequency","double","core_frequency");
  codeGenerator("xpdl:core","frequency_unit","string","core_frequency_unit");
}

void Generator::getMaxDimGrid()
{
  codeGenerator("xpdl:gpu","max_dim_grid_size_x","int","max_dim_grid_size_x");
  codeGenerator("xpdl:gpu","max_dim_grid_size_y","int","max_dim_grid_size_y");
  codeGenerator("xpdl:gpu","max_dim_grid_size_z","int","max_dim_grid_size_z");
}

void Generator::getMaxDimThreadBlock()
{
  codeGenerator("xpdl:gpu","max_dim_thread_block_x","int","max_dim_thread_block_x");
  codeGenerator("xpdl:gpu","max_dim_thread_block_y","int","max_dim_thread_block_y");
  codeGenerator("xpdl:gpu","max_dim_thread_block_z","int","max_dim_thread_block_z");
}

void Generator::getGpuMemSize()
{
  codeGeneratorWithName("xpdl:memory","size","name","int","mem_size","");
  codeGeneratorWithName("xpdl:memory","unit","name","string","mem_size_unit","");
}

void Generator::getGpuCacheSize()
{
  codeGeneratorWithName("xpdl:cache","size","name","int","cache_size","gpu");
  codeGeneratorWithName("xpdl:cache","unit","name","string","cache_size_unit","");
}

void Generator::getGpuGroupNum()
{
  codeGeneratorWithName("xpdl:group","quantity","name","int","group_num","");
}


void Generator::getGpuCompiler()
{
 codeGenerator("xpdl:gpu","compiler","string","compiler");

}


//-------------------------interconnect----------------------------

string Generator::getInterconnectName(){
  /*
    Since the name node has been deleted in the model linker phase, nerver access again !
  */

  if(localRoot->getNodeName() == "xpdl:interconnect")
  {
    Node* target = getTargetNode(localRoot,"type");
    assert(target != NULL);
    string interconnectName = target->getNodeValue();
    return interconnectName;
  }
  else
  {
    return 0;
  }

}

void Generator::getInterconnectId()
{

  codeGenerator("xpdl:interconnect","id","string","xpdl_id");
}

void Generator::getChannelMaxBw()
{
  string interconnectName = getInterconnectName();
  codeGeneratorWithName("xpdl:channel","max_bandwith","name","double","max_bw",interconnectName);
  codeGeneratorWithName("xpdl:channel","max_bandwith_unit","name","string","max_bw_unit",interconnectName);
}


void Generator::getChannelEnergyPerByte()
{
  string interconnectName = getInterconnectName();
  codeGeneratorWithName("xpdl:channel","energy_per_byte","name","double","energy_per_byte",interconnectName);
  codeGeneratorWithName("xpdl:channel","energy_per_byte_unit","name","string","energy_per_byte_unit",interconnectName);

}



//-----------------------------------------------
void Generator::getPlatformDevNum()
{
  writeToFile("int","num_of_cpus",to_string(deviceMap["cpu"]));
  writeToFile("int","num_of_gpus",to_string(deviceMap["gpu"]));
  writeToFileMacro("#define", "XPDL_NUM_OF_GPUS", to_string(deviceMap["gpu"]) );
  writeToFile("int","num_of_memorys",to_string(deviceMap["memory"]));
  writeToFile("int","num_of_interconnections",to_string(deviceMap["interconnect"]));

}

//-----------------------------------------------

void Generator::getLibPath()
{
   codeGenerator("xpdl:library","path","string","path");

}

void Generator::getOSPath()
{
   codeGenerator("xpdl:os","path","string","path");

}

void Generator::getSoftwarePath()
{
   codeGenerator("xpdl:software","path","string","path");

}

void Generator::getLibId()
{
   codeGenerator("xpdl:library","id","string","xpdl_id");
   codeGenerator("xpdl:library","type","string","xpdl_type");

}

void Generator::getOSId()
{
   codeGenerator("xpdl:os","id","string","xpdl_id");
   codeGenerator("xpdl:os","type","string","xpdl_type");

}

void Generator::getSoftwareId()
{
   codeGenerator("xpdl:software","id","string","xpdl_id");
   codeGenerator("xpdl:software","type","string","xpdl_type");


}



