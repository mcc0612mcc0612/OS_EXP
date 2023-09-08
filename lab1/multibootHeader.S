#define MULTIBOOT_HEADER_MAGIC                  0x1BADB002
.globl start       #一般都用start
/*此处，按照multiboot协议要求定义必要的参数*/
/*格式：XXX_ITEM_NAME=value*/
/*value可以是十六进制的（0x开头）、十进制的等等*/
/*为便于阅读，可以适当增加一些注释和空行*/
/*此处开始，按协议标准来定义必须的multiboot header*/

.section ".multiboot_header"      #先起一个section的名字
/*使用.long和前面定义的参数构建头结构，每次32位，格式为：.long XXX_ITEM_NAME*/
.align  4
/* magic */
.long   MULTIBOOT_HEADER_MAGIC
/* flags */
.long   0
/* checksum */
.long   -(MULTIBOOT_HEADER_MAGIC + 0)

.text  #进入代码段
.code32  #32位代码
start:          #这个跟第一行的声明要一致
/*下面屏幕输出OK或其他字符序列*/
/*使用指令movl $0x12345678, 0xB8000*/     #0x1234和0x5678各自输出1个字符
/*根据需要输出多个字符，也可以使用其他mov指令*/
/*可以根据需要使用nop指令隔开不同功能片段，也可以适当使用空行*/
/*根据需要初始化串口*/
/*根据需要串口输出你的字符序列，详见前面串口编程简介*/
/*实验结束，让计算机停机，方法：使用hlt指令，或者死循环*/
  movl $0x2f342f39, 0xB8010 /*PB20111694*/
  movl $0x2f362f31, 0xB800C/*16*/
  movl $0x2f312f31, 0xB8008/*11*/
  movl $0x2f302f32, 0xB8004/*20*/
  movl $0x2f422f50, 0xB8000/*PB*/
  nop
  movw $0x3F8, %dx
  movb $0x50, %al  /* 输出字母P */
  outb %al, %dx  /* put char */
  movb $0x42, %al  /* 输出字母B */
  outb %al, %dx  /* put char */
  movb $0x32, %al  /* 输出字母2 */
  outb %al, %dx  /* put char */
  movb $0x30, %al  /* 输出字母0 */
  outb %al, %dx  /* put char */
  movb $0x31, %al  /* 输出字母1 */
  outb %al, %dx  /* put char */
  movb $0x31, %al  /* 输出字母1 */
  outb %al, %dx  /* put char */
  movb $0x31, %al  /* 输出字母1 */
  outb %al, %dx  /* put char */
  movb $0x36, %al  /* 输出字母6 */
  outb %al, %dx  /* put char */
  movb $0x39, %al  /* 输出字母9 */
  outb %al, %dx  /* put char */
  movb $0x34, %al  /* 输出字母4 */
  outb %al, %dx  /* put char */
  nop
  nop
  hlt


