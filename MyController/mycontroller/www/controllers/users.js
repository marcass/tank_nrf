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
myControllerModule.controller('UsersController', function(alertService,
$scope, $filter, UsersFactory, $location, $uibModal, displayRestError) {
    
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
  $scope.orgList = UsersFactory.query(function(response) {
                    },function(error){
                      displayRestError.display(error);            
                    });
  $scope.filteredList = $scope.orgList;
  
  //Delete
  $scope.delete = function (user, size) {
    var modalInstance = $uibModal.open({
    templateUrl: 'partials/models/deleteModal.html',
    controller: 'UdeleteController',
    size: size,
    resolve: {
      user: function () {return user;}
      }
    });

    modalInstance.result.then(function (selectedUser) {
      UsersFactory.delete({ userId: selectedUser.id},function(response) {
        alertService.success("Deleted a user["+selectedUser.name+"]");
        //Update display table
        $scope.orgList = UsersFactory.query(function(response) {
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
    
  //Add a User
  $scope.add = function (size) {
    var addModalInstance = $uibModal.open({
    templateUrl: 'partials/users/addModal.html',
    controller: 'UaddController',
    size: size,
    resolve: {}
    });

    addModalInstance.result.then(function (newUser) {
      UsersFactory.create(newUser,function(response) {
        alertService.success("Added a user["+newUser.name+"]");
        //Update display table
        $scope.orgList = UsersFactory.query(function(response) {
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
    
  //Update a User
  $scope.update = function (user, size) {
    var editModalInstance = $uibModal.open({
    templateUrl: 'partials/users/updateModal.html',
    controller: 'UupdateController',
    size: size,
    resolve: {user: function () {return user;}}
    });

    editModalInstance.result.then(function (updateUser) {
      UsersFactory.update(updateUser,function(response) {
        alertService.success("Updated a user["+updateUser.name+"]");
        //Update display table
        $scope.orgList = UsersFactory.query(function(response) {
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


//Users Modal
myControllerModule.controller('UdeleteController', function ($scope, $modalInstance, $sce, user) {
  $scope.user = user;
  $scope.header = "Delete User";
  $scope.deleteMsg = $sce.trustAsHtml("You are about to delete a User"
    +"<br>Deletion process will remove complete trace of this user!" 
    +"<br>Click 'Delete' to proceed."
    +"<br><I>User: [id:"+user.id+",userId:"+user.name +",role:"+user.role+"]</I>");
  $scope.remove = function() {
    $modalInstance.close($scope.user);
  };
  $scope.cancel = function () { $modalInstance.dismiss('cancel'); }
});

myControllerModule.controller('UaddController', function ($scope, $modalInstance, TypesFactory) {
  $scope.user = {};
  $scope.header = "Add User";
  $scope.roles = TypesFactory.getUserRoles();
  $scope.add = function() {$modalInstance.close($scope.user); }
  $scope.cancel = function () { $modalInstance.dismiss('cancel'); }
});

myControllerModule.controller('UupdateController', function ($scope, $modalInstance, user) {
  $scope.user = user;
  $scope.header = "Update User";
  $scope.update = function() {$modalInstance.close($scope.user);}
  $scope.cancel = function () { $modalInstance.dismiss('cancel'); }
});
