/*
modified by varind in 2013
this code is public domain, enjoy!
 */

#include <LiquidCrystal.h>
#include <fix_fft.h>
#include <Servo.h> 
#define LCHAN 5
#define RCHAN 4
const int channels = 2;
const int xres = 16;
const int yres = 8;
const int gain = 3;
//int decayrate = 2; // larger number is longer decay
int decaytest = 1;
char im[64], data[64];
char Rim[64], Rdata[64];
char data_avgs[32];
float peaks[32];
int i = 0,val,Rval;
int x = 0, y=0, z=0;
LiquidCrystal lcd(2, 3, 4, 7, 8, 12); //saves 5 pwm pins for servos, leds, etc

Servo servo1, servo2, servo3, servo4;
int startpos = 90;
int calibrate = 0;  //locks servos at startpos (lowest value)
int pos1 = startpos, pos2 = startpos, pos3 = startpos, pos4 = startpos;
int servomap, servomap2;

// VU METER CHARACTERS
byte v1[8] = {
  B00000,B00000,B00000,B00000,B00000,B00000,B00000,B11111};
byte v2[8] = {
  B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111};
byte v3[8] = {
  B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111};
byte v4[8] = {
  B00000,B00000,B00000,B00000,B11111,B11111,B11111,B11111};
byte v5[8] = {
  B00000,B00000,B00000,B11111,B11111,B11111,B11111,B11111};
byte v6[8] = {
  B00000,B00000,B11111,B11111,B11111,B11111,B11111,B11111};
byte v7[8] = {
  B00000,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte v8[8] = {
  B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};


void setup() {
  //  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(1, v1);
  lcd.createChar(2, v2);
  lcd.createChar(3, v3);
  lcd.createChar(4, v4);
  lcd.createChar(5, v5);
  lcd.createChar(6, v6);
  lcd.createChar(7, v7);
  lcd.createChar(8, v8);

  //analogReference(DEFAULT);

  //  servo1.attach(5);
  //  servo2.attach(6);
  //  servo3.attach(9);
  //  servo4.attach(10);

  //  servo1.write(pos1);
  //  servo2.write(pos2);
  //  servo3.write(pos3);
  //  servo4.write(pos4);
}

void loop() {
//  delay(10);
  for (i=0; i < 64; i++){    
    val = ((analogRead(LCHAN) / 4 ) - 128);  // chose how to interpret the data from analog in
    data[i] = val;                                      
    im[i] = 0;   
    if (channels ==2){
      Rval = ((analogRead(RCHAN) / 4 ) - 128);  // chose how to interpret the data from analog in
      Rdata[i] = Rval;                                      
      Rim[i] = 0;   
    }
  };

  fix_fft(data,im,6,0); // Send the data through fft
  if (channels == 2){
    fix_fft(Rdata,Rim,6,0); // Send the data through fft
  }

  // get the absolute value of the values in the array, so we're only dealing with positive numbers
  for (i=0; i< 32 ;i++){  
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); 
  }
  if (channels ==2){
    for (i=16; i< 32 ;i++){  
      data[i] = sqrt(Rdata[i-16] * Rdata[i-16] + Rim[i-16] * Rim[i-16]); 
    }
  }

  // todo: average as many or as little dynamically based on yres
  for (i=0; i<32; i++) {
    data_avgs[i] = (data[i]);// + data[i*2+1]);// + data[i*3 + 2]);// + data[i*4 + 3]);  // add 3 samples to be averaged, use 4 when yres < 16
    data_avgs[i] = constrain(data_avgs[i],0,9-gain);  //data samples * range (0-9) = 9
    data_avgs[i] = map(data_avgs[i], 0, 9-gain, 0, yres);        // remap averaged values
  }



  //servos();
  decay(1);
  //thirtytwoband();
  //mono();
  //stereo8();
  stereo16();


} // end loop








void decay(int decayrate){
  //// reduce the values of the last peaks by 1 
  if (decaytest == decayrate){
    for (x=0; x < 32; x++) {
      peaks[x] = peaks[x] - 1;  // subtract 1 from each column peaks
      decaytest = 0;
      
    }
  }
  decaytest++;

}

void mono(){ 
  for (x=0; x < xres; x++) {  // repeat for each column of the display horizontal resolution
    y = data_avgs[x];  // get current column value 
        z= peaks[x];
    if (y > z){
      peaks[x]=y;
    }
    y= peaks[x]; 

    if (y <= 8){            
      lcd.setCursor(x,0); // clear first row
      lcd.print(" ");
      lcd.setCursor(x,1); // draw second row
      if (y == 0){
        lcd.print(" "); // save a glyph
      }
      else {
        lcd.write(y);
      }
    }
    else{
      lcd.setCursor(x,0);  // draw first row
      if (y == 9){
        lcd.write(" ");  
      }
      else {
        lcd.write(y-8);  // same chars 1-8 as 9-16
      }
      lcd.setCursor(x,1);
      lcd.write(8);  
    } // end display
  }  // end xres
}

void stereo8(){
  for (x=0; x < 8; x++) {
    y = data_avgs[x];
        z= peaks[x];
    if (y > z){
      peaks[x]=y;
    }
    y= peaks[x]; 

    if (y <= 8){            
      lcd.setCursor(x,0); // clear first row
      lcd.print(" ");
      lcd.setCursor(x,1); // draw second row
      if (y == 0){
        lcd.print(" "); // save a glyph
      }
      else {
        lcd.write(y);
      }
    }
    else{
      lcd.setCursor(x,0);  // draw first row
      if (y == 9){
        lcd.write(" ");  
      }
      else {
        lcd.write(y-8);  // same chars 1-8 as 9-16
      }
      lcd.setCursor(x,1);
      lcd.write(8);  
    }
  }
  for (x=16; x < 32; x++) {
    y = data_avgs[x];
       z= peaks[x];
    if (y > z){
      peaks[x]=y;
    }
    y= peaks[x]; 

    if (y <= 8){            
      lcd.setCursor(x-8,0); // clear first row
      lcd.print(" ");
      lcd.setCursor(x-8,1); // draw second row
      if (y == 0){
        lcd.print(" "); // save a glyph
      }
      else {
        lcd.write(y);
      }
    }
    else{
      lcd.setCursor(x-8,0);  // draw first row
      if (y == 9){
        lcd.write(" ");  
      }
      else {
        lcd.write(y-8);  // same chars 1-8 as 9-16
      }
      lcd.setCursor(x-8,1);
      lcd.write(8);  
    }
  }
}

void stereo16(){
  for (x=0; x < 16; x++) {
    y = data_avgs[x];
         z= peaks[x];
    if (y > z){
      peaks[x]=y;
    }
    y= peaks[x]; 
    if (x < xres){            
      lcd.setCursor(x,0); // draw first row
      if (y == 0){
        lcd.print(" "); // save a glyph
      }
      else {
        lcd.write(y);
      }
    }
  }

  for (x=16; x < 32; x++) {
    y = data_avgs[x];
        z= peaks[x];
    if (y > z){
      peaks[x]=y;
    }
    y= peaks[x]; 
    if (x-16 < xres){            
      lcd.setCursor(x-16,1); // draw second row
      if (y == 0){
        lcd.print(" "); // save a glyph
      }
      else {
        lcd.write(y);
      }
    }
  }
}


void thirtytwoband(){
  for (x=0; x < 32; x++) {
    y = data_avgs[x];

        z= peaks[x];
    if (y > z){
      peaks[x]=y;
    }
    y= peaks[x]; 
    if (x < 16){            
      lcd.setCursor(x,0); // draw second row
      if (y == 0){
        lcd.print(" "); // save a glyph
      }
      else {
        lcd.write(y);
      }
    }
    else{
      lcd.setCursor(x-16,1);
      if (y == 0){
        lcd.print(" ");
      }
      else {
        lcd.write(y);
      }
    }
  }
}


void goservo(){
  for (x=0; x < xres; x++) {  // repeat for each column of the display horizontal resolution
    y = data_avgs[x];  // get current column value 
    if (calibrate != 1){
      servomap = map(y, 0, yres, 90, 180);
      servomap2 = map(y, 0, yres, 90, 00);
      if ( x== xres/8){
        servo1.write(servomap);    
      }
      if (x == xres/4){
        servo2.write(servomap2);
      }  
      if (x == xres/2){
        servo3.write(servomap);    
      }
      if (x == xres/1){
        servo4.write(servomap2);
      }
    }
  }
}



