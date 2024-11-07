// Import Firebase Initialization, Firebase Commands, and Node Mailer
const { initializeApp } = require('firebase/app'); 
const { getDatabase, ref, onValue, set } = require('firebase/database');
const nodemailer = require('nodemailer');


// Conigure Firebase with Key
const firebaseConfig = {
    apiKey: "AIzaSyBwOJ2HDW0Au-BpXfcX7Ii6EXwGKJ5GFhg",
    authDomain: "garage-door-sensor.firebaseapp.com",
    databaseURL: "https://garage-door-sensor-default-rtdb.firebaseio.com",
    projectId: "garage-door-sensor",
    storageBucket: "garage-door-sensor.appspot.com",
    messagingSenderId: "822304953828",
    appId: "1:822304953828:web:8e0c204b48088e595ca3f1",
    measurementId: "G-HHCBBP5SB0"
  };

// Initialize App
const app = initializeApp(firebaseConfig);

// Get Reference to Database
const database = getDatabase(app);

// Message for Email
let message;

/////////////////////////////////////////////////
// Function: formatCurrentDateTime()
// Description: This function gets the current 
//              time and date and formats the 
//              data into a message for the 
//              email.
/////////////////////////////////////////////////
function messageTime() {

    let now = new Date();
    let hours = now.getHours();
    let minutes = now.getMinutes();

    // Check if PM
    let PM = (hours >= 12);

    //let hoursUpdated = hours % 12 || 12;
    let hoursUpdated;

    // Convert from Military Time
    if (hours % 12 == 0) {
        hoursUpdated = 12;
    }
    else {
        hoursUpdated = hours % 12;
    }

    // If digit is single then add padding 0 to begenning else keep
    let minutesUpdated = minutes.toString().padStart(2, '0');
    let AMorPM;

    // Set AMorPM
    if (PM == true){
        AMorPM = 'PM'
    }
    else {
        AMorPM = 'AM'
    }
    
    // If digit is single then add padding 0 to begenning else keep
    let month = (now.getMonth() + 1).toString().padStart(2, '0');
    let day = now.getDate().toString().padStart(2, '0');
    let year = now.getFullYear();

    // Email Message Format 
    let message = 'Critical Safety Event at ' + hoursUpdated + ':' + minutesUpdated +  ' ' + AMorPM +  ' on ' +  month + '/' + day + '/' + year;

    return message; 
}

console.log(messageTime());

// Transporter --> Initializes Sender Information
let transporter = nodemailer.createTransport({
    service: 'gmail', 
    auth: {
        user: 'seniordesignsebshivjohnlogan@gmail.com', 
        pass: 'ymgw lmeh tcho qqhy'   
    }
});

// MailOptions --> Sets Sender Email, Reciever Emial, Subject, and text (Empty Till Message is Taken)
let mailOptions = {
    from: 'seniordesignsebshivjohnlogan@gmail.com', 
    to: 'spatel9603@gmail.com',         
    subject: 'ALERT: RECIEVER FAULTY', 
    text: ''

};

/////////////////////////////////////////////////
// Function: sendAlertEmail()
// Description: This function sends a email
//              first by capturing the message 
//              and then using the 
//              transporter.sendMail function
/////////////////////////////////////////////////
function sendAlertEmail(){
    message = messageTime();
    mailOptions.text = message;
    transporter.sendMail(mailOptions, function (error) {
        if (error) {
            return console.log(error);
        }
        // Log Sent Email On Terminal
        console.log('Email Sent');
    });
}

// Database reference to specific spot 'Send_Email'
const emailReference = ref(database, 'Send_Email');

// Check if value is true in database
onValue(emailReference, (snapshot) => {
    const data = snapshot.val(); // Get Value form 'Send Email' in Database

    if (data === true){
        sendAlertEmail(); // Send Email

        set(emailReference, false); // Set Value in Database to False
    }
});