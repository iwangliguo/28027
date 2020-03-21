#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

void GPIO_Int(void);
interrupt void tim0_isr(void);
interrupt void epwm1_isr(void);
void InitEPwmTimer();


int  cnt=0;
int  dir=0; //配置cnt的计数方向
float dutycycle=0.1;
void main(void)
{
  InitSysCtrl();
  GPIO_Init();
 // GpioDataRegs.GPASET.bit.GPIO2=1;
  DINT;  // Disable CPU interrupts
  InitPieCtrl();
  // Disable CPU interrupts and clear all CPU interrupt flags:
  IER = 0x0000;
  IFR = 0x0000;
  InitPieVectTable();

  EALLOW;

  PieVectTable.EPWM1_INT = &epwm1_isr;
  PieVectTable.TINT0 = &tim0_isr;

  PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   //使能PIE
  PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  //使能int1.7
  PieCtrlRegs.PIEIER3.bit.INTx1 = 1;  //使能int3.1

  IER |= M_INT1;//使能GROUP1
  IER |= M_INT3;//使能GROUP3
  EINT;  // Enable CPU interrupts

  EDIS;

  InitEPwmTimer();
  InitCpuTimers();
  ConfigCpuTimer(&CpuTimer0, 60, 20000);    //设置20ms进入一次中断
  StartCpuTimer0();

  while(1)
  {

  }
}


void GPIO_Init(void)
{
    EALLOW;

    InitEPwm1Gpio();


    EDIS;

    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;

    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;

    EDIS;

    GpioDataRegs.GPASET.bit.GPIO2=1; //初始化都灭
    GpioDataRegs.GPASET.bit.GPIO3=1;
}

interrupt void tim0_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    if((!dir)&(cnt<=100))
    {
        cnt++;
        if(cnt>=100)
        {
            cnt=100;
            dir=1;
        }
    }

    if(dir&(cnt>=0))
    {
        cnt--;
        if(cnt<=0)
        {
            cnt=0;
            dir=0;
        }
    }

    EPwm1Regs.CMPA.half.CMPA = 1500+135*cnt;
    dutycycle=(float)(1500+135*cnt)/15000;

}

interrupt void epwm1_isr(void)
{
       // Clear INT flag for this timer
       EPwm1Regs.ETCLR.bit.INT = 1;
       // Acknowledge this interrupt to receive more interrupts from group 3
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;


}

void InitEPwmTimer()
{
   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      // Stop all the TB clocks
   EDIS;

   EPwm1Regs.TBPRD = 15000;

   EPwm1Regs.TBPHS.half.TBPHS = 0;

   EPwm1Regs.TBCTR = 0;
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;
   EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;
   EPwm1Regs.TBCTL.bit.CLKDIV = 1;


   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   EPwm1Regs.CMPA.half.CMPA = 1500;

   EPwm1Regs.AQCTLA.bit.ZRO =AQ_SET;
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
   EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;
   EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;

/*
   // Setup Deadband
   EPwm1Regs.DBCTL.bit.OUT_MODE =;
   EPwm1Regs.DBCTL.bit.POLSEL =;
   EPwm1Regs.DBCTL.bit.IN_MODE =;
   EPwm1Regs.DBRED = ;
   EPwm1Regs.DBFED = ;
*/
   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;

}



