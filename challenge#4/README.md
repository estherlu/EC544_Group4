#Challenge 4, Steering the Car


##Solution Architecture
We used one LIDARLite Sensor on one side of the car and an UltraSonic Sensor in the front to detect walls.
Additionaly, we use the speed sensor on the wheel to monitor the speed of the car on the computer. The data of the speed are transfreed via XBee.

##Design Decisions
We use PID Algorithm on the side to position the car and a rotorey encoder to sense the color wheel inside of the wheel.

##Challenges & Future Improvements
<b>Challenges</b>

Choosing the proper sensors to use and interfacing them together on Arduino
Dealing with limitations of each sensor, calibrating the out puts
Learning to use the PID library and testing a combination of PID coefficients

<b>Future Improvements</b>
Make the crawler adaptive to different environments.
Better remote control.

