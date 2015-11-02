#include "Linescanner.h"

//magic
void Linescanner::scan(int expose)
{  
  digitalWrite(clock, LOW);
  digitalWrite(sync, HIGH);
    
  digitalWrite(clock, HIGH);
  digitalWrite(sync, LOW);
        
  digitalWrite(clock, LOW); 
  
  for (int j = 0; j < 128; j++)
  {
    digitalWrite(clock, HIGH);
    digitalWrite(clock, LOW);
  }

  delayMicroseconds(expose);
  
  digitalWrite(sync, HIGH);
  digitalWrite(clock, HIGH);
  digitalWrite(sync, LOW);
    
  digitalWrite(clock, LOW);
  
}


void Linescanner::read(int* pixels)
{
  //reading data from camera
  for (int i = 0; i < 128; i++)
  {
    pixels[i] = analogRead(data);
    
    digitalWrite(clock, HIGH);
    digitalWrite(clock, LOW);
  }
}
