#include <SPI.h>
#include <Ethernet.h>
#define MaxLoddingTime 5000000U


#include <Wire.h>
#include "OV2640Setting.h"
#define BufferLength 10240U
#define BAD_FRAME_RESET_LIMIT 5
#define ReceiveFrameTimeout 10000U


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168,31,225);

IPAddress server(192,168,31,224);  

EthernetClient client;

unsigned long beginMicros;
unsigned long beginPolling;
bool ConnectTimeOut=false;
int count=0;
unsigned char pixelBuffer[BufferLength];
int HREF_status;
int VSYNC_status=0;
int old_VSYNC_status=0;
bool EndFrame=false;
int ENDCOUNT=0;
bool SucceedGetJPEG=false;
int FailFrameCnt;
String CT="";

void setup() {
pinMode(19,INPUT);
  Ethernet.init(12); 
  Wire.begin();
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Ethernet.begin(mac, ip);
  Serial.print(F("MyIP:"));
  Serial.println(Ethernet.localIP());
  
  IO_Init();
  OV2640_INIT_JPEG();

  delay(1000);
  


}

void loop() {
 analogWrite(19,25);
   beginMicros = micros();
   int TimeOutCount=0;
   while(ConnectTimeOut == false) //5 second TimeOut
   {
		if (client.connect(server, 51376)) 
		{
			Serial.print("connected to ");
			Serial.println(client.remoteIP());   
			ConnectTimeOut= true;
		} 
		else 
		{
		    // if you didn't get a connection to the server:
			if(TimeOutCount % 333 ==0)
			Serial.print(F("."));
			TimeOutCount++;
		}
		
		if(micros()-beginMicros > MaxLoddingTime)
		{
			Serial.println("");
			Serial.println(F("TimeOut"));  
			ConnectTimeOut=true;
		}
   }
   
  

  if(client.available())
  {
	  while(client.available() !=0)
	  {
		char c=client.read();
		CT += c;
	  }

	  //Serial.print(CT);
	  char * tab2 = new char [CT.length()+1];
	  strcpy (tab2, CT.c_str());
	  CT="";
	  
	  if(strstr(tab2,"ButtonOnClick") != 0)
		  PostMethod();
	  free(tab2);
	  
  }
   
   if(client.connected())
   {
	   //when polling every 1 second
	   beginPolling = millis();
	   while(1)
	   {
		   if(millis()-beginPolling >1000 )
		   {
			   GetMethod();
			   break;
		   }
	   }
	     
   }
   
 
  
  
 // if the server's disconnected, stop the client:
  if (!client.connected())
  {
    client.stop();
	ConnectTimeOut=false;
    delay(10);
  }
}
void GetMethod()
{
    Serial.println(F("Get Medthod"));
    
	String h1=String("GET /W5100/W5100IsPolling HTTP/1.1\r\nHost: 192.168.31.224\r\nConnection: keep-alive\r\n\r\n");
    client.print(h1);
 

}

void PostMethod()
{
   Serial.println(F("Post Medthod"));
   CaptureJPEG();
    
	String P1 = String("POST /W5100/ReceiveData HTTP/1.1\r\nHost: 192.168.31.224\r\nContent-Tpye: image/jpeg\r\nConnection: close\r\n");
	String STR_ContentLen = String("Content-Length: ");
	int CONTENT_LEN=ENDCOUNT*2; //which each in buffer has 2 char so ENDCOUNT*2
	String Space =String("\r\n");
	
	client.print(P1+STR_ContentLen+CONTENT_LEN+Space+Space);

	
	for(int k=0;k<ENDCOUNT;k++)
		client.print(tohex(pixelBuffer[k]));



}


void PCLK_inter(void)
{ 
   //**GetPictureWithOV2640
   VSYNC_status=(PINC & (1 << PC3));
   if(VSYNC_status)
   {
	HREF_status=(PINC & (1 << PC5));
    if(HREF_status)
    { 
    pixelBuffer[count]=ov2640_data();
    count++;
    }
   } 
   //******
   
   //**Have One Frame or the Frame bigger than Buffer Tag a BadFrame to it
   if(old_VSYNC_status!=0 && VSYNC_status ==0)
    EndFrame=true;

   if(count>=BufferLength)
	   EndFrame=true;
	//******  
 
   bool IsGoodFrame =false;
   if(EndFrame==true)
   {
	  ENDCOUNT=count;
	  if (ENDCOUNT %1024 == 0 && pixelBuffer[0] == 0xFF && pixelBuffer[1] == 0xD8)
	  {
			unsigned int CheckLength = 1536;
			while (--count && --CheckLength)
			{
				if (pixelBuffer[count] == 0xD9)
					IsGoodFrame=true;
			}
	  }
	  //The Frame is a GoodFrame check (OV2640 JPEG size is 1024 multiple of number)
      if(IsGoodFrame ==true )
	  {		  
		  Serial.print(F("GoodPictureSize:"));
		  Serial.println(ENDCOUNT);
		  SucceedGetJPEG=true;
		  count=0;
      }
	  else //BadFrame
	  {
		Serial.print(F("BadPictureSize:"));
		Serial.println(ENDCOUNT);
		count=0;
		FailFrameCnt++;
	  }	  
	  EndFrame=false;
    }   
      old_VSYNC_status=(PINC & (1 << PC3));
}


void OnOffInterrupt(bool val) 
{
	if(val)
		  attachInterrupt(digitalPinToInterrupt(PCLK), PCLK_inter, RISING);
	else
		  detachInterrupt(digitalPinToInterrupt(PCLK));
}
void CaptureJPEG()
{
	bool EndCaptureAction=false;
	unsigned long LastTimeReceiveFrame =millis();
	FailFrameCnt=0;
	while(EndCaptureAction==false)
	{
		OnOffInterrupt(true);
		if(SucceedGetJPEG==true)
		{
			OnOffInterrupt(false);
			SucceedGetJPEG=false;
			Serial.println(ENDCOUNT);
						
			
			
			EndCaptureAction=true;
		}
		else
		{
			if(FailFrameCnt >= BAD_FRAME_RESET_LIMIT || millis() - LastTimeReceiveFrame >= ReceiveFrameTimeout)
			{
				OnOffInterrupt(false);
				Serial.println(F("Fail"));				
				FailFrameCnt=0;
				EndCaptureAction=true;
			}			
		}
	}
	
}
String tohex(int n) {
  if (n == 0) {
    return "00"; 
  }
  String result = "";
  char _16[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  const int radix = 16;
  while (n) {
    int i = n % radix;          
    result = _16[i] + result;   
    n /= radix;                 
  }
  if (result.length() < 2) {
    result = '0' + result; 
  }
  return result;
}
