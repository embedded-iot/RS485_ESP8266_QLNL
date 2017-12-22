// Config Serial : https://nvsl.github.io/PiDuino_Library/function/2000/02/02/Serial-begin-config.html

#define SS 12
#define TX HIGH
#define RX LOW 
#define RS485 Serial

#define DEBUGGING

unsigned char I1[8],I2[8],I3[8],U1[8],U2[8],U3[8],P1[8],P2[8],P3[8],totalenergy[11];
unsigned char result[200], bufferRS[200]; 
float realnum;

void setup() {
  // Open Serial port with:
  // baud rate: 9600
  // Data size: 8 bits
  // Parity: None
  // Stop Bits: 1 bit
  //Serial.begin(9600, SERIAL_8N1);
  Serial.begin(9600);

  GPIO();
  
  readMFM383();
}

void loop() {
  
}

void Send() {
  int d = 0;
  digitalWrite(SS,TX);
  delay(50);
  Serial.println(String(d++));
  delay(50);
  digitalWrite(SS,RX);
  delay(1000);
}
void GPIO() {
  pinMode(SS, OUTPUT);
}
void show(String str) {
  #ifdef DEBUGGING 
    digitalWrite(SS,RX); // Disabled send RS485 
    Serial.println(str); // Send to Serial
  #endif
}
byte sData[8];
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
  sendArrayToRS485(sData, 8);
}
void sendArrayToRS485(byte sdata[], int len) {
  digitalWrite(SS,TX);
  delay(50);
  for (int i = 0; i< len; i++) {
    RS485.print(sdata[i]);
  }
  digitalWrite(SS,RX);
}
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
  {  
    CRCLo = CRCLo ^ sData[i];
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
      } 
    }
  }
  return(CRCHi<<8 | CRCLo);
}
int rx_485(long timeOut) {
  int len = 0;
  digitalWrite(SS,RX);
  while (timeOut-- > 0){
    if (RS485.available() > 0){
      bufferRS[len++] =  RS485.read();
    }
  }
  return len; 
}

float twobyte2real(unsigned char byte1,unsigned char byte2) {
  unsigned char sign;
  if (byte1>127)
  {
    sign=1;
    byte1 = byte1-128;
  }
  
  byte1=2*byte1;
  
  if (byte2>127)
  {
    byte1=byte1+1;
    byte2=byte2-128;
  }
  
  byte2= byte2 + 128;
  realnum=ldexp(byte2*1./127.7,byte1-127);//returns x * 2^expn.
  if(sign==1)
  {
    realnum = (-1.0)*realnum;
    sign=0;
  }
  return realnum;
}

void cov2real(void) {
  //dien ap
  twobyte2real(bufferRS[3],bufferRS[4]);
  //ftoa(realnum,1,U1);
  
  twobyte2real(bufferRS[7],bufferRS[8]);
  //ftoa(realnum,1,U2);
  
  twobyte2real(bufferRS[11],bufferRS[12]);
  //ftoa(realnum,1,U3);
  
  //dong dien
  twobyte2real(bufferRS[35],bufferRS[36]);
  //ftoa(realnum,3,I1);
  
  twobyte2real(bufferRS[39],bufferRS[40]);
  //ftoa(realnum,3,I2);
  
  twobyte2real(bufferRS[43],bufferRS[44]);
  //ftoa(realnum,3,I3);
  
  //cong suat
  twobyte2real(bufferRS[51],bufferRS[52]);
  //ftoa(realnum*10,3,P1);
  
  twobyte2real(bufferRS[55],bufferRS[56]);
  //ftoa(realnum*10,3,P2);
  
  twobyte2real(bufferRS[59],bufferRS[60]);
  //ftoa(realnum*10,3,P3);
  
  //nang luong
  twobyte2real(bufferRS[119],bufferRS[120]);
  //ftoa(realnum*10+10006.0,2,totalenergy);
  
//  strcpy(result,"thong so");
//  strcat(result,"/U1=");
//  strncat(result,U1,5);
//  strcat(result,"V");
//  
//  strcat(result,"/U2=");
//  strncat(result,U2,5);
//  strcat(result,"V");
//  
//  strcat(result,"/U3=");
//  strncat(result,U3,5);
//  strcat(result,"V");
//  
//  strcat(result,"/I1=");
//  strncat(result,I1,5);
//  strcat(result,"A");
//  
//  strcat(result,"/I2=");
//  strncat(result,I2,5);
//  strcat(result,"A");
//  
//  strcat(result,"/I3=");
//  strncat(result,I3,5);
//  strcat(result,"A");
//  
//  strcat(result,"/P1=");
//  strncat(result,P1,5);
//  strcat(result,"kW");
//  
//  strcat(result,"/P2=");
//  strncat(result,P2,5);
//  strcat(result,"kW");
//  strcat(result,"/P3=");
//  strncat(result,P3,5);
//  strcat(result,"kW");
//  
//  strcat(result,"/Energy=");
//  strncat(result,totalenergy,8);
//  strcat(result,"kWh");
}

void readMFM383() {
  tx_485();
  int len = rx_485(2000);
  if(len > 0 && bufferRS[0]== 3 & bufferRS[1] == 4 & bufferRS[2] == 118)// neu cong to tra ve chinh xac 
  {
     cov2real();           
  }
}

