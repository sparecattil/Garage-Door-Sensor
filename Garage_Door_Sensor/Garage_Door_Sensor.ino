///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////<Garage Door Sensor>/////////////////////////////////
// Group Members: Sebastian Parecatti, Shiv Patel, John Finch, Logan Schimanski
// Description: 
// Contains software implementations for digital filter used on the receiver, This file
// is designed to work on an Arduino processor and has been tested on an Arduino Uno R3
// Sampling Frequency of the filter - Fs = 1000
// 
// Resource:
// A digital frequency selective filter
// A. Kruger, 2019
// revised R. Mudumbai, 2020 & 2024
//////////////////////////////////<Garage Door Sensor>/////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// The following defines are used for setting and clearing register bits
// on the Arduino processor. Low-level stuff: leave alone.
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int analogPin = A5; // Specify analog input pin. Make sure to keep between 0 and 5V.
int LED = A2; // Specify output analog pin with indicator LED 
  
const int n = 3;   // number of past input and output samples to buffer; change this to match order of your filter
int m = 10; // number of past outputs to average for hysteresis

// num and den are the numerator and denominator coeffs of a digital frequency-selective filter
// designed for a sample rate Fs = 1000 HZ
float den[] = {1,                                         
               1.660735491825769738483131732209585607052,
               0.827271945972477884545526194415288046002}; // Denominator Coefficients

float num[] = {0.086364027013761016093873479348985711113,
               0,                                        
              -0.086364027013761016093873479348985711113}; // Numerator Coefficients


float x[n],y[n],yn, s[10];     // Space to hold previous samples and outputs; n'th order filter will require upto n samples buffered

float threshold_val = 0.05; // Threshold value. Anything higher than the threshold will turn the LED off, anything lower will turn the LED on

// Time between samples Ts = 1/Fs. If Fs = 3000 Hz, Ts=333 us; In this case Fs = 1000 Hz
int Ts = 1000;

void setup()
{
   Serial.begin(1200);
   int i;

   sbi(ADCSRA,ADPS2); // The next three lines make the ADC run faster
   cbi(ADCSRA,ADPS1);
   cbi(ADCSRA,ADPS0);

   pinMode(LED,OUTPUT); // Makes the LED pin an output

   for (i=0; i<n; i++)
      x[i] = y[i] = 0;

   for(i = 0; i<m; i++)
    s[i] = 0;
   yn = 0;
}



void loop()
{
   unsigned long t1;
   int i,count,val;
   int k=0;
   float changet = micros();

   count = 0;
   while (1) {
      t1 = micros();

      // Calculate the next value of the difference equation.

      for(i=n-1; i>0; i--){             // Shift samples
         x[i] = x[i-1];                                            
         y[i] = y[i-1];
      }
      
      for(i=m-1; i>0; i--){             // Shift absoulute output
         s[i] = s[i-1];
      }
      val = analogRead(analogPin);  // New input

      x[0] = val*(5.0/1023.0)-2.5;  // Scale to match ADC resolution and range

      yn = num[0] * x[0];
      
      for(i=1;i<n;i++)             // Incorporate previous outputs (y[n])
         yn = yn - den[i]* y[i] + num[i] * x[i];          
         

       y[0] = yn;                  // New output

      //  The variable yn is the output of the filter at this time step.
      //  Now we can use it for its intended purpose:
      //       - Apply theshold
      //       - Apply hysteresis
      //       - What to do when the beam is interrupted, turn on a buzzer, send SMS alert.
      //       - etc.

      s[0] = abs(2*yn);  // Absolute value of the filter output.

      // SAMPLE Hystersis: Take the max of the past 10 samples and compare that with the threshold
      float maxs = 0;
      for(int i = 0; i< m; i++)
      {
        if (s[i]>maxs)
          maxs = s[i];
      }


      
      // Check the output value against the threshold value every 10^6 microseconds or 1 second
      if ((micros()-changet) > 1000e3)
      {
        Serial.println(maxs);

        changet = micros();
        if(maxs < threshold_val)
        {
          digitalWrite(LED, HIGH);
        }
        else
        {
          digitalWrite(LED, LOW);
        }
      }
      
      // The filter was designed for a 1000 Hz sampling rate. This corresponds
      // to a sample every 1000 us. The code above must execute in less time
      // (if it doesn't, it is not possible to do this filtering on this processor).
      // Below we tread some water until it is time to process the next sample

     
      if((micros()-t1) > Ts)
      {
        // if this happens, you must reduce Fs, and/or simplify your filter to run faster        
        Serial.println("MISSED A SAMPLE"); 
      }
      while((micros()-t1) < Ts);      
   }
}
