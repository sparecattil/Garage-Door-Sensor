# Garage-Door-Sensor
![image](https://github.com/user-attachments/assets/681e9fe8-a088-4dd2-ad54-92902608ae01)

## Demo
[Watch Demo](https://www.youtube.com/watch?v=5DMKkt_EakY)

## System Overview
![image](https://github.com/user-attachments/assets/3fd434ec-4ac5-44e8-ad7f-2a05d78b9342)

1. Arduino Uno R3: Toggles an LED based on if a signal is being received or not, therefore, turning it on or off. This device includes an ADC (Analog-to-Digital Converter) that will help process the signal being received so the filter designer created in Matlab can filter out the noise and output a clean square wave back to the Receiver circuit. 
2. ESP32: Monitors LED status and sends an email notification when LED is on, indicating no signal is being received.
3. Receiver Circuit: Uses an IR photodiode to receive a signal from the transmitter, amplifies it with an Op-amp, and sends it to the Arduino Uno for filtering and processing. 
4. Transmitter Circuit: Sends a small square wave signal to the receiver circuit via an IR LED. 

## Receiver Schematic
![image](https://github.com/user-attachments/assets/dbf562eb-b2ec-4bf2-b38a-72a5e79b13e0)

The schematic depicts the schematic for the receiver circuit built for this lab project. This circuit receives an IR signal from a transmitter circuit at the IR photodiode. This signal goes to the op-amp and is amplified by ~400 gain. The signal is then transferred to the ADC in the Arduino Uno to be filtered. If the signal is interrupted, the white LED turns on. The ESP32 reads the pin of the white LED and sends an email notification when the signal gets interrupted (the LED turns on). The rail splitter is used to create a bipolar power supply. It creates a -5 V supply so that the op-amp can see +5 V at the positive input, and -5 V at its negative input. Finally, 1 uF capacitors are used to decouple the power supply rails. Doing this ensures the voltage is smooth at 5 V with little fluctuation in the amplitude. See figure 3 below for a picture of the receiver circuit.

## Software Schematic
![image](https://github.com/user-attachments/assets/3a5282fc-9768-41ab-8cbc-4e5138715327)

### Garage_Door_Sensor.ino
The Garage_Door_Sensor.ino file is designed to work on an Arduino processor to implement a digital frequency-selective filter which filters out unwanted frequencies from an IR signal. This file has been tested on an Arduino Uno R3. It initially filters the signal obtained on the receiver by using the associated numerator and denominator coefficients from a bandpass butterworth filter designed at a sampling frequency, Fs, of 1000 Hz and a filter order of 2. The filter also will only accept values between 415 Hz and 445 Hz. The designed filter and the coefficients corresponding to it were found using MATLAB’s filter designer application. After this, the maximum value of the filter output is found from the last ten samples. This value is then compared to the threshold value to see if the threshold is being exceeded. In the event that the threshold is exceeded, the white LED on the receiver will be turned on to indicate that the transmitter/receiver is not aligned or out of range. The program will also check to see if any samples have been missed by taking the difference between the time after the threshold value has been checked and when the filter starts collecting samples.  

### Email_Notification.ino
The email_notification.ino file alerts our notification system that the transmitter/receiver is not aligned or out of range. The file is designed for an ESP32 microcontroller. This device is programmed to read the on/off status of the receiver white LED. When the white LED is on, the ESP32 will send a true boolean status variable to Firebase, setting the Send_Email attribute on the database. Then the device is put into a lock state, preventing any data from being sent to Firebase. This ensures that data is only sent to Firebase when the status of the white LED changes.  Once the white LED is off, the ESP32 will release the lock allowing for data to be sent the next time the white LED turns on. Assuming the receiver is getting the signal from the transmitter, the ESP32 will remain in an idle state while continuously reading the status of the white LED. 

### Notification.js 
The notification.js file takes positive alerts from firebase and sends out emails to the user containing the timestamp at which the receiver misalignment occurred. The file runs using node.js with a library called nodeMailer. The file establishes a connection with firebase that reads the value of  “Send_Email”, if the value is “true” the file will read the input and tell the established transporter to send the message. The message contains a message of the specified format as shown in Figure 5. After the email has been sent then node will update “Send_Email” to false in firebase. 

## Email Notification
![image](https://github.com/user-attachments/assets/a26982c1-68c4-4e71-b9a5-78fd5f578c25)

