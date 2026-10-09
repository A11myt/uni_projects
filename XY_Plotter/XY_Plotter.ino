#include <stdio.h>
#include <stdlib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

int xPin = 500;
int yPin = 500;
int button = 500;
int menu = 1;
int subMenu = 0;
const int chipSelect = 10;
const int height = 820;
const int width = 1189;
int xPos = 0;
int yPos = 0;
int newX = 0;
int newY = 0;

//stepper setup
const int stepPinY = 6; 
const int dirPinY = 5;
const int stepPinX = 4; 
const int dirPinX = 3; 
const int enaPin = 7;
const int Speed = 2;
const int csPin = 10;

//Die Funktion str2Double koennt ihr aufrufen, wenn ihr aus einem String z.B. "0.01303" einen double Wert machen wollt
double str2Double(String val) {

  char buf[256];
  strcpy(buf, val.c_str());
  double x = atof(buf);
  return x;
}

//Die Funktion str2Int koennt ihr aufrufen, wenn ihr aus einem String z.B. "243" einen int Wert machen wollt
int str2Int(String val) {

  char buf[256];
  strcpy(buf, val.c_str());
  int x = atoi(buf);
  return x;
}


void setup() 
{
digitalWrite(enaPin,LOW);
Serial.begin(9600);
lcd.init();
lcd.backlight();
updateMenu();
pinMode(enaPin,OUTPUT);
  digitalWrite(enaPin,HIGH);
  
  pinMode(stepPinX,OUTPUT); 
  pinMode(dirPinX,OUTPUT);
  digitalWrite(dirPinX,HIGH);
  digitalWrite(stepPinX,LOW);

  pinMode(stepPinY,OUTPUT); 
  pinMode(dirPinY,OUTPUT);
  digitalWrite(dirPinY,HIGH);
  digitalWrite(stepPinY,LOW);

  //versuche die SD-Karte zu initialisieren
  if (!SD.begin(chipSelect)){
    Serial.println("Card failed or not present");
    return;
  }
  

  Serial.println("card initialized.");
  outline();
}

void loop() //ließt die werte vom Joystick aus und setzt Variable für den Menübeitrag
{ 
  yPin = analogRead(A0);
  xPin = analogRead(A1);
  button = analogRead(A2);
  if (button == 0)
    execute();
  if(yPin >= 1000)
  {
   menu--;
   updateMenu();
  }
  while(yPin >= 1000)
  {
   delay(200);
   break;
  }
  
  if(yPin <= 20)
  {
   menu++;
   updateMenu();
  }
  while(yPin <= 20)
  {
   delay(200);
   break;
  } 




}

void updateMenu() //Aktualisiert die Anzeige, wenn ein neuer Menüeintrag ausgewält wurde
{
  switch(menu)
  {
    case 0:
    menu = 1;
    break;

    case 1:
    lcd.clear();
    lcd.print(">Zufalliges Raster<");
    lcd.setCursor(0,1);
    lcd.print("Freies Malen");
    lcd.setCursor(0,2);
    lcd.print("Joystick Analyse");
    lcd.setCursor(0,3);
    lcd.print("Bild");
    break;
    
    case 2:
    lcd.clear();
    lcd.print("Zufalliges Raster");
    lcd.setCursor(0,1);
    lcd.print(">Freies Malen<");
    lcd.setCursor(0,2);
    lcd.print("Joystick Analyse");
    lcd.setCursor(0,3);
    lcd.print("Bild");
    break;
    
    case 3:
    lcd.clear();
    lcd.print("Zufalliges Raster");
    lcd.setCursor(0,1);
    lcd.print("Freies Malen");
    lcd.setCursor(0,2);
    lcd.print(">Joystick Analyse<");
    lcd.setCursor(0,3);
    lcd.print("Bild");
    break;
    
    case 4:
    lcd.clear();
    lcd.print("Zufalliges Raster");
    lcd.setCursor(0,1);
    lcd.print("Freies Malen");
    lcd.setCursor(0,2);
    lcd.print("Joystick Analyse");
    lcd.setCursor(0,3);
    lcd.print(">Bild<");
    break;
    
    case 5:
    menu=4;
    break;
  }
}

void execute() //führt ausgewählten Menüeintrag aus, wenn der Button gedrückt wird
{
  switch(menu)
  {
    case 1:
    break;

    case 2:
    freeDraw();
    break;

    case 3:
    analytics();
    break;

    case 4:
    fileMenu();
    break;
  }
}

void freeDraw()
{
 yPin = analogRead(A0);
 xPin = analogRead(A1);
 button = analogRead(A2);
 while (analogRead(A2)>0)
 {
  while (analogRead(A1) > 520)
      {
        right(1);
        delay(500/(analogRead(A1)/2));
       if (analogRead(A0)> 500)
        {up(1);
        delay(500/(analogRead(A0)/2));}
       else if (analogRead(A0)< 480)
        {down(1);
        delay((analogRead(A0)*2)/500);}
      }
      
    while (analogRead(A1) < 510)
      {
        left(1);
        delay((analogRead(A1)*2)/500);
       if (analogRead(A0)> 500)
        {up(1);
        delay(500/(analogRead(A0)/2));}
       else if (analogRead(A0)< 480)
        {down(1);
        delay((analogRead(A0)*2)/500);}
      }
     while (analogRead(A0) > 490)
      {
        up(1);
        delay(500/(analogRead(A0)/2));
       if (analogRead(A1)> 520)
        {right(1);
        delay(500/(analogRead(A1)/2));}
       else if (analogRead(A1)< 510)
        {left(1);
        delay((analogRead(A1)*2)/500);}
      }
      
    while (analogRead(A0) < 475)
      {
        down(1);
        delay((analogRead(A0)*2)/500);
       if (analogRead(A1)> 520)
        {right(1);
        delay(500/(analogRead(A1)/2));}
       else if (analogRead(A1)< 510)
        {left(1);
        delay((analogRead(A1)*2)/500);}
 
      }
 
  
    }
     while (yPos > 0)
    down(1);
  while (xPos > 0)
    left(1);
    delay(300);
}

void analytics()
{
  up(height/2);
  right(width/4);
    up(30);
    down(60);
    up(30);
  right(width/4);
    up(30);
    down(60);
    up(30);
  right(width/4);
    up(30);
    down(60);
    up(30);
  right(width/4);
  up(height/2);
  left(width/2);
  down(height/4);
    left(30);
    right(60);
    left(30);
  down(height/4);
    left(30);
    right(60);
    left(30);
  down(height/4);
    left(30);
    right(60);
    left(30);
  down(height/4);
  up(height/2);
while (analogRead(A2) > 0)
{
  while ((xPos/width) < (analogRead(A1)/1023)) 
    {right(1);
    if ((yPos/height) < (analogRead(A0)/1023))
      up(1);
    if ((yPos/height) > (analogRead(A0)/1023))
      down(1);
    }
  while ((xPos/width) > (analogRead(A1)/1023)) 
    {left(1);
    if ((yPos/height) < (analogRead(A0)/1023))
      up(1);
    if ((yPos/height) > (analogRead(A0)/1023))
      down(1);
    }
  while ((yPos/height) < (analogRead(A0)/1023)) 
    {up(1);
    if ((xPos/width) < (analogRead(A1)/1023))
      right(1);
    if ((xPos/width) > (analogRead(A1)/1023))
      left(1);
    }
    
  
}  
 while (yPos > 0)
    down(1);
  while (xPos > 0)
    left(1);
}

void fileMenu()
{
 while (subMenu = 1)
 {
  yPin = analogRead(A0);
  xPin = analogRead(A1);
  button = analogRead(A2);
  if (button == 0)
  if(yPin >= 1000)
  {
   file--;
   updateSub();
  }
  while(yPin >= 1000)
  {
   delay(200);
   break;
  }
  
  if(yPin <= 20)
  {
   file++;
   updateSub();
  }
  while(yPin <= 20)
  {
   delay(200);
   break;
  }
 }  
}





void drawPic(String fileName) //Öffnet die Textdatei und malt Inhalt anhand der auf 1 normierten Koordinaten (basier auf code von S. Böhning)
{
  //Name der Datei, die geoeffnet werden soll
  //versuche die Datei zu oeffnen, wenn sie existiert
  if (SD.exists(fileName)) {
    Serial.println("file exists.");
  } else {
    Serial.println("file doesn't exist.");
  }
  File dataFile = SD.open(fileName);
  if(dataFile) {
    Serial.println("file is available");
    
    //lese bis zum Ende der Datei aus
    while (dataFile.available()){


      //lese bis zum naechsten Komma
      String x = dataFile.readStringUntil(',');
      //lese bis zum naechsten Zeilenumsprung
      String y = dataFile.readStringUntil('\n');

      //mache aus den Strings double Werte
      double dx = str2Double(x);
      double dy = 1- str2Double(y);
      newX = dx*width;
      newY = dy*height;
      
      while (newX > xPos)
      {
        right(1);
       if (newY > yPos)
        up(1);
       else if (newY < yPos)
        down(1);
      }
      
      while (newX < xPos)
      {
        left(1);
       if (newY > yPos)
        up(1);
       else if (newY < yPos)
        down(1);

      }
      while (newY > yPos)
      {
        up(1);
       if (newX > xPos)
        up(1);
       else if (newX < xPos)
        down(1);

      }
      
      while (newY < yPos)
      {
        down(1);
       if (newX > xPos)
        up(1);
       else if (newX < xPos)
        down(1);
 
      }
      /*Serial.print(dx, DEC);
      Serial.print("\t-\t");
      Serial.println(dy, DEC);
*/
    }
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }
  while (yPos > 0)
    down(1);
  while (xPos > 0)
    left(1);
}
//Stepper Bewegungsrichtungen:
void right(int steps)
{
  digitalWrite(enaPin,HIGH);
  digitalWrite(dirPinX,HIGH); 
   for(int x = 0; x < steps; x++) {
    
    digitalWrite(stepPinX,HIGH); 
    delay(Speed); 
    digitalWrite(stepPinX,LOW); 
    delay(Speed); 
    xPos++; 
    }
}

void left(int steps)
{
  digitalWrite(enaPin,HIGH);
  digitalWrite(dirPinX,LOW); 
   for(int x = 0; x < steps; x++) {
  
    digitalWrite(stepPinX,HIGH); 
    delay(Speed); 
    digitalWrite(stepPinX,LOW); 
    delay(Speed); 
    xPos--; 
    }
}
void up(int steps)
{
  digitalWrite(enaPin,HIGH);
  digitalWrite(dirPinY,HIGH); 
   for(int x = 0; x < steps; x++) {
    
    digitalWrite(stepPinY,HIGH); 
    delay(Speed); 
    digitalWrite(stepPinY,LOW); 
    delay(Speed); 
    yPos++; 
    }
}

void down(int steps)
{
  digitalWrite(enaPin,HIGH);
  digitalWrite(dirPinY,LOW); 
   for(int x = 0; x < steps; x++) {
    
    digitalWrite(stepPinY,HIGH); 
    delay(Speed); 
    digitalWrite(stepPinY,LOW); 
    delay(Speed); 
    yPos--; 
    }
}

void outline()  //Zeichnet Rahmen um Zeichenbereich
{
  up(height);
  right(width);
  down(height);
  left(width);
} 
