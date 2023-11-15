#include "pcdosBaseVisitor.h"
#include "pcdosParser.h"
#include <algorithm>
#include <any>
#include <iostream>
#include <map>
#include <string>

/* added the Impl at the end of the class to avoid it being .gitignored sorry */
class pcdosVisitorImpl : pcdosBaseVisitor {
  std::map<std::string, std::any> memory;

public:
};
