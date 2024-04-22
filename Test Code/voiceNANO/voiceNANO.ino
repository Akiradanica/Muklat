#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>

const int chipSelect = 10;

const char aud13[] = "f1.wav";
const char aud14[] = "chair.wav";
const char aud15[] = "f2.wav";
const char aud16[] = "door.wav";
const char aud17[] = "f3.wav";
const char aud18[] = "f4.wav";
const char aud19[] = "l1.wav";
const char aud20[] = "l2.wav";
const char aud21[] = "l3s.wav";
const char aud22[] = "l4.wav";
const char aud23[] = "r1.wav";
const char aud24[] = "person.wav";
const char aud25[] = "r2.wav";
const char aud26[] = "stairs.wav";
const char aud27[] = "table.wav";
const char aud28[] = "poste.wav";
const char aud30[] = "r3.wav";
const char aud31[] = "r4.wav";
const char aud32[] = "warning.wav";

void setup() 
{
  Serial.begin(9600);
  
  while (!Serial)

  Serial.println("Initializing SD card...");
  
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (true);
  }

  Serial.println("SD card initialized.");

  // Initialize audio at the correct sample rate
  AudioZero.begin(16000); // Use standard audio sample rate of 44100 Hz
}

void loop() 
{

  File myFile13 = SD.open(aud13);
  File myFile14 = SD.open(aud14);
  File myFile15 = SD.open(aud15);
  File myFile16 = SD.open(aud16);
  File myFile17 = SD.open(aud17);
  File myFile18 = SD.open(aud18);
  File myFile19 = SD.open(aud19);
  File myFile20 = SD.open(aud20);
  File myFile21 = SD.open(aud21);
  File myFile22 = SD.open(aud22);
  File myFile23 = SD.open(aud23);
  File myFile24 = SD.open(aud24);
  File myFile25 = SD.open(aud25);
  File myFile26 = SD.open(aud26);
  File myFile27 = SD.open(aud27);
  File myFile28 = SD.open(aud28);
  //File myFile29 = SD.open(aud29);
  File myFile30 = SD.open(aud30);
  File myFile31 = SD.open(aud31);
  File myFile32 = SD.open(aud32);


/*  if (!myFile) 
  {
    Serial.println("Error opening testA.wav");
    while (true);
  }
*/
  Serial.println("Playing audio...");

  // Start playing the audio file

  AudioZero.play(myFile13);
  AudioZero.play(myFile14);
  AudioZero.play(myFile15);
  AudioZero.play(myFile16);
  AudioZero.play(myFile17);
  AudioZero.play(myFile18);
  AudioZero.play(myFile19);
  AudioZero.play(myFile20);
  AudioZero.play(myFile21);
  AudioZero.play(myFile22);
  AudioZero.play(myFile23);
  AudioZero.play(myFile24);
  AudioZero.play(myFile25);
  AudioZero.play(myFile26);
  AudioZero.play(myFile27);
  AudioZero.play(myFile28);
  //AudioZero.play(myFile29);
  AudioZero.play(myFile30);
  AudioZero.play(myFile31);
  AudioZero.play(myFile32);
  // Wait for the audio to finish playing

  // Close the file
 
  myFile13.close();
  myFile14.close();
  myFile15.close();
  myFile16.close();
  myFile17.close();
  myFile18.close();
  myFile19.close();
  myFile20.close();
  myFile21.close();
  myFile22.close();
  myFile23.close();
  myFile24.close();
  myFile25.close();
  myFile26.close();
  myFile27.close();
  myFile28.close();
  //myFile29.close();
  myFile30.close();
  myFile31.close();
  myFile32.close();

  Serial.println("End of file. Thank you for listening!");

  // Add delay or other logic before playing again
  delay(2000);
}
