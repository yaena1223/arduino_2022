#include <WiFi.h>

const char* ssid     = "AndroidHotspot2C_0C_0A"; //와이파이 이름
const char* password = "********"; //와이파이 비밀번호
String precondition = "충분"; //휴지 이전 상태
String current_condition = "적당"; // 휴지 현재 상태
WiFiServer server(80); //포트번호 80으로 서버 생성

int trigPin = 17; //초음파 센서
int echoPin = 16; //초음파 센서

void setup() {
 
  Serial.begin(115200); //시리얼 통신 초기화
  pinMode(echoPin, INPUT); //초음파 센서
  pinMode(trigPin, OUTPUT); //초음파 센서

  delay(10);

   
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to "); //시리얼모니터 출력
  Serial.println(ssid); //연결된 와이파이 이름

  WiFi.begin(ssid, password); //와이파이 통신 연결

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println(""); //시리얼 모니터에 서버 주소 출력
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  server.begin(); //서버 시작
}

void loop() {
  //와이파이
  WiFiClient client = server.available();   //클라이언트가 접속하는지 확인
  if (client) { //신규 클라이언트가 있다면
    Serial.println("New client"); //시리얼모니터 출력

    boolean currentLineIsBlank = true; //서버화면 초기화를 위한 변수
    while (client.connected()) { //클라이언트 접속한 상태라면 계속 실행
      if (client.available()) { //클라이언트로부터 읽어올 bytes 있다면
        
        char c = client.read(); //데이터 읽기
        Serial.write(c); //시리얼 모니터에 추력

        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response"); //시리얼 모니터에 출력
          client.print(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // 응답 완료 후 접속 닫히게 됨
            "Refresh: 0.01\r\n"        // 0.01초마다 페이지 자동으로 초기화
            "\r\n");
           
          client.print("<!DOCTYPE HTML>\r\n");
          client.print("<html>\r\n");
          long duration, distance; //신호 주기 및 거리 변수
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
        
          duration = pulseIn(echoPin, HIGH); //입력핀의 신호주기 반환
          distance = ((float)(340*duration)/1000)/2; //음속을 사용하여 거리 계산
        
          Serial.print("Distance: "); //시리얼 모니터에 거리 출력
          Serial.print(distance);
          Serial.println("mm");
          delay(100);
          client.println("<meta charset=\"UTF-8\">"); //웹서버에 한글 깨지지 않도록 함
          
          if(precondition != current_condition){ //이전 휴지상태와 현재 휴지상태가 다른 경우
            if(distance <= 80) { //초음파센서와 휴지와의 거리가 80 이하인 경우
              Serial.print("휴지 잔여량: "); //시리얼 모니터에 휴지 잔여량 충분 출력
              Serial.println("충분");
              precondition = "충분"; //이전 휴지상태를 충분으로 변경
              delay(100);
            }
            else if (distance > 80) { //초음파센서와 휴지와의 거리가 80 초과인 경우 
              Serial.print("휴지 잔여량: "); //시리얼 모니터에 휴지 잔여량 부족 출력
              Serial.println("부족");
              precondition = "부족"; //이전 휴지상태를 부족으로 변경
              delay(100);
            }
          client.print("휴지 잔여량: "); //웹서버에 휴지 잔여량을
          client.println(precondition); //이전 휴지상태 변수를 출력하면서 수행
          client.print("<br>\r\n");
          client.print("</html>\r\n");
         
        }
        if (c == '\n') { //new line 시작 시
          currentLineIsBlank = true;
        }
        else if (c != '\r') { //new line이 아닌 경우
          currentLineIsBlank = false;
        }
      }
    }
    // 웹서버에 데이터 받을 시간 부여
    delay(100);
  }
 }
}
