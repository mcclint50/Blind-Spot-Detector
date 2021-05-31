// Author: Colin McClintic
// Computer Engineering Final Project - Blind Spot Detector
// Description: This program sets up the TKDServer, connected to by four clients. The
//    four clients are sending signals that indicate if there is an object in that
//    side's blind spot. If so, the LED that represents that side of the vehicle on the
//    LED Display is set to HIGH.
//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>
//------------------------------------------------------------------------------------
  // Define I/O Pins
  #define     LED0      2           // WIFI Module LED
  #define     LEDN      16          // LED connected to North sensor
  #define     LEDE      5           // LED connected to East sensor
  #define     LEDS      4           // LED connected to South sensor
  #define     LEDW      0           // LED connected to West sensor
//------------------------------------------------------------------------------------
  // Authentication Variables
  char*       TKDssid;              // SERVER WIFI NAME
  char*       TKDpassword;          // SERVER PASSWORD
//------------------------------------------------------------------------------------
  #define     MAXSC     4           // MAXIMUM NUMBER OF CLIENTS
  WiFiServer  TKDServer(9001);      // THE SERVER AND THE PORT NUMBER
  WiFiClient  TKDClient[MAXSC];     // THE SERVER CLIENTS
//====================================================================================

  void setup()
  {
    // Setting The Serial Port
    Serial.begin(115200);           // Computer Communication
    
    // Setting The Mode Of Pins
    pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    pinMode(LEDN, OUTPUT);          // Indicator For North sensor
    pinMode(LEDE, OUTPUT);          // Indicator For East sensor
    pinMode(LEDS, OUTPUT);          // Indicator For South sensor
    pinMode(LEDW, OUTPUT);          // Indicator For West sensor
    
    // Print Message Of I/O Setting Progress
    Serial.println();
    Serial.println("I/O Pins Modes Set .... Done");

    // Setting Up A Wifi Access Point
    SetWifi("TAKEONE", "");
  }

//====================================================================================
  
  void loop()
  {
    // Connects and receives messages from clients
    IsClients2();
    
  }

//====================================================================================
  
  void SetWifi(char* Name, char* Password)
  {
    // Stop Any Previous WIFI
    WiFi.disconnect();

    // Setting The Wifi Mode
    WiFi.mode(WIFI_AP_STA);
    Serial.println("WIFI Mode : AccessPoint Station");
    
    // Setting The Access Point
    TKDssid      = BlindSpotServer;
    TKDpassword  = Password;
    
    // Starting The Access Point
    WiFi.softAP(TKDssid, TKDpassword);
    Serial.println("WIFI < " + String(TKDssid) + " > ... Started");
    
    // Wait For Few Seconds
    delay(1000);
    
    // Getting Server IP
    IPAddress IP = WiFi.softAPIP();
    
    // Printing The Server IP Address
    Serial.print("AccessPoint IP : ");
    Serial.println(IP);

    // Starting Server
    TKDServer.begin();
    Serial.println("Server Started");
  }

//====================================================================================

  void IsClients2()
  {
    if (TKDServer.hasClient())
    {
      for(int i = 0; i < MAXSC; i++)
      {
        //find free/disconnected spot
        if (!TKDClient[i] || !TKDClient[i].connected())
        {
          if(TKDClient[i]) TKDClient[i].stop();
          TKDClient[i] = TKDServer.available();
          Serial.print("New Client : "); Serial.print(String(i+1) + " - ");
          continue;
        }
      }
      // no free / disconnected spot so reject
      // digitalWrite(LED1, HIGH);
      WiFiClient TKDClient = TKDServer.available();
      TKDClient.stop();
    }

    //check clients for data -------------------------------------------------------
    String message = "";
    for(int i = 0; i < MAXSC; i++)
    {
      if (TKDClient[i] && TKDClient[i].connected())
      {
        if(TKDClient[i].available())
        {
          // If Any Data Was Available We Read IT
          while(TKDClient[i].available()) 
          {
            // Read From Client
            Serial.println(TKDClient[i].readStringUntil('\n'));
            message = TKDClient[i].readStringUntil('\n')
            // If the sensor is covered, send its signal to HIGH
            if (message.equals("Output set to HIGH for North sensor")) {
              digitalWrite(LEDN, HIGH);
              delay(1000);
            }
            if (message.equals("Output set to HIGH for East sensor")) {
              digitalWrite(LEDE, HIGH);
              delay(1000);
            }
            if (message.equals("Output set to HIGH for South sensor")) {
              digitalWrite(LEDS, HIGH);
              delay(1000);
            }
            if (message.equals("Output set to HIGH for West sensor")) {
              digitalWrite(LEDW, HIGH);
              delay(1000);
            }
            // Reply To Client
            TKDClient[i].println("<OK>");
            // Set LEDs back to LOW
            digitalWrite(LEDN, LOW);
            digitalWrite(LEDE, LOW);
            digitalWrite(LEDS, LOW);
            digitalWrite(LEDW, LOW);
          }
        }
      }
    }
  }

//====================================================================================
