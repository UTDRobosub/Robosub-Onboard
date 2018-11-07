
////////////////////////////////////////////////////////////////////////////////////////////
//Encoding and decoding; not part of Serial class.
//Copy and paste this code into an arduino and it will still work.

int strFindFirstMasked(char *buf, int maxlen, char until, char mask){
  for(int i=0; i<maxlen; i++){
    if((buf[i]&mask)==until){
      return i;
    }
  }

  return -1;
}

int strFindLastMasked(char *buf, int maxlen, char until, char mask){
  for(int i=maxlen-1; i>=0; i--){
    if((buf[i]&mask)==until){
      return i;
    }
  }
}

const char Serial_MsgBeginChar = '[';
const char Serial_MsgEndChar = ']';
const char Serial_EscapeChar = '\\';

int Serial_SpecialCharCount = 3;
const char Serial_SpecialChars[] = {
  Serial_MsgBeginChar,
  Serial_MsgEndChar,
  Serial_EscapeChar,
};
const char Serial_SpecialCharEscapes[] = {
  '(',
  ')',
  '/',
};

int SerialDataEncode(char *decoded, int decodedlen, char *encoded){
  int encodedlen = 0;

  encoded[encodedlen++] = Serial_MsgBeginChar;

  for(int ci=0; ci<decodedlen; ci++){
    char c = decoded[ci];

    bool special = false;
    for(int scci=0; scci<Serial_SpecialCharCount; scci++){
      if(c==Serial_SpecialChars[scci]){
        encoded[encodedlen++] = Serial_EscapeChar;
        encoded[encodedlen++] = Serial_SpecialCharEscapes[scci];
        special = true;
      }
    }

    if(!special){
      encoded[encodedlen++] = c;
    }
  }

  encoded[encodedlen++] = Serial_MsgEndChar;

  return encodedlen;
}

int SerialDataDecode(char *encoded, int encodedlen, char *decoded){
  int decodedlen = 0;

  for(int ci=0; ci<encodedlen; ci++){
    char c = encoded[ci];

    if(c==Serial_EscapeChar){ //if the current character is the escape, the next character is an escaped special char
      ci++;
      c = encoded[ci];

      bool special = false;
      for(int scci=0; scci<Serial_SpecialCharCount; scci++){
        if(c==Serial_SpecialCharEscapes[scci]){
          decoded[decodedlen++] = Serial_SpecialChars[scci];
          special = true;
        }
      }

      if(!special){ //the last character was the escape but the current one does not match any special characters
        //cout<<"Error: Escaped non-special char "<<c<<endl;
      }
    }else{ //otherwise it's a regular character
      decoded[decodedlen++] = c;
    }
  }

  return decodedlen;
}

//Returns the number of bytes decoded
int SerialReadAndRemoveFirstEncodedDataFromBuffer(char *buf, int *buflen, char *decoded, int maxdecodedlen){
  int startloc = strFindFirstMasked(buf, *buflen, Serial_MsgBeginChar, 0xFF); //find the first start marker in the string
  int endloc = strFindFirstMasked(buf+startloc+1, *buflen-startloc-1, Serial_MsgEndChar, 0xFF);

  if(startloc!=-1 && endloc!=-1){
    endloc += startloc+1;

    startloc = strFindLastMasked(buf, endloc, Serial_MsgBeginChar, 0xFF);

    int encodedlen = endloc-startloc-1;
    char *encoded = (char*)malloc(encodedlen+1);

    memcpy(encoded, buf+startloc+1, encodedlen);

    memmove(buf, buf+endloc, *buflen-endloc-1);
    *buflen -= endloc+1;

    return SerialDataDecode(encoded, encodedlen, decoded);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>
byte servoPin = 9;
Servo servo;


const int recvbufmaxlen = 256;
char recvbuf[recvbufmaxlen];
int recvbuflen = 0;

void SerialReadEntireBuffer(){
  while(Serial.available()){
    char c = Serial.read();
    if(recvbuflen<recvbufmaxlen){
      recvbuf[recvbuflen++] = c;
    }
  }
}

const int decodedmaxlen = 128;
char decoded[decodedmaxlen];
int decodedlen = 0;

void SerialReadNext(){
  SerialReadEntireBuffer();
  decodedlen = SerialReadAndRemoveFirstEncodedDataFromBuffer(recvbuf, &recvbuflen, decoded, decodedmaxlen);
}

const int encodedmaxlen = 64;
char encoded[encodedmaxlen];
int encodedlen = 0;

const int sendstrmaxlen = 32;
char sendstr[sendstrmaxlen];
int sendstrlen = 0;

void sendEncoded(){
  encodedlen = SerialDataEncode(sendstr, sendstrlen, encoded);
  for(int i=0; i<encodedlen; i++){
    Serial.write(encoded[i]);
  }
}

void setup(){
  Serial.begin(9600);
  servo.attach(servoPin);
  servo.writeMicroseconds(1500);
  delay(1000);
}

void loop(){
  SerialReadNext();

  if(decodedlen >= 2)
  {
    
    int data = *((int*)decoded);

    Serial.write("[REPLY ");
    
    if (data >= 1400 && data <= 1600)
    {
      servo.writeMicroseconds(data);

      Serial.print("Wrote data");
    }

    Serial.print(data);
    Serial.write(']');
  }else{
    //Serial.write("[Not Enough Data]");
  }
  
  delay(10);
}

