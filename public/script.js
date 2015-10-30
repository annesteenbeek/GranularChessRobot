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
  $scope.enableEMG = false;
  $scope.usePotmeters = false;
  $scope.controlDirection = false;
  $scope.controlAngle = false;

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

  socket.on("enableEMG", function (input){
      if(input==1){
        $scope.enableEMG = true;
      }else{
        $scope.enableEMG = false;
      }
  });
  socket.on("usePotmeters", function (input){
      if(input==1){
        $scope.usePotmeters = true;
      }else{
        $scope.usePotmeters = false;
      }
  });
  socket.on("controlAngle", function (input){
      if(input==1){
        $scope.controlAngle = true;
      }else{
        $scope.controlAngle = false;
      }
  });
  socket.on("controlDirection", function (input){
      if(input==1){
        $scope.controlDirection = true;
      }else{
        $scope.controlDirection = false;
      }
  });
  socket.on("DOF", function (input){
    switch(input){
      case 1:
        DOF = ["X direction", "First motor"];
        break;

      case 2:
        DOF = ["Y direction", "Second motor"];
        break;

      case 3:
        DOF = ["Z direction", "Servo"];
        break;
      default:
        DOF = ["...", "..."];
    }
      if($scope.angle == true){
        $scope.DOF=DOF[1];
      };
      if($scope.Direction == true){
        $Scope.DOF=DOF[0];
      }else{$scope.DOF="..."}
  });

  socket.on("motor1Pos", function(input){
    $scope.hoek1 = parseFloat(input);
    changeAngles("processingCanvas", $scope.hoek1, $scope.hoek2);
  });
  socket.on("motor2Pos", function(input){
    $scope.hoek2 = parseFloat(input);
    changeAngles("processingCanvas", $scope.hoek1, $scope.hoek2);
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

  $scope.setSwitchPump = function(){
    socket.emit('switchPump');
  }
  $scope.setStartCalibration = function(){
    socket.emit('startCalibration');
  }
  $scope.setEnableEMG = function (){
    $scope.enableEMG = !$scope.enableEMG
    console.log("EMGon state is: "+$scope.enableEMG);
    socket.emit('enableEMG');
  }
  $scope.setUsePotmeters = function (){
    $scope.usePotmeters = !$scope.usePotmeters
    console.log("potOn state is: "+$scope.usePotmeters);
    socket.emit('usePotmeters');
  }
  $scope.setControlAngle = function (){
    $scope.controlAngle = !$scope.controlAngle
    console.log("angle state is: "+$scope.controlAngle);
    socket.emit('controlAngle');
  }
  $scope.setControlDirection = function (){
    $scope.controlDirection = !$scope.controlDirection
    console.log("direction state is: "+$scope.controlDirection);
    socket.emit('controlDirection');
  }


  $scope.changeAngle = function(){
    changeAngles("processingCanvas", $scope.hoek1, $scope.hoek2);
    console.log(Math.cos((pi/180)*30));
  }
})
