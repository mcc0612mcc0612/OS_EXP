/*
 * 识别格式化字符串的核心代码写在本文件中
 * 可以从网上移植代码
 */ 
 
#include <stdarg.h>  
#define MAX_BUFFER_SIZE 1024 
#define BUFFER_OVERFLOW -2

int strlen(const char *str) { int len = 0; while (*str != '\0') { len++; str++; } return len; }
void strcpy(char *dest, const char *src) { while (*src != '\0') { *dest = *src; dest++; src++; } *dest = '\0'; }
void itoa(int value, char *str) { 
	int sign = value < 0 ? -1 : 1; 
	int i = 0; 
	do { 
		str[i++] = '0' + sign * (value % 10); 
		value /= 10; 
	} while (value != 0); 
	if (sign < 0) { 
		str[i++] = '-'; 
	} 
	str[i] = '\0'; 
	reverse(str); // 反转字符串 
}
void reverse(char *str) { 
	int len = strlen(str); 
	int i = 0; int j = len - 1; 
	char temp; 
	while (i < j) { 
		temp = str[i]; 
		str[i] = str[j]; 
		str[j] = temp; 
		i++; 
		j--; 
	} 
}

int vsprintf(char* str, const char* format, va_list arg) {
	char buffer[MAX_BUFFER_SIZE];
	// 定义一个指向缓冲区的指针
	char* ptr = buffer;
	// 定义一个用于存储整数值的字符串
	char num[12];
	// 定义一个用于存储整数值的变量
	int value;
	// 遍历格式字符串
	while (*format != '\0') {
		// 如果遇到%符号，表示需要格式化输出
		if (*format == '%') {
			// 跳过%符号
			format++;
			// 根据不同的格式标记，处理不同的数据类型
			switch (*format) {
				// 如果是d，表示输出十进制整数
				case 'd':
					// 从可变参数列表中获取整数值
					value = va_arg(arg, int);
					// 将整数值转换为字符串
					itoa(value, num);
					// 将字符串复制到缓冲区中
					strcpy(ptr, num);
					// 更新缓冲区指针位置
					ptr += strlen(num);
					format++;
					break;
			}
		}

		// 如果不是%符号，表示直接输出字符
		else {
			// 将字符复制到缓冲区中
			*ptr++ = *format++;
		}
	}
	// 在缓冲区末尾添加空字符
	*ptr = '\0';

	// 将缓冲区的内容复制到目标字符串中
	strcpy(str, buffer);

	// 返回写入的字符数，不包括空字符
	return strlen(str);
}