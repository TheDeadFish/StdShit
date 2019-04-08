// pointer size generics
#ifdef __x86_64__ 
asm(".set ptax,%rax; .set ptbx,%rbx; .set ptcx,%rcx; .set ptdx,%rdx;");
asm(".set ptsi,%rsi; .set ptdi,%rdi; .set ptbp,%rbp; .set ptsp,%rsp;");
asm(".macro pt_inc2 reg; add $2, \\reg; .endm;");
#else
asm(".set ptax,%eax; .set ptbx,%ebx; .set ptcx,%ecx; .set ptdx,%edx;");
asm(".set ptsi,%esi; .set ptdi,%edi; .set ptbp,%ebp; .set ptsp,%esp;");
asm(".macro pt_inc2 reg; inc \\reg; inc \\reg; .endm;");
#endif
