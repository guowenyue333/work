# ----------
# 基础配置
# ----------

# 编译器
CC := gcc

# 编译选项
CFLAGS := -Wall -Wextra -std=c11

# 链接选项
LDFLAGS := 

# 目标可执行文件名
TARGET := ans

# 源文件目录
SRC_DIR := .

# 构建目录
BUILD_DIR := build


# 自动检测


# 获取所有.c文件
SRCS := $(wildcard $(SRC_DIR)/*.c)

# 生成对应的.o文件路径
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# 获取所有.h文件（用于依赖检测）
HEADERS := $(wildcard $(SRC_DIR)/*.h)


# 构建规则


# 默认目标
all: $(BUILD_DIR)/$(TARGET)

# 创建构建目录
$(BUILD_DIR):
	@mkdir -p $@

# 链接可执行文件
$(BUILD_DIR)/$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# 编译每个.c文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# 辅助功能


# 运行程序
run: $(BUILD_DIR)/$(TARGET)
	@echo "运行程序..."
	@./$(BUILD_DIR)/$(TARGET)

# 清理
clean:
	@echo "清理构建文件..."
	@rm -rf $(BUILD_DIR)

# 显示帮助
help:
	@echo "可用命令:"
	@echo "  make       - 编译项目"
	@echo "  make run   - 运行程序"
	@echo "  make clean - 清理构建文件"
	@echo "  make help  - 显示帮助"

.PHONY: all run clean help