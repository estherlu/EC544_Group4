#Challenge 4, Steering the Car


##Solution Architecture
We use two LIDARLite Sensor on both sides of the car and an UltraSonic Sensor in the front to detect walls.
Additionaly, we use the speed sensor on the wheel to monitor the speed of the car on the computer. The data of the speed are transfreed by the Xbee.

##Design Decisions
We use PID Algorithm on the side and rotorey encoder to sense the color wheel inside of the wheel.

##Challenges & Future Improvements
<b>Challenges</b>

Choosing the proper sensors to use and interfacing them together on Arduino
Dealing with limitations of each sensor, calibrating the out puts
Learning to use PID libraries an testing on combination of PID coefficients

<b>Future Improvements</b>
Make the crawler adaptive to different environment.
Better remote control.

