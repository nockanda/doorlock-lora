#include <SPI.h>              // include libraries
#include <LoRa.h>

#define relay 3

//920.9Mhz
const long frequency = 9209E5;

bool relay_is_on = false;
unsigned long t = 0;

void setup() {
  Serial.begin(9600);                   // initialize serial
  pinMode(relay,OUTPUT);
  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Gateway");
  Serial.println("Only receive messages from nodes");
  Serial.println("Tx: invertIQ enable");
  Serial.println("Rx: invertIQ disable");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loop() {
  //릴레이가 켜졌을때를 기준으로 100ms가 지났다면~
  if(relay_is_on){
    if(millis() - t > 100){
      relay_is_on = false;
      digitalWrite(relay,LOW);
    }
  }
/*
  //5초에 한번 실행된다~
  if (runEvery(5000)) { // repeat every 5000 millis

    String message = "HeLoRa World! ";
    message += "I'm a Gateway! ";
    message += millis();

    
    LoRa_sendMessage(message); // send a message
    //1.TX모드로 전환한다
    //2.메시지를 LoRa로 전송한다
    //3.Rx모드로 전환한다

    Serial.println("Send Message!");
  }
  */
}

//수신모드
void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

//송신모드
void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  if(message == "nockanda"){
    Serial.println("도어 잠금이 해제된다!");
    //컨트롤러에게 응답한다
    LoRa_sendMessage("OK");
    //도어잠금을 해제한다
    digitalWrite(relay,HIGH);
    relay_is_on = true;
    t = millis();
    //delay(100);
    //digitalWrite(relay,LOW);
    
  }else{
    Serial.println("저장된 명령이 아니다!!");
    //응답할 가치가 없다!
  }
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
