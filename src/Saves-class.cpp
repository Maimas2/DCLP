#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstring>

#include "main.h"
#include "utils.h"
#include "Log.h"

using namespace std;

string custom_replace(string base, string og, string n);

extern int imageToLoad;

// Sly difference: `Saves` is the original namespace for saving a single, main card in the editor. The `Save` class is for many, separate cards.

class Save {
   map<string, float*> floatPointers;
   map<string, char**> charPointers;
   map<string, int*>   intPointers;
   map<string, bool*>  boolPointers;
   string currentPath = "";
   void save(string file);
   void read(string file);
   void loadDefault();
   Save() {
      floatPointers.clear();
      boolPointers.clear();
      intPointers.clear();
      charPointers.clear();
   }
};
// These are out of class because of a weird unexplicable undefined reference
void Save::read(string file) {
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
void Save::loadDefault() {
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
void Save::save(string file) {
      if(file == "") {
         Log::warning("Save-class path was empty, not saving.");
         return;
      }
      FILE *f = fopen(file.c_str(), "w");
      if(f == NULL) {
         Log::warning("File was NULL, not saving. This is probably an uh oh monent.");
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