#include <Adafruit_NeoPixel.h>

#define LED_PIN 7
 
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 84

int mode;
int wavePos;
int blinkDelay;
int fallingTime;
int riseTime;

int twinkle[3][LED_COUNT];
int dir[LED_COUNT];
int ballcount;
int ball[4];
float ballV[4];

unsigned long lastBlink = 0;
unsigned long lastFalling = 0;
unsigned long lastRising = 0;
unsigned long lastSerial = 0;
unsigned long ballTime[4];
unsigned long t = 0;


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  ballcount=1;
  blinkDelay = 50;
  fallingTime = 20;
  riseTime = 10;
  wavePos = 0;
  mode=1;
  Serial.begin(9600);
  Serial.setTimeout(10);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}


void loop() {
  t = millis();
  if(mode==0)
    runningWave();
  else if(mode ==1)
    sparkling(); 
  else if (mode==2)
    meteor(); 
  else if (mode==3)
    bouncing();
  else if (mode==4)
    runningWave2();
  readSerial();
}

void lowerColor(int i, uint8_t* ret){
  int div;
  long lngRGB = strip.getPixelColor(i);
  uint8_t u8R = (uint8_t)((lngRGB >> 16) & 0xff),
                u8G = (uint8_t)((lngRGB >> 8) & 0xff),
                u8B = (uint8_t)(lngRGB & 0xff);
  if(fallingTime<=0)
    div=1;
  else
    div=fallingTime;
  if(u8R-300/div >0)
    ret[0]=u8R-300/div;
  if(u8G-300/div >0)
    ret[1]=u8G-300/div;
  if(u8B-300/div >0)
    ret[2]=u8B-300/div;
}

void changeColor(int i, uint8_t* ret, int rL, int gL, int bL, bool up, bool rel, bool& dirChange){
  uint8_t col[3]={0,0,0}; 
  long lngRGB = strip.getPixelColor(i);
  uint8_t u8R = (uint8_t)((lngRGB >> 16) & 0xff),
            u8G = (uint8_t)((lngRGB >> 8) & 0xff),
            u8B = (uint8_t)(lngRGB & 0xff);
            
  if(rel){
    col[0]=u8R;
    col[1]=u8G;
    col[2]=u8B;
  }    

  dirChange=false;
  
  if(up){
    for(int i = 0;i<3;i++){
      ret[i]=255;
    }
    if(col[0]+rL <255)
      ret[0]=col[0]+rL;
    if(col[1]+gL <255)
      ret[1]=col[1]+gL;
    if(col[2]+bL<255)
      ret[2]=col[2]+bL;
    for(int i = 0;i<3;i++){
      if(ret[i]==255)
        dirChange = true;
    }
  }
  else{
    for(int i = 0;i<3;i++){
      ret[i]=0;
    }
    if(col[0]-rL >0)
      ret[0]=col[0]-rL;
    if(col[1]-gL>0)
      ret[1]=col[1]-gL;
    if(col[2]-bL>0)
      ret[2]=col[2]-bL;
    for(int i = 0;i<3;i++){
      if(ret[i]==0)
        dirChange = true;
    }
  }
}


void readSerial(){
  String readString;
  while (Serial.available()) {
      delay(3); 
      char c = Serial.read();
      readString += c;
    }
  
    if (readString.length() >0) {
      String id = getValue(readString,'=',0);
      Serial.println("New Setting:");
      if (id == "blinkDelay")     
      {
        String val = getValue(readString,'=',1);
        blinkDelay = val.toInt();
        Serial.println("blinkDelay adjusted");
      }
      else if (id == "fallingTime")
      {
        String val = getValue(readString,'=',1);
        fallingTime = val.toInt();
        Serial.println("fallingTime adjusted");
      }
      else if (id == "riseTime")
      {
        String val = getValue(readString,'=',1);
        riseTime = val.toInt();
        Serial.println("riseTime adjusted");
      }
      else if (id == "mode")
      {
        String val = getValue(readString,'=',1);
        mode = val.toInt();
        Serial.println("mode adjusted");
      }
      else if (id == "ballcount")
      {
        String val = getValue(readString,'=',1);
        ballcount = val.toInt();
        Serial.println("ballcount adjusted");
      }
    }
}

void sparkling(){
  if(riseTime==0){
    if(t-lastBlink>blinkDelay){
      lastBlink=t;
      int r = rand()%LED_COUNT;
      twinkle[0][r]=255;
      dir[r]=0;
    }
  }
  else{
    if(t-lastBlink>blinkDelay){
      lastBlink=t;
      int r = rand()%LED_COUNT;
      dir[r]=1;
    }
  }

  if(t-lastRising>riseTime&&riseTime!=0){
    lastRising=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==1){
        int k = twinkle[0][i];
        strip.setPixelColor(i, k, k, k);
        if(k<245)
          twinkle[0][i]=k+10;
        else{
          twinkle[0][i]=255;
          dir[i]=0;
        }  
      }
    }
  }
  
  if(t-lastFalling>fallingTime){
    lastFalling=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==0){
        int k = twinkle[0][i];
        strip.setPixelColor(i, k, k, k);
        if(k>10)
          twinkle[0][i]=k-10;
        else
          twinkle[0][i]=0;
      }
    }
  }
  strip.show();
  delay(10);
}

void runningWave(){

  if(t-lastBlink>blinkDelay){
    lastBlink=t;
    if(wavePos>=LED_COUNT-1)
      wavePos=-1;

    dir[wavePos+1]=1;
    
    wavePos+=1;
  }
  
  if(t-lastRising>riseTime&&riseTime!=0){
    lastRising=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==1){
        strip.setPixelColor(i, twinkle[0][i], twinkle[1][i], twinkle[2][i]);
        for(int k = 0;k<3;k++){
          if(twinkle[k][i]<245)
            twinkle[k][i]=twinkle[k][i]+10;
          else{
            twinkle[k][i]=255;
            dir[i]=0;
          }
        }  
      }
    }
  }
  
  if(t-lastFalling>fallingTime){
    lastFalling=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==0){
        strip.setPixelColor(i, twinkle[0][i], twinkle[1][i], twinkle[2][i]);
        for(int k = 0;k<3;k++){
          if(twinkle[k][i]>5*(k+1))
            twinkle[k][i]=twinkle[k][i]-5*(k+1);
          else
            twinkle[k][i]=0;
        }
      }
    }
  }
  
  strip.show();
}

void runningWave2(){
  bool changeDir=false;
  
  if(t-lastBlink>blinkDelay){
    lastBlink=t;
    if(wavePos>=LED_COUNT-1)
      wavePos=-1;
    dir[wavePos+1]=1;
    wavePos+=1;
  }
  
  uint8_t values[3]={0,0,0};
  if(t-lastRising>riseTime&&riseTime!=0){
    lastRising=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==1){
        changeColor(i, values, 10, 10, 10, true, true, changeDir);
        strip.setPixelColor(i, values[0], values[1], values[2]);
        if(changeDir)
          dir[i]=0;
      }  
    }
  }
  
  if(t-lastFalling>fallingTime){
    lastFalling=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==0){
        changeColor(i, values, 10, 10, 3, false, true, changeDir);
        strip.setPixelColor(i, values[0], values[1], values[2]);
      }
    }
  }
  
  strip.show();
}

void meteor(){
  int r = random(-20,20);
  if(t-lastBlink>blinkDelay+r){
    lastBlink=t;
    if(wavePos>=LED_COUNT-1)
      wavePos=-1;
    if(riseTime==0){
      twinkle[0][wavePos+1]=255;
      twinkle[1][wavePos+1]=255;
      twinkle[2][wavePos+1]=255;
      dir[wavePos+1]=0;
    }
    else{
      dir[wavePos+1]=1;
    }
    wavePos+=1;
  }

  if(t-lastRising>riseTime&&riseTime!=0){
    lastRising=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==1){
        strip.setPixelColor(i, twinkle[0][i], twinkle[1][i], twinkle[2][i]);
        int r = random(0,30);
        for(int k = 0;k<3;k++){
          if(twinkle[k][i]<255-r)
            twinkle[k][i]=twinkle[k][i]+r;
          else{
            twinkle[k][i]=255;
            dir[i]=0;
          }
        }  
      }
    }
  }

  if(t-lastFalling>fallingTime){
    lastFalling=t;
    for(int i = 0;i<LED_COUNT;i++){
      if(dir[i]==0){
        int m = random(0,LED_COUNT);
        strip.setPixelColor(m, twinkle[0][m], twinkle[1][m], twinkle[2][m]);
        int r = random(0,30);
        for(int k = 0;k<3;k++){
          if(twinkle[k][m]>5*(k+1)+r)
            twinkle[k][m]=twinkle[k][m]-5*(k+1)-r;
          else
            twinkle[k][m]=0;
        }
      }
    }
  }
  
  strip.show();
  delay(0); 
}

void bouncing(){
  for(int i = 0;i<ballcount;i++){
    if(ball[i]<=0&&ballV[i]==0){
      int v0 = random(300,500);
      ball[i]=0;
      ballTime[i]=t;
      ballV[i]=float(v0)/(float)10;
    }
    else{
      float t2 = (float)(t-ballTime[i])/(float)1000;
      ball[i]=ballV[i]*t2-20*pow(t2,2)/2;
      if(ball[i]<-7){
        ballV[i]=0;
      }
    }
  }
  for(int i = 0;i<LED_COUNT;i++){
    uint8_t values[3]={0,0,0};
    lowerColor(i,values);
    
    strip.setPixelColor(i, values[0],values[1],values[2]);
  }
  for(int i = 0;i<ballcount;i++){
    strip.setPixelColor(ball[i], 140,140, 255);
    float v = ballV[i]-20*(t-ballTime[i])/1000;
    if(v>0){
      for(int k = 1;k<v/5;k++){
        if(255>k*50)
          strip.setPixelColor(ball[i]-k, 140,255-k*50, 255-k*50);
        else
          strip.setPixelColor(ball[i]-k, 140,0, 0);
      }
    }
    else{
      for(int k = 1;k<-v/5;k++){
        if(255>k*50)
          strip.setPixelColor(ball[i]+k, 140,0, 255-k*50);
        else
          strip.setPixelColor(ball[i]+k, 140,0, 0);
      }
    }
  }
  strip.show();
}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
