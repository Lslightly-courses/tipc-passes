# tipc-passes

## 题目

1. 基于TIP语言的widening区间分析

## 原始代码解读



## 实验代码简述

首先通过`collectInts`收集指令中的常整数，作为widening区间的整数间隔值。即获取 B 集合。

```
  // Initialize known ints in F
  for (auto& bb : F) {
    for (auto&& i: bb) {
      if (isSupported(i)) {
        collectInts(&i);
      }
    }
  }
```

## 测试方式与测试结果

构建docker镜像与创建容器步骤:

```bash
docker build -t lslightly2357/tipc --network=host .
docker run -dit --tty --name tipc --network host lslightly2357/tipc
docker exec -it tipc /bin/bash
```

进入容器后

```bash
cd /tip/tipc-passes/src/intervalrangepass/
pytest interval_test.py # 执行irpass，得到.irpass与.expected进行比对
```

比对分为IR比对和区间分析结果比对。
- 区间分析结果是输出行的set集合。通过set集合进行比较。使用set比较的原因state map输出乱序
- IR比对采用字符串比对。

## 实验困难记录

在使用Catch测试框架的时候，不能将测试代码和`add_llvm_library`宏放在同一个CMakeLists.txt中，否则会出现找不到头文件的情况。

现在将测试放在[../src/test/](../src/test/)下。
