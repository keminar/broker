# 项目编译配置 Makefile
# 版本：1.0
# 描述：用于编译 broker 程序及其依赖模块

# ---------------------------- 变量定义区 ----------------------------
# 编译器选择：使用 gcc 作为C编译器
CC=gcc

# 编译选项：
# -std=c11 编译时指定较早的 C 标准
# -D_POSIX_C_SOURCE=200809L 显式指定 POSIX 标准编译
# -Wall：开启所有警告
# -O2：启用二级优化
CFLAGS=-std=c11 -D_POSIX_C_SOURCE=200809L -Wall -O2

# 目标程序名称
TARGET = broker

all: $(TARGET)
	@echo "==== 编译完成：$(TARGET) ===="

debug: clean all

# 编译最终程序
$(TARGET): 
	$(CC) $@.c $(CFLAGS)  -o $@

# 清理目标：删除编译生成的文件
clean:
	rm -f $(TARGET)
	@echo "==== 清理完成 ===="
