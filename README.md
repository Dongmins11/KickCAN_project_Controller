# KICK-CAN (깡통차기!)

> **RFID 인증 기반 Bluetooth Controller와 CAN 차량 네트워크를 결합한 STM32 분산 임베디드 RC카 시스템**

<p align="center">
  <img src="https://github.com/VisionAITeamProject/ImageUploadRepo/blob/main/KakaoTalk_20260810_234903664.jpg" width="45%" alt="Image 1">
  <img src="https://github.com/VisionAITeamProject/ImageUploadRepo/blob/main/KakaoTalk_20260810_234903664_01.jpg" width="45%" alt="Image 2">
</p>

## 프로젝트 소개

KICK-CAN은 핸드헬드 Controller와 RC카 내부의 세 개 차량 노드를 연동한 **4-Node 분산 임베디드 시스템**입니다.

사용자가 Controller에서 입력한 조이스틱과 스위치 데이터는 HC-05 Bluetooth를 통해 Main_Node로 전달됩니다. Main_Node는 수신한 명령에 따라 4WD 모터를 제어하며, Node_1과 Node_2는 CAN Bus를 이용해 센서 및 차량 상태 데이터를 공유합니다.

Controller에는 RFID 인증 기능을 적용했습니다. 등록된 사용자 인증이 완료되기 전에는 조작·통신 주변장치의 전원과 제어 패킷 송신을 차단하고, 인증 성공 후에만 차량 조작을 허용하도록 설계했습니다.

| 항목 | 내용 |
|---|---|
| 개발 기간 | 2026.07.23 ~ 2026.07.30 |
| 개발 인원 | 4명 |
| 시스템 구성 | Main_Node, Node_1, Node_2, Controller |
| 차량 내부 통신 | CAN Bus |
| 무선 통신 | HC-05 Bluetooth · UART |
| 내 담당 | **Controller 하드웨어 구성 및 펌웨어 개발** |
| 담당 MCU | STM32F411 |

## 시스템 아키텍처

![KICK-CAN 시스템 아키텍처](assets/system-architecture.png)

<img src="https://github.com/VisionAITeamProject/ImageUploadRepo/blob/main/kick-can-system-architecture-simple-fixed%20(2).png" width="600" alt="System Architecture">

| 노드 | 주요 역할 | 통신 |
|---|---|---|
| Main_Node | 4WD 모터 제어, 배터리 감시, Bluetooth–CAN 게이트웨이 | Bluetooth, UART, CAN |
| Node_1 | 온·습도 및 초음파 센서 처리, Thermal Throttling | CAN 송신·수신 |
| Node_2 | OLED·CLCD 출력, 방향지시등, Fail-safe 경고 | CAN 수신 전용 |
| **Controller** | **RFID 인증, 사용자 입력 처리, 무선 제어 데이터 송신** | **SPI, UART, Bluetooth** |

- Controller는 CAN Bus에 직접 연결되지 않습니다.
- Controller의 제어 명령은 Bluetooth를 통해 Main_Node로 전달됩니다.
- Main_Node는 차량 구동과 Bluetooth–CAN 게이트웨이 역할을 수행합니다.
- Node_2는 데이터의 원래 출처와 관계없이 CAN Bus를 통해서만 정보를 수신합니다.

## 나의 담당 — Controller Node

STM32F411 기반 핸드헬드 Controller의 하드웨어 인터페이스 구성과 펌웨어 개발을 담당했습니다.

Controller는 사용자 입력을 차량 제어 데이터로 변환하는 무선 조종기이자, **인증된 사용자에게만 차량 제어 권한을 허용하는 보안 게이트웨이**입니다.

| 담당 영역 | 구현 내용 | 적용 기술 |
|---|---|---|
| RFID 인증 | MFRC522로 UID를 읽고 등록 사용자 판별 | SPI |
| 전원 인터락 | 인증 전 주변장치 전원 차단, 인증 성공 후 활성화 | GPIO, Relay |
| 조이스틱 입력 | 2축 아날로그 입력 연속 수집 및 안정화 | ADC, Circular DMA, EMA |
| 스위치 입력 | 3단 토글·택트 스위치 상태 처리 | GPIO, EXTI, Debouncing |
| 제어 패킷 | 입력값을 Main_Node용 명령 프레임으로 변환 | Command ID, Data Frame |
| 무선 송신 | HC-05를 통한 제어 패킷 전송 | UART, Bluetooth |
| 시스템 관리 | 인증·전원·통신 상태에 따른 송신 허용 | FreeRTOS, CMSIS-RTOS v2 |

### Controller 동작 흐름

```text
전원 인가
→ STM32·MFRC522 동작
→ RFID 사용자 인증
→ 주변장치 전원 활성화
→ 조이스틱·스위치 입력 처리
→ 제어 패킷 생성
→ HC-05 Bluetooth 송신
→ Main_Node 차량 제어
```

## 핵심 구현

### 1. RFID 인증 기반 이중 인터락

Controller의 전원 영역을 인증에 필요한 **상시 전원 영역**과 인증 후 사용하는 **제어 전원 영역**으로 분리했습니다.

| 구분 | 인증 전 | 인증 후 |
|---|---:|---:|
| STM32F411·MFRC522 | ON | ON |
| 조이스틱·HC-05 등 주변장치 | OFF | ON |
| 사용자 입력 처리 | 비활성 | 활성 |
| 제어 패킷 송신 | 차단 | 정상 상태에서 허용 |

등록 RFID 인증에 성공하면 STM32가 릴레이를 제어하여 조이스틱과 HC-05 등의 주변장치에 전원을 공급합니다. 펌웨어에서도 정상 활성화 상태가 되기 전까지 제어 패킷 송신을 차단하여 **하드웨어 전원 제어와 소프트웨어 송신 제어를 결합한 이중 인터락**을 구현했습니다.

인증 전에는 HC-05 전원이 차단되어 있으므로 인증 실패 상태를 Bluetooth로 전송하지 않습니다. 미등록 카드가 인식되면 잠금 상태를 유지하고 로컬 LED로 인증 실패를 표시합니다.

### 2. ADC Circular DMA 기반 조이스틱 처리

2축 조이스틱의 아날로그 값을 ADC Circular DMA로 연속 수집했습니다. DMA를 사용하여 CPU가 매 변환 결과를 직접 읽는 부하를 줄이고, 입력 수집과 데이터 처리를 분리했습니다.

수집된 값에는 다음 처리를 적용했습니다.

- EMA 필터를 이용한 순간 노이즈 완화
- 조이스틱 물리적 편차를 고려한 중앙값 보정
- 미세한 중앙 흔들림을 제거하는 Dead Zone
- 비정상 범위를 제한하는 Clamp
- 차량 기준에 맞춘 축 방향 및 출력값 변환

이를 통해 조이스틱을 움직이지 않았을 때 발생하는 미세한 ADC 변화가 차량 제어 명령으로 전달되는 현상을 줄였습니다.

### 3. GPIO EXTI와 디바운싱

3단 토글 스위치와 택트 스위치는 GPIO EXTI로 입력 변화를 감지했습니다. ISR에서는 복잡한 로직을 실행하지 않고 입력 이벤트만 전달하고, 실제 디바운싱과 최종 상태 판정은 일반 실행 흐름에서 수행했습니다.

이를 통해 인터럽트 처리 시간을 줄이고 기계식 접점의 채터링으로 동일 명령이 여러 번 발생하는 문제를 방지했습니다.

### 4. Bluetooth 패킷 송신과 상태 관리

조이스틱과 스위치 값을 Command ID와 데이터 영역으로 구성하여 UART로 HC-05에 전달했습니다. Main_Node 담당자와 명령 ID, 데이터 범위, 패킷 구조 및 종료 조건을 협의하고 Controller 측 패킷 생성과 송신 기능을 구현했습니다.

FreeRTOS에서는 RFID 인증, 입력 처리, Bluetooth 송신 및 시스템 상태 관리 기능을 역할별로 분리했습니다. 최종 송신 여부는 인증과 전원 상태를 확인한 뒤 결정하여, 입력 데이터가 생성되더라도 제어 조건이 충족되지 않으면 차량으로 전송되지 않도록 했습니다.

## 문제 해결

| 문제 | 원인 | 해결 방법 |
|---|---|---|
| 조이스틱 중앙값 흔들림 | 아날로그 노이즈와 물리적 중앙 오차 | EMA, 중앙값 보정, Dead Zone, Clamp 적용 |
| 스위치 명령 중복 발생 | 기계식 접점 채터링 | 하드웨어 안정화, 시간 기반 디바운싱, ISR 처리 최소화 |
| RFID 카드 순간 미검출 | 폴링 과정의 일시적인 읽기 실패 | 연속 미검출 조건을 만족할 때 제거 상태로 판정 |
| 인증 실패 송신 명세 충돌 | 인증 전에는 HC-05 전원이 차단됨 | Bluetooth 송신 대신 로컬 LED 표시와 잠금 유지 |
| 기능 간 결합도 증가 | 인증·입력·송신 로직이 하나의 흐름에 집중 | FreeRTOS 기능 분리와 상태 기반 송신 제어 적용 |

## 검증 항목

| 검증 항목 | 확인 내용 |
|---|---|
| 등록 RFID 인증 | 주변장치 전원 활성화 후 입력 및 통신 허용 |
| 미등록 RFID 인식 | 주변장치 전원과 제어 패킷 송신 차단 유지 |
| 조이스틱 중앙 상태 | Dead Zone 범위에서 중립 명령 유지 |
| 조이스틱 방향 입력 | 각 축 입력이 정의한 주행·조향 값으로 변환 |
| 토글·택트 반복 조작 | 디바운싱 적용 후 중복 명령 억제 |
| Controller–Main_Node 통신 | 제어 패킷 수신 및 실제 차량 동작 반영 확인 |
| 전체 시스템 연동 | Bluetooth Controller와 CAN 차량 노드 통합 동작 확인 |

> 정량 수치는 측정 화면이나 로그처럼 근거 자료가 남아 있는 항목만 추가합니다.

## 기술 스택

| 분류 | 기술 |
|---|---|
| MCU·Firmware | STM32F411, C, STM32 HAL |
| RTOS | FreeRTOS, CMSIS-RTOS v2 |
| 입력 처리 | ADC, Circular DMA, GPIO, EXTI |
| 통신 | SPI, UART, HC-05 Bluetooth, CAN Bus |
| 하드웨어 | MFRC522, 2축 Joystick, Toggle·Tact Switch, Relay, LED |
| 협업 | Git, GitHub, Jira, Confluence |

## 프로젝트 결과

- Bluetooth Controller와 CAN 차량 네트워크를 연결한 4-Node 시스템 통합
- RFID 인증과 연동한 Controller 주변장치 전원 제어 구현
- 하드웨어 전원 차단과 펌웨어 송신 차단을 결합한 이중 인터락 적용
- ADC Circular DMA와 입력 필터링을 이용한 조이스틱 입력 안정화
- SPI, ADC, DMA, GPIO EXTI, UART 및 FreeRTOS를 하나의 Controller에 통합
- Main_Node와 제어 프로토콜을 협의하고 실제 차량 연동 테스트 수행

## 향후 개선 방향

- 단순 UID 비교보다 안전한 RFID 인증 방식 적용
- 릴레이를 MOSFET 또는 Load Switch 기반 전원 제어로 개선
- 패킷 CRC, Sequence Number 및 ACK·재전송 구조 추가
- HC-05 연결 해제 감지와 자동 재연결 상태 머신 구현
- 통신 지연, 패킷 손실률 및 반복 동작 성공률 정량 측정
- 실제 소스 코드 기준 프로토콜과 펌웨어 구조 문서화

## 회고

첫 펌웨어 팀 프로젝트를 통해 요구사항 분석, 하드웨어 구성, 주변장치 드라이버 구현, FreeRTOS 기능 분리 및 노드 간 통합 테스트로 이어지는 개발 흐름을 경험했습니다.

SPI, ADC, DMA, GPIO EXTI, UART 같은 기술을 개별적으로 사용하는 데서 그치지 않고, RFID 인증 상태가 전원과 Bluetooth 송신 조건에 어떤 영향을 주는지 함께 설계하면서 하드웨어와 소프트웨어 요구사항을 통합적으로 검토하는 중요성을 배웠습니다.

또한 조이스틱 노이즈, 스위치 채터링, RFID 순간 미검출처럼 실제 하드웨어에서 발생하는 문제를 필터링과 상태 판정 로직으로 개선하며 임베디드 시스템의 예외 처리와 검증 과정을 경험했습니다.

---

**Controller 담당: 신동민**
5. 시연 영상 링크 추가
-->
