//debug.c


#include "typedefine.h "
typedef struct
{
	BYTE footprint;
	BYTE Cntbuff[10];
}Debug_my;


Debug_my m_debug;

void Debug_foot_Log(BYTE num)
{
	m_debug.footprint = m_debug.footprint |(1<<num);
}
void Debug_foot_printf(SHORT frq)
{
	static SHORT frqcnt;

	frqcnt++;
	if(frqcnt >=frq)
	{
		frqcnt = 0;
		PCPrintf("DF %d",m_debug.footprint);
	}
}

void Debug_cnt_Log(BYTE cnt)
{
	m_debug.Cntbuff[cnt]++;
}

void Debug_cnt_printf(BYTE    num, SHORT frq )
{
	static SHORT frqcnt;

	frqcnt++;
	if(frqcnt >=frq)
	{
		frqcnt = 0;
		if(num ==1) PCPrintf("DP %d",m_debug.Cntbuff[0]);
		else if(num ==2) PCPrintf("DP %d, %d",m_debug.Cntbuff[0],m_debug.Cntbuff[1]);
		else if(num ==3) PCPrintf("DP %d, %d, %d",m_debug.Cntbuff[0],m_debug.Cntbuff[1],m_debug.Cntbuff[2]);
		else if(num ==4) PCPrintf("DP %d, %d, %d ,%d",m_debug.Cntbuff[0],m_debug.Cntbuff[1],m_debug.Cntbuff[2],m_debug.Cntbuff[3]);
	}
}
