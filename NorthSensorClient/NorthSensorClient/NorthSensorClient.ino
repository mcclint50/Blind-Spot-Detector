// Author: Colin McClintic
// Computer Engineering Senior Project - Blind Spot Detector
// Description: This program connects to a TKDServer setup in LEDDisplayServer.ino and
//    reads the output of the North proximity sensor. The output is translated into
//    the distance measured by the sensor and that data is sent to the server.
//------------------------------------------------------------------------------------
  #include <Wire.h>
  #include <ESP8266WiFi.h>
//------------------------------------------------------------------------------------
// Defining I/O Pins
//------------------------------------------------------------------------------------
  #define       LED0      2         // WIFI Module LED
  #define       LED1      16        // Connectivity With Client #1
  #define       LED2      0         // Test Output
  #define       Trigger   5         // Trigger signal
  #define       Echo      4         // Echo signal
  #define       TWI_FREQ  400000L   // I2C Frequency Setting To 400KHZ
//------------------------------------------------------------------------------------
// Sensor Variables
//------------------------------------------------------------------------------------
  const String  Sensor            = "North";
  long          duration;
  int           distance;
  int           maxDist           = 144;
//------------------------------------------------------------------------------------
// LED Delay Variables
//------------------------------------------------------------------------------------
  int           LEDState          = LOW;
  unsigned long CurrMillis        = 0;
  unsigned long PrevMillis        = 0;
  unsigned long Interval          = 1000;
//------------------------------------------------------------------------------------
// Authentication Variables
//------------------------------------------------------------------------------------
  char*         TKDssid;
  char*         TKDpassword;
  IPAddress     TKDServer(192,168,4,1);
  WiFiClient    TKDClient;
//====================================================================================

  void setup() 
  {
    /* ----------------------------------------------------------------------
     * Setting The I2C Pins SDA, SCL
     * Because We Didnt Specify Any Pins The Defult
     * SDA = D4 <GPIO2>, SCL = D5 <GPIO14> For ESP8266 Dev Kit Node MCU v3
     --------------------------------------------------------------------- */
    Wire.begin();                   // Begginning The I2C
    
    // Setting Up The I2C Of The MPU9250 ------------------------------------
    Wire.setClock(TWI_FREQ);        // Setting The Frequency MPU9250 Require
        
    // Setting The Serial Port ----------------------------------------------
    Serial.begin(115200);           // Computer Communication
    
    // Setting The Mode Of Pins ---------------------------------------------
    pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    pinMode(LED1, OUTPUT);          // Indicator For Client #1 Connectivity
    pinMode(LED2, OUTPUT);          // Indicator For Test Output
    pinMode(Trigger, OUTPUT);       // Indicator For Trigger signal
    pinMode(Echo, INPUT);           // Indicator For Echo signal
    digitalWrite(LED0, !LOW);       // Turn WiFi LED Off
    
    // Print Message Of I/O Setting Progress --------------------------------
    Serial.println("\nI/O Pins Modes Set .... Done");

    // Starting To Connect --------------------------------------------------
    WiFi.mode(WIFI_STA);            // To Avoid Broadcasting An SSID
    WiFi.begin("TAKEONE");          // The SSID That We Want To Connect To

    // Printing Message For User That Connetion Is On Process ---------------
    Serial.println("!--- Connecting To " + WiFi.SSID() + " ---!");

    // WiFi Connectivity ----------------------------------------------------
    CheckConnectivity();            // Checking For Connection

    // Stop Blinking To Indicate Connected ----------------------------------
    digitalWrite(LED0, !HIGH);
    Serial.println("!-- Client Device Connected --!");

    // Printing IP Address --------------------------------------------------
    Serial.println("Connected To      : " + String(WiFi.SSID()));
    Serial.println("Signal Strength   : " + String(WiFi.RSSI()) + " dBm");
    Serial.print  ("Server IP Address : ");
    Serial.println(TKDServer);
    Serial.print  ("Device IP Address : ");
    Serial.println(WiFi.localIP());
    
    // Conecting The Device As A Client -------------------------------------
    TKDRequest();
  }

//====================================================================================
  
  void loop()
  {
    // Call BlinSpotDetector function which reads the sensor
    BlindSpotDetector();
    Serial.println(distance);
  }


//====================================================================================

  void BlindSpotDetector()
  {
      // Clear the trigPin by setting it LOW:
      digitalWrite(Trigger, LOW);

      // Clear output
      digitalWrite(LED2, LOW);
  
      delayMicroseconds(5);
      // Trigger the sensor by setting the trigPin high for 10 microseconds:
      digitalWrite(Trigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(Trigger, LOW);
  
      // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
      duration = pulseIn(Echo, HIGH);
  
      // Calculate the distance:
      distance = duration*0.0135/2;

      // If distance read is in blind spot range, set output to HIGH
      if (distance > 0 && distance < maxDist) {
          digitalWrite(LED2, HIGH);
          Serial.println("Output set to HIGH for " + Sensor + " sensor");
          TKDClient.println("Output set to HIGH for " + Sensor + " sensor");
          TKDClient.flush();
      }
  
      delay(100);
  }

//====================================================================================

  void CheckConnectivity()
  {
    while(WiFi.status() != WL_CONNECTED)
    {
      for(int i=0; i < 10; i++)
      {
        digitalWrite(LED0, !HIGH);
        delay(250);
        digitalWrite(LED0, !LOW);
        delay(250);
        Serial.print(".");
      }
      Serial.println("");
    }
  }

//====================================================================================

  void TKDRequest()
  {
    // First Make Sure You Got Disconnected
    TKDClient.stop();

    // If Sucessfully Connected Send Connection Message
    if(TKDClient.connect(TKDServer, 9001))
    {
      Serial.println    ("<" + Sensor + " Sensor-CONNECTED>");
      TKDClient.println ("<" + Sensor + " Sensor-CONNECTED>");
    }
  }

//====================================================================================
