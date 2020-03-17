终端一
==

普通计算
===
php example/input.php |php example/one_usleep.php

并发数为10
===
php example/input.php |./broker -c 'php example/plus_usleep.php' -f 10 |php example/join.php

终端二
==
测试写入
===
echo 100000 >> example/input.txt

终端三
==
查看
===
tail -f ./example/out.txt
