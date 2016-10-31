  
/** RF24Mesh_Example.ino by TMRh20
 *
 * This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
 * master node.
 * The nodes will refresh their network address as soon as a single write fails. This allows the
 * nodes to change position in relation to each other and the master node.
 */


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>

//sesnor stuff
#include <NewPing.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

//Configure tig adn echo pins

#define TRIGGER_PIN  4
#define ECHO_PIN     5
#define MAX_DISTANCE 250
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

/* User Configuration: nodeID - A unique identifier for each radio. Allows addressing
 * to change dynamically with physical changes to the mesh.
 *
 * In this example, configuration takes place below, prior to uploading the sketch to the device
 * A unique value from 1-255 must be configured for each node.
 * This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.
 *
 **/
#define nodeID 4


//
// Sleep declarations
//

typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;

void setup_watchdog(uint8_t prescalar);
void do_sleep(void);

const short sleep_cycles_per_transmission = 4;
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;



//uint32_t displayTimer = 0;
uint32_t dist = 0;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
  unsigned long dist;
};

void setup() {

  Serial.begin(115200);
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  //setup watchdog timer
  setup_watchdog(wdt_2s); //2s delay x 4: or see https://tmrh20.github.io/RF24Network/classRF24Network.html#acb09129760ac9b171833af3055b2b6f5 for alternative
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}



void loop() {

  mesh.update();
  dist = sonar.ping_cm();
  if (!mesh.write(&dist, 'M', sizeof(dist))) {

    // If a write fails, check connectivity to the mesh network
    if ( ! mesh.checkConnection() ) {
      //refresh the network address
      Serial.println("Renewing Address");
      mesh.renewAddress();
    } 
    else {
      Serial.println("Send fail, Test OK");
    }
  } 
    
  else {
    Serial.print("Send OK: ");
    Serial.print("Distance to water: ");
    Serial.print(dist);
    Serial.println("cm");
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.print(payload.ms);
    Serial.print(" Distance to water ");
    Serial.println(payload.dist);
  }
  
  // Power down the radio.  Note that the radio will get powered back up
  // on the next write() call.
  radio.powerDown();  
  // Sleep the MCU.  The watchdog timer will awaken in a short while, and
  // continue execution here.
  while( sleep_cycles_remaining )
    do_sleep();  
  sleep_cycles_remaining = sleep_cycles_per_transmission;
}

ISR(WDT_vect)
{
  --sleep_cycles_remaining;
}

void do_sleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out
}



