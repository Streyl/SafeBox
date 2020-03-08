
#include <SPI.h>
#include <Wire.h>

#include <SD.h>
File myFile;
#include <Keypad.h>

#include<EEPROM.h>

#include<dmtimer.h>

#include "sh1106.h"

#include <EtherCard.h>


//--------------OLED------------------------
#define VCCSTATE SH1106_SWITCHCAPVCC
#define WIDTH     128
#define HEIGHT     64
#define NUM_PAGE    8  /* number of pages */

#define OLED_RST  46
#define OLED_DC   47
#define OLED_CS  53
#define SPI_MOSI 51     /* connect to the DIN pin of OLED */
#define SPI_SCK  52     /* connect to the CLK pin of OLED */

uint8_t oled_buf[WIDTH * HEIGHT / 8];

//-----------------Keypad--------------
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};


byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey = 0;

//------------------SD Card Reader-------------------------------
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 53;

//-------------------relay module
const int lock = 7;

//---------------------PASSWORDS

char password[4];
char pass[4];
char pass1[4];
char default_pass[4];
char pass_change[4];
char tryPassword[4];
char timeCounter[4];
char alarmCounter[4];
bool changed;

char hh_timer[2];
char mm_timer[2];
char hh_alarm[2];
char mm_alarm[2];

int alarmh0 = 0;
int alarmh1 = 0;
int alarmm2 = 0;
int alarmm3 = 0;


unsigned long timehh;
unsigned long timemm;
//ETHERNET

//time_t t = now();

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  //----------Passwords

  if (changed == true)
  {
    myFile = SD.open("password.txt", O_WRITE);

    if (myFile) {
      int r = 0;
      while (r < 4)
      {
        Serial.print("Writing to password.txt...");
        Serial.print(pass_change[r]);
        myFile.print(pass_change[r]);
        r++;
      }

      // close the file:
      myFile.close();
    }
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening password.txt");
    }
    changed = false;
  }




  int j = 0;
  myFile = SD.open("password.txt");
  while (myFile.available())
  {
    password[j] = myFile.read();
    //Serial.println(myFile.read());
    j++;
  }
  j = 0;
  myFile.close();

  int k = 0;
  myFile = SD.open("default.txt");
  while (myFile.available())
  {
    default_pass[k] = myFile.read();
    //Serial.println(myFile.read());
    k++;
  }
  k = 0;
  myFile.close();



  //--------------RELAY-MODULE---------------
  pinMode(lock, OUTPUT);
  digitalWrite(lock, HIGH);

  //--------------OLED---------------
  Serial.println("setup()");

  /* display an image of bitmap matrix */
  SH1106_begin();
  SH1106_clear(oled_buf);
  SH1106_bitmap(0, 0, Waveshare12864, 128, 64, oled_buf);
  SH1106_display(oled_buf);
  delay(2000);
  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 8, oled_buf);
  SH1106_string(24, 0, "B", 12, 8, oled_buf);
  SH1106_string(48, 0, "C", 12, 8, oled_buf);
  SH1106_string(72, 0, "D", 12, 8, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);
  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, "Please Choose   Module:A,B,C,D", 16, 4, oled_buf);
  SH1106_display(oled_buf);



}

void loop() {

  SH1106_clear(oled_buf);
  SH1106_clear(oled_buf);
  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 8, oled_buf);
  SH1106_string(24, 0, "B", 12, 8, oled_buf);
  SH1106_string(48, 0, "C", 12, 8, oled_buf);
  SH1106_string(72, 0, "D", 12, 8, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);
  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, "Please Choose   Module:A,B,C,D", 16, 4, oled_buf);
  SH1106_display(oled_buf);
  customKey = customKeypad.getKey();
  if (customKey == '#')
  {
    Serial.println(customKey);
    Serial.println("Changing Password");
    change();
  }
  if (customKey == 'A')
  {
    Serial.println(customKey);
    ModuleA();
  }
  if (customKey == 'B')
  {
    Serial.println(customKey);
    ModuleB();
  }
  if (customKey == 'C')
  {
    Serial.println(customKey);
    ModuleC();
  }
  if (customKey == 'D')
  {
    Serial.println(customKey);
    ModuleD();
  }

}

void change()
{
  Serial.println("Module #");
  int j = 0;

  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 0, oled_buf);
  SH1106_string(24, 0, "B", 12, 0, oled_buf);
  SH1106_string(48, 0, "C", 12, 0, oled_buf);
  SH1106_string(72, 0, "D", 12, 0, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);
  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, "Provide current password", 16, 4, oled_buf);
  SH1106_display(oled_buf);

  Serial.println("Provide current password change()");

  while (j < 4)
  {
    char key = customKeypad.getKey();
    if (key == '#')
    {
      Serial.println(key);
      Serial.println("Changing Password change()");
      j = 5;
      change();
    }
    if (key == '*')
    {
      Serial.println(key);
      Serial.println("Returning");
      setup();
      j = 5;
      loop();
    }
    if (key == 'A')
    {
      Serial.println(key);
      j = 5;
      ModuleA();
    }
    if (key == 'B')
    {
      Serial.println(key);
      j = 5;
      ModuleB();
    }
    if (key == 'C')
    {
      Serial.println(key);
      j = 5;
      ModuleC();
    }
    if (key == 'D')
    {
      Serial.println(key);
      j = 5;
      ModuleD();
    }
    if (key)
    {
      pass[j++] = key;
      if (j == 1)
      {
        SH1106_clear(oled_buf);
      }
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 0, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);
      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_char3216(j * 16, 24, key, oled_buf);
      SH1106_display(oled_buf);
      Serial.print(key);
    }
    key = 0;
  }

  delay(500);
  if (j == 4)
  {
    if (!(strncmp(pass, password, 4)) || !(strncmp(pass, default_pass, 4)))
    {
      Serial.print("Check point 1 change()");
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 0, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);
      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Wrong Password...", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Wrong Password... change()");
      delay(1000);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 0, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);
      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Try Again", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Try Again change()");
      delay(1000);
      change();
    }
    else
    {
      Serial.print("Check point 2 change()");
      j = 0;
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 0, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);
      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Enter New Code:", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Enter New Code: change()");
      while (j < 4)
      {
        char key = customKeypad.getKey();
        if (key == '#')
        {
          Serial.println(key);
          Serial.println("Changing Password change()");
          j = 5;
          change();
        }
        if (key == '*')
        {
          Serial.println(key);
          setup();
          j = 5;
          loop();
        }
        if (key == 'A')
        {
          Serial.println(key);
          j = 5;
          ModuleA();
        }
        if (key == 'B')
        {
          Serial.println(key);
          j = 5;
          ModuleB();
        }
        if (key == 'C')
        {
          Serial.println(key);
          j = 5;
          ModuleC();
        }
        if (key == 'D')
        {
          Serial.println(key);
          j = 5;
          ModuleD();
        }
        if (key)
        {
          pass_change[j] = key;
          if (j == 0)
          {
            SH1106_clear(oled_buf);
          }
          SH1106_string(0, 0, "A", 12, 0, oled_buf);
          SH1106_string(24, 0, "B", 12, 0, oled_buf);
          SH1106_string(48, 0, "C", 12, 0, oled_buf);
          SH1106_string(72, 0, "D", 12, 0, oled_buf);
          timehh = (((millis() / 1000) / 60) / 60) % 24;
          timemm = ((millis() / 1000) / 60) % 60;
          Serial.print(timehh);
          Serial.print(":");
          Serial.print(timemm);
          itoa(timehh, hh_timer, 10);
          SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          SH1106_string(106, 0, ":", 12, 8, oled_buf);
          itoa(timemm, mm_timer, 10);
          SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          SH1106_char3216(j * 16, 24, key, oled_buf);
          SH1106_display(oled_buf);
          Serial.println(key);
          EEPROM.write(j, key); //<---------------------------------------
          j++;
        }
      }
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 0, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, " - Accepted", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println(" Done...... change()");
      delay(1000);
      changed = true;
      setup();
      loop();
    }
  }
}



void ModuleA() //CODE TO OPEN
{
  int i = 0;
  Serial.println("Module A");
  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 0, oled_buf);
  SH1106_string(24, 0, "B", 12, 8, oled_buf);
  SH1106_string(48, 0, "C", 12, 8, oled_buf);
  SH1106_string(72, 0, "D", 12, 8, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);

  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, "Enter Password:", 16, 4, oled_buf);
  SH1106_display(oled_buf);
  Serial.println("Enter Password:");
  while (i < 4)
  {
    char customKeyA = customKeypad.getKey();
    if (customKeyA == '#')
    {
      Serial.println(customKeyA);
      Serial.println("Changing Password change()");
      change();
      i = 5;
    }
    if (customKeyA == '*')
    {
      Serial.println(customKeyA);
      Serial.println("Returning");
      setup();
      i = 5;
      loop();
    }
    if (customKeyA == 'A')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleA();
    }
    if (customKeyA == 'B')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleB();
    }
    if (customKeyA == 'C')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleC();
    }
    if (customKeyA == 'D')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleD();
    }
    if (customKeyA)
    {
      pass[i++] = customKeyA;
      if (i == 1)
      {
        SH1106_clear(oled_buf);
      }
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_char3216(i * 16, 24, customKeyA, oled_buf);
      SH1106_display(oled_buf);
      Serial.print(customKeyA);
    }
  }
  if (i == 4)
  {
    delay(200);
    //for (int j = 0; j < 4; j++)
    //pass[j] = EEPROM.read(j);
    if (!(strncmp(pass, password, 4)) || !(strncmp(pass, default_pass, 4)))
    {
      digitalWrite(lock, HIGH);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Acces Granted", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println(" Acess  Granted");
      delay(8000);
      i = 0;
      digitalWrite(lock, LOW);
      setup();
      loop();
    }
    else
    {
      SH1106_clear(oled_buf);
      SH1106_clear(oled_buf);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Access Denied...", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Access Denied...");
      delay(2000);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 0, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Enter Password:", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Enter Password:");
      i = 0;
      ModuleA();
    }
  }
}





void ModuleB() //TIMER
{
  Serial.println("Module B");
  int i = 0;
  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 8, oled_buf);
  SH1106_string(24, 0, "B", 12, 0, oled_buf);
  SH1106_string(48, 0, "C", 12, 8, oled_buf);
  SH1106_string(72, 0, "D", 12, 8, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);

  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, "Enter Password  to set Timer", 16, 4, oled_buf);
  SH1106_display(oled_buf);
  Serial.println("Enter Password to set Timer");
  while (i < 4)
  {
    char customKeyA = customKeypad.getKey();
    if (customKeyA == '#')
    {
      Serial.println(customKeyA);
      Serial.println("Changing Password change()");
      change();
      i = 5;
    }
    if (customKeyA == '*')
    {
      Serial.println(customKeyA);
      Serial.println("Returning");
      setup();
      i = 5;
      loop();
    }
    if (customKeyA == 'A')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleA();
    }
    if (customKeyA == 'B')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleB();
    }
    if (customKeyA == 'C')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleC();
    }
    if (customKeyA == 'D')
    {
      Serial.println(customKeyA);
      i = 5;
      ModuleD();
    }
    if (customKeyA)
    {
      pass[i] = customKeyA;
      if (i == 0)
      {
        SH1106_clear(oled_buf);
      }
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_char3216(i * 16, 24, customKeyA, oled_buf);
      SH1106_display(oled_buf);
      Serial.print(customKeyA);
      i++;
    }
  }
  if (i == 4)
  {
    delay(200);
    if (!(strncmp(pass, password, 4)) || !(strncmp(pass, default_pass, 4)))
    {
      digitalWrite(lock, HIGH);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Acces Granted", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println(" Acess  Granted");
      delay(3000);
      i = 0;

      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Set Timer", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Set Timer:");

      delay(3000);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Provide time in seconds", 16, 4, oled_buf);
      SH1106_display(oled_buf);

      int q = 0;
      while (q < 4)
      {
        char customKeyB = customKeypad.getKey();
        if (customKeyB == '#')
        {
          Serial.println(customKeyB);
          Serial.println("Changing Password change()");
          change();
          q = 5;
        }
        if (customKeyB == '*')
        {
          Serial.println(customKeyB);
          Serial.println("Returning");
          setup();
          q = 5;
          loop();
        }
        if (customKeyB == 'A')
        {
          Serial.println(customKeyB);
          q = 5;
          ModuleA();
        }
        if (customKeyB == 'B')
        {
          Serial.println(customKeyB);
          q = 5;
          ModuleB();
        }
        if (customKeyB == 'C')
        {
          Serial.println(customKeyB);
          q = 5;
          ModuleC();
        }
        if (customKeyB == 'D')
        {
          Serial.println(customKeyB);
          q = 5;
          ModuleD();
        }
        if (customKeyB)
        {
          timeCounter[q] = customKeyB;
          if (q == 0)
          {
            SH1106_clear(oled_buf);
          }
          SH1106_string(0, 0, "A", 12, 8, oled_buf);
          SH1106_string(24, 0, "B", 12, 0, oled_buf);
          SH1106_string(48, 0, "C", 12, 8, oled_buf);
          SH1106_string(72, 0, "D", 12, 8, oled_buf);
          timehh = (((millis() / 1000) / 60) / 60) % 24;
          timemm = ((millis() / 1000) / 60) % 60;
          Serial.print(timehh);
          Serial.print(":");
          Serial.print(timemm);

          itoa(timehh, hh_timer, 10);
          SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          SH1106_string(106, 0, ":", 12, 8, oled_buf);
          itoa(timemm, mm_timer, 10);
          SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          SH1106_char3216(q * 16, 24, customKeyB, oled_buf);
          SH1106_display(oled_buf);
          Serial.print(customKeyB);
          q++;
        }
      }
      if (q == 4)
      {
        long int timer = 0;

        Serial.println("START");
        for (int w = 0; w < 4; w++)
        {
          int value = 0;
          if (timeCounter[w] == 48)
          {
            value = 0;
          }
          if (timeCounter[w] == 49)
          {
            value = 1;
          }
          if (timeCounter[w] == 50)
          {
            value = 2;
          }
          if (timeCounter[w] == 51)
          {
            value = 3;
          }
          if (timeCounter[w] == 52)
          {
            value = 4;
          }
          if (timeCounter[w] == 53)
          {
            value = 5;
          }
          if (timeCounter[w] == 54)
          {
            value = 6;
          }
          if (timeCounter[w] == 55)
          {
            value = 7;
          }
          if (timeCounter[w] == 56)
          {
            value = 8;
          }
          if (timeCounter[w] == 57)
          {
            value = 9;
          }

          if (w == 0)
          {
            value = value * 1000;
          }
          if (w == 1)
          {
            value = value * 100;
          }
          if (w == 2)
          {
            value = value * 10;
          }
          if (w == 3)
          {
            value = value * 1;
          }


          timer = timer + value;
        }

        Serial.println(timer);

        char c_timer[4];
        itoa(timer, c_timer, 10);


        delay(2000);
        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 0, oled_buf);
        SH1106_string(48, 0, "C", 12, 8, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        itoa(timemm, mm_timer, 10);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Setting timer for", 16, 4, oled_buf);
        SH1106_display(oled_buf);
        Serial.println("Setting timer for:");
        Serial.println(timer);

        q = 0;
        delay(1000);
        while (timer != 0)
        {
          char c_timer[4];
          itoa(timer, c_timer, 10);
          SH1106_clear(oled_buf);
          SH1106_string(0, 0, "A", 12, 8, oled_buf);
          SH1106_string(24, 0, "B", 12, 0, oled_buf);
          SH1106_string(48, 0, "C", 12, 8, oled_buf);
          SH1106_string(72, 0, "D", 12, 8, oled_buf);
          timehh = (((millis() / 1000) / 60) / 60) % 24;
          timemm = ((millis() / 1000) / 60) % 60;
          Serial.print(timehh);
          Serial.print(":");
          Serial.print(timemm);

          itoa(timehh, hh_timer, 10);
          SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          SH1106_string(106, 0, ":", 12, 8, oled_buf);
          itoa(timemm, mm_timer, 10);
          SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          SH1106_string(0, 24, c_timer, 16, 4, oled_buf);
          SH1106_string(0, 44, "Seconds", 16, 4, oled_buf);
          SH1106_display(oled_buf);
          timer --;
          delay(1000);
        }
        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 0, oled_buf);
        SH1106_string(48, 0, "C", 12, 8, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        itoa(timemm, mm_timer, 10);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Opening Safe", 16, 4, oled_buf);
        SH1106_display(oled_buf);

        digitalWrite(lock, LOW);
        delay(5000);
        setup();
        loop();

      }
      else
      {
        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 0, oled_buf);
        SH1106_string(48, 0, "C", 12, 8, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        itoa(timemm, mm_timer, 10);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Please provide 4 digits", 16, 4, oled_buf);
        SH1106_display(oled_buf);
        Serial.println("Timer not set");
        delay(2000);
        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 0, oled_buf);
        SH1106_string(48, 0, "C", 12, 8, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        itoa(timemm, mm_timer, 10);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Try Again", 16, 4, oled_buf);
        SH1106_display(oled_buf);
        Serial.println("Try Again");
        q = 0;
        ModuleB();
      }






    }
    else
    {
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      SH1106_string(96, 0, "23:23", 12, 8, oled_buf);
      SH1106_string(0, 24, "Access Denied...", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Access Denied...");
      delay(2000);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 0, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Try Again", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Try Again");
      i = 0;
      ModuleB();
    }
  }

}








void ModuleC() //ALARM
{

  Serial.println("Module C");
  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 8, oled_buf);
  SH1106_string(24, 0, "B", 12, 8, oled_buf);
  SH1106_string(48, 0, "C", 12, 0, oled_buf);
  SH1106_string(72, 0, "D", 12, 8, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);

  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, "Enter Password  to set Alarm", 16, 4, oled_buf);
  SH1106_display(oled_buf);
  Serial.println("Enter Password to set Alarm");
  int i = 0;
  while (i < 4)
  {
    char customKeyC = customKeypad.getKey();
    if (customKeyC == '#')
    {
      Serial.println(customKeyC);
      Serial.println("Changing Password change()");
      change();
      i = 5;
    }
    if (customKeyC == '*')
    {
      Serial.println(customKeyC);
      Serial.println("Returning");
      setup();
      i = 5;
      loop();
    }
    if (customKeyC == 'A')
    {
      Serial.println(customKeyC);
      i = 5;
      ModuleA();
    }
    if (customKeyC == 'B')
    {
      Serial.println(customKeyC);
      i = 5;
      ModuleB();
    }
    if (customKeyC == 'C')
    {
      Serial.println(customKeyC);
      i = 5;
      ModuleC();
    }
    if (customKeyC == 'D')
    {
      Serial.println(customKeyC);
      i = 5;
      ModuleD();
    }
    if (customKeyC)
    {
      pass[i] = customKeyC;
      if (i == 0)
      {
        SH1106_clear(oled_buf);
      }
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_char3216(i * 16, 24, customKeyC, oled_buf);
      SH1106_display(oled_buf);
      Serial.print(customKeyC);
      i++;
    }
  }
  if (i == 4)
  {
    delay(200);
    if (!(strncmp(pass, password, 4)) || !(strncmp(pass, default_pass, 4)))
    {
      digitalWrite(lock, HIGH);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Acces Granted", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println(" Acess  Granted");
      delay(3000);
      i = 0;

      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Set Alarm", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Set Alarm:");

      int p = 0;
      while (p < 4)
      {
        char customKeyB = customKeypad.getKey();
        if (customKeyB == '#')
        {
          Serial.println(customKeyB);
          Serial.println("Changing Password change()");
          change();
          p = 5;
        }
        if (customKeyB == '*')
        {
          Serial.println(customKeyB);
          Serial.println("Returning");
          setup();
          p = 5;
          loop();
        }
        if (customKeyB == 'A')
        {
          Serial.println(customKeyB);
          p = 5;
          ModuleA();
        }
        if (customKeyB == 'B')
        {
          Serial.println(customKeyB);
          p = 5;
          ModuleB();
        }
        if (customKeyB == 'C')
        {
          Serial.println(customKeyB);
          p = 5;
          ModuleC();
        }
        if (customKeyB == 'D')
        {
          Serial.println(customKeyB);
          p = 5;
          ModuleD();
        }
        if (customKeyB)
        {
          //            if (customKeyB == '0' || customKeyB == '1' || customKeyB == '2')
          //            {
          //              alarmCounter[p] = customKeyB;
          //              alarmh0 = customKeyB;
          //              if (p == 0)
          //              {
          //                SH1106_clear(oled_buf);
          //              }
          //              SH1106_string(0, 0, "A", 12, 8, oled_buf);
          //              SH1106_string(24, 0, "B", 12, 0, oled_buf);
          //              SH1106_string(48, 0, "C", 12, 8, oled_buf);
          //              SH1106_string(72, 0, "D", 12, 8, oled_buf);
          //              timehh = (((millis() / 1000) / 60) / 60) % 24;
          //              timemm = ((millis() / 1000) / 60) % 60;
          //              Serial.print(timehh);
          //              Serial.print(":");
          //              Serial.print(timemm);
          //
          //              itoa(timehh, hh_timer, 10);
          //
          //              itoa(timemm, mm_timer, 10);
          //              SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          //              SH1106_string(106, 0, ":", 12, 8, oled_buf);
          //              SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          //              SH1106_char3216(p * 16, 24, customKeyB, oled_buf);
          //              SH1106_display(oled_buf);
          //              Serial.print(customKeyB);
          //              p++;
          //            }
          //          }
          //        }
          //        if (p == 1)
          //        {
          //          if (alarmCounter[0] != '2')
          //          {
          //            if (customKeyB == '0' || customKeyB == '1' || customKeyB == '2' || customKeyB == '3' || customKeyB == '4' || customKeyB == '5' || customKeyB == '6' || customKeyB == '7' || customKeyB == '8' || customKeyB == '9')
          //            {
          //              alarmCounter[p] = customKeyB;
          //              alarmh1 = customKeyB;
          //              if (p == 0)
          //              {
          //                SH1106_clear(oled_buf);
          //              }
          //              SH1106_string(0, 0, "A", 12, 8, oled_buf);
          //              SH1106_string(24, 0, "B", 12, 0, oled_buf);
          //              SH1106_string(48, 0, "C", 12, 8, oled_buf);
          //              SH1106_string(72, 0, "D", 12, 8, oled_buf);
          //              timehh = (((millis() / 1000) / 60) / 60) % 24;
          //              timemm = ((millis() / 1000) / 60) % 60;
          //              Serial.print(timehh);
          //              Serial.print(":");
          //              Serial.print(timemm);
          //
          //              itoa(timehh, hh_timer, 10);
          //
          //              itoa(timemm, mm_timer, 10);
          //              SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          //              SH1106_string(106, 0, ":", 12, 8, oled_buf);
          //              SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          //              SH1106_char3216(p * 16, 24, customKeyB, oled_buf);
          //              SH1106_display(oled_buf);
          //              Serial.print(customKeyB);
          //              p++;
          //            }
          //          }
          //          else
          //          {
          //            if (customKeyB == '0' || customKeyB == '1' || customKeyB == '2' || customKeyB == '3')
          //            {
          //              alarmCounter[p] = customKeyB;
          //              alarmh1 = customKeyB;
          //              if (p == 0)
          //              {
          //                SH1106_clear(oled_buf);
          //              }
          //              SH1106_string(0, 0, "A", 12, 8, oled_buf);
          //              SH1106_string(24, 0, "B", 12, 0, oled_buf);
          //              SH1106_string(48, 0, "C", 12, 8, oled_buf);
          //              SH1106_string(72, 0, "D", 12, 8, oled_buf);
          //              timehh = (((millis() / 1000) / 60) / 60) % 24;
          //              timemm = ((millis() / 1000) / 60) % 60;
          //              Serial.print(timehh);
          //              Serial.print(":");
          //              Serial.print(timemm);
          //
          //              itoa(timehh, hh_timer, 10);
          //
          //              itoa(timemm, mm_timer, 10);
          //              SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          //              SH1106_string(106, 0, ":", 12, 8, oled_buf);
          //              SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          //              SH1106_char3216(p * 16, 24, customKeyB, oled_buf);
          //              SH1106_display(oled_buf);
          //              Serial.print(customKeyB);
          //              p++;
          //            }
          //          }
          //        }
          //        if (p == 2)
          //        {
          //          if (customKeyB == '0' || customKeyB == '1' || customKeyB == '2' || customKeyB == '3' || customKeyB == '4' || customKeyB == '5' )
          //          {
          //            alarmCounter[p] = customKeyB;
          //            alarmm2 = customKeyB;
          //            if (p == 0)
          //            {
          //              SH1106_clear(oled_buf);
          //            }
          //            SH1106_string(0, 0, "A", 12, 8, oled_buf);
          //            SH1106_string(24, 0, "B", 12, 0, oled_buf);
          //            SH1106_string(48, 0, "C", 12, 8, oled_buf);
          //            SH1106_string(72, 0, "D", 12, 8, oled_buf);
          //            timehh = (((millis() / 1000) / 60) / 60) % 24;
          //            timemm = ((millis() / 1000) / 60) % 60;
          //            Serial.print(timehh);
          //            Serial.print(":");
          //            Serial.print(timemm);
          //
          //            itoa(timehh, hh_timer, 10);
          //
          //            itoa(timemm, mm_timer, 10);
          //            SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          //            SH1106_string(106, 0, ":", 12, 8, oled_buf);
          //            SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          //            SH1106_char3216(p * 16, 24, customKeyB, oled_buf);
          //            SH1106_display(oled_buf);
          //            Serial.print(customKeyB);
          //            p++;
          //          }
          //        }
          //        if (p == 3)
          //        {
          //          if (customKeyB == '0' || customKeyB == '1' || customKeyB == '2' || customKeyB == '3' || customKeyB == '4' || customKeyB == '5' || customKeyB == '6' || customKeyB == '7' || customKeyB == '8' || customKeyB == '9')
          //          {
          alarmCounter[p] = customKeyB;
          //alarmm3 = customKeyB;

          if (p == 0)
          {
            SH1106_clear(oled_buf);
            alarmh0 = customKeyB;
          }
          if (p == 1)
          {
            alarmh1 = customKeyB;
          }
          if (p == 2)
          {
            alarmm2 = customKeyB;
          }
          if (p == 3)
          {
            alarmm3 = customKeyB;
          }
          SH1106_string(0, 0, "A", 12, 8, oled_buf);
          SH1106_string(24, 0, "B", 12, 0, oled_buf);
          SH1106_string(48, 0, "C", 12, 8, oled_buf);
          SH1106_string(72, 0, "D", 12, 8, oled_buf);
          timehh = (((millis() / 1000) / 60) / 60) % 24;
          timemm = ((millis() / 1000) / 60) % 60;
          Serial.print(timehh);
          Serial.print(":");
          Serial.print(timemm);

          itoa(timehh, hh_timer, 10);
          SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          SH1106_string(106, 0, ":", 12, 8, oled_buf);
          itoa(timemm, mm_timer, 10);
          SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          SH1106_char3216(p * 16, 24, customKeyB, oled_buf);
          SH1106_display(oled_buf);
          Serial.print(customKeyB);
          p++;
        }
      }
      if (p == 4)
      {
        int alarm_hh = 0;
        int alarm_mm = 0;
        int value;
        value = alarmh0;

        if (value == 48)
        {
          alarmh0 = 0;
        }
        if (value == 49)
        {
          alarmh0 = 1;
        }
        if (value == 50)
        {
          alarmh0 = 2;
        }
        if (value == 51)
        {
          alarmh0 = 3;
        }
        if (value == 52)
        {
          alarmh0 = 4;
        }
        if (value == 53)
        {
          alarmh0 = 5;
        }
        if (value == 54)
        {
          alarmh0 = 6;
        }
        if (value == 55)
        {
          alarmh0 = 7;
        }
        if (value == 56)
        {
          alarmh0 = 8;
        }
        if (value == 57)
        {
          alarmh0 = 9;
        }

        value = alarmh1;

        if (value == 48)
        {
          alarmh1 = 0;
        }
        if (value == 49)
        {
          alarmh1 = 1;
        }
        if (value == 50)
        {
          alarmh1 = 2;
        }
        if (value == 51)
        {
          alarmh1 = 3;
        }
        if (value == 52)
        {
          alarmh1 = 4;
        }
        if (value == 53)
        {
          alarmh1 = 5;
        }
        if (value == 54)
        {
          alarmh1 = 6;
        }
        if (value == 55)
        {
          alarmh1 = 7;
        }
        if (value == 56)
        {
          alarmh1 = 8;
        }
        if (value == 57)
        {
          alarmh1 = 9;
        }

        value = alarmm2;

        if (value == 48)
        {
          alarmm2 = 0;
        }
        if (value == 49)
        {
          alarmm2 = 1;
        }
        if (value == 50)
        {
          alarmm2 = 2;
        }
        if (value == 51)
        {
          alarmm2 = 3;
        }
        if (value == 52)
        {
          alarmm2 = 4;
        }
        if (value == 53)
        {
          alarmm2 = 5;
        }
        if (value == 54)
        {
          alarmm2 = 6;
        }
        if (value == 55)
        {
          alarmm2 = 7;
        }
        if (value == 56)
        {
          alarmm2 = 8;
        }
        if (value == 57)
        {
          alarmm2 = 9;
        }

        value = alarmm3;

        if (value == 48)
        {
          alarmm3 = 0;
        }
        if (value == 49)
        {
          alarmm3 = 1;
        }
        if (value == 50)
        {
          alarmm3 = 2;
        }
        if (value == 51)
        {
          alarmm3 = 3;
        }
        if (value == 52)
        {
          alarmm3 = 4;
        }
        if (value == 53)
        {
          alarmm3 = 5;
        }
        if (value == 54)
        {
          alarmm3 = 6;
        }
        if (value == 55)
        {
          alarmm3 = 7;
        }
        if (value == 56)
        {
          alarmm3 = 8;
        }
        if (value == 57)
        {
          alarmm3 = 9;
        }

        alarm_hh = alarmh0 * 10 + alarmh1;

        alarm_mm = alarmm2 * 10 + alarmm3;
        Serial.println("IMPORTANT");
        Serial.println(alarm_hh);
        Serial.println(alarm_mm);

        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 8, oled_buf);
        SH1106_string(48, 0, "C", 12, 0, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        itoa(timemm, mm_timer, 10);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Setting Alarm", 16, 4, oled_buf);
        SH1106_display(oled_buf);
        Serial.println("Setting Alarm");
        delay(2000);
        while (timehh != alarm_hh && timemm != alarm_mm)
        {
          SH1106_clear(oled_buf);
          SH1106_string(0, 0, "A", 12, 8, oled_buf);
          SH1106_string(24, 0, "B", 12, 8, oled_buf);
          SH1106_string(48, 0, "C", 12, 0, oled_buf);
          SH1106_string(72, 0, "D", 12, 8, oled_buf);
          timehh = (((millis() / 1000) / 60) / 60) % 24;
          timemm = ((millis() / 1000) / 60) % 60;
          Serial.print(timehh);
          Serial.print(":");
          Serial.print(timemm);

          itoa(timehh, hh_timer, 10);
          SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
          SH1106_string(106, 0, ":", 12, 8, oled_buf);
          itoa(timemm, mm_timer, 10);
          SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
          SH1106_string(0, 24, "Alarm end at:", 16, 4, oled_buf);
          Serial.println("IMPORTANT 2");
          Serial.println(alarm_hh);
          Serial.println(alarm_mm);


          itoa(alarm_hh, hh_alarm, 10);
          Serial.print(alarm_hh);
          Serial.print(":");
          Serial.print(alarm_mm);
          Serial.println("////");
          Serial.println(hh_alarm);
          Serial.println(mm_alarm);
          SH1106_string(0, 44, hh_alarm, 16, 4, oled_buf);
          SH1106_string(30, 44, ":", 16, 4, oled_buf);
          itoa(alarm_mm, mm_alarm, 10);
          SH1106_string(40, 44, mm_alarm, 16, 4, oled_buf);
          SH1106_display(oled_buf);
        }
        digitalWrite(lock, LOW);
        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 8, oled_buf);
        SH1106_string(48, 0, "C", 12, 0, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        itoa(timemm, mm_timer, 10);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Alarm Ended", 16, 4, oled_buf);
        SH1106_display(oled_buf);
        Serial.println("Alarm Ended");
        delay(10000);
        setup();
        loop();
      }
      else
      {
        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 8, oled_buf);
        SH1106_string(48, 0, "C", 12, 0, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);

        itoa(timemm, mm_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Alarm not setted", 16, 4, oled_buf);
        SH1106_display(oled_buf);
        Serial.println("Alarm not setted");
        delay(2000);
        SH1106_clear(oled_buf);
        SH1106_string(0, 0, "A", 12, 8, oled_buf);
        SH1106_string(24, 0, "B", 12, 8, oled_buf);
        SH1106_string(48, 0, "C", 12, 0, oled_buf);
        SH1106_string(72, 0, "D", 12, 8, oled_buf);
        timehh = (((millis() / 1000) / 60) / 60) % 24;
        timemm = ((millis() / 1000) / 60) % 60;
        Serial.print(timehh);
        Serial.print(":");
        Serial.print(timemm);

        itoa(timehh, hh_timer, 10);
        SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
        SH1106_string(106, 0, ":", 12, 8, oled_buf);
        itoa(timemm, mm_timer, 10);
        SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
        SH1106_string(0, 24, "Try Again", 16, 4, oled_buf);
        SH1106_display(oled_buf);
        Serial.println("Try Again");
        delay(2000);
        ModuleC();
      }

    }
    else
    {
      SH1106_clear(oled_buf);
      SH1106_clear(oled_buf);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Access Denied...", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Access Denied...");
      delay(2000);
      SH1106_clear(oled_buf);
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 0, oled_buf);
      SH1106_string(72, 0, "D", 12, 8, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_string(0, 24, "Enter Password:", 16, 4, oled_buf);
      SH1106_display(oled_buf);
      Serial.println("Enter Password:");
      i = 0;
      ModuleC();
    }
  }
}


void ModuleD()
{
  Serial.println("Module D");
  int j = 0;
  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 8, oled_buf);
  SH1106_string(24, 0, "B", 12, 8, oled_buf);
  SH1106_string(48, 0, "C", 12, 8, oled_buf);
  SH1106_string(72, 0, "D", 12, 0, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);

  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, "Enter New Code:", 16, 4, oled_buf);
  SH1106_display(oled_buf);
  Serial.println("Enter New Code: change()");
  while (j < 4)
  {
    char key = customKeypad.getKey();
    if (key == '#')
    {
      Serial.println(key);
      Serial.println("Changing Password change()");
      j = 5;
      change();
    }
    if (key == '*')
    {
      Serial.println(key);
      setup();
      j = 5;
      loop();
    }
    if (key == 'A')
    {
      Serial.println(key);
      j = 5;
      ModuleA();
    }
    if (key == 'B')
    {
      Serial.println(key);
      j = 5;
      ModuleB();
    }
    if (key == 'C')
    {
      Serial.println(key);
      j = 5;
      ModuleC();
    }
    if (key == 'D')
    {
      Serial.println(key);
      j = 5;
      ModuleD();
    }
    if (key)
    {
      pass_change[j] = key;
      if (j == 0)
      {
        SH1106_clear(oled_buf);
      }
      SH1106_string(0, 0, "A", 12, 8, oled_buf);
      SH1106_string(24, 0, "B", 12, 8, oled_buf);
      SH1106_string(48, 0, "C", 12, 8, oled_buf);
      SH1106_string(72, 0, "D", 12, 0, oled_buf);
      timehh = (((millis() / 1000) / 60) / 60) % 24;
      timemm = ((millis() / 1000) / 60) % 60;
      Serial.print(timehh);
      Serial.print(":");
      Serial.print(timemm);

      itoa(timehh, hh_timer, 10);
      SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
      SH1106_string(106, 0, ":", 12, 8, oled_buf);
      itoa(timemm, mm_timer, 10);
      SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
      SH1106_char3216(j * 16, 24, key, oled_buf);
      SH1106_display(oled_buf);
      Serial.println(key);
      EEPROM.write(j, key); //<---------------------------------------
      j++;
    }
  }


  SH1106_clear(oled_buf);
  SH1106_string(0, 0, "A", 12, 8, oled_buf);
  SH1106_string(24, 0, "B", 12, 8, oled_buf);
  SH1106_string(48, 0, "C", 12, 8, oled_buf);
  SH1106_string(72, 0, "D", 12, 0, oled_buf);
  timehh = (((millis() / 1000) / 60) / 60) % 24;
  timemm = ((millis() / 1000) / 60) % 60;
  Serial.print(timehh);
  Serial.print(":");
  Serial.print(timemm);

  itoa(timehh, hh_timer, 10);
  SH1106_string(96, 0, hh_timer, 12, 8, oled_buf);
  SH1106_string(106, 0, ":", 12, 8, oled_buf);
  itoa(timemm, mm_timer, 10);
  SH1106_string(111, 0, mm_timer, 12, 8, oled_buf);
  SH1106_string(0, 24, " - Accepted", 16, 4, oled_buf);
  SH1106_display(oled_buf);
  Serial.println(" Done...... change()");
  delay(1000);
  changed = true;
  setup();
  loop();
}
