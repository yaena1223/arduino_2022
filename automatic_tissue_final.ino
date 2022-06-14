#include <Stepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);//l6x2의 LCD 디스플레이 주소 설정
int stepsPerRev = 2048; //스텝모터가 360도 돌도록 함

//아두이노 보드의 디지털 11,10,9,8 핀을
//스텝모터의 IN1, IN3, IN2, IN4에 연결
Stepper stepper(stepsPerRev, 11,9,10,8); 


int btn1 = 3; //버튼1(소변) 연결 핀
int btn2 = 4; //버튼2(대변) 연결 핀

bool btn1_current = false; //버튼1(소변)의 현재 상태
bool btn2_current = false; //버튼2(대변)의 현재 상태
bool btn1_previous = false; //버튼1(소변)의 이전 상태
bool btn2_previous = false; //버튼2(대변)의 이전 상태 

int b1_cnt = 0; //버튼1을 누른 횟수
int b2_cnt = 0; //버튼2을 누른 횟수

bool door_close_current = true; //문의 이전 상태
bool door_close_previous = true; //문의 현재 상태

void setup() {
  stepper.setSpeed(10); //스텝모터 속도 설정
  
  pinMode(btn1,INPUT); //버튼1(소변) 연결 핀을 입력으로 설정
  pinMode(btn2,INPUT); //버튼2(대변) 연결 핀을 입력으로 설정
  
  pinMode(2,INPUT); //마그네틱 센서 연결 핀을 입력으로 설정
  pinMode(1,OUTPUT); //LED 연결 핀을 출력으로 설정
  
  Serial.begin(9600); //시리얼 통신 초기화
  lcd.init(); //LCD 초기화

  lcd.backlight(); //백라이트를 킴
  lcd.print("Welcome! :)");
  lcd.setCursor(0,0); //첫 번째 줄 첫째 칸
}

void loop() {
  
  btn1_current = digitalRead(btn1); //버튼1의 값을 읽음
  btn2_current = digitalRead(btn2); //버튼2의 값을 읽음
  
  if(btn1_current){ //버튼1이 눌렸으면(값이 True)
    if(btn1_previous == false){ //그리고 이전의 상태가 false라면
      b1_cnt++; //상태가 바뀐 경우에만 버튼1을 누른 횟수 증가
      btn1_previous = true; 

      //소변의 경우, 휴지 4칸을 자동으로 나오게 하도록 설정함
      //각도 계산 결과, 540도 도는 것이 적합하여 1.5배로 계산함
      stepper.step(stepsPerRev*1.5); 
      delay(10);

      Serial.print("버튼1 클릭횟수 : "); //버튼 클린 횟수가 변경되었는지 확인
      Serial.println(b1_cnt); //이전 보다 1 증가된 숫자가 출력될 것
      Serial.print("버튼2 클릭횟수 : ");
      Serial.println(b2_cnt); //버튼2를 누른 것이 아니므로, 이 숫자는 변함이 없음
      
      lcd.clear(); //lcd 화면 초기화
      lcd.print("Used toilet roll"); 
      lcd.setCursor(0,1); //두 번째 줄 첫째 칸
      //총 휴지 사용 길이 : 버튼 1을 누른 횟수 * 4칸 + 버튼 2를 누른 횟수 * 8칸
      lcd.print(b1_cnt*4+b2_cnt*8); 
    }
  }
  else{
    btn1_previous = false;
  }
  
  btn2_current = digitalRead(btn2);//버튼 2의 값을 읽어옴
  
  if(btn2_current){//버튼2가 눌렸으면
    if(btn2_previous == false){//그리고 이전 상태 false라면
      b2_cnt++; //상태가 바뀐 경우에만 버튼2 누른 횟수 증가
      btn2_previous = true;

      //대변의 경우, 휴지 8칸을 자동으로 나오게 하도록 설정함
      //각도 계산 결과, 3바퀴(1080도) 도는 것이 적합하여 3배로 계산함
      stepper.step(stepsPerRev*3); //스텝모터
      delay(10);

            
      Serial.print("버튼1 클릭횟수 : "); //버튼 클린 횟수가 변경되었는지 확인
      Serial.println(b1_cnt); //버튼1을 누른 것이 아니기 때문에, 이전과 같은 값이 출력 될 것
      Serial.print("버튼2 클릭횟수 : "); //버튼 2를 눌렀으므로,
      Serial.println(b2_cnt); //이전보다 1 증가한 겂이 출력됨
      
      lcd.clear(); //lcd 화면 초기화
      lcd.print("Used toilet roll");  
      lcd.setCursor(0,1); //두 번째 줄 첫째 칸
      //총 휴지 사용 길이 : 버튼 1을 누른 횟수 * 4칸 + 버튼 2를 누른 횟수 * 8칸
      lcd.print(b1_cnt*4+b2_cnt*8);
 
      
    }
  }
  else{
    btn2_previous = false;
  }
  
  //마그네틱 센서 코드
  door_close_current = digitalRead(2); //문이 닫혀 있는지 상태를 확인함
  if(door_close_current == false){ //문이 열리면 초기화(문이 열렸다는 것은, 사람이 나갔다는 것이므로)
    if (door_close_previous == true){//이전 상태가 문이 닫힌 경우일 경우에만(상태가 다를 경우에만)
      b1_cnt = 0; //버튼 1을 누른 횟수 초기화
      b2_cnt = 0; //버튼 2를 누른 횟수 초기화
      
      Serial.println("문열림");//문이 열렸음을 확인
      Serial.print("버튼1 클릭 횟수 : "); 
      Serial.println(b1_cnt);//초기화 되었는지 확인, 0출력됨
      Serial.print("버튼2 클릭 횟수 : ");
      Serial.println(b2_cnt);//초기화 되었는지 확인, 0출력됨
      Serial.println();
      
      lcd.clear();
      lcd.print("Goodbye :)");//문이 열렸으므로 lcd에 Goodbye:)를 띄움
      delay(1000);
      door_close_previous = false; //문이 열린상태임을 기록. 
    }     
  }
  else{//문이 닫힌 경우
    if(door_close_previous == false){//이전에 문이 열려 있던 상태라면(이전과 상태가 다르면),
      door_close_previous = true; //문이 닫히면 닫혔다고 기록
      
      Serial.println("문닫힘");//문이 닫혔음을 확인
      Serial.println("휴지 카운트 시작\n");
      lcd.clear();
      lcd.print("Welcome :)"); //문이 닫혔다는 것은, 새로운 사람이 들어왔다는 것이므로 Welcome:)을 띄움
    }
  }


  
}
