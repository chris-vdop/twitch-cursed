#include "configFile.h"



configFile* configFile::instance = NULL;

configFile::configFile()
{

}


bool configFile::isNumber(string _input)
{
    std::string::const_iterator it = _input.begin();
    while (it != _input.end() && std::isdigit(*it)) ++it;
      return !_input.empty() && it == _input.end();
}


int configFile::readFile(string _inputFile)
{
    // do the actual reading
    string line;
    ifstream myfile ( _inputFile.c_str() );
    originalFile.clear();
    if ( myfile.is_open() ) {
        while ( myfile.good() ) {
            getline ( myfile,line );
            originalFile.push_back ( line );
        }
        myfile.close();
    } else
        return -1;

    return 0;
}


int configFile::writeFile(string _outputFile)
{
  string line;
  ofstream myfile;
  myfile.open(_outputFile.c_str());
  if (myfile.is_open())
  {
    map<string, string>::iterator myIter;
    for (myIter = dataMap.begin(); myIter != dataMap.end();++myIter)
    {
      myfile << myIter->first << " " <<myIter->second << endl;
    }
    myfile.flush();
    myfile.close();
    return 0;
      
  }
  else
    return -1;
}





string configFile::IntToStr(int _arg)
{

    stringstream sstr;
    sstr << _arg;
    return sstr.str();
}

int configFile::StrToInt(string _arg)
{
    stringstream sstr;
    int i;
    sstr << _arg;
    sstr >> i;
    return i;
}

double configFile::StrToDouble(string _arg)
{
    stringstream sstr;
    double i;
    sstr << _arg;
    sstr >> i;
    return i;
}



vector< string > configFile::explodeStringCustomDelim(const string& _input, const string& _delim)
{
    vector<string> arr;

    int _inputleng = _input.length();
    int delleng = _delim.length();
    if ( delleng==0 )
        return arr;//no change

    int i=0;
    int k=0;
    while ( i<_inputleng )
    {
        int j=0;
        while ( i+j<_inputleng && j<delleng && _input[i+j]==_delim[j] )
            j++;
        if ( j==delleng )
        {   //found _delim
            if (_input.substr ( k, i-k ) != "")
                arr.push_back ( _input.substr ( k, i-k ) );
            i+=delleng;
            k=i;
        }
        else
        {
            i++;
        }
    }
    if (_input.substr ( k, i-k ) != "" && _input.substr ( k, i-k ) != " " && _input.substr ( k, i-k ) != "\t")
        arr.push_back ( _input.substr ( k, i-k ) );
    return arr;
}

vector< string > configFile::explodeStringWhiteSpace(string _input)
{
    vector<string> array;

    int inputlength = _input.length();
    int whitespaceLength = 1;
    int i = 0;
    int k = 0;
    while (i < inputlength) {
        int j = 0;
        while (i + j < inputlength && j < whitespaceLength
                && (_input[i + j] == ' ' || _input[i + j] == '\t'))
            j++;
        if (j == whitespaceLength) { //found whitespace
            if (_input.substr(k, i - k) != "")
                array.push_back(_input.substr(k, i - k));
            i += whitespaceLength;
            k = i;
        } else {
            i++;
        }
    }
    if (_input.substr(k, i - k) != "")
        array.push_back(_input.substr(k, i - k));
    return array;
}


int configFile::parseFile()
{
    keyMap.clear();
    dataMap.clear();

    if (originalFile.size() == 0)
        return -1;

    for (unsigned int i=0; i < originalFile.size() ; i++ ) {
        //commented lines start with a * and are completely ignored
        if ( originalFile.at ( i ).substr(0,1) == "*" )
            continue;
        if ( originalFile.at ( i ).substr(0,1) == "#" )
            continue;
        if ( originalFile.at ( i ).substr(0,2) == "//" )
            continue;
        //the rest gets exploded
        vector<string> boomResult = explodeStringWhiteSpace(originalFile.at(i));
        //then put into the map.
        if (boomResult.size() > 0)
        {
	    string key = boomResult.at(0);
	    boomResult.erase(boomResult.begin(),boomResult.begin()+1);
            dataMap[key]=implodeString(boomResult," ");
// 	    cout << key << " " << implodeString(boomResult, " ") << endl;
            keyMap.push_back(key);
        }
    }
    
    return 0;
}


string configFile::implodeString(vector< string > _strList, string _delim)
{
  string retVal;
  for (unsigned int i = 0; i < _strList.size();i++)
    retVal.append(_strList.at(i)+_delim);
  
  return retVal;
  
}


configFile::~configFile()
{

}

