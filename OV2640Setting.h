#ifndef OV2640Setting_H
#define OV2640Setting_H

#define OV2640_ADDRESS    0x30
#define VSYNC      9 
#define HREF       11 
#define PCLK       21 

unsigned char write_reg8x8(unsigned char regID, unsigned char regDat);
int wrSensorRegs8_8(const struct sensor_reg reglist[]);

void IO_Init(void);
void OV2640_INIT_JPEG();

void OV2640_Auto_Exposure(unsigned char level);
void OV2640_Light_Mode(unsigned char mode);
void OV2640_Brightness(unsigned char bright);
void OV2640_Color_Saturation(unsigned char sat);

unsigned char ov2640_data(void);




#endif
