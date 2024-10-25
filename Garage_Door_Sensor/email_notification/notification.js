const { initializeApp } = require('firebase/app');
const { getDatabase, ref, onValue, set } = require('firebase/database');
const nodemailer = require('nodemailer');

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

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

let transporter = nodemailer.createTransport({
    service: 'gmail', 
    auth: {
        user: 'seniordesignsebshivjohnlogan@gmail.com', 
        pass: 'ymgw lmeh tcho qqhy'   
    }
});

let mailOptions = {
    from: 'seniordesignsebshivjohnlogan@gmail.com', 
    to: 'spatel9603@gmail.com',         
    subject: 'ALERT: INTERUPT HAS OCURRED',           
};


function sendAlertEmail(){
    transporter.sendMail(mailOptions, function (error) {
        if (error) {
            return console.log(error);
        }
        console.log('Email Sent');
    });
}

const emailReference = ref(database, 'Send_Email');

onValue(valueRef, (snapshot) => {
    const data = snapshot.val();
    
    if (data === true){
        sendAlertEmail();

        set(emailReference, false);
    }
});