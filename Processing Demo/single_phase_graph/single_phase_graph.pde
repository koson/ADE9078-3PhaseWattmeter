//ADE9078 single phase bar graph visulization for fft
//Enoch Chau 2019

import processing.serial.*;

Serial myPort;

float maxFreq = 64;
float maxValue = 2000;
int sampleSize = 64;

float[]decimalAv = new float[2*sampleSize];


void mapping(float decimal[],float rectWidth, float rectHeight){
  int i;
  for(i=0; i<decimal.length;i+=2){
    rect((i/2)*rectWidth, height - decimal[i+1]*rectHeight , rectWidth , decimal[i+1]*rectHeight);
  }
}

void s2f_array(String header, String inString_parts[], float output_array[]){
    int i;
    if(header.equals(inString_parts[0]) == true ){
      for (i = 0; i<output_array.length; i++){
          output_array[i] = float(inString_parts[i+1]);
          print(output_array[i]);
          print(" ");
      }
      print("\n");
    }
}

void zeroArray(float array[]){
  int i;
  for (i = 0; i < array.length; i++){
    array[i] = 0;
  }
}

void setup(){
  size (1600,900);
  String usingSerial = Serial.list()[0];
  myPort = new Serial(this, usingSerial, 115200);
  
  zeroArray(decimalAv);
  
  myPort.bufferUntil('$');
}

void draw(){
  background(255);
  stroke(0);
  float rectWidth = width/maxFreq;
  float rectHeight = height/maxValue;
  fill(0, 191, 255);
  mapping(decimalAv,rectWidth,rectHeight);
}

void serialEvent(Serial myPort){
  int i;
    
  String inString = myPort.readStringUntil('$');
  println(inString);
  
  if (inString != null){
    String[] inStringParts = splitTokens(inString,":,;$\n\r");
    
    for(i=0; i<inStringParts.length; i++){
      print(inStringParts[i]);
      print(' ');
    }
    println();  
    s2f_array("Av", inStringParts, decimalAv);
    
    for (i=0; i<decimalAv.length; i++){
      print(decimalAv[i]);
      print(' ');
    }
  }
}
