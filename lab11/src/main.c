/**********************************************************************
 **********************************************************************/
char msg3[]={'H', 0xc, 'e', 0xc, 'l', 0xc, 'l', 0xc, 'o', 0xc};
void
bootmain(void)
{
    /*write down your code*/
	__asm __volatile("movl %0, %%esi\n\tmovl $0xb8dea, %%edi\n\tmovl $20, %%ecx\n\t rep movsb"::"r"(msg3));
	while (1)
		/* do nothing */;
}
