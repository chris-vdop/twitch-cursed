/**
 * \author Christopher "VdoP" Regali
 * \file twitch.h
 * \brief contains the headers for the twitch cpp interface
 * \date 2014
 */



#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <curl/curl.h>
#include <jansson.h>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include "configFile.h"
using namespace std;

#ifndef TWITCH_H
#define TWITCH_H



typedef struct
{
  bool online;
  int viewers;
  string name;
  string game;
  string display_name;
  
} twitchStreamInfo;


typedef struct
{
  string name;
  int viewers;
  int channels;
  
} twitchGameInfo;



class twitchCPP
{
public:
  twitchCPP(){};
  ~twitchCPP(){};
  
  int loadStreamInfo(string _strName);
  int loadManyStreams(vector<string> _nameList,bool insertStructs=true);
  int loadWholeGame(string _gameName, bool insertStructs = true);
  
  twitchStreamInfo& getStreamInfo(int _index){return gottenStreamInfo.at(_index);}
  int getStreamInfoCount(){return gottenStreamInfo.size();}
  
  
  int getGameInfoCount(){return gamesPlayed.size();}
  twitchGameInfo& getGameInfo(int _index){return gamesPlayed.at(_index);}

  
  static size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata);
  std::ostringstream stream;
  
  int getIndexByName(string _name);
  
  int refreshStreams();
  int refreshGames();
  
  //boldly copied from: https://code.google.com/p/twitcurl/source/browse/trunk/libtwitcurl/urlencode.cpp?r=47
  static string char2hex( char dec );
  static string urlencode( const std::string &c );
  
  
  
private:
  int parseStreamsArray(json_t* _streamsArray);
  vector<twitchStreamInfo> gottenStreamInfo;
  vector<twitchGameInfo> gamesPlayed;
  
  std::string DownloadJSON(std::string URL);
};


#endif

