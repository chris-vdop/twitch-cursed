/**
 * \author Christopher "VdoP" Regali
 * \file configFile.h
 * \brief contains the headers for the interface to the ascii configuration file
 * \date 2014
 */


#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

#ifndef CONFIGFILE_H
#define CONFIGFILE_H
using namespace std;

class configFile
{
public:
  
  static configFile* getInstance(){ 
     if (instance == NULL)
       instance = new configFile();
     return instance;
     
  };
    
    ~configFile();
    int readFile(std::string _inputFile);
    int parseFile();
    int writeFile(std::string _outputFile);
    
    map<string, string>& getDataMap() {
        return dataMap;
    }
    string getContents(string _key){return dataMap[_key];}
    string setContents(string _key, string _contents){dataMap[_key]=_contents;}

    static vector<string> explodeStringCustomDelim ( const string& _input,const string& _delim );

    static vector<string> explodeStringWhiteSpace(string _input);
    static string implodeString(vector< string > _strList, string _delim);
    
    static int  StrToInt(string _arg);
    static string IntToStr(int _arg);
    static double StrToDouble(string _arg);

private:
    static configFile* instance;
    configFile();
    map<string, string> dataMap;
    vector<string> originalFile;
    vector<string> keyMap;

};


#endif

