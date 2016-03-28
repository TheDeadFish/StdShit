
asm(".section .text$cmd_esc_len;"
	"_cmd_esc_msk: .long 0xFFFFFFFF,"
	"0x58001367, 0x78000000, 0x10000000,"
	"0xFFFFFFFF, 5, 0, 0;"
	"cmd_esc_len1: incl %eax; cmd_esc_len2:"
	"addl %edx, %eax; test %ebx,%ebx; jg 5f;"
	"je 6f; incl %eax; 5: incl %eax; 6: ret;" );

asm(".section .text$cmd_esc_lenA; _cmd_esc_lenA:"
	".long cmd_esc_len2, cmd_esc_len1, cmd_esc_getA;"
	"cmd_esc_getA: movzbl (%edi), %ebx; incl %edi; ret;");

asm(".section .text$cmd_esc_lenW; _cmd_esc_lenW:"
	".long cmd_esc_len2, cmd_esc_len1, cmd_esc_getW;"
	"cmd_esc_getW: movzwl (%edi), %ebx; add $2, %edi; ret;");

asm(".section .text$cmd_esc_cpyA;"
	"_cmd_esc_cpyA: .long 3f, 2f, cmd_esc_getA;"
    "2: movb $34, (%eax); incl %eax;"
	"3: decl %esi; js 4f; movb $92, (%eax);"
	"inc %eax; jmp 3b; 4: test %ebx,%ebx; jg 5f;"
	"je 6f; movb $94, (%eax); incl %eax; 5:"
	"movb %bl, (%eax); incl %eax; 6: ret;" );
	
asm(".section .text$cmd_esc_cpyW;"
	"_cmd_esc_cpyW: .long 3f, 2f, cmd_esc_getW;"
    "2: movw $34, (%eax); addl $2, %eax;"
	"3: decl %esi; js 4f; movw $92, (%eax);"
	"addl $2, %eax; jmp 3b; 4: test %ebx,%ebx; jg 5f;"
	"je 6f; movw $94, (%eax); addl $2, %eax; 5:"
	"movw %bx, (%eax); addl $2, %eax; 6: ret;" );
	
ASM_FUNC("__Z14cmd_escape_lenPKcPci@12",
	"movl $_cmd_esc_lenA, %edx; xor %eax, %eax; jmp _cmd_escape@20");
ASM_FUNC("__Z14cmd_escape_lenPKwPwi@12",
	"movl $_cmd_esc_lenW, %edx; xor %eax, %eax; jmp _cmd_escape@20");
ASM_FUNC("__Z10cmd_escapePcPKcS_i@16",
	"movl $_cmd_esc_cpyA, %edx; jmp _cmd_escape@20");
ASM_FUNC("__Z10cmd_escapePwPKwS_i@16",
	"movl $_cmd_esc_cpyW, %edx; jmp _cmd_escape@20");
	
extern "C"
size_t REGCALL(2) cmd_escape(size_t dstLen, void* vtable,
	const void* src, const void* end, byte flags)
{
	asm("movl %%ecx, %%ebp; andl $16, %%ebp; loop: orl $-1, %%esi;"
		"1: incl %%esi; cmpl %3, %%edi; je end; call *8(%%edx);"
		"cmpl $92, %%ebx; je 1b; cmpl $127, %%ebx; ja norm;"
		"bt %%ebx, _cmd_esc_msk(%%ebp); jnc norm;"
		"testl %%ebx, %%ebx; je end; cmpl $34, %%ebx; je quot;"
		"cmpl $37, %%ebx; je pcnt; testb %%cl, %%cl; jns chg_quot;"
		"norm: call *(%%edx); jmp loop; quot: leal 1(%%esi,%%esi),"
		"%%esi; testl %%ebp, %%ebp; jne norm; jmp esc_char;"
		"pcnt: testb $8, %%cl; jne norm; esc_char:"
		"bts $31, %%ebx; testb %%cl, %%cl; jns norm;"
		"chg_quot: xorb $128, %%cl; call *4(%%edx); jmp loop;"
		"end: xorl %%ebx, %%ebx; testb %%cl,%%cl; js in_quot;"
		"testb $6, %%cl; je done; testb $2, %%cl; jne en_quot;"
		"testl %%esi, %%esi; je done; en_quot: addl $4, %%edx;"
		"in_quot: testb $2, %%cl; jne done;" 
		"add %%esi, %%esi; movb $34, %%bl; done: call *(%%edx);"
	: "+a"(dstLen) : "c"(flags), "D"(src), "m"(end), "d"(vtable) :
		"ebx", "esi", "ebp", "memory"); return dstLen;
}
