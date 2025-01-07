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

// Analysis state (a map lattice)
typedef DenseMap<Value*, Interval> StateMap;

struct IntervalRangeAnalysis : public FunctionPass {
  static char ID;

  IntervalRangeAnalysis() : FunctionPass(ID) {
    knownInts.insert(minf);
    knownInts.insert(pinf);
  }

  bool runOnFunction(Function& f) override;
  void runOnInst(Instruction* i, StateMap& state, Interval& current, Interval& old, bool useWiden);
  void collectInts(Instruction* i);
  void printKnownInts(raw_fd_ostream& s);

  std::set<int> knownInts;
};

}  // namespace rangeanalysis
