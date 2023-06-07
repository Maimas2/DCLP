#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstring>

#include "main.h"
#include "utils.h"
#include "Log.h"
#include "ImguiManager.h"

using namespace std;

string custom_replace(string base, string og, string n) {
   vector<string> s = split(base, og);
   string tor;
   for(int i = 0; i < s.size(); i++) {
      tor += s[i] + (i == s.size()-1 ? "" : n);
   }
   return tor;
}

bool False = false;

extern int imageToLoad;

namespace Saves {
   map<string, float*> floatPointers;
   map<string, char**> charPointers;
   map<string, int*>   intPointers;
   map<string, bool*>  boolPointers;

   bool isMasterSave = true;

   void masterInit() {
      isMasterSave = true;

      floatPointers.clear();
      boolPointers.clear();
      intPointers.clear();
      charPointers.clear();

      charPointers["recentFiles0"] = &(recentFiles[0]);
      charPointers["recentFiles1"] = &(recentFiles[1]);
      charPointers["recentFiles2"] = &(recentFiles[2]);
      charPointers["recentFiles3"] = &(recentFiles[3]);
      charPointers["recentFiles4"] = &(recentFiles[4]);

      intPointers["uiType"]                    = &currentMenuTypee;
      intPointers["recentFilesNum"]            = &numberOfRecentFiles;

      boolPointers["isFullscreen"]             = &isFullscreen;
      boolPointers["isLowRes"]                 = &isLowRes;
   }
   void initDefaultSave() {
      isMasterSave = false;
      
      floatPointers.clear();
      boolPointers.clear();
      intPointers.clear();
      charPointers.clear();

      floatPointers["xMove"]                   = &xMove;
      floatPointers["yMove"]                   = &yMove;
      floatPointers["zoom"]                    = &zoom;
      floatPointers["textXTweak"]              = &textXTweak;
      floatPointers["textSizeTweak"]           = &textSizeTweak;
      floatPointers["textYPosTweak"]           = &textYPosTweak;
      floatPointers["expansionIconXSizeTweak"] = &expansionIconXSizeTweak;
      floatPointers["expansionIconYSizeTweak"] = &expansionIconYSizeTweak;
      floatPointers["tweakDividingLineY"]      = &tweakDividingLineY;
      floatPointers["bottomTextSizeTweak"]     = &bottomTextSizeTweak;
      floatPointers["matWidthTweak"]           = &matWidthTweak;
      floatPointers["newlineSizeTweak"]        = &newlineSizeTweak;
      floatPointers["expansionIconXPosTweak"]  = &expansionIconXPosTweak;

      floatPointers["customColor1"] = &customCardColor[0];
      floatPointers["customColor2"] = &customCardColor[1];
      floatPointers["customColor3"] = &customCardColor[2];

      floatPointers["customEmbellishmentColor1"] = &customEmbellishmentColor[0];
      floatPointers["customEmbellishmentColor2"] = &customEmbellishmentColor[1];
      floatPointers["customEmbellishmentColor3"] = &customEmbellishmentColor[2];

      floatPointers["customSideColor1"] = &customSideColor[0];
      floatPointers["customSideColor2"] = &customSideColor[1];
      floatPointers["customSideColor3"] = &customSideColor[2];

      floatPointers["secondCustomCardColor1"] = &secondCustomCardColor[0];
      floatPointers["secondCustomCardColor2"] = &secondCustomCardColor[1];
      floatPointers["secondCustomCardColor3"] = &secondCustomCardColor[2];

      charPointers["cardTitle"]    = &cardTitle;
      charPointers["cardType"]     = &cardType;
      charPointers["cardText"]     = &cardText;
      charPointers["cardCost"]     = &cardCost;
      charPointers["cardPreview"]  = &cardPreview;
      charPointers["cardVersion"]  = &cardVersion;
      charPointers["cardCredit"]   = &cardCredit;
      charPointers["pictureUrl"]   = &iconUrl;
      charPointers["expansionUrl"] = &expansionUrl;
      charPointers["heirloomText"] = &heirloomText;
      charPointers["matText"]      = &matText;

      intPointers["cardColor"]     = &cardColor;
      intPointers["cardSecondary"] = &cardSecondary;
      intPointers["cardLayout"]    = &cardLayout;
      intPointers["matColor"]      = &matColor;

      boolPointers["isTraveler"]                    = &isTraveler;
      boolPointers["largeSingleLineVanillaBonuses"] = &largeSingleLineVanillaBonuses;
      boolPointers["isTrait"]                       = &isTrait;
      boolPointers["isSupply"]                      = &isSupply;
      boolPointers["isTwoLinedType"]                = &twoLinedType;
   }
   void save(string file) {
      if(!isMasterSave && uiMode != 3) {
         int findRecentFile = -1;
         const char* tempFile = file.c_str();
         for(int i = 0; i < 5; i++) {
            if(strcmp(tempFile, recentFiles[i]) == 0) {
               findRecentFile = i;
               Log::debug("Found recentFile at " + to_string(findRecentFile));
               break;
            }
         }
         if(findRecentFile == -1) {
            numberOfRecentFiles = min(5, ++numberOfRecentFiles);
         }
         for(int i = (findRecentFile == -1 ? 4 : findRecentFile-1); i > 0; i--) {
            //recentFiles[i] = recentFiles[i-1];
            memcpy(recentFiles[i], recentFiles[i-1], strlen(recentFiles[i-1]));
            recentFiles[i][strlen(recentFiles[i-1])] = '\0';
         }
         
         memcpy(recentFiles[0], file.c_str(), file.size());
         recentFiles[0][file.size()+1] = '\0';

         for(int i = 0; i < 5; i++) {
            string base = string(recentFiles[i]);
            base = split(base, "/").back();
            recentFilesBeautified[i] = base;
         }
      }

      FILE *f = fopen(file.c_str(), "w");
      if(f == NULL) {
         cout << "File was NULL, not saving. This is probably an uh oh monent." << endl;
         return;
      }
      for(std::map<string, float*>::iterator it = floatPointers.begin(); it != floatPointers.end(); ++it) {
         fputs(("f" + it->first + ":" + to_string(*(it->second)) + "\n").c_str(), f);
      }
      for(std::map<string, char**>::iterator it = charPointers.begin(); it != charPointers.end(); ++it) {
         if(it->first.starts_with("recentFiles")) {
            fputs(("crecentFiles" + to_string(it->first[it->first.size()-1]-48) + ":" + recentFiles[it->first[it->first.size()-1]-48] + "\n").c_str(), f);
            //it->first[it->first.size()-1]-48
            continue;
         }
         fputs(("c" + it->first + ":" + *(it->second) + "\n").c_str(), f);
      }
      for(std::map<string, int*>::iterator it = intPointers.begin(); it != intPointers.end(); ++it) {
         fputs(("i" + it->first + ":" + to_string(*(it->second)) + "\n").c_str(), f);
      }
      for(std::map<string, bool*>::iterator it = boolPointers.begin(); it != boolPointers.end(); ++it) {
         if(it->second == nullptr) {
            Log::warning("An iterator's `second` was a nullptr, skipping.");
            continue;
         }
         fputs(("b" + it->first + ":" + to_string(*(it->second)) + "\n").c_str(), f);
      }
      fclose(f);
   }
   void save() {
      if(currentFile == "") {
         Log::warning("currentFile was empty, so not saving.");
         return;
      }
      string g = string(cardText);
      g = custom_replace(g, "\n", "\\n");
      cardText = (char*)g.c_str();
      save(currentFile);
   }
   void read(string file) {
      imageToLoad = 0;

      ifstream in(file.c_str());
      if(!in.is_open()) {
         Log::warning("Unable to open save file " + file + "!");
         in.close();
         return;
      }
      string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
      in.close();
      vector<string> lines = split(contents, "\n");

      for(int i = 0; i < lines.size(); i++) {
         if(lines[i].find(":") == -1) continue;
         char type = lines[i].at(0);
         if(type == 'f') {
            string name = lines[i].substr(1, lines[i].find(":")-1);
            if(floatPointers[name] == nullptr) continue;
            *(floatPointers[name]) = stof(lines[i].substr(lines[i].find(":")+1));
         }
      }

      for(int i = 0; i < lines.size(); i++) {
         if(lines[i].find(":") == -1) continue;
         char type = lines[i].at(0);
         if(type == 'c') {
            string name = lines[i].substr(1, lines[i].find(":")-1);
            if(charPointers[name] == nullptr) continue;
            string src = (lines[i].substr(lines[i].find(":")+1)) + (char)0;
            src = custom_replace(src, "\\n", "\n");
            memcpy(*(charPointers[name]), (char*)src.c_str(), src.size());
         }
      }

      for(int i = 0; i < lines.size(); i++) {
         if(lines[i].find(":") == -1) continue;
         char type = lines[i].at(0);
         if(type == 'i') {
            string name = lines[i].substr(1, lines[i].find(":")-1);
            if(intPointers[name] == nullptr) continue;
            *(intPointers[name]) = stoi(lines[i].substr(lines[i].find(":")+1));
         }
      }

      for(int i = 0; i < lines.size(); i++) {
         if(lines[i].find(":") == -1) continue;
         char type = lines[i].at(0);
         if(type == 'b') {
            string name = lines[i].substr(1, lines[i].find(":")-1);
            if(boolPointers[name] == nullptr) continue;
            *(boolPointers[name]) = (lines[i].substr(lines[i].find(":")+1) == "0" ? false : true);
         }
      }
   }
   void read() {
      read(currentFile);
   }
   void readFirst() {
      masterInit();
      read("master.mdclp");

      for(int i = 0; i < 5; i++) {
         string base = string(recentFiles[i]);
         base = split(base, "/").back();
         recentFilesBeautified[i] = base;
      }
      
      initDefaultSave();
   }
   void exit() {
      masterInit();
      save("master.mdclp");
      isMasterSave = false;
      initDefaultSave();
      save();
   }
}