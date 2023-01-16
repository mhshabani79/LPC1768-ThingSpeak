#include "LPC17xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

int string_analyze(void);		
void hexseg(int);
void stringer(void);
void stringer2(void);
int delay (int);  // Delay function
unsigned char getchar1 (bool);  // Get Char from UART1
unsigned char getchar2 (void);  // Get Char from UART2
void sendchar1 (unsigned char ); // Send Char from UART1
void sendchar2 (unsigned char ); //  Send Char from UART2
void sendstring1 (char *);  //  Send string from UART1
void sendstring2 (char *);   // Send string from UART2



char string_in[2000]="KHAALI!";


int main (){
	

	
	 //init UART1
	LPC_SC->PCLKSEL0|=0X0;     //SET CLOCK OF UART0    CPUCLK/4=24MHZ
	LPC_UART1->LCR=0X83;      //SET 8bit data & enable dlab
	LPC_UART1->DLL=13;        // SET BAUD RATE = 115200
	LPC_UART1->DLM=0;
	LPC_UART1->LCR=0X3;      // DESABLE DLAB
	LPC_UART1->FCR=0X7;      // SET FIFO AND CLAER
	LPC_PINCON->PINSEL0 |= (1 << 30);             // Pin P0.10 used as TXD2 (Com2) 
  LPC_PINCON->PINSEL1 |= (1 << 0); // SET PIN FOR UART0
	LPC_UART1->IER=0X01;
	
	// init UART2
	LPC_SC->PCLKSEL0|=0X0;     //SET CLOCK OF UART0    CPUCLK/4=24MHZ
	LPC_PINCON->PINSEL0 |= (1 << 20);             // Pin P0.10 used as TXD2 (Com2) 
  LPC_PINCON->PINSEL0 |= (1 << 22);             // Pin P0.11 used as RXD2 (Com2) 	
	LPC_SC->PCONP = LPC_SC->PCONP|(1<<24);	      //Open UART2 power control bit	           
	LPC_UART2->LCR=0x83;      //SET 8bit data & enable dlab
	LPC_UART2->DLL=13;        // SET BAUD RATE = 115200
	LPC_UART2->DLM=0;
	LPC_UART2->LCR=0X3;      // DESABLE DLAB
	LPC_UART2->FCR=0X7;      // SET FIFO AND CLAER
	LPC_UART2->IER=0X01;


	int analog;
	PINSEL_CFG_Type adcpinsel;
	adcpinsel.Funcnum=1;
	adcpinsel.OpenDrain=PINSEL_PINMODE_NORMAL;
	adcpinsel.Pinmode=PINSEL_PINMODE_PULLUP;
	adcpinsel.Pinnum=25;
	adcpinsel.Portnum=0;
	
	PINSEL_ConfigPin(&adcpinsel);
	
	ADC_Init(LPC_ADC,100000);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_2,ENABLE);

	hexseg(2);
	
	char str[75];

/*
	sprintf (str,"AT+RST%c%c",0x0d,0x0a);          // AT+CWJAP=\"MPLab\",\"MpProject1400\  AT+CWMODE=1    //AT+GMR //AT+CWLAP   //AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80 //
	sendstring1(str);
	delay(1000);
	stringer(70,1);
	delay(1000);
	sendstring2(string_in);  */
	
	delay(1000000);
	
	sprintf (str,"AT%c%c",0x0d,0x0a);       
	sendstring1(str);
	stringer();
	sendstring2(string_in);
		
	
	sprintf (str,"AT+CWMODE=1%c%c",0x0d,0x0a);          
	sendstring1(str);
	stringer();
	sendstring2(string_in);


	sprintf (str,"AT+CWJAP=\"MPLab\",\"MpProject1400\"%c%c",0x0d,0x0a);          
	sendstring1(str);
	stringer();
	sendstring2(string_in);	

int k=0;
while(1) {
	k++;
	if(k==7) k=1;
	//k=6;
	
	ADC_StartCmd(LPC_ADC,ADC_START_NOW);
	while(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_2,ADC_DATA_DONE)==0);
	analog=ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
	int temp=(analog*4)-7800;
	
	sprintf (str,"%c%c***** Temprature is = %d *****%c%c",0x0d,0x0a,temp,0x0d,0x0a);
	sendstring2(str);
	
	memset(string_in,0,sizeof(string_in));
	sprintf (str,"AT+CIPMUX=0%c%c",0x0d,0x0a);          
	sendstring1(str);
	stringer();
	sendstring2(string_in);	 	


	memset(string_in,0,sizeof(string_in));
	sprintf (str,"AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80%c%c",0x0d,0x0a);          
	sendstring1(str);
	stringer();
	sendstring2(string_in);	
	

	memset(string_in,0,sizeof(string_in));
	int size_inst;
	if(k%6!=0) size_inst=46+(int)(ceil(log10(temp))); else size_inst=47; 
	sprintf (str,"AT+CIPSEND=%d%c%c",size_inst,0x0d,0x0a);  //49or48 for writing / 47 for reading 
	sendstring1(str);
	stringer();
	sendstring2(string_in);	
	
	
	memset(string_in,0,sizeof(string_in));
	char write_inst[]="GET /update?api_key=K801QQ0UAIUVHN0W&field1=";
	char read_inst[]="GET /channels/1405439/fields/2.json?results=6";
	if(k%6==0)
	sprintf (str,"%s%c%c",read_inst,0x0d,0x0a);      //GET /channels/1405439/fields/1.json?results=1   //GET /update?api_key=K801QQ0UAIUVHN0W&field1=250
	else sprintf (str,"%s%d%c%c",write_inst,temp,0x0d,0x0a);
	
	sendstring1(str); 
	sprintf (str,"AT+CIPCLOSE%c%c",0x0d,0x0a);       
	sendstring1(str);
	//stringer(1026,0);
	stringer2();
	sendstring2(string_in);
	
	if(k%6==0) 
	{
		int cond=string_analyze();
		hexseg(cond);
		sprintf (str,"%c%c***** Condition is = %d *****%c%c",0x0d,0x0a,cond,0x0d,0x0a);
		sendstring2(str);
	}
	
delay(60000000);
}
	
}

	





void stringer()
	{
	int t=0;
	char temp[]="HI"; //
	char close[]="OK";  //
		
	while( strcmp(close,temp)!=0 )  //t<count  
	{							
		string_in[t]=getchar1(0);
		t++;
		delay(10);
		if(t>1) //
			sprintf(temp,"%c%c",string_in[t-2],string_in[t-1]); //
	}
	
}

void stringer2() {
	int t=0;
	char temp[]="HICHII";
	char close[]="CLOSED";
	
	while( strcmp(close,temp)!=0  )   
	{							
		string_in[t]=getchar1(0);
		t++;
		delay(10);
		if(t>5)
			sprintf(temp,"%c%c%c%c%c%c",string_in[t-6],string_in[t-5],string_in[t-4],string_in[t-3],string_in[t-2],string_in[t-1]);
	}
}
	
int delay(int count)  // Delay function
{
  int j=0,i=0;
  for(j=0;j<count;j++)
  {
    // At 100Mhz, the below loop introduces  DELAY of 1 us
    for(i=0;i<23;i++)
		{
			__asm__("nop\n\t");
  }
}
	return 10;
}
unsigned char getchar1 (bool sel)  // Get Char from UART1
{	
	if(!sel)
	{
	while (!(LPC_UART1->LSR & 1<<0));
	}
	else if (sel)
	{
		int t=0;
		while ( !(LPC_UART1->LSR & 1<<0) )
		{
				t++;
		//		delay(1);
				if(t>=240000) break;
		}  
	}
	return LPC_UART1->RBR;   
}

unsigned char getchar2 (void)  // Get Char from UART2
{
	while (!(LPC_UART2->LSR & 1<<0));  
	return LPC_UART2->RBR;  
}


void sendchar1 (unsigned char ch) // Send Char from UART1
{
	while (!(LPC_UART1->LSR & 1<<5));
	LPC_UART1->THR=ch;
	LPC_UART1->TER |= (1 << 7);
}

void sendchar2 (unsigned char ch) //  Send Char from UART2
{
	while (!(LPC_UART2->LSR & 1<<5));
	LPC_UART2->THR=ch;
	LPC_UART2->TER |= (1 << 7);
}

void sendstring1 (char *str)  //  Send string from UART1
{
	for (int i=0;str[i]!='\0';i++)
	sendchar1(str[i]);
	
}

void sendstring2 (char *str)   // Send string from UART2
{
	for (int i=0; str[i] ;i++)
	sendchar2 (str[i]);
	
}

void hexseg(int a)
{
	GPIO_SetDir(0,0x000000FF, 1);
	GPIO_SetDir(2,0x000000FF, 1);
	GPIO_ClearValue(0, 0x000000FF);
	GPIO_ClearValue(2, 0x000000FF);
	if(a==1)
	{
		GPIO_SetValue(0, 0x000000C7);  //R
		GPIO_SetValue(2, 0x00000098);
	}
	else if(a==0)
	{
		GPIO_SetValue(0, 0x000000BB);  //S
		GPIO_SetValue(2, 0x00000088);
	}
	else
	{
	GPIO_ClearValue(0, 0x000000FF);
	GPIO_ClearValue(2, 0x000000FF);
	}
	
}

int string_analyze()
{
	char f[]="field2";
	char f2[]="hichii";
	
	int t=1999;
	while(t>=6)
	{
		
		sprintf(f2,"%c%c%c%c%c%c",string_in[t-5],string_in[t-4],string_in[t-3],string_in[t-2],string_in[t-1],string_in[t]);
		if( strcmp(f,f2)==0 )
		{	if( string_in[t+4]=='0' )
				return 0;
			else if( string_in[t+4]=='1' )
				return 1;
		}
		t--;
			
	}
	return 2;
}

