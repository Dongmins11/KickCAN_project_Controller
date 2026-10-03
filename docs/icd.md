# ICD · 시스템 통신 명세

[README로 돌아가기](../README.md) · [원본 Excel](files/ICD.xlsx)

원본 Excel의 셀 내용을 Markdown 표로 옮겼습니다. 설계 당시의 값·표기이며, 실제 Controller 구현과 다른 부분은 아래에 별도로 적었습니다. 원본의 서식과 삽입 그림은 Excel 파일에서 확인할 수 있습니다.

## 시트1

| 번호 (제어 명령) | 기능 (설명 및 용도) | 기능명 | 송신 (From) | 수신 (To) | 목적지 ID | 데이터 형식 (이벤트/업데이트) | Value Type | Value | Descrption | 비고 |
|---|---|---|---|---|---|---|---|---|---|---|
| 10 | Auth Control | — | Controller | Main_Node | 1 | Event | NULL | NULL | 연결 끊기기전 메세지 송신 | BT end byte 추가 |
| 11 | Steering Control | — | Controller | Main_Node | 3 | 1ms | unt16_t, uint16_t | 0~1024, 0~1024 | X 2byte,  Y 2byte = Total 4byte | — |
| 12 | Horn Signal | — | Controller | Main_Node | 3 | Event | unit8_t | Horn : 1 | 1 클락션 스위치 이벤트 | — |
| 13 | Turn Signal | — | Controller | Main_Node | 3 | Event | unit8_t | 0 : 1 : 2 | 0 : 미들, 1: 왼쪽, 2: 오른쪽 | — |
| 60 | Foward/Backward/Stop | STEERINGUPDATE | Main MCU | Node_1<br>+<br>Node_2 |  2, 3 | Event | unit8_t | Foward: 0<br>Backward: 1<br>Stop:2<br>TURN: 3 | 전후진 상태 변경시 송신 | — |
| 61 | Horn Control | HORN_ON | Main MCU | Node_2 | 3 | Event | unit8_t | OFF:0<br>ON:1 | 크락션 버튼 입력시 송신 | — |
| 62 | AUTH FAIL | AUTH FAIL | Main MCU | Node_2 | 3 | Event | uint8_t | FAIL: 0<br>OK:1 | 키인증 실패시 송신 | — |
| 63 | Turn Signal | TURN_SIG_ON | Main MCU | Node_2 | 3 | Event | uint8_t | Stop:0<br>Left:1<br>Right:2 | 방향지시등 상태 변경시 송신 | — |
| 64 | BAT measurement | BAT_SAND | Main MCU | Node_2 | 3 | 1s | uint16_t | 8400 ~ 6000(V) | 1s마다 송신 | — |
| 65 | Velocity measurement | VELO_SAND | Main MCU | Node_2 | 3 | 100ms | uint16_t | 0 ~ 20 m/s | 0.1초마다 송신 | — |
| 110 | Temp Monitor | ICD_CMD_TEMP_WARNING | Node_1 | Node_2 | 3 | Event | uint8_t | Warning : 1 | 온도 측정 값 50도 이상 경고 | — |
| 110 | Temp Monitor | ICD_CMD_TEMP_NORMAL | Node_1 | Node_2 | 3 | Event | uint8_t | Normal : 0 | 온도 측정 값 안정 상태 확인 | — |
| 112 | Light Control | ICD_CMD_LIGHT_FAST_ON | Node_1 | Node_2 | 3 | Event | uint8_t | Fast On : 2 | 빠른 헤드라이트 점등 | — |
| 112 | Light Control | ICD_CMD_LIGHT_FADE_ON | Node_1 | Node_2 | 3 | Event | uint8_t | Natural On : 1 | 자연스러운 라이트 점등 | — |
| 112 | Light Control | ICD_CMD_LIGHT_OFF | Node_1 | Node_2 | 3 | Event | uint8_t | Off : 0 | 라이트 해제 | — |
| 113 | Ultrasonic Control | ICD_CMD_FRONT_LEVEL1 | Node_1 | Node_2 | 3 | Event | uint8_t | Level 1 : 1 | 전방 장애물 감지 30cm 알림 | — |
| 113 | Ultrasonic Control | ICD_CMD_FRONT_LEVEL2 | Node_1 | Node_2 | 3 | Event | uint8_t | Level 2 : 2 | 전방 장애물 감지 20cm 알림 | — |
| 113 | Ultrasonic Control | ICD_CMD_FRONT_LEVEL3 | Node_1 | Node_2 | 3 | Event | uint8_t | Level 3 : 3 | 전방 장애물 감지 10cm 알림 | — |
| 113 | Ultrasonic Control | ICD_CMD_FRONT_CLEAR | Node_1 | Node_2 | 3 | Event | uint8_t | Clear : 0 | 전방 장애물 해제 알림 | — |
| 115 | Ultrasonic Control | ICD_CMD_REAR_LEVEL1 | Node_1 | Node_2 | 3 | Event | uint8_t | Level 1 : 1 | 후방 장애물 감지 30cm 알림 | — |
| 115 | Ultrasonic Control | ICD_CMD_REAR_LEVEL2 | Node_1 | Node_2 | 3 | Event | uint8_t | Level 2 : 2 | 후방 장애물 감지 20cm 알림 | — |
| 115 | Ultrasonic Control | ICD_CMD_REAR_LEVEL3 | Node_1 | Node_2 | 3 | Event | uint8_t | Level 3 : 3 | 후방 장애물 감지 10cm 알림 | — |
| 115 | Ultrasonic Control | ICD_CMD_REAR_CLEAR | Node_1 | Node_2 | 3 | Event | uint8_t | Clear : 0 | 후방 장애물 해제 알림 | — |
| 160 | — | — | Node_2 | — | — | — | — | — | — | — |

## 현재 코드와의 차이

Controller 현재 코드에서 명령 10의 논리 목적지는 Node 2(3), 명령 11은 Main Node(1)입니다. 조이스틱 데이터 순서는 Y→X이며 각 값의 범위는 0–1023입니다. 방향지시등 enum은 중앙 0 / 오른쪽 1 / 왼쪽 2 / NONE 4로 원본 명세의 좌우 값과 다릅니다. 수신 측 코드와 일치하는지는 별도 확인이 필요합니다. 원본의 1ms 주기를 현재 펌웨어의 보장된 주기로 해석하지 않습니다. 그 외 노드의 현재 구현은 이 Controller 저장소만으로 확정하지 않았습니다.
