#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

//#include <fstream>
//#include "xmlParser.h"

#include <iostream>
#include <iomanip>
#include <sstream>  
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include "tree.h"

using namespace std;

class Generator{

public:
	
	Generator(Node*);

	~Generator();
	
	void setUpHeader();
	void cpuClassGen(Node*,string);
	void memClassGen(Node*,string);
	void gpuClassGen(Node*,string);
	void interconnectClassGen(Node*,string);
	void swClassGen(Node*,string);

	void platformClassGen();

	void deviceTemplateGen();
	void swTemplateGen();
	void swSetGen();
	void runtime_worker_gen();


	void insertNamespace();
	void endOfClass();
	void endOfClassSW();
	void swDefGen();
	

    
private:
	//int accessCount = 0;
	int swClassGenCount=0;


	map<string,int> deviceMap; //device count for all xpdl hardware
	map<string,string> swMap; //TODO:check if it is necessary

	vector<string>libList; //name list for each software type
  	vector<string>osList;  //same
  	vector<string>swList;  //same


	ofstream m_file; //filename for code generation
	Node* root;      //IR
	Node* localRoot; //temporary IR pointer
	int cores_in_group, group_num; //cpu's count

	vector<Node*> getNodesAddress(string nodeName) ;
	vector<Node*> getNodesAddressRec(string nodeName,Node* node,vector<Node*>&);
	bool checkIfChild(string,Node*);
	Node* getTargetNode(Node*,string);

	void setUpClass(string);
	void setUpClassSW(string);
	void writeToFile(string,string,string);
	void writeToFileMacro(string,string,string);
	void writeToFileSW(string,string,string);
	void codeGenerator(string,string,string,string);
	void codeGeneratorWithName(string,string,string,string,string,string);
	string getClassName(Node*,string);

	unsigned long long get_MB(const string& size, const string& unit);
	//string get_info(Node* matched_sub_tree, string target_parent_name, string target_child_name);
	string get_info(Node* matched_sub_tree, const vector<string>& paths);

	void emptyClassGen();
	string removeXpdInStr(string);
    
    //cpu code
	void getCpuId();
	void getCpuCoresFreq();
	void getCacheSize();
	void getPowerModelType();
	void getCpuGroupNum();
	void getCpuName();
	void getNumOfCores();
	//void getCpuGroupNum();

	//memory
	void getMemSize();
	void getMemType();
	void getMemStaticPwr();
	void getMemId();
	void getMemBandwith();

    //GPU
    void getGpuId();
    void getGpuNumOfCores();
    void getGpuName();
    void getGpuProgrammingModel();
    void getComputeCapability();

    void getMaxDimGrid();

    void getMaxDimThreadBlock();

    void getGpuGroupNum();

    void getGpuCoreFreq();
    void getGpuMemSize();
    void getGpuCacheSize();
    void getGpuCompiler();

    //interconnect
    string getInterconnectName();
    void getChannelMaxBw();
    void getChannelEnergyPerByte();
    void getInterconnectId();

    //

    void getPlatformDevNum();
    void vecGen();

    void getLibPath();
    void getOSPath();
    void getSoftwarePath();
    void getLibId();
    void getOSId();
    void getSoftwareId();




};



#endif

