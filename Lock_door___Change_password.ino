//Lưu ý khi nhập reset mật khẩu bằng code bắt buộc phải nhập mật khẩu để mởi cửa lần đầu sau đó mới tiến hành thay đổi mật khẩu mới
/*
   Giải thích hàm (strncmp(new_password, initial_password, 4
    strncmp()hàm trả về 0 khi hai chuỗi giống nhau và khác 0 khi chúng khác nhau.
    Sử dụng !toán tử (logic not), kết quả sẽ là 
      Dòng code 144 : true (1) khi các chuỗi giống nhau 
      Dòng code 078 : false (0) khi chúng khác nhau.
*/
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
char password[4];//Định dạng mật khẩu có 4 ký tự bao gồm số và chữ
char initial_password[4],new_password[4];
int i=0;
int lock = 2;//Relay Chân D2
int ledg = 3;//Đèn xanh chân D3
int ledr = 4;//Đèn đỏ chân D4
int BUZZER = 13;//buzzer chân D13
char key_pressed=0;
const byte rows = 4;
const byte columns = 4;
char hexaKeys[rows][columns] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte row_pins[rows] = {12, 11, 10, 9};
byte column_pins[columns] = {8, 7, 6, 5};
Keypad keypad_key = Keypad(makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);
int wrong_num=0;
void setup() {
  Serial.begin(9600);
//Hàm này sẽ mở comment code đi và chạy lần đầu khi nạp code cho Arduino mới và sau đó comment lại và không cần chạy cho các lần điều chỉnh code lần sau nữa
//  for(int j=0;j<4;j++)
//    EEPROM.write(j, j+49);//Ghi chuỗi kí tự 1234 vô EEPROM của arduino làm mật khẩu ban đầu
  pinMode(lock, OUTPUT);
  pinMode(ledg, OUTPUT);
  pinMode(ledr, OUTPUT);
  lcd.init();                    
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("WELCOME");
  chaychu();
  lcd.setCursor(0,0);
  lcd.print("Nhap mat khau    ");
  
}

void loop() {
  digitalWrite(lock, LOW);
  key_pressed = keypad_key.getKey();
  if ((int)keypad_key.getState() ==  PRESSED){
    tone(BUZZER, 988);
    delay(50);
    noTone(BUZZER);
  }
  if(key_pressed=='#'){// Nút nhấn trên keypad
    change_password();
    tone(BUZZER, 988);
    delay(50);
    noTone(BUZZER);
  }
  if (key_pressed){//Hàm chuyển mật khẩu nhập vào hiển thị trên LCD dưới dạng ****
    password[i++]=key_pressed;//Lệnh nhập pass sẽ hiển thị lần lượt từng kí tự mà mình nhập
    lcd.print(key_pressed);
    delay(500);
    lcd.setCursor(i-1,1);
    lcd.print("*");
  }
  if(i==4){//Nếu nhập mật khẩu đúng 4 chữ số
    delay(200);//đợi 0.2s
    for(int j=0;j<4;j++)//Đưa vô vòng lặp
      initial_password[j]=EEPROM.read(j);//Đọc mật khẩu cũ trong EEPROM của arduino
    if(!(strncmp(password, initial_password,4))){//so sánh mật khẩu mình nhập dòng code 68 có đúng với mật khẩu ban đầu mặc định dòng code 37 ko
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("MOI VAO");
      digitalWrite(lock, HIGH);
      tone(BUZZER, 1976);
      delay(50);
      noTone(BUZZER);
      digitalWrite(ledg, HIGH);
      delay(10000);//Relay sẽ bật mở cửa trong 10s
      digitalWrite(ledg, LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Nhap mat khau    ");
      i=0;//Trở về vị trí ban đầu để nhập mật khẩu mới
      wrong_num=0;
    }
    else{//Ngược lại
      digitalWrite(lock, LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MAT KHAU SAI");
      digitalWrite(BUZZER, HIGH);
      digitalWrite(ledr, HIGH);
      delay(10000);//Delay 30s = 3 lần delay 10s cho lần nhập sai đầu tiên
      delay(10000);
      delay(10000);
      digitalWrite(ledr, LOW);
      lcd.setCursor(0,1);
      lcd.print("Nhan # de thay mk");
      delay(3000);
      tone(BUZZER, 500);
      delay(50);
      noTone(BUZZER);
      lcd.clear();
      lcd.print("Nhap mat khau    ");
      lcd.setCursor(0,1);
      i=0;
      wrong_num++;
    }
  }
}

void change_password(){
  int j=0;
  lcd.clear();
  lcd.print("Nhap mat khau cu");
  lcd.setCursor(0,1);
  while(j<4){
    char key=keypad_key.getKey();
    if(key){
      if ((int)keypad_key.getState() ==  PRESSED) {
        tone(BUZZER, 988);
        delay(50);
        noTone(BUZZER);
      }
      new_password[j++]=key;
      lcd.print(key);
      delay(500);
      lcd.setCursor(j-1,1);
      lcd.print("*");
    }
    key=0;
  }
  delay(500);

  if((strncmp(new_password, initial_password, 4))){//so sánh chuỗi kí tự giữa mật khẩu mới và mật khẩu ban đầu Có khác nhau ko nếu khác thực hiện hàm dưới 
    lcd.clear();
    lcd.print("MAT KHAU SAI");
    digitalWrite(BUZZER, HIGH);
    digitalWrite(ledr, HIGH);
    delay(10000);
    delay(10000);
    delay(10000);
    digitalWrite(ledr, LOW);
    lcd.setCursor(0,1);
    lcd.print("Thu lai sau");
    tone(BUZZER, 500);
    delay(2000);
    noTone(BUZZER);
    wrong_num++;
  }else{
    j=0;
    lcd.clear();
    lcd.print("Mat khau moi");
    lcd.setCursor(0,1);
    while(j<4){
      char key=keypad_key.getKey();
      if(key){
        if ((int)keypad_key.getState() ==  PRESSED){
          tone(BUZZER, 988);
          delay(50);
          noTone(BUZZER);
        }
        initial_password[j]=key;
        lcd.print(key);
        delay(500);
        lcd.setCursor(j,1);
        lcd.print("*");
        EEPROM.write(j,key);//Ghi giá trị key vừa nhập vô EEPROM
        j++;
      }
    }
    lcd.clear();
    lcd.print("  Da thay doi!");
    tone(BUZZER, 500);
    delay(1500);
    noTone(BUZZER);
  }
  lcd.clear();
  lcd.print("Nhap mat khau    ");
  lcd.setCursor(0,1);
  key_pressed=0;
}

void chaychu()//Hàm chạy hiệu ứng chữ Welcome
{
  for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(150);
  }
  for (int positionCounter = 0; positionCounter < 29; positionCounter++) {
    lcd.scrollDisplayRight();
    delay(150);
  }
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(150);
  }
}
