/**
 * \author Christopher "VdoP" Regali
 * \file pluginBase.h
 * \brief Contains the headers for the pluginBase class
 * \date 2014
 */


#include <iostream>
#include <cstdlib>
#include <ncurses.h>
#include <menu.h>
#include <menu.h>
#include <cstdlib>
#include <vector>
#include <string>
#include "configFile.h"


#ifndef pluginBase_HH
#define pluginBase_HH


using namespace std;

class pluginBase
{
public:
  string getName(){return name;}
  string getDescription(){return description;}
  //the hook for the atexit
  virtual void cleanUpCurses()=0;
  //the function that is called when this plugin gets activated
  virtual void runGui(WINDOW* _pluginWindow)=0;
  
  virtual void handleChar(int _ch)=0;
  
protected:
  WINDOW* pluginWindow;
  string name;
  string description;
  
  
  
};


class pluginList
{
public:
  ~pluginList(){};
   static pluginList* getInstance(){ 
     if (instance == NULL)
       instance = new pluginList();
     return instance;
     
  };
  std::vector<pluginBase*> activated_classes;
  

private:
  static pluginList* instance;
  pluginList(){
  };
  
  
  
};
#endif


