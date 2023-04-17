#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "../lib/Login.h"
#include "../lib/Logo.h"
#include "RTClib.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

#define FMD_PIN 2
#define ECHO    3
#define TIRG    4


int py;     //year
int pmo;    //month
int pd;     //day
int ph;     //hour
int pmi;    //minute
int ps;     //second

// 创建 SSD1306 实例
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// 创建 rtc 实例
RTC_DS1307 rtc;
// 时间和目标时间
DateTime time, tartime = DateTime(1970, 1, 1, 0, 0, 0);

int Mod = 0;
void setup() {

    // 引脚设置
    pinMode(FMD_PIN, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(TIRG, OUTPUT);

    // 初始化串口
    Serial.begin(9600, SERIAL_8E2);
    // 初始化 SSD1306
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
        Serial.println("failed");
        for (;;);
    } else {
        Serial.print("Hello");
    }
    // 初始化 RTC
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        for(;;);
    } else {
        Serial.print("Hello");
    }
    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running, let's set the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }


    // -------------------------------------------------------------------------------------------------------
    // 开机显示
    // 字体设置
    display.setTextColor(WHITE);
    display.setTextSize(2);
    // 显示姓名学号
    display.clearDisplay();
    display.drawBitmap(0, 0, gImage_Login, 128, 64, WHITE);
    display.display();
    delay(1500);
    // 显示logo
    display.clearDisplay();
    display.drawBitmap(26, 0, gImage_logo, 77, 64, WHITE);
    display.display();
    delay(1500);

}


void warning() {

    tone(FMD_PIN, 882, 1000);
    
}


char week[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// 设置模式：显示当前时间或者设置时间。
// 0 为显示当前时间，1 为设置时间， 2 为设置闹钟

// 设置时间
void set_adj() {

    rtc.adjust(DateTime(py, pmo, pd, ph, pmi, ps));

}


// 显示时间
void show_time() {

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print(String(time.year()) + '-' + String(time.month()) + '-' + String(time.day()));
    display.setCursor(110,0);
    display.print(week[time.dayOfTheWeek()]);
    display.setTextSize(2);
    display.setCursor(20, 30);
    display.print(String(time.hour()) + ':' + String(time.minute()) + ':' + String(time.second()));
    display.display();

}

void loop() {


    time = rtc.now();

    show_time();


    // 到达目标时间时触发 warning
    if(time == tartime) {
        warning();
        delay(3000);
    }

    delay(100);

}



// 提示设置闹钟成功
void hint() {

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 10);
    display.println("The clock has been set to");
    display.setTextSize(1);
    display.println(String(tartime.year()) + '-' + String(tartime.month()) + '-' + String(tartime.day()));
    display.print(String(tartime.hour() + ':' + String(time.minute())) + ':' + String(time.second()));
    display.display();
}



// 设置串口事件，改变模式， 并设置时间
void serialEvent() {

    while (Serial.available()) {
        
        // 读取新的字节
        Mod = Serial.parseInt();
        py = Serial.parseInt();
        pmo = Serial.parseInt();
        pd = Serial.parseInt();
        ph = Serial.parseInt();
        pmi = Serial.parseInt();
        ps = Serial.parseInt();
        tartime = DateTime(py, pmo, pd, ph, pmi, ps);
        if(Mod == 1) {
            rtc.adjust(tartime);
            tartime = DateTime(1970, 1, 1, 0, 0, 0);
        } else if( Mod == 2) {
            hint();
            delay(2000);
        } 
        Mod = 0;
        delay(10);
    }

}