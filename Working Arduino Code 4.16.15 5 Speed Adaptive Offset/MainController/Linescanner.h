#ifndef LINESCANNER_H
#define LINESCANNER_H

#include "Arduino.h"
#define  LENGTH 128

class Linescanner {

  private:
    int data;           // digital pin
    int sync;           // digital pin
    int clock;          // analog pin
    int pixels[128];
  
  public:
    int maximum;
    int minimum;
    int mean;
    int left;
    int center;
    int right;
  
   /*
    * Description: Constructor
    * Params: data - analog pin number on microcontroller
    *         sync - digital pin number on microcontroller
    *         clock - digital pin number on microcontroller
    */
    Linescanner(int data, int sync, int clock)
                : data(data), sync(sync), clock(clock) {
                  maximum = 0;
                  minimum = 0;
                  mean = 0;
    }
    
   /*
    * Description: It signals the line scanner to start scanning with the given exposed time
    * Params: expose - time to expose to light
    */
    void scan(int expose);
 
   /*
    * Description: reads pixels of Linescanner after scan was called
    *              and stores them in int* pixels
    * Params: pixels - int array reference to store pixels data
    */
    void read(int* pixels);
    
   /*
    * Description:
    * Params: 
    */
    void parse(int* pixels); // edge detect? or whatever we want to do with the data
   
};

#endif // LINESCANNER_H 
