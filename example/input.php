<?php
$x = 0;
while(1) {
    //从尾部读
    $fp = fopen("./example/input.txt", "r");
    if ($x) {
        fseek($fp, $x);
    } else {
        fseek($fp, 0, SEEK_END);
    }
    $tmp = fgets($fp, 1024);
    $x = ftell($fp);
    //echo "b$x\n";
    $tmp = trim($tmp);
    if ($tmp) {
        $t = intval($tmp);
        file_put_contents("./example/out.txt", time() ."\t" ."get $t" . "\n", FILE_APPEND);
        for ($i=1; $i<=$t; $i++) {
            echo $i."\n";
        }
        for ($j=1; $j<=10; $j++) {
            echo "end\n";
        }
    }
    sleep(1);
}

