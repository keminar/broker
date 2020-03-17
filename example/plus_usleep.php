#!/usr/bin/php -q
<?php

$fp = fopen("php://stdin", "r");
function read($fp) {
    $i = 0;
    $t = 0;
    while(!feof($fp)) {
        $i++;
        $tmp = fgets($fp, 1024);
        //收到结束响应
        if ($tmp == "end\n") {
            if ($t) {
                // 输出聚合值
                echo $t. "\n";
                $t = 0;
                $i = 0;
            }
            //透传end
            echo $tmp;
            continue;
        }
        $tmp = trim($tmp);
        if ($tmp) {
            $t += intval($tmp);
            usleep(100);
            if ($i>=5) {
                echo $t. "\n";
                $t = 0;
                $i = 0;
            }
        }
    }

}
read($fp);
fclose($fp);
