#include "msp.h"


/**
 * main.c
 */

#define M_SEC 16
void init_ports(){
    P4->DIR = 0;
    P4->SEL0 = (1<<0);
    P2->DIR = 0xFF;
    P2->SEL0 = (1<<4);
}
void init_clock(){
    CS->KEY = CS_KEY_VAL;
    CS->CTL1 = CS_CTL1_SELS__DCOCLK | CS_CTL1_DIVS_6;
}
//period in ms
void init_timer(int period){
    period = period * M_SEC;
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1 | TIMER_A_CTL_IE;
    TIMER_A0->EX0 = TIMER_A_EX0_IDEX__3;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[0] = period;

    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
}
void init_interrupts(){
    NVIC_EnableIRQ(TA0_0_IRQn);
    NVIC_EnableIRQ(ADC14_IRQn);
    NVIC_SetPriority(TA0_0_IRQn, 1);
    //NVIC_SetPriority(ADC14_IRQn, 2);
}
void init_ADC(){
    ADC14->CTL0 = ADC14_CTL0_ON | ADC14_CTL0_SHP;
    ADC14->MCTL[0] = ADC14_MCTLN_INCH_13;
    ADC14->IER0 = ADC14_IER0_IE0;
}

void TA0_0_IRQHandler(void){
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    ADC14->CLRIFGR0 |= ADC14_CLRIFGR0_CLRIFG0;
}
void ADC14_IRQHandler(void){
    TIMER_A0->CCR[1] = ADC14->MEM[0]*M_SEC/0x3FFF + M_SEC;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}
void main(void){
    init_ports();
    init_clock();
    init_interrupts();
    init_timer(20);
    init_ADC();
    while(1){

    }
}
