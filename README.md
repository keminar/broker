# broker

功能：
===
你是不是有过这种情况，从redis或是kafka或mysql读取到数据太多，一个脚本消费慢到赶不上数据产生的速度。或许你

会说redis 是原子性的，可以多开几个脚本。但是如果日志源不是redis而是其它呢？比如mysql。

或许你依然可以举很多可行性方案。但是这个脚本只是提供另一种可行性。一个脚本（生产者）串行从（redis、mysql、

kafka）取得日志，通过linux系统管道取模并发给另一个脚本（消费者）的N个进程。从而加速消费速度。

编译
===
make all

用法
===
$ ./broker --help
用法: broker --producer=[FILE] --consumer=[FILE] [OPTION]
并发起多个消费者进程消费生产者的日志.

    -p  --producer=TARGET       日志生产者
    -c, --consumer=TARGET       日志消费者
    -f, --fork-consumers=NUM    最大并发消费者数, 默认 1
        --help                  显示当前的帮助信息
        --version               显示版本信息并且退出

示例
===
一个消费者
```
$ time ./broker -p ./example/a.php -c ./example/b.php
test0 11530 1 18:37:15
test1 11530 2 18:37:16
test2 11530 3 18:37:17
test3 11530 4 18:37:18
test4 11530 5 18:37:19
test5 11530 6 18:37:20
test6 11530 7 18:37:21
test7 11530 8 18:37:22
test8 11530 9 18:37:23
test9 11530 10 18:37:24

real    0m11.122s
user    0m0.016s
sys     0m0.156s
```

10个消费者
```
$ time ./broker -p ./example/a.php -c ./example/b.php -f 10
test1 11540 1 18:37:56
test2 11542 1 18:37:56
test0 11539 1 18:37:56
test4 11545 1 18:37:56
test3 11544 1 18:37:56
test5 11550 1 18:37:56
test6 11549 1 18:37:56
test9 11553 1 18:37:56
test7 11551 1 18:37:56
test8 11552 1 18:37:56

real    0m2.249s
user    0m0.109s
sys     0m1.297s
```