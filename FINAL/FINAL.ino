// Functions Declaration
void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
void post_to_firebase(String data);
boolean waitResponse(String expected_answer = "OK", unsigned int timeout = 2000);



// Module SIM800L
#include <SoftwareSerial.h>
//GSM Module RX pin to Arduino 4
//GSM Module TX pin to Arduino 3
#define rxPin 3
#define txPin 4
SoftwareSerial SIM800(rxPin, txPin);



#define redLed    11
#define yellowLed 12
#define greenLed  13
#define buzzer1   6



// SIM
const String APN  = "internet";
const String USER = "";
const String PASS = "";



// Firebase
const String FIREBASE_HOST  = "https://prj2-forestfirealarm-default-rtdb.asia-southeast1.firebasedatabase.app/";
const String FIREBASE_SECRET  = "Sc7DhL8xEwy3xeG1nvCa1WLa0i0pFIN3KNr9K9Uz";
const String FIREBASE_PATH  = "/Location 1";



#define USE_SSL true
#define DELAY_MS 500
int RxBuffer;
String data;



void setup() {
  //Begin serial communication with Serial Monitor
  Serial.begin(9600);

  //Begin serial communication with SIM800
  SIM800.begin(9600);

  Serial.println("Initializing SIM800...");
  init_gsm();
  
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer1, OUTPUT);
}



void loop() {
  if (!is_gprs_connected()) {
    gprs_connect();
  }
  
  if (Serial.available() > 0) {
    RxBuffer = Serial.read();
    
    if (RxBuffer == 1) {
      digitalWrite(greenLed, HIGH);
      digitalWrite(yellowLed, LOW);
      digitalWrite(redLed, LOW);
      noTone(buzzer1);
    }
    else if (RxBuffer == 2) {
      digitalWrite(yellowLed, HIGH);
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, LOW);
      tone(buzzer1, 4000);
    }
    else if (RxBuffer == 3) {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, LOW);
      tone(buzzer1, 4000);

      data = "{";
      data += "\"status\":\"" + String("warning") + "\"";
      data += "}";  
      post_to_firebase(data);
    }
    else if (RxBuffer == 4) {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, LOW);
      noTone(buzzer1);

      data = "{";
      data += "\"status\":\"" + String("normal") + "\"";
      data += "}";  
      post_to_firebase(data);
    }  
  }
  delay(1000);
}



void post_to_firebase(String data)
{
  //Start HTTP connection
  SIM800.println("AT+HTTPINIT");
  waitResponse();
  delay(DELAY_MS);
  
  //Enabling SSL 1.0
  if (USE_SSL == true) {
    SIM800.println("AT+HTTPSSL=1");
    waitResponse();
    delay(DELAY_MS);
  }
  
  //Setting up parameters for HTTP session
  SIM800.println("AT+HTTPPARA=\"CID\",1");
  waitResponse();
  delay(DELAY_MS);
  
  //Set the HTTP URL - Firebase URL and FIREBASE SECRET
  SIM800.println("AT+HTTPPARA=\"URL\"," + FIREBASE_HOST + FIREBASE_PATH + ".json?auth=" + FIREBASE_SECRET);
  waitResponse();
  delay(DELAY_MS);
  
  //Setting up re direct
  SIM800.println("AT+HTTPPARA=\"REDIR\",1");
  waitResponse();
  delay(DELAY_MS);
  
  //Setting up content type
  SIM800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  waitResponse();
  delay(DELAY_MS);
  
  //Setting up Data Size
  //+HTTPACTION: 1,601,0 - error occurs if data length is not correct
  SIM800.println("AT+HTTPDATA=" + String(data.length()) + ",10000");
  waitResponse("DOWNLOAD");
  
  //Sending Data
  SIM800.println(data);
  waitResponse();
  delay(DELAY_MS);
  
  //Sending HTTP POST request
  SIM800.println("AT+HTTPACTION=1");

  for (uint32_t start = millis(); millis() - start < 20000;) {
    while (!SIM800.available());
    String response = SIM800.readString();
    if (response.indexOf("+HTTPACTION:") > 0)
    {
      Serial.println(response);
      break;
    }
  }
  delay(DELAY_MS);

  //+HTTPACTION: 1,603,0 (POST to Firebase failed)
  //+HTTPACTION: 0,200,0 (POST to Firebase successfull)
  //Read the response
  SIM800.println("AT+HTTPREAD"); 
  waitResponse("OK");
  delay(DELAY_MS);
  
  //Stop HTTP connection
  SIM800.println("AT+HTTPTERM");
  waitResponse("OK", 1000);
  delay(DELAY_MS);
}



// Initialize GSM Module
void init_gsm()
{
  //Testing AT Command
  SIM800.println("AT");
  waitResponse();
  delay(DELAY_MS);

  //Checks if the SIM is ready
  SIM800.println("AT+CPIN?");
  waitResponse("+CPIN: READY");
  delay(DELAY_MS);
  
  //Turning ON full functionality
  SIM800.println("AT+CFUN=1");
  waitResponse();
  delay(DELAY_MS);
  
  //Turn ON verbose error codes
  SIM800.println("AT+CMEE=2");
  waitResponse();
  delay(DELAY_MS);
  
  //Enable battery checks
  SIM800.println("AT+CBATCHK=1");
  waitResponse();
  delay(DELAY_MS);
  
  //Register Network (+CREG: 0,1 or +CREG: 0,5 for valid network)
  //+CREG: 0,1 or +CREG: 0,5 for valid network connection
  SIM800.println("AT+CREG?");
  waitResponse("+CREG: 0,");
  delay(DELAY_MS);
  
  //setting SMS text mode
  SIM800.print("AT+CMGF=1\r");
  waitResponse("OK");
  delay(DELAY_MS);
}



//Connect to the internet
void gprs_connect()
{
  //DISABLE GPRS
  SIM800.println("AT+SAPBR=0,1");
  waitResponse("OK", 60000);
  delay(DELAY_MS);
  
  //Connecting to GPRS: GPRS - bearer profile 1
  SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  waitResponse();
  delay(DELAY_MS);
  
  //sets the APN settings for your sim card network provider.
  SIM800.println("AT+SAPBR=3,1,\"APN\"," + APN);
  waitResponse();
  delay(DELAY_MS);
  
  //sets the user name settings for your sim card network provider.
  if (USER != "") {
    SIM800.println("AT+SAPBR=3,1,\"USER\"," + USER);
    waitResponse();
    delay(DELAY_MS);
  }
  
  //sets the password settings for your sim card network provider.
  if (PASS != "") {
    SIM800.println("AT+SAPBR=3,1,\"PASS\"," + PASS);
    waitResponse();
    delay(DELAY_MS);
  }
  
  //after executing the following command. the LED light of sim800l blinks very fast (twice a second)
  //enable the GPRS: enable bearer 1
  SIM800.println("AT+SAPBR=1,1");
  waitResponse("OK", 30000);
  delay(DELAY_MS);
  
  //Get IP Address - Query the GPRS bearer context status
  SIM800.println("AT+SAPBR=2,1");
  waitResponse("OK");
  delay(DELAY_MS);
}



/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Function: gprs_disconnect()
  AT+CGATT = 1 modem is attached to GPRS to a network.
  AT+CGATT = 0 modem is not attached to GPRS to a network
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
boolean gprs_disconnect()
{
  SIM800.println("AT+CGATT=0");
  waitResponse("OK", 60000);
  return true;
}



/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Function: gprs_disconnect()
  checks if the gprs connected.
  AT+CGATT = 1 modem is attached to GPRS to a network.
  AT+CGATT = 0 modem is not attached to GPRS to a network
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
boolean is_gprs_connected()
{
  SIM800.println("AT+CGATT?");
  if (waitResponse("+CGATT: 1", 6000) == 1) {
    return false;
  }
  return true;
}



//Handling AT COMMANDS
boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000) //uncomment if syntax error (arduino)
//boolean waitResponse(String expected_answer, unsigned int timeout) //uncomment if syntax error (esp8266)
{
  uint8_t x = 0, answer = 0;
  String response;
  unsigned long previous;

  //Clean the input buffer
  while (SIM800.available() > 0) SIM800.read();

  previous = millis();
  do {
    //if data in UART INPUT BUFFER, reads it
    if (SIM800.available() != 0) {
      char c = SIM800.read();
      response.concat(c);
      x++;
      //checks if the (response == expected_answer)
      if (response.indexOf(expected_answer) > 0) {
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}
