#include "msp.h"


/**
 * main.c
 */
/*
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
    P4->DIR = 0;        //set Port 4 to input direction
    P4->SEL0 = (1<<0);  //select secondary function for port 4.0

    //store converted signal from A13 in memory register 0
    ADC14->MCTL[0] = ADC14_MCTLN_INCH_13;

    //activate unit
    ADC14->CTL0 = ADC14_CTL0_ON;

    //ADC14->CTL0 |= ADC14_CTL0_SSEL__SMCLK;
    //sample-hold-pulse mode
    ADC14->CTL0 |= ADC14_CTL0_SHP;
    //enable conversions
    ADC14->CTL0 |= ADC14_CTL0_ENC;
}
*/
//----------------------------------------
void init_ports(){
    P4->DIR = 0;
    P4->SEL0 = (1<<0);
    P2->DIR = 0xFF;
    P2->SEL0 = (1<<4);
}
void init_clock(){
    CS->KEY = CS_KEY_VAL;
    CS->CTL1 = CS_CTL1_SELS__DCOCLK | CS_CTL1_DIVS_7;
}
//period in 10s of ms
void init_timer(int period){
    period = period * 78;
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1 | TIMER_A_CTL_IE;
    TIMER_A0->EX0 = TIMER_A_EX0_IDEX__3;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[0] = period;

    //TIMER_A0->CCR[1] = period/2;

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
}
void ADC14_IRQHandler(void){
    //stop timer before writing to register?
    TIMER_A0->CCR[1] = ADC14->MEM[0]*78/0x3FFF/5;
}

/*
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // stop watchdog timer
    P2->DIR= 0xFF;  //set Port 2 to output direction
    P2->OUT = (0<<0);  //set bit 0 of port 2 (LED), clear others bits
    //select primary module (T_0.1) to drive pin 4 in port 2
    //P2->SEL0 = (1<<4);
    //P2->SEL1 = (0<<4);

    //uint32_t c = 3000000/(128*64);
        //conv_time = time*c;

    //pwm(2);
    //init_timer();
    //gen_interrupt(5);
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
        P2->OUT= joy_in;  //toggle light
        //
        ADC14->CTL0 |= ADC14_CTL0_SC;

    }
    */
void main(void){
    init_ports();
    init_clock();
    init_interrupts();
    init_timer(2);
    init_ADC();
    while(1){

    }
}
