#include "OV2640Setting.h"
#include "OV2640MyRegs.h" 
#include <Arduino.h>
#include <Wire.h>

unsigned char write_reg8x8(unsigned char regID, unsigned char regDat)
{
  Wire.beginTransmission(OV2640_ADDRESS);
  Wire.write(regID);
  Wire.write(regDat);
  Wire.endTransmission(); //结束通信
  return (1);
}
int wrSensorRegs8_8(const struct sensor_reg reglist[])
{

    int err = 0;
    uint16_t reg_addr = 0;
    uint16_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    while ((reg_addr != 0xff) | (reg_val != 0xff))
    {
      reg_addr = pgm_read_word(&next->reg);
      reg_val = pgm_read_word(&next->val);
      err = write_reg8x8(reg_addr, reg_val);
      next++;
    }
  return 1;
}

void OV2640_INIT_JPEG()
{
  write_reg8x8(0xff,0x01);
  write_reg8x8(0x12,0x80); //All Resgister Reset to factory dafault values

 
  
  wrSensorRegs8_8(OV2640_INIT);
  delay(10);
}
// void StartTakeAPicture()
// {
   // attachInterrupt(digitalPinToInterrupt(PCLK), PCLK_inter, RISING);	
// }




void IO_Init(void)
{
  //*****pinMode(A0~A7)
  DDRA &= ~(1 << PA0);
  PORTA |= (1 << PA0);
  
  DDRA &= ~(1 << PA1);
  PORTA |= (1 << PA1);
  
  DDRA &= ~(1 << PA2);
  PORTA |= (1 << PA2);
  
  DDRA &= ~(1 << PA3);
  PORTA |= (1 << PA3);
  
  DDRA &= ~(1 << PA4);
  PORTA |= (1 << PA4);
  
  DDRA &= ~(1 << PA5);
  PORTA |= (1 << PA5);
  
  DDRA &= ~(1 << PA6);
  PORTA |= (1 << PA6);
  
  DDRA &= ~(1 << PA7);
  PORTA |= (1 << PA7);
  //***********
  
 
  pinMode(PCLK, INPUT_PULLUP);

  DDRC &= ~(1 << PC3);//VSYNC
  PORTC |= (1 << PC3); //INPUT_PUULUP

  DDRC &= ~(1 << PC5);//HREF
  PORTC |= (1 << PC5); //INPUT_PUULUP

}

void OV2640_Auto_Exposure(unsigned char  level)
{  
  switch(level)
  {
    case 0:
      wrSensorRegs8_8(OV2640_AUTOEXPOSURE_LEVEL0);
      break;
    case 1:
      wrSensorRegs8_8(OV2640_AUTOEXPOSURE_LEVEL1);
      break;
    case 2:
      wrSensorRegs8_8(OV2640_AUTOEXPOSURE_LEVEL2);
      break; 
    case 3:
      wrSensorRegs8_8(OV2640_AUTOEXPOSURE_LEVEL3);
      break;   
    case 4:
      wrSensorRegs8_8(OV2640_AUTOEXPOSURE_LEVEL4);
      break;
  }
}

void OV2640_Light_Mode(unsigned char  mode)
{
  unsigned char  regccval=0X5E;//Sunny 
  unsigned char  regcdval=0X41;
  unsigned char  regceval=0X54;
  switch(mode)
  { 
    case 0://auto 
      write_reg8x8(0XFF,0X00);  
      write_reg8x8(0XC7,0X10);//AWB ON 
      return;   
    case 2://cloudy
      regccval=0X65;
      regcdval=0X41;
      regceval=0X4F;
      break;  
    case 3://office
      regccval=0X52;
      regcdval=0X41;
      regceval=0X66;
      break;  
    case 4://home
      regccval=0X42;
      regcdval=0X3F;
      regceval=0X71;
      break;  
  }
  write_reg8x8(0XFF,0X00);  
  write_reg8x8(0XC7,0X40); //AWB OFF 
  write_reg8x8(0XCC,regccval); 
  write_reg8x8(0XCD,regcdval); 
  write_reg8x8(0XCE,regceval);  
}

void OV2640_Brightness(unsigned char  bright)
{
//  write_reg8x8(0xff, 0x00);
//  write_reg8x8(0x7c, 0x00);
//  write_reg8x8(0x7d, 0x04);
//  write_reg8x8(0x7c, 0x09);
//  write_reg8x8(0x7d, bright<<4); 
//  write_reg8x8(0x7d, 0x00); 
write_reg8x8(0xff, 0x00);
write_reg8x8(0x7c, 0x00);
write_reg8x8(0x7d, 0x00);
}

void OV2640_Color_Saturation(unsigned char  sat)
{ 
  unsigned char  reg7dval=((sat+2)<<4)|0X08;
  write_reg8x8(0XFF,0X00);   
  write_reg8x8(0X7C,0X00);   
  write_reg8x8(0X7D,0X02);       
  write_reg8x8(0X7C,0X03);     
  write_reg8x8(0X7D,reg7dval);     
  write_reg8x8(0X7D,reg7dval);     
}


unsigned char ov2640_data(void)
{
  unsigned char tmp=0;
  unsigned char DATA7 = 0;
  unsigned char DATA6 = 0;
  unsigned char DATA5 = 0;
  unsigned char DATA4 = 0;
  unsigned char DATA3 = 0;
  unsigned char DATA2 = 0;
  unsigned char DATA1 = 0;
  unsigned char DATA0 = 0;

  DATA7 = (PINA & _BV(0));//00000001 高位元
  DATA6 = (PINA & _BV(1));//00000010
  DATA5 = (PINA & _BV(2));//00000100
  DATA4 = (PINA & _BV(3));//00001000
  DATA3 = (PINA & _BV(4));//00010000
  DATA2 = (PINA & _BV(5));//00100000
  DATA1 = (PINA & _BV(6));//01000000
  DATA0 = (PINA & _BV(7));//10000000
  tmp = DATA7 | DATA6| DATA5|DATA4|DATA3|DATA2|DATA1|DATA0;//開發版OV接腳
// tmp = (DATA7 << 7) | (DATA6 << 5) | (DATA5 << 3) | (DATA4 << 1)
//        | (DATA3 >> 1) | (DATA2 >> 3) | (DATA1 >> 5) | (DATA0 >> 7);//獨立OV接腳

  return tmp;
}
