#pragma once

#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm-14/llvm/IR/Constants.h>
#include <set>

#include "Interval.h"

using namespace interval;

using namespace llvm;

namespace rangeanalysis {

struct IntervalRangeAnalysis : public FunctionPass {
  static char ID;

  IntervalRangeAnalysis() : FunctionPass(ID) {}

  bool runOnFunction(Function& f) override;
  void collectInts(Instruction* i);

  std::set<int> knownInts;
};

}  // namespace rangeanalysis
