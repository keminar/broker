#!/usr/bin/php -q
<?php
$fp = fopen("php://stdin", "r");
function read($fp) {
    $t = 0;
    $e = 0;
    while(!feof($fp)) {
        $tmp = fgets($fp, 1024);
        if ($tmp=="end\n") {
            $e++;
            //计算10个end都拿到计算结束
            if ($e==10) {
                break;
            }
            continue;
        }
        $tmp = trim($tmp);
        if ($tmp) {
            usleep(100);
            $t += intval($tmp);
        }
    }
    file_put_contents("./example/out.txt", time() . "\tsum $t\n", FILE_APPEND);
    read($fp);
}
read($fp);
fclose($fp);
