/*
 * 本文件实现vga的相关功能，清屏和屏幕输出
 * clear_screen和append2screen必须按照如下借口实现
 * 可以增加其他函数供clear_screen和append2screen调用
 */
#define index_base 0x3D4
#define data_base 0x3D5
#define LINE 0xE
#define COLUMN 0XF
#define VGA_INIT_POINTER 0xB8000
#define SIZEOFWORD 2
#define LINENUMBER 25
#define COLUMNNUMBER 80
extern void outb (unsigned short int port_to, unsigned char value);
extern unsigned char inb(unsigned short int port_from);
//VGA字符界面规格：25行80列
//VGA显存初始地址为0xB8000
/*
行号寄存器的索引号是14，即0xE；列号寄存器的索引号是15，即0xF
索引端口地址是：0x3D4   数据端口地址是：0x3D5
*/
short cur_line=0;
short cur_column=0;//当前光标位置
char * vga_init_p=(char *)0xB8000;
int pos;

int get_pos(void){
	return pos = (cur_line)*COLUMNNUMBER*SIZEOFWORD +(cur_column)*SIZEOFWORD;
}

void update_cursor(void){//通过当前行值cur_cline与列值cur_column回写光标
	//填写正确的内容
	outb(index_base,LINE);
	outb(data_base,cur_line);
	outb(index_base,COLUMN);
	outb(data_base,cur_column);
}

short get_cursor_position(void){//获得当前光标，计算出cur_line和cur_column的值
	//填写正确的内容   
	outb(index_base,LINE);
	cur_line = inb(data_base);
	outb(index_base,COLUMN);
	cur_column = inb(data_base);
}


void clear_screen(void) {
	char str;
	//填写正确的内容    
	/*
	cld				              # make direction flag count up
	movl	$_end, %ecx		    # find end of .bss
	movl	$_bss_start, %edi	# edi = beginning of .bss
	subl	%edi, %ecx		    # ecx = size of .bss in bytes
	shrl	%ecx			        # size of .bss in longs
	shrl	%ecx
	
	xorl	%eax, %eax		    # value to clear out memory
	repne			            	# while ecx != 0
	stosl				            # clear a long in the bss
	*/
	/*for(int i = 0; i < get_pos(); i++){
		vga_init_p[i] = '0';
	}*/
	for (int i = 0; i < LINENUMBER; ++i)
		for (int j = 0; j < COLUMNNUMBER * 2; j += 2) {
			vga_init_p[i * COLUMNNUMBER * 2 + j] = 0x00;
			vga_init_p[i * COLUMNNUMBER * 2 + j + 1] = 0x07;
		}
	cur_line = 0;
	cur_column = 0;
	update_cursor();
}


void scroll_up(void){
	int backline = COLUMNNUMBER*SIZEOFWORD;
	pos = get_pos();
	for (int i = backline; i < pos; i++){
		vga_init_p[i - backline] = vga_init_p[i];
	}
	for(int i = pos -backline; i < pos; i = i + 2){
		vga_init_p[i] = 0x00;
		vga_init_p[i+1] = 0x07;
	}
}
void outputonechar(char *str,int color){
	get_pos();
	if(*str == '\r'|| *str == '\n'){
		if(cur_line == LINENUMBER -1){
			scroll_up();
			//cur_line++;
			cur_column = 0;
		}
		else{
			cur_line++;
			cur_column = 0;
		}
	}
	else{
		vga_init_p[pos] = *str;
		vga_init_p[pos + 1] = color;
		if(cur_column == COLUMNNUMBER - 1){
			if(cur_line == LINENUMBER - 1){
				scroll_up();
				cur_column = 0;
			}
			else{
				cur_column = 0;
				cur_line++;
			}
		}
		else{
			cur_column += 1;
		}	
	}
	update_cursor();
}

void append2screen(char *str,int color){ 
	get_pos();
	while(*str){
		outputonechar(str,color);
		str++;
	}
}
