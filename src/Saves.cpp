#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstring>

#include "main.h"
#include "utils.h"

using namespace std;

string file = "save.dclp";

string custom_replace(string base, string og, string n) {
   vector<string> s = split(base, og);
   string tor;
   for(int i = 0; i < s.size(); i++) {
      tor += s[i] + (i == s.size()-1 ? "" : n);
   }
   return tor;
}

namespace Saves {
   map<string, float*> floatPointers;
   map<string, char**> charPointers;
   map<string, int*>   intPointers;
   map<string, bool*>  boolPointers;
   void init() {
      floatPointers["xMove"] = &xMove;
      floatPointers["yMove"] = &yMove;
      floatPointers["zoom"] = &zoom;
      floatPointers["textXTweak"] = &textXTweak;
      floatPointers["textSizeTweak"] = &textSizeTweak;
      floatPointers["textYPosTweak"] = &textYPosTweak;
      floatPointers["expansionIconXSizeTweak"] = &expansionIconXSizeTweak;
      floatPointers["expansionIconYSizeTweak"] = &expansionIconYSizeTweak;
      floatPointers["tweakDividingLineY"] = &tweakDividingLineY;
      floatPointers["bottomTextSizeTweak"] = &bottomTextSizeTweak;

      charPointers["cardTitle"] = &cardTitle;
      charPointers["cardType"] = &cardType;
      charPointers["cardText"] = &cardText;
      charPointers["cardCost"] = &cardCost;
      charPointers["cardPreview"] = &cardPreview;
      charPointers["cardVersion"] = &cardVersion;
      charPointers["cardCredit"] = &cardCredit;
      charPointers["pictureUrl"] = &iconUrl;
      charPointers["expansionUrl"] = &expansionUrl;
      charPointers["heirloomText"] = &heirloomText;

      intPointers["cardColor"]     = &cardColor;
      intPointers["cardSecondary"] = &cardSecondary;
      intPointers["cardLayout"]    = &cardLayout;
      intPointers["matColor"]      = &matColor;

      boolPointers["isLowRes"] = &isLowRes;
      boolPointers["isTraveler"] = &isTraveler;
      boolPointers["largeSingleLineVanillaBonuses"] = &largeSingleLineVanillaBonuses;
      boolPointers["isTrait"] = &isTrait;
   }
   void save() {
      string g = string(cardText);
      g = custom_replace(g, "\n", "\\n");
      cardText = (char*)g.c_str();
      FILE *f = fopen(file.c_str(), "w");
      if(f == NULL) {
         cout << "File was NULL, not saving. This is probably an uh oh monent." << endl;
         return;
      }
      for(std::map<string, float*>::iterator it = floatPointers.begin(); it != floatPointers.end(); ++it) {
         fputs(("f" + it->first + ":" + to_string(*(it->second)) + "\n").c_str(), f);
      }
      for(std::map<string, char**>::iterator it = charPointers.begin(); it != charPointers.end(); ++it) {
         fputs(("c" + it->first + ":" + *(it->second) + "\n").c_str(), f);
      }
      for(std::map<string, int*>::iterator it = intPointers.begin(); it != intPointers.end(); ++it) {
         fputs(("i" + it->first + ":" + to_string(*(it->second)) + "\n").c_str(), f);
      }
      for(std::map<string, bool*>::iterator it = boolPointers.begin(); it != boolPointers.end(); ++it) {
         fputs(("b" + it->first + ":" + to_string(*(it->second)) + "\n").c_str(), f);
      }
      fclose(f);
   }
   void read() {
      ifstream in(file.c_str());
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
            string src = (lines[i].substr(lines[i].find(":")+1) + "\0");
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
}