/*
 * Copyright (C) 2015 Jeeva Kandasamy (jkandasa@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
myControllerModule.controller('ForwardPayloadController', function(alertService,
$scope, $filter, TimersFactory, ForwardPayloadFactory, $uibModal, $stateParams, displayRestError) {
  
  $scope.sensor = TimersFactory.getSensorData({"id":$stateParams.id});
    
  $scope.filteredList=[];
  $scope.orgList=[];
  $scope.config = {
    itemsPerPage: 10,
    maxPages:10,
    fillLastPage: false
  }

  //Filter
  $scope.updateFilteredList = function() {
    $scope.filteredList = $filter("filter")($scope.orgList, $scope.query);
  };
  
  // Call and Run function every second
  $scope.orgList = ForwardPayloadFactory.getAll({"id":$stateParams.id},function(response) {
                    },function(error){
                      displayRestError.display(error);            
                    });
  $scope.filteredList = $scope.orgList;
  //setInterval($scope.getTimers, 1000*30);
  

  
  //Add new
  $scope.add = function (size) {
    var addModalInstance = $uibModal.open({
    templateUrl: 'partials/forwardPayload/addModal.html',
    controller: 'PPMaddController',
    size: size,
    resolve: {sensor: function () {return $scope.sensor;}}
    });

    addModalInstance.result.then(function (plProxy) {
      ForwardPayloadFactory.create(plProxy, function(response) {
        alertService.success("Added new entry");
        //Update display table
        $scope.orgList = ForwardPayloadFactory.getAll({"id":$stateParams.id}, function(response) {
        },function(error){
          displayRestError.display(error);          
        });
        $scope.filteredList = $scope.orgList;
      },function(error){
        displayRestError.display(error);            
      });
    }), 
    function () {
      //console.log('Modal dismissed at: ' + new Date());
    }
  };
    
  //Delete timer
  $scope.delete = function (plProxy, size) {
    var modalInstance = $uibModal.open({
    templateUrl: 'partials/models/deleteModal.html',
    controller: 'PPMdeleteController',
    size: size,
    resolve: {
      plProxy: function () {return plProxy;}
      }
    });
    modalInstance.result.then(function (plProxy) {
      ForwardPayloadFactory.delete({id: plProxy.id},function(response) {
        alertService.success("Deleted an entry["+plProxy.sensorDestination.nameWithNode+"]");
        //Update display table
        $scope.orgList = ForwardPayloadFactory.getAll({"id":$stateParams.id}, function(response) {
        },function(error){
          displayRestError.display(error);            
        });
        $scope.filteredList = $scope.orgList;
      },function(error){
        displayRestError.display(error);            
      });    
    }), 
    function () {
      //console.log('Modal dismissed at: ' + new Date());
    }
  };
});
myControllerModule.controller('PPMaddController', function ($scope, $modalInstance, TypesFactory, sensor) {
  $scope.plProxy = {};
  $scope.plProxy.sensorSource = {};
  $scope.plProxy.sensorDestination = {};
  $scope.plProxy.sensorDestination.node = {};
  $scope.plProxy.sensorSource.id = sensor.id;
  $scope.nodes = TypesFactory.getNodes();
  $scope.sourceSensorVariableTypes = TypesFactory.getSensorVariableTypesBySensorRefId({id: sensor.id});
  
  //Update sensors
  $scope.refreshSensors = function(nodeId){
      return TypesFactory.getSensors({id: nodeId});
  };
  
  //Updated Variable Types
  $scope.refreshVariableTypes = function(sensorRefId){
      return TypesFactory.getSensorVariableTypesBySensorRefId({id:sensorRefId});
  };
  
  $scope.header = "New Forward Payload entry for '"+sensor.nameWithNode+"'";
  $scope.add = function() {$modalInstance.close($scope.plProxy); }
  $scope.cancel = function () { $modalInstance.dismiss('cancel'); }
});

//Delete Modal
myControllerModule.controller('PPMdeleteController', function ($scope, $modalInstance, $sce, plProxy) {
  $scope.header = "Delete a Forward Payload entry";
  $scope.deleteMsg = $sce.trustAsHtml("You are about to delete a forward entry"
    +"<br>Click 'Delete' to proceed."
    +"<br>Sensor Name: ["+plProxy.sensorDestination.nameWithNode+"]");
  $scope.remove = function() {
    $modalInstance.close(plProxy);
  };
  $scope.cancel = function () { $modalInstance.dismiss('cancel'); }
});
