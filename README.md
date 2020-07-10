# LightDetector
controls a robotic arm that tracks light, sends the position of that arm to a db (HTTP post), updates a page with the position of the arm and date
## Schematic
![Schematic](https://github.com/aidangannon/LightDetector/blob/master/Images/Schematic.PNG)
## LDR handling and servo control
written for the pic16f877a chip. Two devices control two servo motors asyncronously, sending data via UART. Both devices share the same bus;
a locking mechanism is in place to prevent two devices from using the same bus.
## Backend
written in ASP.Net; for .Net Core. Data stored on SQL Server DB.
### Class diagram
![Class diagram](https://github.com/aidangannon/LightDetector/blob/master/Backend%20API/UML/Class%20diagram.PNG)
### sequence diagrams
![Sequence Diagrams](https://github.com/aidangannon/LightDetector/blob/master/Backend%20API/UML/Sequence%20diagrams.PNG)
