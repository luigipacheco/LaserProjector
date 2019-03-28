// See LICENSE file for details
// Copyright 2016 Florian Link (at) gmx.de

#include "Laser.h"
#include "Drawing.h"
#include "Cube.h"
#include "Objects.h"
#include "Logo.h"
#include <Servo.h>

Servo servoVer; //Vertical Servo
Servo servoHor; //Horizontal Servo


int x;
int y;
int a;
int b;
int c;

int prevX;
int prevY;
// Create laser instance (with laser pointer connected to digital pin 5)
Laser laser(6);

void setup()
{  
  laser.init();
  Serial.begin(9600);
  servoVer.attach(5); //Attach Vertical Servo to Pin 5
  servoHor.attach(3); //Attach Horizontal Servo to Pin 6
  servoVer.write(90);
  servoHor.write(90);
}

void Pos()
{
  if(prevX != x || prevY != y)
  {
    int servoX = map(x, 640, 0, 30, 150); //int servoX = map(x, 640, 0, 1, 179);
    int servoY = map(y, 480, 0, 30, 150); //int servoY = map(y, 480, 0, 1, 179);

    /*servoX = min(servoX, 179);
    servoX = max(servoX, 70);
    servoY = min(servoY, 179);
    servoY = max(servoY, 95);*/
    
    servoHor.write(servoX);
    servoVer.write(servoY);
    //servoHor.write(x);
    //servoVer.write(y);
  }
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

void drawTest()
{
  laser.setScale(0.5);
  laser.setOffset(0,0);
  Drawing::drawObject(draw_test, sizeof(draw_test));
 
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

void loc(int x, int y,int grow, int grid, String text, int ms){
  for (int p = 0;p<ms ;p++){
  for(int i = 0; i<grow*5 ; i+=grow){
  for(int t = 0; t<2 ; t=t+1){
    if(grid){
  drawGrid(1,1,true);
    }
  rect(x-(i/2),y-(i/2),i,i);
  laser.setOffset(x+500,y-200);
  laser.setScale(0.3);
  Drawing::drawString(text,0,0,1);
      }
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
void bounce(int ms){
  laser.setScale(1);
  laser.setOffset(0,0);
  float xpos = 2048;
  float ypos= 0;    // Starting position of shape    
  float xspeed = 100;  // Speed of the shape
  float yspeed = 100;  // Speed of the shape
  int xdirection = 2;  // Left or Right
  int ydirection = 1;  // Top to Bottom
  int width = 4095;
  int height = 4095;
  float scale = 0.2;
  int rad = 500;
  drawCircle(xpos, ypos, scale, 8);
  for (int i=0; i<ms;i++){
  xpos = xpos + ( xspeed * xdirection );
  ypos = ypos + ( yspeed * ydirection );

  if (xpos > width || xpos < 0) {
    xdirection *= -1;
  }
  if (ypos > width || ypos < 0) {
    ydirection *= -1;
  }
  drawCircle(xpos, ypos, scale, 8);
  drawGrid(2,2,false);
  }
}
void simpleText(String text, int x, int y,float s, int ms)
{
 // int w1 = Drawing::stringAdvance("WHAT");
 for (int i = 0;i<ms ;i++){
  laser.setOffset(0,0);
  laser.setScale(s);
  Drawing::drawString(text,x,y,1);
 }
  
}
void drawPolygon(int arr){
  int arrsize = sizeof(arr);
  Serial.println(arrsize);
}
void measure(int x1, int y1, int x2, int y2){
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.drawline(x1,y1,x2,y2);
}

void plug(int x,int y, int ms){
  for (int i = 0;i<ms ;i++){
  int sx = 600; 
  int sy = 600;
  //rect(x-(sx/2),y-(sy/2),sx,sy);
  drawCircle(x,y,.3,12);
  drawCircle(x-(sx/4),y,.05,12);
  drawCircle(x+(sx/4),y,.05,12);
  drawGrid(3,3,false);
  laser.setOffset(x+(sx/2)+100,y);
  laser.setScale(0.3);
  Drawing::drawString("220 V",0,0,1);
  }}
  void window(int x,int y,int sx, int sy, String text, int ms){
     for (int i = 0;i<ms ;i++){
  rect(x-(sx/2),y-(sy/2),sx,sy);
  rect(x-(sx/2-100),y-(sy/2-100),sx-200,sy-200);
  laser.setOffset(x+(sx/2)+100,y);
  laser.setScale(0.15);
  Drawing::drawString(text,0,0,1);
  drawGrid(1,1,true);
  }}
  void quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.drawline(x1,y1,x2,y2);
  laser.drawline(x2,y2,x3,y3);
  laser.drawline(x3,y3,x4,y4);
  laser.drawline(x4,y4,x1,y1);
  }
  void triangle(int x1, int y1, int x2, int y2, int x3, int y3){
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.drawline(x1,y1,x2,y2);
  laser.drawline(x2,y2,x3,y3);
  laser.drawline(x3,y3,x1,y1);
  }
  void pent(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,int x5, int y5){
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.drawline(x1,y1,x2,y2);
  laser.drawline(x2,y2,x3,y3);
  laser.drawline(x3,y3,x4,y4);
  laser.drawline(x4,y4,x5,y5);
  laser.drawline(x5,y5,x1,y1);
  }
  void sebCube(){
   laser.setScale(1);
   laser.setOffset(0,0);

   laser.drawline(1683,3778,1683,640);
   laser.drawline(1683,640,3951,546);
   laser.drawline(3951,546,2206,444);
   laser.drawline(2206,444,2207,2969);
   laser.drawline(2207,2969,46,3274);
   laser.drawline(46,3274,1683,3778);
   laser.drawline(1683,3778,3962,3388);
   laser.drawline(3962,3388,2207,2969);
   laser.drawline(54,518,1683,640);
   laser.drawline(54,518,46,3274);
   laser.drawline(2206,444,54,518);
   laser.drawline(3951,546,3962,3388);
  }
  void sebCube2(){
  quad(3951,546,3962,3388,2207,2969,2206,444);
  quad(2206,444,3951,546,1683,640,54,518);
  quad(54,518,1683,640,1683,3778,46,3274);
  quad(46,3274,1683,3778,3962,3388,2207,2969);
  laser.setScale(1);
  laser.setOffset(0,0);
  laser.drawline(2207,2969,3962,3388);
  laser.drawline(3962,3388,3951,546);
  //laser.drawline(3951,546,2206,444);
  }
  
  
 void loop() {
  
  if(Serial.available() > 0)
  {
    if(Serial.read() == 'X')
    {//
      //Serial.println("HI x");
      x = Serial.parseInt();
      if(Serial.read() == 'Y')
      {
        //Serial.println("HI Y");
        y = Serial.parseInt();
        Pos();
        if(Serial.read() == 'A')
        {
          a = Serial.parseInt();
          if(Serial.read() == 'B')
          {
          b = Serial.parseInt();
          if(Serial.read() == 'C')
            {
            c = Serial.parseInt();
            } 
        }
      }
    }
    }
    while(Serial.available() > 0)
    {
      Serial.read();
    }
  }
  //sebCube();
  //simpleText("MRAC 2019",20,20,0.5,50);
  //window(1000,3000, 500, 1000,"W1",100);
  //quad(500,1000,500,3000,2000,3900,2000,1500);
  //quad(2000,3900,3500,3000,3500,1000,2000,1500);
  //quad(500,1000,2000,1500,3500,1000,2000,500);
  //pent(1,1,1,500,1300,2500,3500,3500,1000,500);
  //sebCube2();
//  quad(1886,1172,3933,1514,1453,1635,63,1229);
//  quad(63,1229,1453,1636,1460,3318,70,2366);
//  quad(70,2366,1460,3318,3935,3034,1890,2232);
//  quad(1890,2232,3935,3034,3933,1514,1886,1172);

  //triangle(1,1,500,500,1300,2500);
  //window(3000,3000,600,1500,"W2",100);
  //window(2000,1000,500,500,"W3",100);
  //const int Array[][2] = {{1,2},{3,4},{5,6}};
  //drawPolygon(Array);
  //drawTest();
  //plug(2000,1500,100);
  //simpleText("I D",200,2000,0.4,1);
  //simpleText("DUDE1",200,500,0.4,1);
  //loc(2040,2040, 200,false,"TEST 1 ",100);
  // bounce(100);
  //loc(3500,3500, 200,false,"",1);
  //measure(0,1,4000,4500);
  //globe(20);
  //noise(1,1);
  //noise(50,3500);
  //noise(4095,4095);
 // noise(2048,2048);
  //drawCircle(2048,2048,.5,24);
  //drawCircle(2048,2048,.05,24);
//drawGrid(3,3,true);
  //noise(3500,50);
  //rect(1,1,4000,1000);
 // rect(1,1,1000,4000);
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
  //drawScroller(String("DUDE1!"),1,1024,500);
  //drawScroller(String("MRAC"),1,2048,500);
  //drawWeLove();
  //drawArduino2DRotate();
  //whatAbout3D();
  Vector3i vn = {a, b , c};
  rotateCube(1,vn);
  //drawObjects();
  //drawHeart();
  //drawBike();
  //circle();
  //drawArduino3D();
  //drawScroller(String("SOURCE CODE AVAILABLE ON GITHUB!"),0.25,2048,100);
//  drawObjects();
 // jumpingText();
}
