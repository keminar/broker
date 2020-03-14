#!/usr/bin/php -q
<?php
function read() {
    $i = 0;
    $pid = getmypid();
    $fp = fopen("php://stdin", "r");
    while(!feof($fp)) {
        sleep(1);
        $i++;
        $tmp = fgets($fp, 1024);
        $tmp = trim($tmp);
        if ($tmp) {
            echo $tmp . " " . $pid . " " . $i . " ". date("H:i:s") . "\n";
        }
    }
    fclose($fp);
}
read();