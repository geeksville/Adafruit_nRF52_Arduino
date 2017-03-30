/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/*
 * Sketch use HID Consumer Key API to send Volume Down key when
 * PIN_SHUTTER is grounded. Which cause mobile to capture photo
 * when you are in Camera App
 */
#include <bluefruit.h>

BLEDis bledis;
BLEHidAdafruit blehid;

#define PIN_SHUTTER   27

void setup() 
{
  pinMode(PIN_SHUTTER, INPUT_PULLUP);
  
  Serial.begin(115200);

  Serial.println("Bluefruit52 HID Camera Shutter Example");
  Serial.println("--------------------------------------");

  Serial.println();
  Serial.println("Go to your phone's Bluetooth settings to pair your device");
  Serial.println("then open an Camera application");

  Serial.println();
  Serial.printf("Wire your Pin %d to GND to capture picture\n", PIN_SHUTTER);
  Serial.println();

  Bluefruit.begin();
  Bluefruit.setName("Bluefruit52");

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  /* Start BLE HID
   * Note: Apple requires BLE device must have min connection interval >= 20m
   * ( The smaller the connection interval the faster we could send data).
   * However for HID and MIDI device, Apple could accept min connection interval 
   * up to 11.25 ms. Therefore BLEHidAdafruit::begin() will try to set the min and max
   * connection interval to 11.25  ms and 15 ms respectively for best performance.
   */
  blehid.begin();

  /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
   */
  /* Bluefruit.setConnInterval(9, 12); */

  // Setup Advertising Packet
  setupAdv();

  // Start Advertising
  Bluefruit.Advertising.start();
}

void setupAdv(void)
{  
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
}

void loop() 
{
  // Connected and Bonded/Paired
  if ( Bluefruit.connected() && Bluefruit.connPaired() )
  {
    // Pin is wired to GND
    if ( digitalRead(PIN_SHUTTER) == 0 )
    {
      // Turn on LED Red when start sending
      digitalWrite(LED_RED, 1);
      
      // Send Volumn Down key press
      // Check BLEHidGerneric.h for list of defined consumer usage code
      blehid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_DECREMENT);

      // Delay a bit between repots
      delay(10);
      
      // Send key release
      blehid.consumerKeyRelease();

      // Turn off LED Red
      digitalWrite(LED_RED, 0);

      // Delay to avoid constant capturing
      delay(500);
    }
  }
}

