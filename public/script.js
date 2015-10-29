var app = angular.module('MyApp', []);

app.factory('socket', function ($rootScope) {
  var socket = io.connect();
  return {
    on: function (eventName, callback) {
      socket.on(eventName, function () {  
        var args = arguments;
        $rootScope.$apply(function () {
          callback.apply(socket, args);
        });
      });
    },
    emit: function (eventName, data, callback) {
      socket.emit(eventName, data, function () {
        var args = arguments;
        $rootScope.$apply(function () {
          if (callback) {
            callback.apply(socket, args);
          }
        });
      })
    }
  };
});

var serialPortList = [];

app.controller('nodeSerial', function($scope, socket){

  $scope.connected = false;
  $scope.ports = ["..."];

  socket.on('theta1', function (input) {
    $scope.hoek1 = input;
    changeAngles("processingCanvas", $scope.hoek1, $scope.hoek2);

  });

  socket.on('theta2', function (input) {
    $scope.hoek2 = input;
    changeAngles("processingCanvas", $scope.hoek1, $scope.hoek2);
  });

  socket.on('DOF', function (input){
    $scope.DOF = input;
  })

  socket.on('serialPorts', function (input){
    console.log(input);
    $scope.ports = input;
  });

  socket.on("connectedSerial", function (input){
    console.log("connected to port: "+input);
    $scope.connected = true;
  });

  socket.on("failed", function (input){
    console.log("failed to connect: "+input);
  });

  socket.on("disconnected", function (input){
    console.log("disconnected from port: "+input);
    $scope.connected = false;
  });

  $scope.serialRefresh = function(){
  socket.emit('serialRefresh');
  };

  $scope.connectSerial = function(){
    console.log("Connecting to: "+$scope.selectedPort);
    socket.emit('connectSerial', $scope.selectedPort)
  }
  $scope.disconnectSerial = function(){
    console.log("Disconnecting serialport");
    socket.emit('disconnectSerial');
  }

  $scope.blinkLed = function(ledColor){
  	socket.emit('ButtonPush', ledColor);
  	console.log('ButtonPush', ledColor);
  };

  $scope.changeAngle = function(){
    changeAngles("processingCanvas", $scope.hoek1, $scope.hoek2);
    console.log(Math.cos((pi/180)*30));

  }
})
