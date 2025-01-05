/tip/tipc/build/src/tipc -do $1.tip
llvm-dis-14 $1.tip.bc
opt-14 -enable-new-pm=0 -load /tip/tipc-passes/build/src/intervalrangepass/irpass.so -mem2reg -irpass < $1.tip.bc >/dev/null 2>$1.irpass
