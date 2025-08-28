#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// 시리얼 통신을 위한 버퍼 크기
#define SERIAL_BUFFER_SIZE 64

// 버퍼 및 세마포어 선언
char txBuffer[SERIAL_BUFFER_SIZE];
char rxBuffer[SERIAL_BUFFER_SIZE];
SemaphoreHandle_t xSerialSemaphore;

// 태스크 함수 선언
void TaskSerial(void *pvParameters);
void TaskTransmit(void *pvParameters);
void TaskReceive(void *pvParameters);

void setup() {
  // 디버그용 시리얼 초기화
  Serial.begin(115200);
  while (!Serial) 
  {
    ; // 시리얼 포트가 연결될 때까지 대기
  }
  
  // 통신용 시리얼 초기화
  Serial1.begin(9600);
  while (!Serial1) 
  {
    ; // 시리얼 포트가 연결될 때까지 대기
  }
  
  // 세마포어 생성
  xSerialSemaphore = xSemaphoreCreateMutex();
  
  if (xSerialSemaphore != NULL) {
    // 디버그 메시지 출력 태스크 생성
    xTaskCreate(
      TaskSerial,
      "Serial",
      128,   // 스택 크기
      NULL,
      2,     // 우선순위
      NULL);
      
    // 데이터 송신 태스크 생성
    xTaskCreate(
      TaskTransmit,
      "Transmit",
      128,   // 스택 크기
      NULL,
      1,     // 우선순위
      NULL);
      
    // 데이터 수신 태스크 생성
    xTaskCreate(
      TaskReceive,
      "Receive",
      128,   // 스택 크기
      NULL,
      1,     // 우선순위
      NULL);
      
    // 태스크 스케줄러 시작
    vTaskStartScheduler();
  }
}

void loop() {
  // FreeRTOS가 태스크를 관리하므로 메인 루프는 사용하지 않음
}

// 디버그 메시지 출력 태스크
void TaskSerial(void *pvParameters) {
  (void) pvParameters;
  
  for (;;) {
    // 세마포어를 획득하여 디버그 메시지 출력
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)10) == pdTRUE) {
      Serial.println("Debug: System running");
      
      // 세마포어 반환
      xSemaphoreGive(xSerialSemaphore);
    }
    
    // 1초마다 메시지 출력
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// 데이터 송신 태스크
void TaskTransmit(void *pvParameters) {
  (void) pvParameters;
  
  for (;;) {
    // 전송할 데이터 준비 (예: 센서 데이터 또는 테스트 데이터)
    sprintf(txBuffer, "Data: %lu", millis());
    
    // 세마포어 획득
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)10) == pdTRUE) {
      // 디버그 메시지
      Serial.print("Transmitting: ");
      Serial.println(txBuffer);
      
      // 데이터 전송
      Serial1.println(txBuffer);
      
      // 세마포어 반환
      xSemaphoreGive(xSerialSemaphore);
    }
    
    // 500ms마다 데이터 전송
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// 데이터 수신 태스크
void TaskReceive(void *pvParameters) {
  (void) pvParameters;
  
  int index = 0;
  char incomingByte;
  
  for (;;) {
    // Serial1에서 데이터 수신 확인
    if (Serial1.available() > 0) {
      // 세마포어 획득
      if (xSemaphoreTake(xSerialSemaphore, (TickType_t)10) == pdTRUE) {
        // 데이터 읽기
        incomingByte = Serial1.read();
        
        // 버퍼에 저장 (버퍼 오버플로우 방지)
        if (index < SERIAL_BUFFER_SIZE - 1) {
          rxBuffer[index] = incomingByte;
          index++;
          
          // 줄바꿈 문자가 감지되면 수신 완료로 처리
          if (incomingByte == '\n') {
            rxBuffer[index] = '\0';  // 문자열 종료
            
            // 디버그 메시지
            Serial.print("Received: ");
            Serial.println(rxBuffer);
            
            // 수신 버퍼 초기화
            index = 0;
          }
        } else {
          // 버퍼 오버플로우 방지를 위해 인덱스 초기화
          index = 0;
        }
        
        // 세마포어 반환
        xSemaphoreGive(xSerialSemaphore);
      }
    }
    
    // 잠시 대기 (CPU 사용량 감소)
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}