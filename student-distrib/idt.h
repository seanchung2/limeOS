#ifndef _IDT_H
#define _IDT_H

/* Define variables */
#define SYSTEM_CALL_VEC_NUM 0x80 

/* Setup a macro handler */
#define EXECPTION_HANDLER (exeception_name,msg_print)	\
void exeception_name()									\
{														\
	printf("%s\n",#msg_print); 							\
	while(1);											\
}

/* Define the handler */												
void DE();
void DB();
void NMI();
void BP();
void OF();
void BR();
void OF();
void BR();
void UD();
void NM();
void DF();
void CSO();
void TS();
void NP();
void SS();
void GP();
void PF();
void MF();
void AC();
void MC();
void XF();

/* initialize the IDT */
extern void init_idt();

#endif