 
 
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

uint32_t displayTimer = 0;
int ID = 0;

void setup() {
  Serial.begin(115200);

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  //Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();
  Serial.println("5;1;0;0;15;35"); //arbitary senosr ID assignment as only one sensor on the node, message type, ack and subtype (S_DISTANCE) presentation of sensor

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
//        ID = mesh.getNodeID(header.from_node);
//        switch(ID){
//          case 5:  //us sensor on node
//            Serial.print(ID);
//            Serial.print(";");
            Serial.print("5;1;1;0;13;"); //arbitary senosr ID assignment as only one sensor on the node, message type, ack and subtype (S_DISTANCE)
//            break;
//          case 1: //timer module
//            Serial.print(ID);
//            Serial.print(";");
//            Serial.print("2;1;0;23;"); //arbitary senosr ID assignment as only one sensor on the node, message type, ack and subtype (S_CUSTOM)
//            break;
//        }         
        Serial.println(dat);
        break;
      default: 
        network.read(header,0,0); 
        Serial.println(header.type);
        break;
    }
  }
  
//  if(millis() - displayTimer > 5000){
//    displayTimer = millis();
//    Serial.println(" ");
//    Serial.println(F("********Assigned Addresses********"));
//     for(int i=0; i<mesh.addrListTop; i++){
//       Serial.print("NodeID: ");
//       Serial.print(mesh.addrList[i].nodeID);
//       Serial.print(" RF24Network Address: 0");
//       Serial.println(mesh.addrList[i].address,OCT);
//     }
//    Serial.println(F("**********************************"));
//  }
}
