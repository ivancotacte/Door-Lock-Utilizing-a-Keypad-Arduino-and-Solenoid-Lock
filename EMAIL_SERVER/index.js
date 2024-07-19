const express = require('express');
const bodyParser = require('body-parser');
require('dotenv').config();

const app = express();
const PORT = process.env.PORT || 3000;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

const { sendEmail } = require('./controller/emailSender.js');

app.get('/', (req, res) => {
    res.json({ message: 'Hello World!' });
});

app.post('/email', (req, res) => {
    const { openDoor, closedDoor, incorrectDoor, someoneChangepasscode, successChangepasscode , apiKey } = req.body;

    if (!apiKey || apiKey !== process.env.API_KEY) {
        return res.status(401).json({ message: 'Unauthorized' });
    }

    if (openDoor) {
        sendEmail('door', 'opened');
    } else if (closedDoor) {
        sendEmail('door', 'closed');
    } else if (incorrectDoor) {
        sendEmail('intruder');
    } else if (someoneChangepasscode) {
        sendEmail('someoneChangepasscode');
    } else if (successChangepasscode) {
        sendEmail('successChangepasscode');
    }
    
    res.json({ message: 'Email sent successfully!' });
});

app.post('/test', (req, res) => {
    sendEmail('door', 'opened');

    res.json({ message: 'Email sent successfully!' });
});

app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});