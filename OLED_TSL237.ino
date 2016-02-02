#include <Wire.h>
#include <FreqMeasure.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#define sclk 13
#define miso 12
#define mosi 11
#define dc   7
#define oledcs   10
#define rst  9

//Farben
#define	BLACK           0b0000000000000000
#define WHITE           0b1111111111111111
#define GRAY1          0b11000110000110000
#define GRAY2          0b11100111000111000
#define GRAY3          0b11110111100111100
#define GRAY4          0b11011111001110111


Adafruit_SSD1351 tft = Adafruit_SSD1351(oledcs, dc, rst);

void setup()
{
  tft.begin();
  tft.setRotation(2);//Dreht Bild um 180 Grad
  tft.fillScreen(BLACK);
}

int counter = 0;
float maxValue = 40;
float lastFreq = 0;
float currentFreq = 0;
int color = BLACK;

void loop()
{
  currentFreq = read_TSL237_Hz();

  if (currentFreq > maxValue)
  {
    maxValue = currentFreq;
  }

  if (currentFreq > 100000)
  {
    color=GRAY4;
  }
  else if (currentFreq > 50000)
  {
        color=GRAY3;
  }
  else if (currentFreq > 10000)
  {
        color=GRAY2;
  }
  else
  {
       color=GRAY1; 
  }

  if (abs(currentFreq - lastFreq) > 1000)//Hysterese - vermeidet zu starkes Zappeln
  {
    lastFreq = currentFreq;

    //alten Text löschen
    tft.fillRect(0, 0, 128, 8, BLACK);

    //Text in die Mitte der Bildschirms setzen
    tft.setCursor(50, 0);
    tft.print(currentFreq);  
  }
  else
  {
    currentFreq = lastFreq;
  }
  
  float f = currentFreq / maxValue;
  tft.fillRect(2 * counter, 128 - f * 118, 2, f * 118,  color); //x,y,w,h,farbe
  tft.setTextColor(color);




  delay(50);

  counter++;
  if (counter > 63)
  {
    counter = 0;
//    maxValue = currentFreq;
    //Graph löschen
    tft.fillRect(0, 10, 128, 118,  BLACK);//x,y,w,farbe
  }
}


float read_TSL237_Hz()
{
  double sum = 0; //Summe von 30 Messwerten
  double frequenz = 0.0f;//gemessene Frequenz des Sensors
  int count = 0; //wird unten in der for Schleife verwendet

  FreqMeasure.begin();////Frequenz-Messung beenden
  for (count = 0; count < 30; count++) //30 Messwerte werden erfasst
  {
    while (!FreqMeasure.available())
    {
      //auf neuen Messwert warten
    }
    // Einzelmessungen summieren
    sum = sum + FreqMeasure.read();
  }
  FreqMeasure.end();//Frequenz-Messung beenden
  frequenz = F_CPU / (sum / count);//F_CPU ist die Taktfrequenz des Arduinos

  return frequenz;
}
