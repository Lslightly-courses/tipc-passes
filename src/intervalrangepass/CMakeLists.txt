add_llvm_library(irpass MODULE Interval.cpp IntervalRangeAnalysis.cpp)

add_library(interval Interval.cpp IntervalRangeAnalysis.cpp)