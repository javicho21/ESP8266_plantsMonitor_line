void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
     Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
       Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("javier/board1", "Atlas Scientific connected");
      // ... and resubscribe
      //      client.subscribe("inTopic");
    } else {
       Serial.print("failed, rc=");
       Serial.print(client.state());
       Serial.println(" try again in 5 seconds");
       //Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
