var com = require("serialport");
var app = require("express")();
const say = require("say");
let lastSpokenWords = '';

//import serialport module
var SerialPort = require("serialport");
var Readline = SerialPort.parsers.Readline;

//set parameters for the serialport
// var serialPort = new SerialPort("/dev/cu.SLAB_USBtoUART", {
var serialPort = new SerialPort("/dev/cu.usbmodem6157721", {
  baudRate: 9600
});

//the readline parser will delimit the data on a newline
var parser = new Readline();
serialPort.pipe(parser);

//setup a webserver on poert 3000
var server = app.listen(3000);
var io = require("socket.io")(server);

//Serve index.html when some make a request of the server
app.get("/", function(req, res) {
  res.sendFile(__dirname + "/index.html");
});

//send a message when the serialport connects successfully
serialPort.on("open", function() {
  console.log("Communication is on");
});

//when data is recieved log it to the console
parser.on("data", function(data) {
  io.sockets.emit("data", data);
  if (lastSpokenWords !== data) {
    say.speak(data);
    lastSpokenWords = data;
  }
  console.log(data);
});

// web-player-data
//when data is recieved log it to the console
//Process incoming messages
io.on('connection', function (socket) {
  socket.on('web-player-data', function (msg) {
      console.log(msg.val);
      // if (msg.val === 'green') {
        serialPort.write('G', function (err) {
          if (err) {
            return console.log('Error on write: ', err.message);
          }
          console.log(msg + ' written');
        });
      // }
  });
});
