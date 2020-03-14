#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <error.h>
#include <errno.h>

// 一行最大字节
#define MAXLINE 65535

// 最大并发数
#define MAXFORK 20

// 应用名
#define PACKAGE_NAME "broker"

// 版本
#define PACKAGE_VERSION "v0.1"

#define _(Msgid) ((const char*)Msgid)

// 生产者
static char *producer;

// 消费才
static char *consumer;

// 并发数
static int forks;

enum
{
    GETOPT_HELP_CHAR = CHAR_MAX - 2,
    GETOPT_VERSION_CHAR = CHAR_MAX - 3,
};

// 解析参数
static int decode_switches(int argc, char **argv);
void usage_zh(int status);

#define GETOPT_HELP_OPTION_DECL \
    "help", no_argument, NULL, GETOPT_HELP_CHAR

#define GETOPT_VERSION_OPTION_DECL \
    "version", no_argument, NULL, GETOPT_VERSION_CHAR


static struct option const long_options[] =
{
        {"producer", required_argument, NULL, 'p'},
        {"consumer", required_argument, NULL, 'c'},
        {"fork-consumers", required_argument, NULL, 'f'},
        {GETOPT_HELP_OPTION_DECL},
        {GETOPT_VERSION_OPTION_DECL},
        {NULL, 0, NULL, 0}
};

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    FILE *fpin, *fpout[MAXFORK];
    int i, j;
	forks = 1;

	decode_switches(argc, argv);
	if (producer == NULL) {
		usage_zh(EXIT_FAILURE);
	}
	if (consumer == NULL) {
		usage_zh(EXIT_FAILURE);
	}

	// 调起生产程序
    if ((fpin = popen(producer, "r")) == NULL) {
        perror("popen producer err");
        return 0;
    }

	// 调用多个消费者
	for (i=0; i< forks; i++) {
		if ((fpout[i] = popen(consumer, "w")) == NULL) {
			perror("popen consumer err");
			return 0;
		}
	}

	// 转发日志
    i = 0;
    while (fgets(line, MAXLINE, fpin) != NULL) {
		j = i % forks;
        if (fputs(line, fpout[j]) == EOF) {
			perror("fputs to consumer err");
			return 0;
		}
		// 特别重要，不然就不是并发了，要达到一定量或是pclose时才会真的发送
		fflush(fpout[j]);
        i++;
    }
	// 关闭
    if (pclose(fpin)) {
		if (errno == 0) {
        	perror("pclose fgets err");
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
                            "p:c:f:",
                            long_options, &oi);

        //每次执行会打印多次, 最后一次也是-1
        //printf("c=%d, optind=%d\r\n", c, optind);
        if (c == -1)
        {
            break;
        }
        switch (c)
        {
		case 'p':
		{
			producer = (char *)optarg;
			break;
		}
        case 'c':
        {
            consumer = (char *)optarg;
            break;
        }
        case 'f':
        {
            forks = atoi(optarg);
			if (forks <= 0 || forks > MAXFORK) {
				error(EXIT_FAILURE, 0, _("invalid --fork-consumers: %s range[1-%d]"),  optarg, MAXFORK);
			}
            break;
        }
        case GETOPT_VERSION_CHAR:
            printf("%s version=%s\r\n", PACKAGE_NAME, PACKAGE_VERSION);
            exit(EXIT_SUCCESS);
        case GETOPT_HELP_CHAR:
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
        fputs(_("并发起多个消费者进程消费生产者的日志.\n\
\n\
"),
              stdout);
        fputs(_("\
    -p  --producer=TARGET       日志生产者\n\
    -c, --consumer=TARGET       日志消费者\n\
    -f, --fork-consumers=NUM    最大并发消费者数, 默认 1\n\
        --help                  显示当前的帮助信息\n\
        --version               显示版本信息并且退出\n\
\n\
"),
              stdout);
    }
    exit(status);
}
