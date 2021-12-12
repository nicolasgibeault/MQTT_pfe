#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <USB_TCPM.h>
#include <FUSB302.h>

#define	 FUSB302_D_Register_DeviceID 	0x01
#define	 FUSB302_D_Register_Switches0 	0x02
#define	 FUSB302_D_Register_Switches1 	0x03
#define	 FUSB302_D_Register_Measure 	0x04
#define	 FUSB302_D_Register_Slice 		0x05
#define	 FUSB302_D_Register_Control0 	0x06
#define	 FUSB302_D_Register_Control1 	0x07
#define	 FUSB302_D_Register_Control2 	0x08
#define	 FUSB302_D_Register_Control3 	0x09
#define	 FUSB302_D_Register_Mask1	 	0x0A
#define	 FUSB302_D_Register_Power	 	0x0B
#define	 FUSB302_D_Register_Reset	 	0x0C
#define	 FUSB302_D_Register_OCPreg	 	0x0D
#define	 FUSB302_D_Register_Maska	 	0x0E
#define	 FUSB302_D_Register_Maskb	 	0x0F
#define	 FUSB302_D_Register_Control4	0x10
#define	 FUSB302_D_Register_Status0a	0x3C
#define	 FUSB302_D_Register_Status1a	0x3D
#define	 FUSB302_D_Register_Interrupta	0x3E
#define	 FUSB302_D_Register_Interruptb	0x3F
#define	 FUSB302_D_Register_Status0	 	0x40
#define	 FUSB302_D_Register_Status1	 	0x41
#define	 FUSB302_D_Register_Interrupt 	0x42
#define	 FUSB302_D_Register_FIFOs	 	0x43

 typedef struct {
	 uint8_t DeviceID;
	 uint8_t Switches0;
	 uint8_t Switches1;
	 uint8_t Measure;
	 uint8_t Slice;
	 uint8_t Control0;
	 uint8_t Control1;
	 uint8_t Control2;
	 uint8_t Control3;
	 uint8_t Mask1;
	 uint8_t Power;
	 uint8_t Reset;
	 uint8_t OCPreg;
	 uint8_t Maska;
	 uint8_t Maskb;
	 uint8_t Control4;
	 uint8_t Status0a;
	 uint8_t Status1a;
	 uint8_t Interrupta;
	 uint8_t Interruptb;
	 uint8_t Status0;
	 uint8_t Status1;
	 uint8_t Interrupt;
	 uint8_t FIFOs;
 } FUSB302_D_Registers_st;
