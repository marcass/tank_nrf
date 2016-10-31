 
 
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
RF24 radio(7,8);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

//uint32_t displayTimer = 0;
//Configure the sensor array for sensible messaging to myController
//input each sensor node id (as assigned in arduino sketch) into the array in setup - need to declare here
int tank = 5;
int timer = 1;
//place number of sensors in ths variable so that for loop iterates correctly
int sens = 2;
//establish array variables for access
int n;
int node;
//create string constant for presentation to myContollor
String present = ";1;0;0;15;35"; //arbitary senosr ID assignment as only one sensor on the node, message type, ack and subtype (S_DISTANCE) presentation of sensor
//Create string for "set" in myControllor
//String set = ";1;1;0;13;";
//String set_dat;


void setup() {
  Serial.begin(115200);
  //initialise the array with th number of sensors (same as variable "sens")
  int array[2] = {};
  //enter the node id's into the array
  array[0] = timer;
  array[1] = tank;

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  //Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();
  //present sensors to myController
  for ( n = 0; n < sens; n++) {
    node_id = array[n];
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
      // Display the incoming values from the sensor nodes
      case 'M': 
        network.read(header,&dat,sizeof(dat)); 
        //format for myController: http://www.mysensors.org/download/serial_api_14#variable-types
        node = mesh.getNodeID(header.from_node);
        Serial.print("NodeID = ");
        Serial.println(node);
        switch(node){
          case 5:
           Serial.print("5;1;1;0;13;"); //arbitary senosr ID assignment as only one sensor on the node, message type, ack and subtype (S_DISTANCE)
           Serial.println(dat);
           break;
         case 1:
//          Serial.println("Got node 1");
          Serial.print("1;1;1;0;23;"); //arbitary senosr ID assignment as only one sensor on the node, message type, ack and subtype (S_CUSTOM)
          Serial.println(dat);
          break;
        }
      default: 
        network.read(header,0,0); 
        Serial.println(header.type);
        break;
    }
  }
}
