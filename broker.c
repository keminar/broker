#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <error.h>
#include <errno.h>
#include <string.h>

// 一行最大字节
#define MAX_LINE 65535

// 最大并发数
#define MAX_FORK 50

// 最大步长
#define MAX_STEP 100

// 应用名
#define PACKAGE_NAME "broker"

// 版本
#define PACKAGE_VERSION "v0.1"

#define _(Msgid) ((const char*)Msgid)

// 生产者
static char *producer;

// 消费者
static char *consumer;

// 并发数
static int forks;

// 步长
static int step;

// 解析参数
static int decode_switches(int argc, char **argv);
// 递增
static int increment(int val);
//帮助
void usage_zh(int status);

static struct option const long_options[] =
{
        {"producer", required_argument, NULL, 'p'},
        {"consumer", required_argument, NULL, 'c'},
        {"fork-consumers", required_argument, NULL, 'f'},
        {"step", required_argument, NULL, 's'},
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}
};

int main(int argc, char *argv[])
{
    char line[MAX_LINE];
    FILE *fpin, *fpout[MAX_FORK];
    int i, j, z;
	forks = 1;
    step = 1;
    producer = "stdin";

	decode_switches(argc, argv);
	if (consumer == NULL) {
		usage_zh(EXIT_FAILURE);
	}

    if (strcmp(producer, "stdin") != 0) {
        // 调起生产程序
        if ((fpin = popen(producer, "r")) == NULL) {
            perror("popen producer err");
            return 0;
        }
    } else {
        fpin = stdin;
    }

	// 调用多个消费者
	for (i=0; i< forks; i++) {
		if ((fpout[i] = popen(consumer, "w")) == NULL) {
			perror("popen consumer err");
			return 0;
		}
	}

	// 转发日志
    i = j =0;
    while (fgets(line, MAX_LINE, fpin) != NULL) {
        // 计算进程号
		z = j % forks;
        if (fputs(line, fpout[z]) == EOF) {
			perror("fputs to consumer err");
			return 0;
		}
        // 特别重要，不然就不是并发了，默认要达到一定量或是pclose时才会真的发送
        fflush(fpout[z]);
        // 先位移一小步
        i = increment(i);
        // 达到大步长，位移大步
        if (i % step == 0) {
            j = increment(j);
        }
    }
	// 关闭
    if (strcmp(producer, "stdin") != 0) {
        if (pclose(fpin)) {
            if (errno == 0) {
                perror("pclose fgets err");
            }
        }
    }
	for (i=0; i< forks; i++) {
		if (pclose(fpout[i])) {
			if (errno == 0) {
				perror("pclose forks err");
			}
		}
	}
}

// 安全的递增
static int increment(int val)
{
    if (val >= INT_MAX) {
        // 重置下，防止日志多时溢出
        val = 0;
    } else {
        val++;
    }
    return val;
}

// 解析参数
static int decode_switches(int argc, char **argv)
{
    for (;;)
    {
        int oi = -1;
        //形式如 a:b::cd: ，分别表示程序支持的命令行短选项有-a、-b、-c、-d，冒号含义如下：
        //(1)只有一个字符，不带冒号——只表示选项， 如-c 
        //(2)一个字符，后接一个冒号——表示选项后面带一个参数，如-a 100
        //(3)一个字符，后接两个冒号——表示选项后面带一个可选参数，选项与参数之间不能有空格, 形式应该如-b200
        int c = getopt_long(argc, argv,
                            "c:f:hp:s:v",
                            long_options, &oi);

        //每次执行会打印多次, 最后一次也是-1
        //printf("c=%d, optind=%d\r\n", c, optind);
        if (c == -1)
        {
            break;
        }
        switch (c)
        {
        case 'c':
        {
            consumer = (char *)optarg;
            break;
        }
        case 'f':
        {
            forks = atoi(optarg);
			if (forks <= 0 || forks > MAX_FORK) {
				error(EXIT_FAILURE, 0, _("invalid --fork-consumers: %s range[1-%d]"),  optarg, MAX_FORK);
			}
            break;
        }
		case 'p':
		{
			producer = (char *)optarg;
			break;
		}
        case 's':
        {
            step = atoi(optarg);
			if (step <= 0 || step > MAX_STEP) {
				error(EXIT_FAILURE, 0, _("invalid --step: %s range[1-%d]"),  optarg, MAX_STEP);
			}
            break;
        }
        case 'v':
            printf("%s version=%s\r\n", PACKAGE_NAME, PACKAGE_VERSION);
            exit(EXIT_SUCCESS);
        case 'h':
            usage_zh(EXIT_SUCCESS);
            break;
        default:
            usage_zh(EXIT_FAILURE);
            break;
        }
    }
    //optind：表示的是下一个将被处理到的参数在argv中的下标值
    return optind;
}

// 中文
void usage_zh(int status)
{
    if (status != EXIT_SUCCESS)
    {
        fprintf(stderr, _("Try '%s --help' for more information\n"), PACKAGE_NAME);
    }
    else
    {
        printf(_("用法: %s --producer=[FILE] --consumer=[FILE] [OPTION]\n"), PACKAGE_NAME);
        fputs(_("并发多个消费者进程消费生产者的日志, 支持管道输入.\n\
\n\
--step * --fork-consumers 不要超过日志总条数, 否则效率反而会低.\n\
\n\
"),
              stdout);
        fputs(_("\
    -p  --producer=TARGET       日志生产者，标准输入为stdin (默认)\n\
    -c, --consumer=TARGET       日志消费者\n\
    -f, --fork-consumers=NUM    最大并发消费者数, 默认 1, 最大50\n\
    -s, --step=NUM              每一个进程连续写几条日志, 默认 1, 最大100\n\
    -h, --help                  显示当前的帮助信息\n\
    -v, --version               显示版本信息并且退出\n\
\n\
"),
              stdout);
    }
    exit(status);
}
