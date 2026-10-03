# Controller 구현 상세

[README로 돌아가기](../README.md)

현재 저장소의 Controller 구현을 기준으로 정리한 문서입니다. 팀 전체 설계 자료는 `docs/files/`에 보관합니다.

## 상태 관리

| 상태 | 의미 | 주행·스위치 명령 |
|---|---|---|
| `SYSTEM_LOCKED` | 부팅 후 등록 카드 대기, 릴레이 OFF | 차단 |
| `SYSTEM_WAIT_BT` | 인증 후 Bluetooth 연결 대기 | 차단 |
| `SYSTEM_ACTIVE` | 연결 및 인증 알림 송신 완료 | 허용 |
| `SYSTEM_AUTH_FAILED` | 동작 중 미등록 카드 인식 | 차단 |

- `LOCKED`에서 등록 카드 인증 → 릴레이 ON → `WAIT_BT`.
- `WAIT_BT`에서 연결 감지 및 인증 알림 송신 성공 → `ACTIVE`.
- `ACTIVE`에서 연결 해제 → 송신 차단 및 `WAIT_BT`.
- 연결 대기 또는 동작 중 미등록 카드 인식 → `AUTH_FAILED`. 연결된 경우 실패 상태를 통보합니다.
- `AUTH_FAILED`에서 등록 카드를 다시 인식하면 연결 상태에 따라 재활성화 또는 연결 대기로 전환합니다.
- 최초 잠금 상태에서 미등록 카드를 인식한 경우에는 Bluetooth가 켜져 있지 않으므로 원격 실패 알림을 전송하지 않습니다.
- 동작 중 인증 실패 시 릴레이를 끄는 코드는 없으며, 제어 명령 송신을 차단하는 방식입니다.
- 카드의 연속 미검출 3회는 카드 제거 판정에 사용합니다. 카드 제거 자체가 제어 권한 해제를 의미하지는 않습니다.

코드: [System_Manager.c](https://github.com/Dongmins11/KickCAN_project_Controller/blob/main/My_main/SYSTEM/System_Manager.c), [RFID_Com.c](https://github.com/Dongmins11/KickCAN_project_Controller/blob/main/My_main/RFIDCOM/RFID_Com.c)

## Task와 공유 자원

| 실행 단위 | 역할 |
|---|---|
| `RFIDTask` | RC522 초기화, 카드 읽기 및 인증 이벤트 전달 |
| `JoystickTask` | 제어 가능 상태에서 필터링한 조이스틱 값 송신 |
| `ControlTask` | 인증·연결 상태 전이, 스위치 디바운싱, LED 갱신 |
| GPIO EXTI callback | Bluetooth·스위치 변화를 Thread Flag로 전달 |
| `UART_MUTEX` | 공통 UART 송신 함수의 동시 접근 제어 |

`BluetoothTask`는 생성되어 있으나 현재 루프의 AT/테스트 호출은 주석 처리되어 있습니다. 실제 명령 송신은 Joystick/Control 실행 흐름에서 공통 송신 함수를 호출하는 구조입니다.

`JoystickTask`에는 `osDelay(10)`이 있으나, 송신 실행 시간과 Mutex 대기 시간이 더해지므로 이를 정확한 10ms 송신 주기나 보장된 100Hz로 표현하지 않습니다. 스위치 디바운싱 상수는 100이며, 해당 프로젝트의 RTOS tick 설정과 함께 해석해야 합니다.

## 조이스틱 입력

1. ADC1의 PA0·PA1을 10-bit로 연속 변환합니다.
2. Circular DMA로 두 축의 값을 버퍼에 갱신합니다.
3. 고정소수점 EMA 필터를 적용합니다. 갱신 가중치는 새 값 1/8입니다.
4. 필터 결과가 10 미만이면 0, 1000 초과이면 1023으로 끝값을 보정합니다.
5. 프레임 데이터에 Y 상위·하위 바이트, X 상위·하위 바이트 순서로 기록합니다.

현재 활성 경로에는 중앙 Dead Zone과 중앙값 보정이 없고, 축 반전 함수 호출도 주석 처리되어 있습니다.

## 송신 프레임

| Offset | 크기 | 내용 |
|---|---|---|
| 0 | 1 byte | Start `0xAA` |
| 1 | 1 byte | 논리 목적지 ID |
| 2 | 1 byte | 명령 ID |
| 3–9 | 7 bytes | 명령 데이터 |
| 10 | 1 byte | Offset 0–9의 XOR 체크섬 |
| 11 | 1 byte | End `0xFF` |

USART1은 코드에서 460800 baud, 8N1로 설정되어 있습니다. HC-05와 Main Node 측 설정도 일치해야 합니다.

일부 명령은 10회 반복 송신합니다. 이는 ACK 기반 성공 확인이나 재전송 프로토콜과는 다르며, 현재 체크섬도 CRC가 아닌 XOR입니다.

## 빌드 안내

저장소의 `CMakeLists.txt`와 `CMakePresets.json`은 CMake 3.22 이상, Ninja, GNU Arm Embedded Toolchain을 사용하는 Debug/Release 구성을 제공합니다.

```sh
cmake --preset Debug
cmake --build --preset Debug
```

이 문서 정리 과정에서는 펌웨어 빌드·보드 플래시·실물 동작을 다시 수행하지 않았습니다. 위 명령은 저장소 설정에 따른 빌드 진입점입니다.

## 개선 과제

- 고정 UID 비교를 넘어선 인증 방식 검토
- 반복 송신을 ACK·시퀀스 기반으로 정리하고 수신 측 처리와 함께 검증
- 송신 주기·지연·패킷 손실 측정
- 설계 문서와 최종 핀 설정·프로토콜을 함께 갱신
