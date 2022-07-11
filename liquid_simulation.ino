#include<Wire.h>
#include <math.h>

#define ROW_1 9
#define ROW_2 12
#define ROW_3 7
#define ROW_4 13
#define ROW_5 2
#define ROW_6 6
#define ROW_7 A2  
#define ROW_8 5

#define COL_1 A3
#define COL_2 11
#define COL_3 10
#define COL_4 4
#define COL_5 8
#define COL_6 3
#define COL_7 A1
#define COL_8 A0

const byte rows[] = {
    ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};
const byte col[] = {
  COL_1,COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

const int MPU=0x68; 
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(9600);

  for (byte i = 2; i <= 13; i++)
      pinMode(i, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
}
void loop(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);  
  AcX=Wire.read()<<8|Wire.read();    
  AcY=Wire.read()<<8|Wire.read();  

  simulateLiquid(AcX, AcY);
//  testfx();
}

void simulateLiquid(int16_t x, int16_t y)
{
  byte matrix[] =
  {
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111
  };
  byte m[8];

  if(y>16000)
      y=16000;
    if(y<-16000)
      y=-16000;

    if(x>16000)
      x=16000;
    if(x<-16000)
      x=-16000;
      
  if(y>0&&y<16000 && x>0&&x<16000)          //  1st quadrant (0-90 degree)
  {   
    matrix[0] = matrix[0] << bitShiftLayer0(x);   // 8-8-5
    matrix[1] = matrix[1] << bitShiftLayer1(x);   // 8-8-5
    matrix[2] = matrix[2] << bitShiftLayer2(x);   // 8-7-5
    matrix[3] = matrix[3] << bitShiftLayer3(x);   // 8-6-5 
    matrix[4] = matrix[4] << bitShiftLayer4(x);   // 8-5-5
    matrix[5] = matrix[5] << bitShiftLayer5(x);   // 0-4-5
    matrix[6] = matrix[6] << bitShiftLayer6(x);   // 0-3-5
    matrix[7] = matrix[7] << bitShiftLayer7(x);   // 0-2-5
    for(byte i=0;i<8;i++)
    {
       m[i]=matrix[i];
    }
  }else if(y>-16000&&y<0 && x>0&&x<16000)   //  2nd quadrant (91-180 degree)
  {
    matrix[7] = matrix[7] << bitShiftLayer0(x);   // 8-8-5
    matrix[6] = matrix[6] << bitShiftLayer1(x);   // 8-8-5
    matrix[5] = matrix[5] << bitShiftLayer2(x);   // 8-7-5
    matrix[4] = matrix[4] << bitShiftLayer3(x);   // 8-6-5 
    matrix[3] = matrix[3] << bitShiftLayer4(x);   // 8-5-5
    matrix[2] = matrix[2] << bitShiftLayer5(x);   // 0-4-5
    matrix[1] = matrix[1] << bitShiftLayer6(x);   // 0-3-5
    matrix[0] = matrix[0] << bitShiftLayer7(x);   // 0-2-5
    for(byte i=0;i<8;i++)
    {
       m[i]=matrix[i];
    }
  }else if(y>-16000&&y<0 && x>-16000&&x<0)  //  3rd quadrant (180-270 degree)
  {
    matrix[7] = matrix[7] >> bitShiftLayer0(-x);   // 8-8-5
    matrix[6] = matrix[6] >> bitShiftLayer1(-x);   // 8-8-5
    matrix[5] = matrix[5] >> bitShiftLayer2(-x);   // 8-7-5
    matrix[4] = matrix[4] >> bitShiftLayer3(-x);   // 8-6-5 
    matrix[3] = matrix[3] >> bitShiftLayer4(-x);   // 8-5-5
    matrix[2] = matrix[2] >> bitShiftLayer5(-x);   // 0-4-5
    matrix[1] = matrix[1] >> bitShiftLayer6(-x);   // 0-3-5
    matrix[0] = matrix[0] >> bitShiftLayer7(-x);   // 0-2-5
    for(byte i=0;i<8;i++)
    {
       m[i]=matrix[i];
    }
  }else if(y>0&&y<16000 && x>-16000&&x<0)   //  4th quadrant (270-359 degree)
  { 
    matrix[0] = matrix[0] >> bitShiftLayer0(-x);   // 8-8-5
    matrix[1] = matrix[1] >> bitShiftLayer1(-x);   // 8-8-5
    matrix[2] = matrix[2] >> bitShiftLayer2(-x);   // 8-7-5
    matrix[3] = matrix[3] >> bitShiftLayer3(-x);   // 8-6-5 
    matrix[4] = matrix[4] >> bitShiftLayer4(-x);   // 8-5-5
    matrix[5] = matrix[5] >> bitShiftLayer5(-x);   // 0-4-5
    matrix[6] = matrix[6] >> bitShiftLayer6(-x);   // 0-3-5
    matrix[7] = matrix[7] >> bitShiftLayer7(-x);   // 0-2-5
    for(byte i=0;i<8;i++)
    {
       m[i]=matrix[i];
    }
       
  }
  int counter=0;
  while(counter<5)
  {
    printOnMatrix(m);
    counter++;
  }
  
  
}

void printOnMatrix(byte b[])
{
  for(int i=0; i<8; i++)
  {
    digitalWrite(rows[i], HIGH);
    for(int j=0; j<8; j++)
    {
      digitalWrite(col[j], (~b[i] >> j) & 0x01);
      digitalWrite(col[j], 1);
    }
    digitalWrite(rows[i], LOW);
  }
}

// 8-8-5 top layer
int bitShiftLayer0(int x)
{
  return round(-exp(x/1740.0-8.1)+8);
}

//  8-8-5 2nd layer
int bitShiftLayer1(int x)
{
  return round(-exp(x/2670.0-4.9)+8);
}

// 8-7-5
int bitShiftLayer2(int x)
{
  return round(-exp(x/11600.0)+9);
}

//  8-6-5
int bitShiftLayer3(int x)
{
  return round(exp(-x/11500.0+1.38)+8-exp(1.38));
}

//  8-5-5
int bitShiftLayer4(int x)
{
  return round(exp(-x/2800.0+1.1)+8-exp(1.1));
}

//  0-4-5
int bitShiftLayer5(int x)
{
  return round(-exp(-x/5600.0+1.66)+exp(1.66));
}

//  0-3-5
int bitShiftLayer6(int x)
{
  return round(-exp(-x/19700.0+2.2)+exp(2.2));
}

//  0-2-5
int bitShiftLayer7(int x)
{
  return round(exp(x/18200.0+1.27)-exp(1.27));
}
