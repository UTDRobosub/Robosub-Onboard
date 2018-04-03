	
void setup(){
  Serial.begin(9600);
}

void sendslow(char *ss){
  int i = 0;
  while(ss[i]!='\0'){
    Serial.write(ss[i++]);
    delay(20);
  }
}

char encoded[512];

const char *sendstr = "serial [test]";

void sendEncodedSendstr(){
  int sendstrlen = 0;
  while(sendstr[++sendstrlen]!='\0');
  
  int encodedlen = SerialDataEncode((char*)sendstr, sendstrlen, encoded);

  Serial.print(encoded);
  
  //Serial.write('\n');
}

const int recvbufmaxlen = 1024*4-1;
char recvbuf[recvbufmaxlen+1];
int recvbuflen = 0;

char decoded[512];

void recvEncodedAndMirror(){
  while(Serial.available()){
    char c = Serial.read();
    if(recvbuflen<recvbufmaxlen){
      recvbuf[recvbuflen++] = c;
    }
  }
  
  while(true){
    int decodedlen = SerialReadAndRemoveFirstEncodedDataFromBuffer(recvbuf, &recvbuflen, decoded, 512);
    
    if(decodedlen==0){ return; }
  
    decoded[decodedlen] = '\0';
    
    int encodedlen = SerialDataEncode(decoded, decodedlen, encoded);
    for(int i=0; i<encodedlen; i++){
      Serial.write(encoded[i]);
    }
  }
}

void loop(){
  recvEncodedAndMirror();
  //sendEncodedSendstr();
  delay(500);
}

