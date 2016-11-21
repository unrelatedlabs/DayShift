// 2016, Peter Kuhar

 angular.module("DayShift",[]).controller("MainController", function($q,$scope){
    var main = this;

    main.settings = {
    }
    main.settings.startTime = 4.5;
    main.settings.endTime = 8.16667;
    main.settings.intervalTime = 7.5;

    main.selectedDevice = null
    main.devices = {};


    main.scanForDevices = function(){
        main.status = "Scanning..."
        function failure(e){
            alert("Failed "+e);
        }

        ble.startScan([], function(device) {
         console.log(JSON.stringify(device));
         $scope.$apply(function(){
            main.devices[device.id] = device;
        })
     }, failure);

        setTimeout(ble.stopScan,
            5000,
            function() { 
                console.log("Scan complete"); 
                main.status = "Scan Complete"
            },
            function() { 
                console.log("stopScan failed");
                main.status = "Scan Complete"
            }
        );
    }

    main.deviceId = "AA8B2626-49F8-436A-881D-7487A19E7A26";

    function write(uuid,value){
        var deferred = $q.defer();
        var data = new Float32Array(1);
        data[0] = value;
        ble.connect(main.selectedDevice.id, function(){
            console.log("Connected");
            ble.write(main.deviceId, "19B10000-E8F2-537E-4F6C-D104768A1214", uuid, data.buffer, 
                function(){
                    deferred.resolve();
                },
                function(error){
                    console.log(error);
                    deferred.reject(error)
                }
                );
        },
        function(error){
            console.log("Error ", error)
            deferred.reject(error)
        })

        return deferred.promise
    }

    main.selectDevice = function(device){
        main.selectedDevice = device
        console.log("selected device", device)
    }

    main.updateSettings = function(settings){

        main.status = "Updating..."

        write("19B10002-E8F2-537E-4F6C-D104768A1214", settings.startTime).then(function(){
            return write("19B10003-E8F2-537E-4F6C-D104768A1214", settings.endTime)
        }).then(function(){
            return write("19B10004-E8F2-537E-4F6C-D104768A1214", settings.intervalTime)
        }).then(function(){
            var currentTime = new Date().getHours()  + new Date().getMinutes()/60.0;
            return write("19B10005-E8F2-537E-4F6C-D104768A1214", currentTime )
        }).then(function(){
            ble.disconnect(main.selectedDevice.id);
            main.status = "Settings updated"
            console.log("Ok");
        },function(error){
            ble.disconnect(main.selectedDevice.id);

            main.status = "Error " + error;

            console.log("Final error",error)
        })
    }

}).directive("timeOfDay", function(){
   return {
      require: 'ngModel',
      link: function(scope, element, attrs, ngModelController) {
        ngModelController.$parsers.push(function(data) {
          //convert data from view format to model format
          var date = new Date(data);
          return date.getHours() + date.getMinutes()/60 + date.getSeconds()/3600; 
      });

        ngModelController.$formatters.push(function(data) {
          //convert data from model format to view format
          var hours = Math.floor(data);
          var minutes =  Math.floor(data * 60) % 60 ;

          return new Date(1970,1,1,hours,minutes) //converted
      });
    }
};
});

var app = {
    // Application Constructor
    initialize: function() {
        this.bindEvents();
    },
    // Bind Event Listeners
    //
    // Bind any events that are required on startup. Common events are:
    // 'load', 'deviceready', 'offline', and 'online'.
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
    },
    // deviceready Event Handler
    //
    // The scope of 'this' is the event. In order to call the 'receivedEvent'
    // function, we must explicitly call 'app.receivedEvent(...);'
    onDeviceReady: function() {
        app.receivedEvent('deviceready');


    },
    // Update DOM on a Received Event
    receivedEvent: function(id) {
        var parentElement = document.getElementById(id);
        var listeningElement = parentElement.querySelector('.listening');
        var receivedElement = parentElement.querySelector('.received');

        listeningElement.setAttribute('style', 'display:none;');
        receivedElement.setAttribute('style', 'display:block;');

        console.log('Received Event: ' + id);
    }
};

app.initialize();
