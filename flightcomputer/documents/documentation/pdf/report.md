** **

<div>

# Kon-tiki final technical report: COMPASS

</div>

** **

<div>

# About the project

</div>

the goal of the project is to give us an acurate reading of the rockst
state eg velocity, position, orientation, and acceleration. this is so
that when the time is right we can deploy the parachute and land the
rocket safely without tearing it apart. it wil also give us a better
understanding of the rocket and how it behaves in the air. and more
easily recover the rocket We do this by using a combination of sensors
and a microcontroller to process the data and send it to the ground
station as well as store it on the rocket for redundancy and flight data
analysis.

** **

<div>

# Summary

</div>

The project is a success, we have a working prototype that has been
tested in the field and in the lab. the major takeaway is that the
system works well for altitude estimation and orientation. but x and y
velocity and position is prone to drift. The reason for this is the lack
of absolute positioning and velocity sensors like GPS and airspeed
sensors. The orientation and altitude states have such sensors and are
therefore more accurate. Despite this, the system does a good job of
giving a rough estimate of the velocity and position. With so called
"dead reckoning" we can estimate the position and velocity without the
need of telecommunication that GPS needs.

** **
