#include "msp.h"


/**
 * main.c
 */
void init_timer(){
    //write password
    CS->KEY = CS_KEY_VAL;
    //set frequency to 1.5 MHz
    //CS->CTL0 = CS_CTL0_DCORSEL_0; //| (CS_CTL0_DCOTUNE_MASK & 0xFFFF);
    //select DCO as clock source for signal SMCLK and divide by 128
    CS->CTL1 = CS_CTL1_SELS__DCOCLK | CS_CTL1_DIVS_7;
    //divide by 3
    TIMER_A0->EX0 = TIMER_A_EX0_IDEX__3;
    //divide by 8
    //TIMER_A0->CTL |= TIMER_A_CTL_ID__8;
    //reset comparator
    //TIMER_A0->CCTL[0] = 0;
    //set timer to use SMCLK signal and set up mode (count up to comparator value)
    TIMER_A0->CTL |= (TIMER_A_CTL_SSEL_MASK & TIMER_A_CTL_TASSEL_2) | (TIMER_A_CTL_MC_MASK & TIMER_A_CTL_MC_1);
}
void delay(uint16_t time){
    //set counter comparator
    TIMER_A0->CCR[0] = time;
    //reset timer
    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
    //wait until CCIFG flag is one
    while(!(TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)){

    }
    //clear the CCIFG flag
    TIMER_A0->CCTL[0] = TIMER_A0->CCTL[0] & !(TIMER_A_CCTLN_CCIFG);
}
void pwm(uint16_t width){
    //convert to ms
    width = width * 78;
    //set counter comparator
    TIMER_A0->CCR[0] = width;
    //set output mode to toggle (Change to something/reset)
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    //write some value to the CCR[1] that is less than width
    TIMER_A0->CCR[1] = width/2;
    //reset timer
    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
}

void TA0_0_IRQHandler(void){
    //clear the CCIFG flag
    TIMER_A0->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);
    //toggle signal
    P2->OUT ^= (1<<5);
}
void init_interrupt(){
    NVIC_EnableIRQ(TA0_0_IRQn);
    NVIC_SetPriority(TA0_0_IRQn,1);
    //enable interrupts
    TIMER_A0->CTL |= TIMER_A_CTL_IE;
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;

}
void gen_interrupt(uint16_t width){
    width = width * 78;
    //set counter comparator
    TIMER_A0->CCR[0] = width;
    //reset timer
    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
}

void init_analog(){
    P5->DIR = 0;  //set Port 2 to input direction
    P5->SEL0 = (1<<5);
    P5->SEL1 = (0<<5);

    //store converted signal from A0 in memory register 0
    ADC14->CTL0 &= ~ADC14_CTL1_CSTARTADD_MASK;
    ADC14->MCTL[0] = 0;

    ADC14->CTL0 = ADC14_CTL0_SC | ADC14_CTL0_ENC | ADC14_CTL0_ON;

}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // stop watchdog timer
    P2->DIR= 0xFF;  //set Port 2 to output direction
    P2->OUT = (0<<0);  //set bit 0 of port 2 (LED), clear others bits
    //select primary module (T_0.1) to drive pin 4 in port 2
    P2->SEL0 = (1<<4);
    P2->SEL1 = (0<<4);

    //uint32_t c = 3000000/(128*64);
        //conv_time = time*c;

    //pwm(2);
    init_interrupt();
    init_timer();
    gen_interrupt(5);
    init_analog();

    int l = 0;

    while(1) {
        int joy_in = ADC14->MEM[0];
        //P2->OUT ^= (1<<j);    // Toggles the bit 0 of the output register
        //j = (j+1)%3;
        //poll interrupt flag
        //P2->OUT = (TIMER_A0->CCTL[1]<<4);
        //P2->OUT ^= (1<<4);

        //for(i=0; i<1000; i++);      // Wait
        //int i;
        //for(i=0; i<1000; i++){
            //delay(10);
        //}
        //l = !l;
        //P2->OUT= (l<<5)|(l<<0);  //toggle light

    }

}
