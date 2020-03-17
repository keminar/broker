流式调用某函数10万次
==

![演示效果](https://github.com/keminar/broker/blob/master/example/stream.png)


说明
====
红色用普通串行计算，在one_usleep中调用了函数10万次， 将1到10万累加， 共执行60秒
黄色用管道分成10个进程并行处理，每个plus_usleep进程调用函数1万次，共同将1到10万分组累加成N个值。
    再通过join把N进行聚合相加，（等于比红色还多了一步聚合操作），共执行6秒

从执行输出看除了执行效率有差异外，结果都是一样的。说明准确性没有问题。

执行平台
====
win10 + wsl

终端下
====
```
//普通计算
php example/input.php |php example/one_usleep.php

//并发数为10
php example/input.php |./broker -c 'php example/plus_usleep.php' -f 10 |php example/join.php
```

终端左上
====
```
//写入要调用的次数
echo 100000 >> example/input.txt
```

终端右上
====
```
//查=看调用消耗时间和计算1->10万累加值
tail -f ./example/out.txt
```