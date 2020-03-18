流式调用某函数10万次
==

![演示效果](https://github.com/keminar/broker/blob/master/example/stream.png)


说明
====
* 函数busy中的usleep(100);代表实际耗时的业务
* 红色用普通串行计算，在one_usleep中调用了函数10万次， 将1到10万累加， 共执行60秒
* 黄色用管道分成10个进程并行处理，每个plus_usleep进程调用函数1万次，共同将1到10万分组累加成N个值。
    再通过join把N进行聚合相加，（等于比红色还多了一步聚合操作），共执行6秒. plus_usleep中有些是在
    与0相加这和join里的加法次数是相等的(所以join不能再usleep，就重复了), 统一把usleep算在plus_usleep好统计。


从执行输出看除了执行效率有差异外，结果都是一样的。说明准确性没有问题。

程序不段监听input.txt一有输入马上进入处理流程并一级级转给下一层处理，可以N级传递，
处理完马上出结果。

更形象化说明，借用 https://www.cnblogs.com/CherryTab/p/12129378.html 的一张图

![CherryTab](https://github.com/keminar/broker/blob/master/example/cherryTab.png)


测试平台
====
win10 + wsl 模拟linux， 如果用原生linux耗时会有些出入(整体都会变短)

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

扩展
====
可以将一个复杂的业务拆分成多个简单的独立业务，通过管道串联，每一个业务设置自己的需要的并发量。

如：
```
php a.php|./broker -c 'php b.php' -f 2|./broker -c 'php c.php' -f 3|./broker -c 'php d.php' -f 4
```

a, b , c, d 四个脚本可能接收的数据是完全一样的，只是每个脚本执行的时长不同，设置不同的并发让整体
业务的执行速度提高，而不因为其中某一个业务拖累其它业务导致堆积。而且因为业务拆的很细维护起来也
会更容易些。可能需要注意的是实际业务中要对每一个程序接收的输入进行一些校验，防止后面程序把前面的
程序报错信息当成正常输入计算了导致业务出错。


