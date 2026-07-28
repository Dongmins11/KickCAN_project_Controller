#include "RC522.h"
#include "cmsis_os.h"

volatile uint8_t rc522IrqFlag;
extern SPI_HandleTypeDef hspi1;
/* ---------------------------------------------------------------
 * SPI 저수준 함수
 * --------------------------------------------------------------- */
void RC522_CS_Low(void)
{
    HAL_GPIO_WritePin(PA8_D7_RFCC_GPIO_Port, PA8_D7_RFCC_Pin, GPIO_PIN_RESET);
}
 
void RC522_CS_High(void)
{
    HAL_GPIO_WritePin(PA8_D7_RFCC_GPIO_Port, PA8_D7_RFCC_Pin, GPIO_PIN_SET);
}
 
void RC522_WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t tx[2];
    tx[0] = (reg << 1) & 0x7E;   /* bit7=0: write */
    tx[1] = val;
 
    RC522_CS_Low();
    HAL_SPI_Transmit(&hspi1, tx, 2, HAL_MAX_DELAY);
    RC522_CS_High();
}
 
uint8_t RC522_ReadReg(uint8_t reg)
{
    uint8_t addr = ((reg << 1) & 0x7E) | 0x80;   /* bit7=1: read */
    uint8_t rx = 0;
 
    RC522_CS_Low();
    HAL_SPI_Transmit(&hspi1, &addr, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, &rx, 1, HAL_MAX_DELAY);
    RC522_CS_High();
 
    return rx;
}
 
void RC522_SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t cur = RC522_ReadReg(reg);
    RC522_WriteReg(reg, cur | mask);
}
 
void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t cur = RC522_ReadReg(reg);
    RC522_WriteReg(reg, cur & (~mask));
}
 
/* ---------------------------------------------------------------
 * 리셋 / 초기화
 * --------------------------------------------------------------- */
void RC522_Reset(void)
{
    HAL_GPIO_WritePin(PC7_D9_RFRST_GPIO_Port, PC7_D9_RFRST_Pin, GPIO_PIN_RESET);
    osDelay(2);
    HAL_GPIO_WritePin(PC7_D9_RFRST_GPIO_Port, PC7_D9_RFRST_Pin, GPIO_PIN_SET);
    osDelay(50);   /* 오실레이터 안정화 대기 */
 
    RC522_WriteReg(CommandReg, PCD_RESETPHASE);
    osDelay(50);
}
 
void RC522_AntennaOn(void)
{
    uint8_t temp = RC522_ReadReg(TxControlReg);
    if ((temp & 0x03) != 0x03)
    {
        RC522_SetBitMask(TxControlReg, 0x03);
    }
}
 
void RC522_Init(void)
{
    RC522_Reset();
 
    RC522_WriteReg(TModeReg, 0x8D);
    RC522_WriteReg(TPrescalerReg, 0x3E);
    RC522_WriteReg(TReloadRegL, 30);
    RC522_WriteReg(TReloadRegH, 0);
    RC522_WriteReg(TxASKReg, 0x40);
    RC522_WriteReg(ModeReg, 0x3D);
 
    /* IRQ 마스크: RxIEn(수신완료)=0x20, ErrIEn(에러)=0x02, TimerIEn(타임아웃)=0x01
     * IRqInv(bit7)=0 유지 -> IRQ 핀은 기본 오픈드레인 Active Low (EXTI Falling과 매칭) */
    RC522_WriteReg(ComIEnReg, 0x20 | 0x02 | 0x01);
 
    RC522_AntennaOn();
}


/* ---------------------------------------------------------------
 * FIFO에 커맨드 데이터를 넣고 실행 -> IRQ로 완료/타임아웃 대기
 * (CommIrqReg를 SPI로 반복 폴링하지 않고 EXTI 인터럽트로 대기)
 * --------------------------------------------------------------- */
uint8_t RC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
                             uint8_t *backData, uint16_t *backLen)
{
    uint8_t status = MI_ERR;
    uint8_t n;
    uint8_t waitIRq = 0x00;

    if (command == PCD_TRANSCEIVE)
    {
        waitIRq = 0x30;   /* RxIRq | IdleIRq */
    }

    RC522_ClearBitMask(ComIrqReg, 0x80);   /* Set1=0 -> 전체 IRQ 비트 클리어 */
    RC522_SetBitMask(FIFOLevelReg, 0x80);  /* FIFO flush */
    RC522_WriteReg(CommandReg, PCD_IDLE);

    for (uint8_t i = 0; i < sendLen; i++)
    {
        RC522_WriteReg(FIFODataReg, sendData[i]);
    }

    RC522_WriteReg(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        RC522_SetBitMask(BitFramingReg, 0x80);   /* StartSend=1 */
    }

    uint32_t startTick = HAL_GetTick();
    rc522IrqFlag = 0;
    uint8_t irqVal = 0;
    while (1)
    {
        irqVal = RC522_ReadReg(ComIrqReg);
        if (rc522IrqFlag || (irqVal & waitIRq) || (irqVal & 0x01))   /* 0x01 = TimerIRq */
        {
            break;
        }
        if ((HAL_GetTick() - startTick) > 25)   /* 25ms 타임아웃 */
        {
            break;
        }
    }

    RC522_ClearBitMask(BitFramingReg, 0x80);

    if (rc522IrqFlag || (irqVal & waitIRq))
    {
        uint8_t errorReg = RC522_ReadReg(ErrorReg);
        if ((errorReg & 0x1B) == 0x00)   /* BufferOvfl, CollErr, CRCErr, ProtocolErr 없음 */
        {
            status = MI_OK;

            if (command == PCD_TRANSCEIVE)
            {
                n = RC522_ReadReg(FIFOLevelReg);
                if (n > 16) n = 16;

                *backLen = n;
                for (uint8_t i = 0; i < n; i++)
                {
                    backData[i] = RC522_ReadReg(FIFODataReg);
                }
            }
        }
    }
    else
    {
        status = MI_TIMEOUT;
    }

    return status;
}
 
/* ---------------------------------------------------------------
 * 카드 요청 (REQA) - 통신 범위 내 카드가 있는지 확인
 * --------------------------------------------------------------- */
uint8_t RC522_Request(uint8_t reqMode, uint8_t *tagType)
{
    uint8_t status;
    uint16_t backBits;
 
    RC522_WriteReg(BitFramingReg, 0x07);   /* 마지막 바이트 중 유효 비트 수 = 7 */
 
    tagType[0] = reqMode;
    status = RC522_ToCard(PCD_TRANSCEIVE, tagType, 1, tagType, &backBits);
 
    if ((status != MI_OK) || (backBits != 2))
    {
        status = MI_ERR;
    }
 
    return status;
}
 
/* ---------------------------------------------------------------
 * 충돌방지(Anticollision) - 카드 UID(4byte) 읽기
 * --------------------------------------------------------------- */
uint8_t RC522_Anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t serNumCheck = 0;
    uint16_t unLen;
 
    RC522_WriteReg(BitFramingReg, 0x00);
 
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
 
    status = RC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
 
    if (status == MI_OK)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[4])
        {
            status = MI_ERR;
        }
    }
 
    return status;
}
 
