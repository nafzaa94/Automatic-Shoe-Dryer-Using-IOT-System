void setup() 
{
    Serial.begin(9600);     // communication with the host computer
    //while (!Serial)   { ; }
 
    // Start the software serial for communication with the ESP8266
    Serial1.begin(9600);  
 
    Serial.println("");
    Serial.println("Remember to to set Both NL & CR in the serial monitor.");
    Serial.println("Ready");
    Serial.println("");    
}
 
void loop() 
{
    // listen for communication from the ESP8266 and then write it to the serial monitor
    if ( Serial1.available() )   {  Serial.write( Serial1.read() );  }
 
    // listen for user input and send it to the ESP8266
    if ( Serial.available() )       {  Serial1.write( Serial.read() );  }
}
