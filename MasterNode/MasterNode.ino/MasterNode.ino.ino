 
 
 /** RF24Mesh_Example_Master.ino by TMRh20
  * 
  *
  * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
  * will receive all data from sensor nodes.
  *
  * The nodes can change physical or logical position in the network, and reconnect through different
  * routing nodes as required. The master node manages the address assignments for the individual nodes
  * in a manner similar to DHCP.
  *
  */



  
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>

/***** Configure the chosen CE,CS pins *****/
RF24 radio(8,7);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

uint32_t displayTimer = 0;
int ID = 0;

/**************************************************
 * Configure the sensor array for sensible messaging to myController
 * 
 * Tank detail can be viewd here: https://paper.dropbox.com/doc/Arduino-Water-Tank-Level-kHoAXxvG9oPQK7naydd9i
 * Name              |Tank no.| Sensor
 * ==============================================================
 * Top tank          | 1      | Y
 * Noel's Break tank | 2      | Y
 * Sal's Bush   tank | 3      | Y
 * Yards             | 4      | N
 * Home hill         | 5      | N
 * Home              | 0      | N
 */

const int TOP = 1;
const int NOEL = 2;
const int SAL = 3;
const int SENS = 3;
int node_id;
// presentation described here: https://www.mysensors.org/download/serial_api_20
String present = ";1;0;0;15";

void setup() {
  Serial.begin(115200);

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();
  //present sensors to MyController
  int sensors[] = {TOP, NOEL, SAL};
  for (int n = 0; n < SENS; n++) {
    node_id = sensors[n];
    String present_node = (node_id + present);
    Serial.println(present_node);
  }

}


void loop() {    

  // Call mesh.update to keep the network updated
  mesh.update();
  
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();
  
  
  // Check for incoming data from the sensors
  if(network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    
    uint32_t dat=0;
    switch(header.type){
      // Display the incoming millis() values from the sensor nodes
      case 'M': 
        network.read(header,&dat,sizeof(dat)); 
//        Serial.print("Node id is ");
//        Serial.println(mesh.getNodeID(header.from_node));
        Serial.print(mesh.getNodeID(header.from_node));
        //set,req described here: https://www.mysensors.org/download/serial_api_20
        Serial.print(";1;1;1;13;"); // = "SENSOR_ID";"COMMAND (update value)";"ACK (this is an ack message);"TYPE" (distance);
        Serial.println(dat);//payload
        break;
      default: 
        network.read(header,0,0); 
        Serial.println(header.type);
        break;
    }
  }
  
  if(millis() - displayTimer > 5000){
    displayTimer = millis();
    Serial.println(" ");
    Serial.println(F("********Assigned Addresses********"));
     for(int i=0; i<mesh.addrListTop; i++){
       Serial.print("NodeID: ");
       Serial.print(mesh.addrList[i].nodeID);
       Serial.print(" RF24Network Address: 0");
       Serial.println(mesh.addrList[i].address,OCT);
     }
    Serial.println(F("**********************************"));
  }
}
