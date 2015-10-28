var http = require('http');
var express = require('express');
var app = express();
var serialport = require("serialport");
var server = http.createServer(app).listen(3000);
var io = require('socket.io').listen(server);
var i = 0;
var portNames ={};
app.use(express.static(__dirname + '/public'));
console.log("Created server on port: " + server.address().port)

var serialName = ' '; // start with empty port
var serial = new serialport.SerialPort(serialName,{
    baudRate: 9600,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    flowControl: false,
    parser: serialport.parsers.readline("\n")
}, false); // don't open serialport at startup.

io.sockets.on('connection', function (socket) {
	//Connecting to client 
	console.log('Socket connected');
	socket.emit('connected');
	getSerialPorts(function(portList){
		socket.emit('serialPorts', portList);  // on connection, emit serialports
	});

	serial.on('data', function(data){
		console.log(data);
		parseSerial(socket, data);
	});

	socket.on('serialRefresh', function(){
		getSerialPorts(function(portList){
			socket.emit('serialPorts', portList);
		});
	});

	socket.on('connectSerial', function(port){
		connectSerial(port, function(result){
			socket.emit(result[0], result[1]); // emit failed or connected
		});
	});
	socket.on('disconnectSerial', function(){
		disconnectSerial(function(result){
			socket.emit(result[0], result[1]); // emit conformation of disconnect
		});
	});

	// all functions that write to serial port.
	// if(serial.isOpen()){
		socket.on('ButtonPush', function(color){
			buttonPush(color);
		});
	// };
});

function parseSerial(socket, data){
	console.log(data);
	data = data.split("\t"); // split data in array by tabs
	data.forEach(function(dataset){
		dataset = dataset.split(" "); // split subset by spaces
		var key = dataset.shift();
		dataset.forEach(function(elementValue){
			socket.emit(key, elementValue); // emit every key and their values
		})
	});
};

function buttonPush(ledColor){
		console.log("send: led"+ledColor);
		serial.write("led"+ledColor+"\n");
	}

function getSerialPorts(callback){
	var portNames = [];
	// list all ports and connect to first serial port on default
	serialport.list(function (err, portList) {
	  portList.forEach(function(port) {
	    portNames.push(port.comName);
	  });
	    if (portNames.length > 0) {
	    	console.log(portNames);
	    	callback(portNames);
	    }else{
	    	console.log("No serialports available.");
	    	callback([]);
	    };
	});
};

function connectSerial(portName, callback){
	if(serial.isOpen()){
		serial.close();
	}
	serial.path = portName;
	serial.open(function (error) {
	  	if ( error ) {
	    	console.log('failed to open serial: '+error);
	    	callback(["failed", error]);
	  	} else {
		    console.log('opened Serial');
		    callback(["connectedSerial", portName]);
	    };
	});
};

function disconnectSerial(callback){
	if(serial.isOpen()){
		serial.close();
		console.log("Disconnected from: "+serial.path);
		callback(["disconnected", serial.path]);
	};
};