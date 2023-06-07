#include <string>
#include <map>

using namespace std;

class Save {
   public:
   map<string, float*> floatPointers;
   map<string, char**> charPointers;
   map<string, int*>   intPointers;
   map<string, bool*>  boolPointers;
   void save(string file);
   void read(string in);
   void init();
   void loadDefault();
};