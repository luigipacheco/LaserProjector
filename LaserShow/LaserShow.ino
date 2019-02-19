// See LICENSE file for details
// Copyright 2016 Florian Link (at) gmx.de

#include "Laser.h"
#include "Drawing.h"
#include "Cube.h"
#include "Objects.h"
#include "Logo.h"

// Create laser instance (with laser pointer connected to digital pin 5)
Laser laser(5);

void setup()
{  
  laser.init();
  //Serial.begin(9600);
}


void letterEffect()
{  
  String dyn = "GROUP2";
  String lu  = "LASER DRONE";
  int w = Drawing::stringAdvance(lu);
  laser.setScale(3048./w);
  laser.setOffset(2048,2048);
  for (int i = 0;i<35;i++) {
    Drawing::drawString(dyn, -w/2,0,4);
    for (int i = 0;i<8;i++){ 
      if (lu[i]>dyn[i]) dyn[i]++;
      if (lu[i]<dyn[i]) dyn[i]--;
    }
  }
  int clip = 0;
  for (int i = 0;i<60;i++) {
    laser.setClipArea(clip, 0, 4095-clip, 4095);
    Drawing::drawString(lu, -w/2,0,1);
    clip += 2048 / 50;
  }
  laser.resetClipArea();
}

void presents() {
  String str = "MRAC";
  int w = Drawing::stringAdvance(str);
  laser.setScale(3048./w);
  laser.setOffset(2048,2048);
  float scale = 1;
  for (int i = 0; i<100;i++) {
    scale -= 0.01;
    laser.setScale(scale);
    Drawing::drawString(str,-w/2, 0);
  }
}

void arduino()
{
  String str = "ARDUINO";
  int w = Drawing::stringAdvance(str);
  laser.setScale(0.5);
  laser.setOffset(1024,1024);
  int count = 360/4;
  int angle = 45;
  for (int i = 0;i<count;i++) {
    Matrix3 world;
    world = Matrix3::rotateX(angle % 360);
    laser.setEnable3D(true);
    laser.setMatrix(world);
    laser.setZDist(2000);
    Drawing::drawString(str,-w/2,-500, 1);
    angle += 8;
  }
  laser.setEnable3D(false);
}

void laserShow()
{
  String str = "LASER";
  int w = Drawing::stringAdvance(str);
  int count = 360/4;
  int angle = 0;
  laser.setScale(0.5);
  for (int i = 0;i<count;i++) {
    Matrix3 world;
    laser.setEnable3D(true);
    world = Matrix3::rotateX(angle % 360);
    laser.setMatrix(world);
    laser.setZDist(4000);
    laser.setOffset(1024,1024 + 600);
    Drawing::drawString(str,-w/2,-500, 1);
    world = Matrix3::rotateY(angle % 360);
    laser.setMatrix(world);
    laser.setOffset(1024,1024 - 600);
    Drawing::drawString("SHOW",-w/2,-500, 1);
    angle += 8;
  }
  laser.setEnable3D(false);
}

// electric globe
void globe(int count) {
  laser.setScale(1);
  laser.setOffset(2048,2048);
  for (int i = 0;i<count;i++) {
    laser.on();
    int pos = random(360)/5 * 5;
    int diff1 = random(35);
    int diff2 = random(35);
    int diff3 = random(35);
    for (int r = 0;r<=360;r+=5)
    {    
      laser.sendto(SIN(r)/16, COS(r)/16);
      if (r == pos)   {    
        laser.sendto(SIN(r+diff1)/32, COS(r+diff2)/32);
        laser.sendto(SIN(r+diff2)/64, COS(r+diff3)/64);
        laser.sendto(0, 0);
        laser.sendto(SIN(r+diff3)/64, COS(r+diff3)/64);
        laser.sendto(SIN(r+diff2)/32, COS(r+diff1)/32);
        laser.sendto(SIN(r)/16, COS(r)/16);
      }
    }
  }
}

// draw a circle using sin/cos
void circle(float sides) {
  const int scale = 12;
  laser.sendto(SIN(0)/scale, COS(0)/scale);
  laser.on();
  for (int r = 0;r<=360;r+=360/sides)
  {    
    laser.sendto(SIN(r)/scale, COS(r)/scale);
  }
  laser.off();
}

// Draw circle and count down from 9 to 1
void countDown() {
  laser.setScale(1);
  laser.setOffset(2048,2048);
  int center = Drawing::advance('9');
  for (char j = '9';j>'0';j--) {
    float scale = 0.0;
    
    float step = 0.01;
    for (int i = 0;i<40;i++) {
      laser.setScale(1);
      circle(32);
      laser.setScale(scale);
      Drawing::drawLetter(j, -center/3, -center*2/3 + 100);   
      scale += step;
      step += 0.002;
    }
  }
}
  
  

// simple fixed logo
void drawLogoSimple()
{
  long centerX, centerY, w,h;
  Drawing::calcObjectBox(draw_logo, sizeof(draw_logo)/4, centerX, centerY, w, h);
  laser.setScale(4096/(float)h);
  laser.setOffset(2048,2048);
  for (int i = 0;i<100;i++) {
    Drawing::drawObject(draw_logo, sizeof(draw_logo)/4, -centerX, -centerY);
  }
}

// logo with drawing effect
void drawLogo()
{
  long centerX, centerY, w,h;
  Drawing::calcObjectBox(draw_logo, sizeof(draw_logo)/4, centerX, centerY, w, h);

  int count = 200;
  laser.setScale(4096/(float)h);
  laser.setOffset(2048,2048);

  long maxMove = 0;
  for (int i = 0;i<count;i++) {
    laser.setMaxMove(maxMove);
    maxMove += 400;
    Drawing::drawObject(draw_logo, sizeof(draw_logo)/4, -centerX, -centerY);
    if (laser.maxMoveReached()) {
      long x, y;
      laser.getMaxMoveFinalPosition(x,y);
      laser.resetMaxMove();
      laser.off();
      laser.sendtoRaw(x,y);
      laser.on();
      laser.sendtoRaw(2047,0);
    }
  }
  laser.resetMaxMove();
  long pos = 0;
  while (pos < 4095) {
    laser.setClipArea(pos, 0, 4095,4095);
    pos += 100;
    Drawing::drawObject(draw_logo, sizeof(draw_logo)/4, -centerX, -centerY);
  }
  laser.resetClipArea();
}
  

// rotating arduino logo
void drawArduino2DRotate()
{
  long centerX, centerY, hX, hY, w,h;
  Drawing::calcObjectBox(draw_arduino, sizeof(draw_arduino)/4, centerX, centerY, w, h);
  int angle = 0;
  int x = -centerX + 100;
  for (;x<4396;) {
    laser.setOffset(x,2048);
    laser.setScale(1.);
    Drawing::drawObjectRotated(draw_arduino, sizeof(draw_arduino)/4, centerX, centerY, angle % 360);
    angle += 8;
    x += 40;
  }
}

void drawPlane()
{
  int count = 180;
  float scale = 1;
  laser.setScale(2);
  laser.setOffset(0,0);
  long x = 4096;
  long y = 4096;
  for (int i = 0;i<count;i++) {
    laser.setScale(scale);
    laser.setOffset(x,y);
    Drawing::drawObject(draw_plane, sizeof(draw_plane)/4);
    x -= 20*scale;
    y -= 20;
    scale += 0.01;
  }
}

void drawBike()
{
  int count = 140;
  laser.setScale(3);
  laser.setOffset(0,0);
  long x = -2500;
  long y = 1000;
  for (int i = 0;i<count;i++) {
    laser.setOffset(x,y);
    Drawing::drawObject(draw_bike, sizeof(draw_bike)/4);
    x += 50;
  }
}

void drawHeart()
{
  int count = 140;
  laser.setScale(3);
  laser.setOffset(0,0);
  long x = -2500;
  long y = 1000;
  for (int i = 0;i<count;i++) {
    laser.setOffset(x,y);
    Drawing::drawObject(draw_heart, sizeof(draw_heart)/4);
    x += 50;
  }
}


void drawArduino3D()
{
  laser.setScale(1.);
  laser.setOffset(0,0);
  long centerX, centerY, w,h;
  Drawing::calcObjectBox(draw_arduino, sizeof(draw_arduino)/4, centerX, centerY, w, h);
  int count = 360/4;
  int angle = 0;
  for (int i = 0;i<count;i++) {
    Drawing::drawObjectRotated3D(draw_arduino, sizeof(draw_arduino)/4, centerX, centerY, angle % 360, 0, 1000);
    angle += 8;
  }
}
void whatAbout3D()
{
  int w1 = Drawing::stringAdvance("WHAT");
  int w2 = Drawing::stringAdvance("ABOUT");
  int w3 = Drawing::stringAdvance("3D");
  long centerX, centerY, w,h;
  Drawing::calcObjectBox(draw_question, sizeof(draw_question)/4, centerX, centerY, w, h);

  laser.setOffset(2048,2048);
  laser.setScale(0.5);
  for (int i = 0; i<50;i++) Drawing::drawString("WHAT",-w1/2, SIN((i*10) % 360)/100., 1);
  laser.setScale(0.5);
  for (int i = 0; i<50;i++) Drawing::drawString("ABOUT", -w2/2 + SIN((i*10) % 360)/100., 0, 1);
  laser.setScale(1.);
  for (int i = 0; i<120;i++) Drawing::drawString("3D", -w3/2 + SIN((i*4) % 360)/100., COS((i*4) % 360)/100., 1);
  float scale = 0;
  for (int i = 0;i<200;i++) {
    laser.setScale(scale);
    Drawing::drawObject(draw_question, sizeof(draw_question)/4, -centerX, -centerY);
    scale += 0.02;
  }
}

// arduino + heart
void drawWeLove()
{
  int w1 = Drawing::stringAdvance("ARDUINO");
  long centerX, centerY, w,h;
  Drawing::calcObjectBox(draw_heart, sizeof(draw_heart)/4, centerX, centerY, w, h);

  laser.setOffset(2048,2048);
  long maxMove = 0;
  for (int i = 0;i<300;i++) {
    laser.setMaxMove(maxMove);
    maxMove += 200;
    laser.setScale(0.4);
    Drawing::drawString("ARDUINO",-w1/2, SIN((i*10) % 360)/100.);
    if (i>100) {
      laser.resetMaxMove();
      laser.setScale(2 + SIN((i*10)%360) / 10000.);
      Drawing::drawObject(draw_heart, sizeof(draw_heart)/4, -centerX, -centerY);
    }
    if (i>150) {
      maxMove -= 400;
    }
  }
  laser.resetMaxMove();
}

// currently unused, some more objects
void drawObjects()
{
  int count = 100;
  laser.setScale(2);
  laser.setOffset(0,0);

  for (int i = 0;i<count;i++) Drawing::drawObject(draw_island, sizeof(draw_island)/4);
  for (int i = 0;i<count;i++) Drawing::drawObject(draw_glasses, sizeof(draw_glasses)/4);
  for (int i = 0;i<count;i++) Drawing::drawObject(draw_smoking, sizeof(draw_smoking)/4);
}

// draws text as scroller from right to left
void drawScroller(String s, float scale = 0.5, int offsetY = 2048, int speed = 100)
{
  int charW = Drawing::advance('I'); // worst case: smallest char
  int maxChar = (4096. / (charW * scale) );
  // some senseful max buffer, don't use a very small scale...
  char buffer[100];
  for (int j = 0;j<maxChar;j++) {
    buffer[j] = ' ';
  }
  laser.setOffset(0,offsetY);
  laser.setScale(scale);
  int scrollX = 0;
  for (int c = 0; c < s.length() + maxChar; c++) {
    int currentScroll = Drawing::advance(buffer[0]);
    while (scrollX < currentScroll) {
      long time = millis();
      int x = -scrollX;;
      int y = 0;
      bool somethingDrawn = false;
      for (int i = 0;i<maxChar;i++) {
        if (buffer[i] != ' ') {
          somethingDrawn = true;
        }
        x += Drawing::drawLetter(buffer[i], x, y);
        if (x > 4096 / scale) {
          break;
        }
      }
      if (!somethingDrawn) { scrollX = currentScroll; }
      scrollX += speed / scale;
      long elapsed = millis() - time;
      if (elapsed < 50) { delay(50-elapsed); }

    }
    scrollX -= currentScroll;
    for (int k = 0;k<maxChar-1;k++) {
      buffer[k] = buffer[k+1];
    }
    if (c<s.length()) {
      buffer[maxChar-1] = s[c];
    } else{
      buffer[maxChar-1] = ' ';
    }
  }
}

void noise(int ox, int oy) {
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.sendto(ox,oy);
  laser.sendto(random(0,4095),random(0,4095));
  //Serial.println("--------------------------");
  /*laser.on();
  for (int r = 0;r<=100;r)
  {    
    laser.sendto(r/100,0);
  }
  laser.off();*/
      
}
void rect(int x1, int y1, int x2 , int y2) {
  const int scale = 2;
  int count = 1;
  laser.setScale(1);
  laser.setOffset(0,0);
//  laser.on();
//  laser.sendto(x1,y1);
//  laser.sendto(x1,y1+y2);
//  laser.sendto(x1+x2,y1+y2);
//  laser.sendto(x1+x2,y1);
//  laser.sendto(x1,y1);
//  laser.off();
    laser.drawline(x1,y1,x1,y1+y2);
    laser.drawline(x1,y1+y2,x1+x2,y1+y2);
    laser.drawline(x1+x2,y1+y2,x1+x2,y1);
    laser.drawline(x1+x2,y1,x1,y1);
 
      
}
void circle2() {
  const int scale = 1;
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.sendto(SIN(0), COS(0));
  laser.on();
  for (int r = 5;r<=3600;r+=5)
  {    
    laser.sendto(SIN(r*2), COS(r*3));
  }
  laser.off();
}

void drawCircle(int x1, int y1, float s, float sides) {
  laser.setScale(s);
  laser.setOffset(x1,y1);
  laser.off();
  circle(sides);
}
void drawCrosshair(){
  //drawCircle(2048,2048,0.75,12);
  //drawCircle(2048,2048,1,12);
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.drawline(1,2048,2048,2048);
  laser.drawline(2048,2048,4095,2048);
  laser.drawline(2048,1,2048,2048);
  laser.drawline(2048,2048,2048,4095);
  laser.drawline(0,0,4095,4095);
  laser.drawline(4095,0,0,4095);

  //laser.off();
  //laser.drawline(x1,y1,x1,y2);
}

void drawGrid(int sx ,int sy, int closed){
  int xgrid = 4096/sx;
  int ygrid = 4096/sy;
  laser.setScale(1);
  laser.setOffset(0,0);
  if(closed){
    rect(0,0,4095,4094);
    }
  //for (int l = 4096/sx;r<=3600;r+=5)
  for(int l = 4096/sx ; l<xgrid*sx; l+=xgrid){
    laser.drawline(l,1,l,4096);
  }
  for(int p = 4096/sy ; p<ygrid*sy; p+=ygrid){
    laser.drawline(1,p,4096,p);
  }
}

void loc(int x, int y,int grow, int grid){
  for(int i = 0; i<grow*5 ; i+=grow){
  for(int t = 0; t<2 ; t=t+1){
    if(grid){
  drawGrid(3,3,true);
    }
  rect(x-(i/2),y-(i/2),i,i);
  }
  }
  }
  
void fiveSquares(){
   rect(0,0,500,500);
  rect(2048-250,2048-250,500,500);
  rect(4095-500,4095-500,500,500);
  rect(0,4095-500,500,500);
  rect(4095-500,0,500,500);
}
void loop() {
 // loc(1000,3000, 200,true);
  //noise(1,1);
  //noise(50,3500);
  //noise(4095,4095);
 // noise(2048,2048);
 // drawCircle(2048,2048,.5,12);
//drawGrid(3,3,true);
  //noise(3500,50);
  //rect(1,1,4000,1000);
   //rect(1,1,1000,4000);

 //drawCircle(2047,2047,1,12);
   //fiveSquares();
  //drawCrosshair();
  //countDown();
  //letterEffect();
  //presents();
  //arduino();
  //laserShow(); 
  //drawPlane();
  //drawLogo();
  //drawScroller(String("WAZAP!"),2,1024,500);
  //drawScroller(String("MRAC"),2,2048,500);
  //drawWeLove();
  //drawArduino2DRotate();
  whatAbout3D();
  //rotateCube(200);
  //drawObjects();
  //drawHeart();
  //drawBike();
  //circle();
  //drawArduino3D();
  //drawScroller(String("SOURCE CODE AVAILABLE ON GITHUB!"),0.25,2048,100);
  //drawObama();
//  drawObjects();
 // jumpingText();
}
