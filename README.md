## What is an Opensource IoT-hub?

This opensource project is a basic foundation to develop IoT platform for any industrial application using esp8266 based modules and RaspberryPi plafform. Please contribute and enrich this project.

Here we take a Greenhouse Controlling System for the example.

This IoT Greenhouse consists of low power Node MSU esp8266 and few sensors. Here i have used following Sensor modules.

        * AM2301 Humidity and Temperature sensor
        * BH1750FVI Light sensor
        * Soil moisture sensor
        * Rain drop detection sensor     
        
## Architecture

This code base devided in to 3 parts

        * sensor node ( This is end devices which will be basically Sensors and Actuators )
        * server ( Server to handle all the Sensor and Actuator Data )
        * processing ( Application of Computer Vision to your project via RaspberryPi using OpenCV)

 Akila Wickey
