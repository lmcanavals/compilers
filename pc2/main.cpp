#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

#include "pcdosLexer.h"
#include "pcdosParser.h"
#include "antlr4-runtime.h"

int main(int argc, char **argv) {
  if (argc <= 1)
    return -1;
  std::ifstream is;
  is.open(argv[1]);
  antlr4::ANTLRInputStream input(is);
  pcdosLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  pcdosParser parser(&tokens);
  auto tree = parser.prog();
	//EvalVisitorImpl eval;
  //eval.visitProg(tree);
	std::cout << tree->toStringTree(true) << std::endl;
}
