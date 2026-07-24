#pragma once

#include "Controller.h"

/* ---- MFRC522 레지스터 주소 ---- */
#define CommandReg      0x01
#define ComIEnReg       0x02
#define ComIrqReg       0x04
#define DivIrqReg       0x05
#define ErrorReg        0x06
#define FIFODataReg     0x09
#define FIFOLevelReg    0x0A
#define ControlReg      0x0C
#define BitFramingReg   0x0D
#define CollReg         0x0E
#define ModeReg         0x11
#define TxControlReg    0x14
#define TxASKReg        0x15
#define CRCResultRegH   0x21
#define CRCResultRegL   0x22
#define TModeReg        0x2A
#define TPrescalerReg   0x2B
#define TReloadRegH     0x2C
#define TReloadRegL     0x2D
#define VersionReg      0x37
 
/* ---- PCD(리더) 커맨드 ---- */
#define PCD_IDLE        0x00
#define PCD_TRANSCEIVE  0x0C
#define PCD_RESETPHASE  0x0F
#define PCD_CALCCRC     0x03
 
/* ---- PICC(카드) 커맨드 ---- */
#define PICC_REQIDL     0x26
#define PICC_ANTICOLL   0x93
 
/* ---- 상태 코드 ---- */
#define MI_OK           0
#define MI_ERR          1
#define MI_TIMEOUT      2



void     RC522_CS_Low(void);
void     RC522_CS_High(void);
void     RC522_WriteReg(uint8_t reg, uint8_t val);
uint8_t  RC522_ReadReg(uint8_t reg);
void     RC522_SetBitMask(uint8_t reg, uint8_t mask);
void     RC522_ClearBitMask(uint8_t reg, uint8_t mask);
 
void     RC522_Reset(void);
void     RC522_AntennaOn(void);
void     RC522_Init(void);
 
uint8_t  RC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
                              uint8_t *backData, uint16_t *backLen);
uint8_t  RC522_Request(uint8_t reqMode, uint8_t *tagType);
uint8_t  RC522_Anticoll(uint8_t *serNum);
