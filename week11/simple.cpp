#include "llvm/ADT/APInt.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>


using namespace std;
using namespace llvm;

static Function *createFactorialFunction(Module *M, LLVMContext &Context) {
  FunctionType *FactorialTy = FunctionType::get(
      Type::getInt32Ty(Context), {Type::getInt32Ty(Context)}, false);
  Function *FactorialFun =
      Function::Create(FactorialTy, Function::ExternalLinkage, "factorial", M);

  BasicBlock *BB = BasicBlock::Create(Context, "entry", FactorialFun);
  BasicBlock *ReturnBB = BasicBlock::Create(Context, "return", FactorialFun);
  BasicBlock *RecurseBB = BasicBlock::Create(Context, "recurse", FactorialFun);

  Value *zero = ConstantInt::get(Type::getInt32Ty(Context), 0);
  Value *one = ConstantInt::get(Type::getInt32Ty(Context), 1);
  Argument *n = &*FactorialFun->arg_begin();
  n->setName("n");

  Value *eq = new ICmpInst(BB, ICmpInst::ICMP_EQ, n, zero, "eq");
  BranchInst::Create(ReturnBB, RecurseBB, eq, BB);

  ReturnInst::Create(Context, one, ReturnBB);

  Value *Sub = BinaryOperator::CreateSub(n, one, "sub", RecurseBB);
  CallInst *CallFactorial1 =
      CallInst::Create(FactorialFun, Sub, "f1", RecurseBB);
  CallFactorial1->setTailCall();

  Value *Mult = BinaryOperator::CreateMul(CallFactorial1, n, "mult", RecurseBB);
  ReturnInst::Create(Context, Mult, RecurseBB);

  return FactorialFun;
}

int main(int argc, char **argv) {
  int N = argc > 1 ? atol(argv[1]) : 10;

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  auto TheContext = make_unique<LLVMContext>();
  auto TheModule = make_unique<Module>("main.bf", *TheContext);
  //auto TheBuilder = make_unique<IRBuilder<>>(*TheContext);

  Function *FactorialFun =
      createFactorialFunction(TheModule.get(), *TheContext);

  string ErrStr;
  ExecutionEngine *EE =
      EngineBuilder(std::move(TheModule)).setErrorStr(&ErrStr).create();

	if (!EE) {
		errs() << argv[0] << ": Error error :(" << ErrStr << '\n';
		return 1;
	}
	errs() << "all good\n";
	errs() << "factorial(" << N << ") with JIT\n";

	vector<GenericValue> Args(1);
	Args[0].IntVal = APInt(32, N);
	GenericValue GV = EE->runFunction(FactorialFun, Args);

	outs() << "Result: " << GV.IntVal << "\n";

  /*FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), false);
  Function *MainFunc =
      Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());

  TheBuilder->SetInsertPoint(BasicBlock::Create(*TheContext, "beg", MainFunc));

  TheModule->print(errs(), nullptr);*/
  return 0;
}
