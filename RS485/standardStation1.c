/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
� Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 5/27/2014
Author  : NeVaDa
Company : 
Comments: 


Chip type               : ATmega128L
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0     
Data Stack size         : 1024    
*****************************************************/

#include <mega128.h>
#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

unsigned char intFlag=0,intFlag0=0;//,tuy[5];
unsigned char msg[200],msg0[200],callnumber[13],rcallnumber[13],command[],time[20],status[200];
unsigned char I1[8],I2[8],I3[8],U1[8],U2[8],U3[8],P1[8],P2[8],P3[8],totalenergy[11]; 
unsigned char sData[8]; // outgoing buffer from MASTER to SLAVES
unsigned char bydata,autosend,autotime; // counter variable
float realnum;

unsigned int j=0,j0=0,malenh=99;

#define role1 PORTB.0
#define role2 PORTB.1
#define resetSim900 PORTE.5

#define COL1 PORTC.0
#define COL2 PORTC.1
#define COL3 PORTC.2
#define COL4 PORTC.3

#define ROW1 PORTC.4
#define ROW2 PORTC.5
#define ROW3 PORTC.6
#define ROW4 PORTC.7

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART Receiver buffer
#define RX_BUFFER_SIZE0 64
char rx_buffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0 <= 256
unsigned char rx_wr_index0,rx_rd_index0,rx_counter0;
#else
unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
#endif

// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow0;

// USART0 Receiver interrupt service routine, ket noi MFM383A
interrupt [USART0_RXC] void usart0_rx_isr(void)
{
char status,data;
status=UCSR0A;
intFlag0=1;
data=UDR0;    
msg0[j0]=data;
++j0;
}
#pragma used+
void putchar0(char c)
{
while ((UCSR0A & DATA_REGISTER_EMPTY)==0);
UDR0=c;
}
#pragma used-
#ifndef _DEBUG_TERMINAL_IO_


// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+

#pragma used-
#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE 8
char tx_buffer0[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE <= 256
unsigned char tx_wr_index0,tx_rd_index0,tx_counter0;
#else
unsigned int tx_wr_index0,tx_rd_index0,tx_counter0;
#endif

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter0 == TX_BUFFER_SIZE);
#asm("cli")
if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer0[tx_wr_index0++]=c;
#if TX_BUFFER_SIZE != 256
   if (tx_wr_index0 == TX_BUFFER_SIZE) tx_wr_index0=0;
#endif
   ++tx_counter0;
   }
else
   UDR0=c;
#asm("sei")
}
#pragma used-
#endif
//===================================================
// USART1 Receiver buffer,ket noi SIMCOM
#define RX_BUFFER_SIZE1 8
char rx_buffer1[RX_BUFFER_SIZE1];

#if RX_BUFFER_SIZE1 <= 256
unsigned char rx_wr_index1,rx_rd_index1,rx_counter1;
#else
unsigned int rx_wr_index1,rx_rd_index1,rx_counter1;
#endif

// This flag is set on USART1 Receiver buffer overflow
bit rx_buffer_overflow1;

// USART1 Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rx_isr(void)
{
char status,data;
status=UCSR1A;
intFlag=1;
data=UDR1;    
msg[j]=data;
++j;
}
#pragma used+
void putchar1(char c)
{
while ((UCSR1A & DATA_REGISTER_EMPTY)==0);
UDR1=c;
}
#pragma used-
//===================================================
// Declare your global variables here
// Declare your global variables here
void send2Sim900(unsigned char *data)
{
    unsigned int i;
      for(i=0;i< strlen(data);++i)
      {putchar1(data[i]);}     
}

void xoachuoi(void)
{
      unsigned int i; 
      
      for(i=0;i<200;++i)
        {msg0[i]=NULL;} 
        
      for(i=0;i<200;++i)
        {msg[i]=NULL;}         

      for(i=0;i<200;++i)
        {status[i]=NULL;} 
               
      for(i=0;i<strlen(command);++i)
        {command[i]=NULL;}
               
      for(i=0;i<13;++i)
        {callnumber[i]=NULL;}                  
}

void clear_msg(void)
{
while (intFlag==0)//xoa tin nhan cu
    { 
    send2Sim900("AT+CMGD=1"); 
    putchar1(0x0d); //<CR> 
    delay_ms(500);
    } 
intFlag=0; 
    while (intFlag==0)//xoa tin nhan cu
    { 
    send2Sim900("AT+CMGD=2"); 
    putchar1(0x0d); //<CR> 
    delay_ms(500);
    } 
intFlag=0;              
}

//==============================
//khoi tao sim900
void initSim900(void)
{
while (intFlag==0)
      { 
      send2Sim900("ATZ"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0;
      
while (intFlag==0)
      { 
      send2Sim900("ATE0"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0; 
     
while (intFlag==0)
      { 
      send2Sim900("AT+CLIP=1"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0; 
     
while (intFlag==0)
      { 
      send2Sim900("AT&W"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0; 

while (intFlag==0)
      { 
      send2Sim900("AT+CMGF=1"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0; 
     
while (intFlag==0)
      { 
      send2Sim900("AT+CSAS"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0;     

while (intFlag==0)
      { 
      send2Sim900("AT+CNMI=2,2,0,0,0"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0;                        
/////////////////////    
while (intFlag==0)
      { 
      send2Sim900("AT"); 
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0;
while (intFlag==0)
     { 
      send2Sim900("AT+CGMM");
      putchar1(0x0d); //<CR> 
      delay_ms(500);
     } 
     intFlag=0;
/////////////////////////////////////
while (intFlag==0)//xoa tin nhan cu
      { 
      send2Sim900("AT+CMGD="); 
      putchar1(0x22);//"
      send2Sim900("DEL ALL");
      putchar1(0x22);//"
      putchar1(0x0d); //<CR> 
      delay_ms(500);
      } 
     intFlag=0; 
//while (intFlag==0)//xoa tin nhan cu
     // { 
      //send2Sim900("AT+CMGD=2"); 
      //putchar1(0x0d); //<CR> 
      //delay_ms(500);
      //} 
     //intFlag=0; 
////////////////////////////////////       
while (intFlag==0)
      { 
      send2Sim900("AT+CGMI");
      putchar1(0x0d); //<CR> 
      delay_ms(5000);
      role2=1;
      delay_ms(1000);
      role2=0;
      delay_ms(1000);
     } 
     intFlag=0;              
}
// chuong trinh goi 
void callbySim900(unsigned char *phonenumber)
{
      char phone[11];
      strcpy(phone,phonenumber); 
      strcat(phone,";");
      putchar1(0x41);//A
      putchar1(0x54);//T 
      putchar1(0x44);//D 
      send2Sim900(phone);
      putchar1(0x0d); //<CR> 
}
//chuong trinh gui tin nhan SMS
void sendmessageSim900(unsigned char *phonenumber,unsigned char *text)
{  
     #asm("cli")// cam ngat toan bo
     
     send2Sim900("AT+CMGS=");
     putchar1(0x22);//"
     send2Sim900(phonenumber);
     putchar1(0x22);//"
     putchar1(0x0d);//CR 
     delay_ms(110);
     send2Sim900(text);  
     putchar1(0x1a);//ma chot 
     putchar1(0x1b);//ma ket thuc gui tin
     
     delay_ms(3000);//doi cho het ngat tra ve 
     #asm("sei")// cho phep ngat toan bo     
}
//
void gettime(void)
{
//get time
unsigned int k;  
for(k=27;k<=43;++k)
time[k-27]=msg[k];
//index=0; 
}
//
void getcommand(void)
{
//get command
unsigned int k;     
for(k=83;k<=(strlen(msg)-3);++k)
{command[k-83]=msg[k];} 
//index=0;              
}
//
void getnumber(void)
{
//get number
unsigned int k;    
for(k=9;msg[k]!=34;++k)
callnumber[k-9]=msg[k];
//index=0;               
}
/////
//===================================================
 unsigned char bydata; // counter variable
 unsigned char sData[8]; // outgoing buffer from MASTER to SLAVES

signed int CRC16( int iLeng) 
{    
     signed int CRCLo,CRCHi; 
    signed int CL,CH; 
     signed int SHi,SLo; 
    int i,Flag; 
     
    CRCLo = 0xff; 
    CRCHi = 0xff; 
    CL = 0x01; CH = 0xa0; 
    for(i=0; i<iLeng; i++) 
    {  CRCLo = CRCLo ^ sData[i]; 
        for( Flag=0; Flag<=7; Flag++) 
        { 
            SHi = CRCHi; 
            SLo = CRCLo; 
            CRCHi = CRCHi>>1; 
            CRCLo = CRCLo>>1; 
            if((SHi&0x01) == 0x01) 
            CRCLo = CRCLo|0x80; 
            if((SLo&0x01) == 0x01) 
            { 
                CRCHi = CRCHi ^ CH; 
                CRCLo = CRCLo ^ CL; 
                } } } 
    return(CRCHi<<8 | CRCLo); 
}

void tx_485()
{  
  signed int  crcData;
       
       sData[0] = 0x03; // SLAVE  address 
       sData[1] = 0x04; //ma ham
       sData[2] = 0x00; // 
       sData[3] = 0x01; // 2 byte dia chi
       sData[4] = 0x00; //     
       sData[5] = 0x3b; // 2 byte so thanh ghi can doc het l� 0x3b 
                
      crcData = CRC16(6); 
      sData[6] = crcData & 0xff;        
      sData[7] = crcData >> 8; 
  
      PORTE.2=1; // enable RS-485 tx  
      for (bydata = 0; bydata < 8 ; bydata++)
        {
        putchar(sData[bydata]); // Send slaves 
        delay_ms(1); 
        }
      PORTE.2=0;
      delay_ms(600);//impotant
      PORTE.2=1;
 }
//======================================================
//chuong trinh chuyen so lieu
void twobyte2real(unsigned char byte1,unsigned char byte2)
{  
unsigned char sign;

if (byte1>127)
    {
    sign=1;
    byte1=byte1-128;}
    
byte1=2*byte1;
        
if (byte2>127)
    {byte1=byte1+1;
    byte2=byte2-128;
    }

byte2=byte2+128;
realnum=ldexp(byte2*1./127.7,byte1-127);//returns x * 2^expn.
if(sign==1)
    {
    realnum = (-1.0)*realnum;
    sign=0;
    }     
}
//=======================================================
void cov2real(void)
{
//dien ap 
twobyte2real(msg0[3],msg0[4]);
ftoa(realnum,1,U1); 

twobyte2real(msg0[7],msg0[8]);
ftoa(realnum,1,U2); 

twobyte2real(msg0[11],msg0[12]);
ftoa(realnum,1,U3); 
                                                                
//dong dien                                                                
twobyte2real(msg0[35],msg0[36]);
ftoa(realnum,3,I1); 
                                
twobyte2real(msg0[39],msg0[40]);
ftoa(realnum,3,I2); 
                                
twobyte2real(msg0[43],msg0[44]);
ftoa(realnum,3,I3); 
                                
//cong suat
twobyte2real(msg0[51],msg0[52]);
ftoa(realnum*10,3,P1); 
                                
twobyte2real(msg0[55],msg0[56]);
ftoa(realnum*10,3,P2); 

twobyte2real(msg0[59],msg0[60]);
ftoa(realnum*10,3,P3); 

//nang luong                               
twobyte2real(msg0[119],msg0[120]);
ftoa(realnum*10+10006.0,2,totalenergy); 
                                
strcpy(status,"thong so"); 
strcat(status,"/U1=");  
strncat(status,U1,5);
strcat(status,"V");
                                
strcat(status,"/U2=");                         
strncat(status,U2,5);
strcat(status,"V");
                                
strcat(status,"/U3=");
strncat(status,U3,5);
strcat(status,"V");
                                
strcat(status,"/I1=");
strncat(status,I1,5);
strcat(status,"A");
                                
strcat(status,"/I2=");                         
strncat(status,I2,5);
strcat(status,"A");
                                
strcat(status,"/I3=");
strncat(status,I3,5);
strcat(status,"A");
                                
strcat(status,"/P1=");
strncat(status,P1,5);
strcat(status,"kW");
                                
strcat(status,"/P2=");                         
strncat(status,P2,5);
strcat(status,"kW");
strcat(status,"/P3=");
strncat(status,P3,5);
strcat(status,"kW");
                                
strcat(status,"/Energy=");
strncat(status,totalenergy,8);                                                
strcat(status,"kWh");              
}
//=======================================================
void readMFM383(void)
{
tx_485();
if(intFlag0==1 & msg0[0]==3 & msg0[1]==4 & msg0[2]==118)// neu cong to tra ve chinh xac 
{
    cov2real();                                                                                                                                                                                                                                               
    sendmessageSim900(rcallnumber,status);
}
else
    sendmessageSim900(callnumber,"loi cong to");  
}
//=======================================================
void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x03;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Port E initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0xff;

// Port F initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTF=0x00;
DDRF=0x00;

// Port G initialization
// Func4=In Func3=In Func2=In Func1=In Func0=In 
// State4=T State3=T State2=T State1=T State0=T 
PORTG=0x00;
DDRG=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
ASSR=0x00;
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// OC1C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: Timer3 Stopped
// Mode: Normal top=0xFFFF
// OC3A output: Discon.
// OC3B output: Discon.
// OC3C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=0x00;
TCCR3B=0x00;
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;
OCR3CH=0x00;
OCR3CL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: Off
// INT5: Off
// INT6: Off
// INT7: Off
EICRA=0x00;
EICRB=0x00;
EIMSK=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

ETIMSK=0x00;

// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud Rate: 9600
UCSR0A=0x00;
UCSR0B=0x98;
UCSR0C=0x06;
UBRR0H=0x00;
UBRR0L=0x33;

// USART1 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART1 Receiver: On
// USART1 Transmitter: On
// USART1 Mode: Asynchronous
// USART1 Baud Rate: 9600
UCSR1A=0x00;
UCSR1B=0x98;
UCSR1C=0x06;
UBRR1H=0x00;
UBRR1L=0x33;
// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// Global enable interrupts
delay_ms(500);
resetSim900=0; //kh?i dong sim900
delay_ms(5000);
resetSim900=1;
delay_ms(5000);
#asm("sei")// cho phep ngat toan bo
//init sim900
initSim900();
intFlag=0;
intFlag0=0;
j=0;
j0=0;
xoachuoi();//xoa 
while (1)
      {    
       // Place your code here
       ++autotime; 
       if (autotime>40)//thoi gian giua 2 lan truyen
        {autotime=0;}
        
       if(autosend ==1 & autotime==40)
        {
                tx_485();
                if(intFlag0==1 & msg0[0]==3 & msg0[1]==4 & msg0[2]==118)// neu cong to tra ve chinh xac 
                    {
                    cov2real();                                                                                                                                                                                                                                               
                    sendmessageSim900(rcallnumber,status);
                    }
                else
                    {sendmessageSim900(rcallnumber,"can't access MFM383");}    
       }  
           
      if(intFlag==1)//co tin hieu bao qua Sim
      { 
        delay_ms(1000);
        if(msg[10]=='8')// can tach lenh va loai cac ngat khong phai do message
            {   
                getnumber();
                getcommand(); 
                //tim ma lenh
                if(strcmp(command,"00")==0)
                    {malenh=0;}
                if(strcmp(command,"01")==0)
                    {malenh=1;}
                if(strcmp(command,"10")==0)
                    {malenh=2;}
                if(strcmp(command,"11")==0)
                    {malenh=3;}
                if(strcmp(command,"02")==0)
                    {malenh=4;}
                if(strcmp(command,"03")==0)
                    {malenh=5;} 
                if(strcmp(command,"C3")==0)
                    {malenh=6;}                      
                delay_ms(1000); //can du thoi gian                                                             
                switch  (malenh)  
                        {
                    case 0:
                        role1=0;
                        delay_ms(10);
                        role2=0;
                        sendmessageSim900(callnumber,"done");
                        break;                        
                    case 1: 
                        role1=0;
                        delay_ms(10);
                        role2=1;                    
                        sendmessageSim900(callnumber,"done");
                        break;
                    case 2:
                        role1=1;
                        delay_ms(10);
                        role2=0;                      
                        sendmessageSim900(callnumber,"done");
                        break;
                    case 3: 
                        role1=1;
                        delay_ms(100);
                        role2=1;
                        delay_ms(100);                      
                        sendmessageSim900(callnumber,"done");
                        break;
                    case 4:
                        tx_485();  
                        if(intFlag0==1 & msg0[0]==3 & msg0[1]==4 & msg0[2]==118)// neu cong to tra ve chinh xac
                            {cov2real();                                                                                                                                                                                                                                               
                            sendmessageSim900(callnumber,status);
                            }
                        else
                            sendmessageSim900(callnumber,"can't access MFM383");    
                        break;
                    case 5: 
                        autosend=1;
                        strcpy(rcallnumber,callnumber);                      
                        sendmessageSim900(callnumber,"into auto mode"); 
                        break;
                    case 6: 
                        autosend=0;                   
                        sendmessageSim900(callnumber,"quit auto mode");
                        break;                            
                    default:
                        sendmessageSim900(callnumber,"Command wrong");
                        break;
                        }
                delay_ms(1000);      
                clear_msg();
            } 
                 
                j=0; 
                j0=0;
                intFlag=0;
                intFlag0=0;
                xoachuoi();//xoa
                malenh=99;       
      }  
      delay_ms(1000);
   }     
}
