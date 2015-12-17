//Whiteboard Serial Controller
#include <AccelStepper.h>
#include <Servo.h>
#include "WhiteboardSerialDuo.h"

// Define steppers
AccelStepper m1(AccelStepper::FULL4WIRE, 8, 9, 10, 11, true);
AccelStepper m2(AccelStepper::FULL4WIRE, 2, 3, 4, 5, true);

// Define servo
Servo s1;  
int servoPos = 0;

int STEPS = 200;
int DIAMETER = 5; // 5mm
double CIRCUMFERENCE = DIAMETER * PI; // 15.7 mm
double m2s = CIRCUMFERENCE / STEPS; // 0.019625 mm for step

int MOTOR_DISTANCE = 1155;
Point M1 = {0, 0};
Point M2 = {M1.x + MOTOR_DISTANCE, 0};

Point currentPoint = {MOTOR_DISTANCE/2, 200};
MotorVal initialStepper = getStepsFor(currentPoint);

double distance(Point a, Point b) {
  double dx = a.x - b.x;
  double dy = a.y - b.y;
  return sqrt(dx*dx + dy*dy);
}

void setup()
{
  //setup steppers
  m1.setMaxSpeed(1000);
  m1.setSpeed(1000);
  //m1.setAcceleration(0);
  m2.setMaxSpeed(1000);
  m2.setSpeed(1000);
  //m2.setAcceleration(0);
  // attach servo
  s1.attach(13);

  // initialize the serial port:
  Serial.begin(9600);
}

MotorVal getStepsFor(Point _pos) {
    MotorVal val;
    val.M1 = distance(M1, _pos)/m2s;
    val.M2 = distance(M2, _pos)/m2s;

    val.M1 -= initialStepper.M1;
    val.M2 -= initialStepper.M2;
    
    return val;
}

// Serial
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;

void loop() {
  serialEvent(); //call the function
  // print the string when a newline arrives:
  if (stringComplete) {
    parseCommand(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  if (m1.distanceToGo() != 0) m1.runSpeedToPosition();
  if (m2.distanceToGo() != 0) m2.runSpeedToPosition();
  //m1.run();
  //m2.run();
}

void parseCommand(String command) {
   Serial.println("RECEIVED: "+inputString);
   inputString.trim();
   inputString.toLowerCase();
   if (inputString=="pu") {
       penUp();
   } else if (inputString=="pd") {
      penDown();
   } else if (inputString=="w") {
       moveUp();
   } else if (inputString=="s") {
      moveDown();
   } else if (inputString=="a") {
      moveLeft();
   } else if (inputString=="d") {
      moveRight();
   } else if (inputString=="p") {
      drawPattern();
   } else if (inputString.startsWith("m")) {
     currentPoint.x = getValue(inputString, ':', 1).toInt();
     currentPoint.y = getValue(inputString, ':', 2).toInt();
     movePen();
   }
}

void movePen() {
//  Serial.println("X: " + String(currentPoint.x));
//  Serial.println("Y: " + String(currentPoint.y));
Serial.println(String(currentPoint.x) + " " + String(currentPoint.y));

  MotorVal target = getStepsFor(currentPoint);
  m1.moveTo(target.M1);
  m2.moveTo(target.M2);

  long d1 = m1.distanceToGo();
  long d2 = m2.distanceToGo();
  double s1 = 100;
  double s2 = 100;

  if (d1<d2) {
    s1 = s2*(double)d1/(double)d2;
  } else {
    s2 = s1*(double)d2/(double)d1;
  }

  m1.setSpeed((float)s1);
  m2.setSpeed((float)s2);
}

void moveLeft() {
  currentPoint.x -= 10;
  movePen();
}

void moveRight() {
  currentPoint.x += 10;
  movePen();
}

void moveUp() {
  currentPoint.y -= 10;
  movePen();
}

void moveDown() {
  currentPoint.y += 10;
  movePen();
}

void penUp() {
  s1.write(180);              
  delay(15);  
}

void penDown() {
   s1.write(0);              
   delay(15);  
}

void serialEvent() {
 while (Serial.available()) {
   // get the new byte:
   char inChar = (char)Serial.read();
   // add it to the inputString:
   inputString += inChar;
   // if the incoming character is a newline, set a flag
   // so the main loop can do something about it:
   if (inChar == '\n') {
     stringComplete = true;
   }
 }
}

String getValue(String data, char separator, int index) {
 int found = 0;
 int strIndex[] = { 0, -1  };
 int maxIndex = data.length()-1;
 for(int i=0; i<=maxIndex && found<=index; i++){
 if(data.charAt(i)==separator || i==maxIndex){
 found++;
 strIndex[0] = strIndex[1]+1;
 strIndex[1] = (i == maxIndex) ? i+1 : i;
 }
}
 return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void drawPattern() {
  int i,j;
//
//  for (i=0; i<5; i++) moveLeft();
//  while (m1.distanceToGo() != 0 && m2.distanceToGo() != 0) {
//    m1.runSpeedToPosition();
//    m2.runSpeedToPosition();
//  }
//
//  for (i=0; i<10; i++) {
//    Serial.println("Moving Down");
//    moveDown();
//    while (m1.distanceToGo() != 0 && m2.distanceToGo() != 0) {
//      m1.runSpeedToPosition();
//      m2.runSpeedToPosition();
//    }
//      
//    for (j=0; j<10; j++) {
//      if (i%2==0) moveRight();
//      else moveLeft();
//      
//      while (m1.distanceToGo() != 0 && m2.distanceToGo() != 0) {
//        m1.runSpeedToPosition();
//        m2.runSpeedToPosition();
//      }
//    }
//  }
  int dx = 30;
  float dyf = 0.75;
  float dyf2 = 0.5;
  int points[20][2] = { 
                      {577,200}, 
                      {577+(int)(dyf*dx), 200+dx},
                      {577+(int)(dyf*dx-dyf2*dyf*dx),200+dx},
                      {577+(int)(2*dyf*dx), 200+2*dx},
                      {577+(int)(2*dyf*dx-2*dyf2*dyf*dx), 200+2*dx},
                      {577+(int)(3*dyf*dx), 200+3*dx},
                      {577+(int)(3*dyf*dx-3*dyf2*dyf*dx), 200+3*dx},
                      {577+(int)(4*dyf*dx), 200+4*dx},
                      {577+dx/2, 200+4*dx},
                      {577+dx/2, 200+5*dx},
                      {577-dx/2, 200+5*dx},
                      {577-dx/2, 200+4*dx},
                      {577-(int)(4*dyf*dx), 200+4*dx},
                      {577-(int)(3*dyf*dx-3*dyf2*dyf*dx), 200+3*dx},
                      {577-(int)(3*dyf*dx), 200+3*dx},
                      {577-(int)(2*dyf*dx-2*dyf2*dyf*dx), 200+2*dx},
                      {577-(int)(2*dyf*dx), 200+2*dx},
                      {577-(int)(dyf*dx-dyf2*dyf*dx), 200+dx},
                      {577-(int)(dyf*dx), 200+dx},
                      {577,200}
//{577, 200},
//{584, 210},
//{580, 210},
//{592, 220},
//{584, 220},
//{599, 230},
//{588, 230},
//{607, 240},
//{627, 240},
//{627, 250},
//{527, 250},
//{527, 240},
//{547, 240},
//{566, 230},
//{555, 230},
//{570, 220},
//{562, 220},
//{574, 210},
//{570, 210},
//{577, 200}
                    }; 

  for (i=0; i<20; i++) {
    currentPoint.x = points[i][0];
    currentPoint.y = points[i][1];
    movePen();
    while (!(m1.distanceToGo() == 0 && m2.distanceToGo() == 0)) {
//      Serial.println(String(m1.distanceToGo()) + " " + String(m2.distanceToGo()));
      if (m1.distanceToGo() != 0)
        m1.runSpeedToPosition();
       if (m2.distanceToGo() != 0)
        m2.runSpeedToPosition();
     }
  }
}

