#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum Token {
  tok_eof = -1,

  tok_def = -2,
  tok_extern = -3,

  tok_identifier = -4,
  tok_number = -5,
};

static string IdentifierStr;
static double NumVal;

static int gettok() {
  static int LastChar = ' ';

  while (isspace(LastChar)) {
    LastChar = getchar();
  }

  if (isalpha(LastChar)) {
    IdentifierStr = LastChar;
    while (isalnum((LastChar = getchar()))) {
      IdentifierStr += LastChar;
    }
    if (IdentifierStr == "def") {
      return tok_def;
    }
    if (IdentifierStr == "extern") {
      return tok_extern;
    }
    return tok_identifier;
  }

  if (isdigit(LastChar) || LastChar == '.') {
    string NumStr;
    do {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return tok_number;
  }

  if (LastChar == '#') {
    do {
      LastChar = getchar();
    } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != EOF) {
      return gettok();
    }
  }

  if (LastChar == EOF) {
    return tok_eof;
  }

  int ThisChar = LastChar;
  LastChar = getchar();

  return ThisChar;
}

namespace {

class ExprAST {
public:
  virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
  double Val;

public:
  NumberExprAST(double Val) : Val(Val) {}
};

class VariableExprAST : public ExprAST {
  string Name;

public:
  VariableExprAST(const string &Name) : Name(Name) {}
};

class BinaryExprAST : public ExprAST {
  char Op;
  unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, unique_ptr<ExprAST> LHS, unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExprAST : public ExprAST {
  string Callee;
  vector<unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const string &Callee, vector<unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args)) {}
};

class PrototypeAST {
  string Name;
  vector<string> Args;

public:
  PrototypeAST(const string &Name, vector<string> Args)
      : Name(Name), Args(std::move(Args)) {}
  const string &getName() const { return Name; }
};

class FunctionAST {
  unique_ptr<PrototypeAST> Proto;
  unique_ptr<ExprAST> Body;

public:
  FunctionAST(unique_ptr<PrototypeAST> Proto, unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

}; // namespace

static int CurTok;
static int getNextToken() { return CurTok = gettok(); }

static map<char, int> BinopPrecedence;

static int GetTokPrecedence() {
  if (!isascii(CurTok)) {
    return -1;
  }

  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0) {
    return -1;
  }
  return TokPrec;
}

unique_ptr<ExprAST> LogError(const char *Str) {
  printf("Error: %s\n", Str);
  return nullptr;
}
unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}

static unique_ptr<ExprAST> ParseExpression();

// numberexpr ::= number
static unique_ptr<ExprAST> ParseNumberExpr() {
  auto Result = make_unique<NumberExprAST>(NumVal);
  getNextToken();
  return std::move(Result);
}

// parenexpr ::= '(' expression ')'
static unique_ptr<ExprAST> ParseParenExpr() {
  getNextToken();
  auto V = ParseExpression();
  if (!V) {
    return nullptr;
  }
  if (CurTok != ')') {
    return LogError("Expected ')'");
  }
  getNextToken();
  return V;
}

// identifier
//   ::= identifier
//   ::= identifier '(' expression* ')'
static unique_ptr<ExprAST> ParseIdentifierExpr() {
  string IdName = IdentifierStr;

  getNextToken();

  if (CurTok != '(') {
    return make_unique<VariableExprAST>(IdName);
  }

  getNextToken();
  vector<unique_ptr<ExprAST>> Args;
  if (CurTok != ')') {
    while (true) {
      if (auto Arg = ParseExpression()) {
        Args.push_back(std::move(Arg));
      } else {
        return nullptr;
      }

      if (CurTok == ')') {
        break;
      }

      if (CurTok != ',') {
        return LogError("Expected ')' or ',' in argument list");
      }
      getNextToken();
    }
  }

  getNextToken();

  return make_unique<CallExprAST>(IdName, std::move(Args));
}

// primary
//   ::= identifierexpr
//   ::= numberexpr
//   ::= parenexpr
static unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
  case tok_identifier:
    return ParseIdentifierExpr();
  case tok_number:
    return ParseNumberExpr();
  case '(':
    return ParseParenExpr();
  default:
    return LogError("Unknown token, expecting expression");
  }
}

// binoprhs
//   ::= ('+' primary)*
static unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                         unique_ptr<ExprAST> LHS) {
  while (true) {
    int TokPrec = GetTokPrecedence();

    if (TokPrec < ExprPrec) {
      return LHS;
    }

    int BinOp = CurTok;
    getNextToken();

    auto RHS = ParsePrimary();
    if (!RHS) {
      return nullptr;
    }

    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec) {
      RHS = ParseBinOpRHS(TokPrec + 1, std::move(LHS));
      if (!RHS) {
        return nullptr;
      }
    }

    LHS = make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
  }
}

// expression
//   ::= primary binoprhs
static unique_ptr<ExprAST> ParseExpression() {
  auto LHS = ParsePrimary();
  if (!LHS) {
    return nullptr;
  }
  return ParseBinOpRHS(0, std::move(LHS));
}

// prototype
//   :: id '(' id* ')'
unique_ptr<PrototypeAST> ParsePrototype() {
  if (CurTok != tok_identifier) {
    return LogErrorP("Expected function name prototype");
  }

  string FnName = IdentifierStr;
  getNextToken();
  if (CurTok != '(') {
    return LogErrorP("Expected '(' in prototype");
  }
  vector<string> ArgNames;
  while (getNextToken() == tok_identifier) {
    ArgNames.push_back(IdentifierStr);
  }
  if (CurTok != ')') {
    LogErrorP("Expected ')' in prototype");
  }
  getNextToken();

  return make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

// definition ::= 'def' prototype expression
static unique_ptr<FunctionAST> ParseDefinition() {
  getNextToken();
  auto Proto = ParsePrototype();
  if (!Proto)
    return nullptr;

  if (auto E = ParseExpression())
    return make_unique<FunctionAST>(std::move(Proto), std::move(E));
  return nullptr;
}

// toplevelexpr ::= expression
static unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    auto Proto = make_unique<PrototypeAST>("__anon_expr", vector<string>());
    return make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }
  return nullptr;
}

// external ::= 'extern' prototype
static unique_ptr<PrototypeAST> ParseExtern() {
  getNextToken();
  return ParsePrototype();
}

static void HandleDefinition() {
  if (ParseDefinition()) {
    fprintf(stderr, "Parsed a function definition.\n");
  } else {
    getNextToken();
  }
}

static void HandleExtern() {
  if (ParseExtern()) {
    fprintf(stderr, "Parsed an extern\n");
  } else {
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  if (ParseTopLevelExpr()) {
    fprintf(stderr, "Parsed a top-level expr\n");
  } else {
    getNextToken();
  }
}

// top ::= definition | external | expression | ';'
static void MainLoop() {
  while (true) {
    fprintf(stderr, "ready> ");
    switch (CurTok) {
    case tok_eof:
      return;
    case ';':
      getNextToken();
      break;
    case tok_def:
      HandleDefinition();
      break;
    case tok_extern:
      HandleExtern();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

int main() {
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;

  fprintf(stderr, "ready> ");
  getNextToken();

  MainLoop();

  return 0;
}
