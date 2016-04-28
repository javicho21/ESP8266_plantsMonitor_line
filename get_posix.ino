void get_posix(){   //get posix data from NTP server
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  
  delay(1000);  // wait to see if a reply is available
  
  int cb = udp.parsePacket();
  if (!cb) {
  // Serial.println("no packet yet");
  }
  else {
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord; //Seconds since Jan 1 1900
    const unsigned long seventyYears = 2208988800UL;  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    unsigned long epoch = (secsSince1900 - seventyYears); // subtract seventy years:
    
    readTime = String(epoch) + "000000000"; // print Unix time:
    //Serial.println(readTime);
  }
}


unsigned long sendNTPpacket(IPAddress& address){  // send an NTP request to the time server at the given address
  
  memset(packetBuffer, 0, NTP_PACKET_SIZE); // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
