#!/usr/bin/php -q
<?php
function busy($a, $b)
{
	usleep(100);
	return $a + $b;
}
$fp = fopen("php://stdin", "r");
function read($fp) {
	$t = 0;
	$e = 0;
	$n = 0;
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
			$t = busy($t, intval($tmp));
			$n++;
		}
	}
	file_put_contents("./example/out.txt", time() . "\trun $n\n", FILE_APPEND);
	file_put_contents("./example/out.txt", time() . "\tsum $t\n", FILE_APPEND);
}
while (1) {
	read($fp);
}
fclose($fp);
