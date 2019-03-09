/*********************************************************************
 
 *--------------------------------------------------------------------
 
 * Description	: This example scans the channel ADC0.5. Voltage at
 * 				the pin P1.31 is varied by varying the pot P1. Since ref
 *				voltage is 3.3, ADC output range is 000 to FFF (12 bit) 
 **********************************************************************/

#include<LPC17xx.h>
#include<stdio.h>

#include"AN_LCD.h"

#define	Ref_Vtg		9.99 //reference voltage is set 9.99 as it is scaled and added to 90
#define	Full_Scale	0xFFF			//12 bit ADC

void pwm_init(void);
unsigned long int j,key,tmp=0;
unsigned char flag;

int main(void)
{
	unsigned long adc_temp;
	unsigned int i;
	float in_vtg, cur_freq, diff_freq;
	float fmr[7] = {91.1,91.9,92.7,93.5,94.3,95.95,98.3};
	unsigned char vtg[7],dval[7];
	unsigned char Msg3[11] = {"FM:"};
	unsigned char Msg4[12] = {"ADC OUTPUT:"};

	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_SC->PCONP |= (1<<15); //Power for GPIO block
  lcd_init();
  LPC_PINCON->PINSEL3 |= 0xC0000000;	//P1.31 as AD0.5
	LPC_SC->PCONP |= (1<<12);		//enable the peripheral ADC
	
  SystemCoreClockUpdate();
	
	temp1 = 0x80; //display message on lcd setting first row cursor
	lcd_com();  
	delay_lcd(800);
	lcd_puts(&Msg3[0]); //display "FM:" message 

	pwm_init(); //initialise pwm module

	while(1)
	{
		LPC_ADC->ADCR = (1<<5)|(1<<21)|(1<<24);	//ADC0.5, start conversion and operational	
		for(i=0;i<2000;i++);			//delay for conversion
		while((adc_temp = LPC_ADC->ADGDR) == 0x80000000);	//wait till 'done' bit is 1, indicates conversion complete
		adc_temp = LPC_ADC->ADGDR; //retrieve value
		adc_temp >>= 4; //shift adgdr values to LSBs
		adc_temp &= 0x00000FFF;			//12 bit ADC
		in_vtg = (((float)adc_temp * (float)Ref_Vtg))/((float)Full_Scale) + 90;	//calculating input analog voltage
		cur_freq = in_vtg; //the current frequency value is calculated by adding 90
		sprintf(vtg,"%3.2fMHz",in_vtg);	//convert the readings into string to display on LCD
		//sprintf(dval,"%x",adc_temp);
		for(i=0;i<2000;i++);

		temp1 = 0x85; //display frequency first row cursor at 5th posittion
		lcd_com();
		delay_lcd(800);
		lcd_puts(&vtg[0]);// display "9x.xx" value
		
		
		for(i=0;i<7;i++)
		{
			diff_freq = cur_freq - fmr[i]; // obtain difference 
			
			if(diff_freq<0)
			{
				diff_freq = -diff_freq;  //obtain difference modulus
			}
			
			if(diff_freq<=0.05)
			{
				LPC_PWM1->MR4 = 30000;   //maximum brightness if difference is minimum
				LPC_PWM1->LER = 0X000000FF; //set Load enable register
				break;
			}
			
			else if(diff_freq<=0.1)
			{
				LPC_PWM1->MR4 = 22500; //decrease brightness as difference increases
				LPC_PWM1->LER = 0X000000FF; //set Load enable register
				break;
			}
			
			else if(diff_freq<=0.2)
			{
				LPC_PWM1->MR4 = 15000; //decrease brightness as difference increases
				LPC_PWM1->LER = 0X000000FF; //set Load enable register
				break;
			}
			
			else if(diff_freq<=0.4)
			{
				LPC_PWM1->MR4 = 7500; //decrease brightness as difference increases
				LPC_PWM1->LER = 0X000000FF; //set Load enable register
				break;
			}
			
			else if(diff_freq<=0.5)
			{
				LPC_PWM1->MR4 = 3000; //decrease brightness as difference increases
				LPC_PWM1->LER = 0X000000FF; //set Load enable register
				break;
			}
			
			else
			{
				LPC_PWM1->MR4 = 0; //zero brightness when difference is large
				LPC_PWM1->LER = 0X000000FF; //set Load enable register
			}
		}
		

		/*temp1 = 0xCB;
		lcd_com();
		delay_lcd(800);
		lcd_puts(&dval[0]);*/

        for(i=0;i<200000;i++);
        for(i=0;i<7;i++)
        vtg[i] = 0x00;
        adc_temp = 0;
        in_vtg = 0;
	}
}

void pwm_init(){
	LPC_SC->PCONP |= (1<<6);  //POWER ON PWM1 ( can also be done through system config wizard)
	LPC_PINCON->PINSEL3 |= 0x00008000; //PWM1.4 is selected for the pin P1.23
	
	LPC_PWM1->PR = 0;//PRESCALE 0
	LPC_PWM1->PCR = 0X00001000; //SELECT PWM1.1 SINGLE EDGE
	LPC_PWM1->MCR = 0X00000002; //Reset TC on PWMMR0  **
	LPC_PWM1->MR0 = 30000; //setup match register 0 count
	LPC_PWM1->MR4 = 0x00000000; 
	LPC_PWM1->LER = 0X000000FF; //enable shadow copy register
	LPC_PWM1->TCR = 0X00000002; //RESET COUNTER AND PRESCALER
	LPC_PWM1->TCR = 0X00000009; //enable PWM and counter
	
	return;
}

 



