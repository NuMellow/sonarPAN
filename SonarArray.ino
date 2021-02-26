#include "ListLib.h";

const unsigned int BAUD_RATE=9600;

//Target, Position of the target
unsigned int vertTarget;
unsigned int horiTarget;

//CurentPos, Current Position of the robot
unsigned int vertPos;
unsigned int horiPos;

//vars for robust calibrate
unsigned int NUM_SONAR; //Number of sonar sensors connected
List<int> TRIG_PINS;
List<int> ECHO_PINS;
List<int> defaultDistances; //Keeps track of default distances measured by the sensors

unsigned int NUM_HORIZONTAL;
unsigned int NUM_VERTICAL;
List<int> horizontalSensors; //Stores index values of horizontal sensors;
List<int> verticalSensors;  //Stores index values of vertical sensors;


/* 
 * CALIBRATE SENSORS
 * This method gets the number of sonar sensors to be set up by the user
 * It also requests the user to enter which pins the sensors' echo and trig
 * pins are connected to on the arduino
 */
void calibrateSensors() {
  Serial.print("Enter number of sonar sensors: ");
  while(Serial.available() ==0){
    
  }
  NUM_SONAR = Serial.parseInt();

  for(int i =0; i < NUM_SONAR; i++)
  {
    int num = i+1;
    String trigPrompt = "Enter TRIG_PIN " + String(num) + ": ";
    Serial.print(trigPrompt);
    while(Serial.available()==0)
    {
      
    }
    TRIG_PINS.Add(Serial.parseInt());
    Serial.println();
    String echoPrompt = "Enter ECHO_PIN " + String(num) + ": ";
    Serial.print(echoPrompt);
    while(Serial.available()==0)
    {
      
    }
    ECHO_PINS.Add(Serial.parseInt());
    Serial.println();
  }

  for(int i= 0; i < NUM_SONAR; i++)
  {
    Serial.println("TRIG " + String(i+1) + "= " + String(TRIG_PINS[i]));
    Serial.println("ECHO " + String(i+1) + "= " + String(ECHO_PINS[i]));
  }
}

/*
 * SET ORIENT SENSORS
 * This method sets the number of horizontally and vertically facing sonar sensors
 * and saves their index values. This will be useful when calculating the position
 * of a moving object, by knowing which set of sensors to use in the calculation.
 * 
 * It takes a boolean argument that determines whether setting the horizontal
 * sensors(true) or the vertical(false).
 */
void setOrientSensors(bool horizontal) {
  int count = 0;
  String orientation = "Horizontal";
  if(!horizontal)
    orientation = "Veritical";

  Serial.println("Enter number of " + orientation + " sensors: ");
  while(Serial.available() == 0){}
  count = Serial.parseInt();
  if(!horizontal)
  {
    NUM_VERTICAL = count;
  }
  else
    NUM_HORIZONTAL = count;
  Serial.println();

  Serial.println("Enter sensor number: ");
  while(Serial.available() ==0){}
  for(int i=0; i< count; i++)
  {
    if(horizontal)
      horizontalSensors[i] = Serial.parseInt() -1;
    else
      verticalSensors[i] = Serial.parseInt() -1;
  }
}

/*
 * PRINT ORIENT SENSORS
 * This method prints the index values of the horizontal and vertical
 * sensors.
 * 
 * It takes a boolean arguement that determines whether to print the
 * horizontal sensor indexes(true) or the vertical(false)
 */
void printOrientSensors(bool horizontal)
{
  if(horizontal)
  {
    Serial.println("Horizontal Sensors:");
    for(int i=0; i < NUM_HORIZONTAL; i++)
    {
      Serial.println(horizontalSensors[i]);
    }
  }
  else
  {
    Serial.println("Vertical Sensors:");
    for(int i=0; i < NUM_VERTICAL; i++)
    {
      Serial.println(verticalSensors[i]);
    }
  }
  
  Serial.println();
}

/*
 * SET_DEFAULT_DISTANCES
 * This method gets the default distances for each sensor in the room.
 * This allows the system to later be able to determine the robot position
 * and send appropriate navigation commands
 */
void setDefaultDistances(){
  for(int i = 0; i < NUM_SONAR; i++)
  {
    int set = 0;
    int distance = -1;
    while(set == 0)
    {
      distance = getDistance(i);
      Serial.println("Sensor " + String(i+1) + " distance: " + String(distance));
      Serial.println("Is this okay? (1=yes, 0=no)");
      
      while(Serial.available() == 0){}

      set = Serial.parseInt();
      
    }

    defaultDistances[i] = distance;
      
  }

  printDefaultDistances();
}

/*
 * PRINT DEFAULT DISTANCES
 * This method prints out the default distance measured by the sensors when
 * no moving obstancles are present
 */
void printDefaultDistances()
{
  for(int i = 0; i < NUM_SONAR; i++)
  {
    Serial.println("Sensor " + String(i+1) + " default: " + String(defaultDistances[i]));
  }
}

/*
 * CALCULATE POSITION
 * This method reads in the horizontal and vertical distances measured by the sensor
 * network. If none of the sensors detect the robot, -1 is sent to the method and the position
 * is not updated. Otherwise the global position variables are updated with the sensor readings.
 */
void calculatePosition(int horizontalDist, int verticalDist)
{
  /*
    Handle conditions when 
      horizontalDist and verticalDist are known
      horizontalDist = known and verticalDist = unknown(-1)
      horizontalDist = unknown(-1) and verticalDist = known
      horizontalDist = unknown(-1) and verticalDist = unknown(-1)
   */

  if(horizontalDist >= 0 && verticalDist >= 0)
  {
    //Both values are known. update the position
    vertPos = verticalDist;
    horiPos = horizontalDist;
  }
  else if(horizontalDist >= 0 && verticalDist < 0)
  {
    //Horizontal position is known. Update horizontal, use old Vertical
    horiPos =horizontalDist;
    
  }
  else if(horizontalDist < 0 && verticalDist >= 0)
  {
    //Vertical position in known. Update vertical, use old Horizontal
    vertPos = verticalDist;
  }
  else
  {
    //None are known, use old position, (or stop robot)
    
  }
}

/*
 * ACTION
 * The action method compares the robot position with the target position to determine
 * what action the robot should take e.g. GO FORWARD, TURN LEFT, END etc.
 */
void action()
{
  String commands[] = {"FWD","RFWD","R","RBCK","BCK","LBCK","L","LFWD"};
  unsigned int index = -1;
  
  if((vertPos - vertTarget) > 0)
  {
    index = 0;
    if((horiPos - horiTarget) > 0)
      index += 1;    
    else
      index += 7;
  }
  else
  {
    index = 4;
    if((horiPos - horiTarget) > 0)
      index -= 1;    
    else
      index += 1;
  }

  
}

void allTrigLow()
{
  for(int i =0; i < NUM_SONAR; i++)
  {
    digitalWrite(TRIG_PINS[i], LOW);
  }
}

int getDistance(int index)
{
  digitalWrite(TRIG_PINS[index], HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PINS[index], LOW);

  const unsigned long duration = pulseIn(ECHO_PINS[index], HIGH);
  int distance = -1;
  if(duration==0){
    Serial.println("Warning: no pulse from sensor " + String(index+1) );
  }
  else{
    distance = duration/29/2;
  }
  return distance;
}
/*
 * Code to run at start up.
 * Should call calibrateSensors and setTarget
 */
void setup() {
  Serial.begin(BAUD_RATE);
  calibrateSensors();

  for(int i = 0; i<NUM_SONAR; i++)
  {
    pinMode(TRIG_PINS[i], OUTPUT);
    pinMode(ECHO_PINS[i], INPUT);
  }

  setOrientSensors(true);
  printOrientSensors(true);

  setOrientSensors(false);
  printOrientSensors(false);
  
  //This method determines the default distances of the sensors return when there is no
  //robot in "sight".
  setDefaultDistances();
  
}


/*
 * Code running continuously
 */
void loop() {
  allTrigLow();
  delayMicroseconds(2);

  int distance[NUM_SONAR];
  for(int i = 0; i < NUM_SONAR; i++)
  {
    distance[i] = getDistance(i);
    Serial.println("Sensor " + String(i+1) + ": " + String(distance[i]));
  }

  //Method that finds the position of the robot by sensing which horizontal and vertical sensors
  //are not measuring their default distances (Robot not in sight) and use those distances to
  //determine the location of the robot.

//  int horizontalSensors[] = {distance1, distance3};
//  int verticalSensors[] = {distance2};
//  int sensHoriz = -1;
//  int sensVert = -1;
//  for (int i = 0; i < sizeof(defaultDistances); i+=2)
//  {
//    if(horizontalSensors[i] < defaultDistances[i])
//    {
//      sensHoriz = i;
//      break;
//    }
//  }
//
//  for(int i = 1; i < sizeof(defaultDistances); i+=2)
//  {
//    if(verticalSensors[i] < defaultDistances[i])
//    {
//      sensVert = i;
//      break;
//    }
//  }
//
//  if(sensHoriz >= 0 && sensVert >= 0)
//  {
//    calculatePosition(horizontalSensors[sensHoriz], verticalSensors[sensVert]);
//  }
//  else if(sensHoriz >= 0 && sensVert < 0)
//  {
//    calculatePosition(horizontalSensors[sensHoriz], sensVert);
//  }
//  else if(sensHoriz < 0 && sensVert >= 0)
//  {
//    calculatePosition(sensHoriz, verticalSensors[sensVert]);
//  }
//  else
//  {
//    calculatePosition(sensHoriz, sensVert);
//  }
  
  Serial.println();
  Serial.println();
  delay(2000);

}
