# HSI · 전체 노드 핀맵

[README로 돌아가기](../README.md) · [원본 Excel](files/HSI_3.xlsx)

원본 Excel의 셀 내용을 Markdown 표로 옮겼습니다. 설계 당시의 값·표기이며, 실제 Controller 구현과 다른 부분은 아래에 별도로 적었습니다. 원본의 서식과 삽입 그림은 Excel 파일에서 확인할 수 있습니다.

## main_node

| Func1 | Used Pin | Arduino | User Label |
|---|---|---|---|
| USART2_RX | PA3 | — | PA3_RX |
| USART2_TX | PA2 | — | PA2_TX |
| USART1_RX | PB15 | — | PB15_BRX |
| USART1_TX | PB14 | — | PB14_BTX |
| EXTI | PA8 | — | PA8_BSTAT |
| TIM3_CH1 (PWM) | PA6 | — | PA6_ENB |
| GPIO_Output | PA7 | — | PA7_IN4 |
| GPIO_Output | PB0 | — | PB0_IN3 |
| GPIO_Output | PB8 | — | PB8_IN2 |
| GPIO_Output | PB7 | — | PB7_IN1 |
| TIM4_CH1 (PWM) | PB6 | — | PB6_ENA |
| FDCAN_RX | PB12 | — | PB12_CANR |
| FDCAN_TX | PB13 | — | PB13_CANT |
| TIM2_CH1(IC) | PA5 | — | PA5_RTRIG |
| TIM2_CH3(IC) | PB10 | — | PB10_LTRIG |
| ADC | PA1 | — | PA1_VBT |

## node_1

| Func1 | Used Pin | Arduino | User Label |
|---|---|---|---|
| USART2_RX | PA3 | D0 | PA3_D0_RX |
| USART2_TX | PA2 | D1 | PA2_D1_TX |
| GPIO_Output | PC0 | A5 | PC0_A5_HCTr1 |
| GPIO_Output | PC1 | A4 | PC1_A4_HCTr2 |
| TIM3_CH1 (IC) | PA6 | D12 | PA6_D12_HCEc1 |
| TIM3_CH2 (IC) | PA7 | D11 | PA7_D11_HCEc2 |
| ADC_CH0 | PA0 | A0 | PA0_A0_LS1 |
| ADC_CH1 | PA1 | A1 | PA1_A1_LS2 |
| ADC_CH4 | PA4 | A2 | PA4_A2_TP |
| CAN_RX | PA11 | Morpho | PA11_CanRx |
| CAN_TX | PA12 | Morpho | PA12_CanTx |

## node_2

| Func1 | Used Pin | Arduino | User Label | 비고 (연결 대상) |
|---|---|---|---|---|
| SPI1_SCK | PA5 | D13 | D13_PA5_TFTSCK | TFT #1·#2 SCK (공유) |
| SPI1_MOSI | PA7 | D11 | D11_PA7_TFTMOS | TFT #1·#2 MOSI (공유) |
| GPIO_Output | PB6 | D10 | D10_PB6_T1CS | TFT #1 CS |
| GPIO_Output | PA8 | D7 | D7_PA8_T1DC | TFT #1 DC |
| GPIO_Output | PA9 | D8 | D8_PA9_T1RST | TFT #1 RST |
| GPIO_Output | PC7 | D9 | D9_PC7_T2CS | TFT #2 CS |
| GPIO_Output | PB10 | D6 | D6_PB10_T2DC | TFT #2 DC |
| GPIO_Output | PB5 | D4 | D4_PB5_T2RST | TFT #2 RST |
| CAN_RX | PA11 | Morpho | M_PA11_CANRX | CAN 트랜시버 RX |
| CAN_TX | PA12 | Morpho | M_PA12_CANTX | CAN 트랜시버 TX |
| I2C1_SCL | PB8 | D15 | D15_PB8_LCDSCL | CLCD SCL |
| I2C1_SDA | PB9 | D14 | D14_PB9_LCDSDA | CLCD SDA |
| GPIO_Output | PC0 | A5 | A5_PC0_LEDTL | 방향지시등 전면 좌 |
| GPIO_Output | PC1 | A4 | A4_PC1_LEDTR | 방향지시등 전면 우 |
| GPIO_Output | PA4 | A2 | A2_PA4_LEDTBL | 방향지시등 후면 좌 |
| GPIO_Output | PC4 | Morpho | M_PC4_LEDTBR | 방향지시등 후면 우 |
| TIM3_CH3 | PB0 | A3 | M_PB0_LEDHL | 헤드라이트 좌 (PWM, 서서히 밝아짐) |
| TIM3_CH4 | PB1 | Morpho | M_PB1_LEDHR | 헤드라이트 우 (PWM, 서서히 밝아짐) |
| TIM2_CH1 | PA0 | A0 | A0_PA0_BUZ1 | 부저 #1 클락션 |
| TIM4_CH2 | PB7 | Morpho | M_PB7_BUZ2 | 부저 #2 경고음 |

## controller

| Func1 | Used Pin | Arduino | User Label |
|---|---|---|---|
| USART2_RX | PA3 | D0 | PA3_D0_RX |
| USART2_TX | PA2 | D1 | PA2_PD1_TX |
| ADC1_IN0 | PA0 | A0 | PA0_A0_JOY_X |
| ADC1_IN1 | PA1 | A1 | PA1_A1_JOY_Y |
| GPIO_EXIT | PA4 | A4 | PA4_A2_KLAXON |
| USART1_RX | PA10 | D2 | PA10_D2_BRX |
| USART2_TX | PA9 | D8 | PA9_D8_BTX |
| GPIO_Output | PB10 | D6 | PB10_D6_BEN |
| GPIO_EXIT | PB5 | D4 | PB5_D4_BSTATE |
| GPIO_Input | PC0 | A5 | PC0_A5_RELAY |
| SPI1_SCK | PA5 | D13 | PA5_D13_RFSCK |
| SPI1_MISO | PA6 | D12 | PA6_D12_RFMISO |
| SPI1_MOSI | PA7 | D11 | PA7_D11_RFMOSI |
| GPIO_Output | PA8 | D7 | PA8_D7_RFCC |
| GPIO_Output | PC7 | D9 | PC7_D9_RFRST |
| GPIO_EXIT | PB6 | D10 | PB6_D10_RFIRQ |
| GPIO_EXIT | PB0 | A3 | PB0_A3_TurnL |
| GPIO_EXIT | PC1 | A4 | PC1_A4_TurnR |
| GPIO_Output | PC8 | - | PC8_LEDR |
| GPIO_Output | PC6 | - | PC6_LEDG |
| GPIO_Output | PC5 | - | PC5_LEDB |

## 현재 코드와의 차이

Controller의 PA9는 원본에 USART2_TX로 기록되어 있지만 현재 `usart.c`는 USART1_TX로 설정합니다. PC0 릴레이 핀은 원본에 GPIO_Input으로 기록되어 있지만 `gpio.c`는 GPIO Output으로 설정합니다. GPIO_EXIT는 원본 표기입니다. PB6 RFID IRQ 핀은 설정되어 있으나 현재 RC522 통신 완료 확인은 레지스터 폴링 방식입니다.
