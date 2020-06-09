# Architecture of Traffic Simulator


## Classes

### Objs.go

  It creates all the main structs and types that will be needed throughout the project.

### Board.go

  It is the canvas (in console) that we use to draw and set all the pieces, for example, cars, semaphores, buildings, etc.

### Simulator.go
  
  Simulation works as main given that it is in charge of invoking and generating all the functions required for the project     to run
  
### Path.go

   Every single car has its own starting point and destination generated randomly; therefore, they have a unique path that      will be calculated and printed in this class 

### Car.go

  It's an instance of a car obj, has the basic CRUD operations that will be needed in the project. 

### Semaphore.go

   It's an instance of a semaphore obj. It has the creation method and also the initialization and modification of a state      (Color of the light).

### Validator.go

  It validates the input of the user: width, number of cars, and number of semaphores.

### Server.go

  (Not ready for production)
  It exports the cars data through an API handler using MUX and HTTP request on Restful and Json.

