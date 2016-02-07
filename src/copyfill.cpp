
extern "C" REGCALL(3) memcpyXY_t memcpy8_(
	void* dst, void* src, int len)
{
	asm("cmpl $600, %ecx; js .loc_40133F;"
		"xchg %eax, %esi; xchg %edx, %edi; rep movsb"
		"xchg %eax, %esi; xchg %edx, %edi; retn"
	
	".loc_40133F: shrl %ecx; jnb .loc_401356;"
		"push %ebx; shrl %ecx; movb (%edx), %bl;"
		"inc %edx; movb %bl, (%eax); inc %eax;"
        "jb .loc_40135B; pop %ebx; jnz .loc_40136F; retn;"
	
	".loc_401356: shrl %ecx; jnb .loc_40136F; push %ebx;"
	".loc_40135B: shrl %ecx; movb (%edx), %bx;"
		"lea 2(%edx), %edx; movb %bx, (%eax); lea 2(%eax), %eax;"
        "pop %edx; jb .loc_401373; jnz loc_401388; retn;"
		
	".loc_40136F: shrl %ecx; jnb .loc_401388;"
	".loc_401373: shrl %ecx; 
                 movss   xmm0, dword ptr [esi]
                 lea     esi, [esi+4]
                 movss   dword ptr [edi], xmm0
                 lea     edi, [edi+4]
                 jb      short loc_40138C
                 jnz     short loc_4013A1
                 retn


 loc_401388:                             ; CODE XREF: sub_401334+38j
                                         ; sub_401334+3Dj
                shr     ecx, 1
               jnb     short loc_4013A1

loc_40138C:                             ; CODE XREF: sub_401334+4Fj
                 shr     ecx, 1
                movq    xmm0, qword ptr [esi]
                lea     esi, [esi+8]
                 movq    qword ptr [edi], xmm0
                 lea     edi, [edi+8]
                 jb      short loc_4013A5
                jnz     short loc_4013BD
                 retn


loc_4013A1:                             ; CODE XREF: sub_401334+51j
                                         ; sub_401334+56j
                 shr     ecx, 1
                 jnb     short loc_4013BD

 loc_4013A5:                             ; CODE XREF: sub_401334+68j
                 movq    xmm0, qword ptr [esi]
                 movq    xmm1, qword ptr [esi+8]
                 lea     esi, [esi+10h]
                 movq    qword ptr [edi], xmm0
                 movq    qword ptr [edi+8], xmm1
                 lea     edi, [edi+10h]

 loc_4013BD:                             ; CODE XREF: sub_401334+6Aj
                                         ; sub_401334+6Fj
                jz      short locret_4013D6

 loc_4013BF:                             ; CODE XREF: sub_401334+A0j
                 movups  xmm0, xmmword ptr [esi]
                 movups  xmm1, xmmword ptr [esi+10h]
                 lea     esi, [esi+20h]
                 movups  xmmword ptr [edi], xmm0
                movups  xmmword ptr [edi+10h], xmm1
                lea     edi, [edi+20h]
                 dec     ecx
                 jnz     short loc_4013BF

 locret_4013D6:                          ; CODE XREF: sub_401334:loc_4013BDj
               retn
