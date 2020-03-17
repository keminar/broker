#!/usr/bin/php -q
<?php
function busy($a, $b)
{
	usleep(100);
	return $a + $b;
}
$fp = fopen("php://stdin", "r");
function read($fp) {
	$i = 0;
	$t = 0;
	$n = 0;
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
			if ($n) {
				file_put_contents("./example/out.txt", time() . "\trun $n\n", FILE_APPEND);
				$n = 0;
			}
			//透传end
			echo $tmp;
			continue;
		}
		$tmp = trim($tmp);
		if ($tmp) {
			$t = busy($t, intval($tmp));
			$n++;
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
