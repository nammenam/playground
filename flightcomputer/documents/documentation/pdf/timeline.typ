#import "@preview/fletcher:0.5.0" as fletcher: diagram, node, edge
#import fletcher.shapes: house, hexagon, chevron
// #set page(width: auto, height: auto, margin: .5cm)

#set text(
  font: "inter",
  size: 9pt,
)
#show strong: it => [
  #set text(font: "DM Sans", weight: "bold")
  #it
]

#let blob(pos, label, tint: white, ..args) = node(
  pos, align(left + top, label), 
  width: 3cm, height: 2cm,
  fill: green.lighten(50%),
  stroke: 2pt + tint.darken(75%),
  corner-radius: 5pt,
  shape: chevron,
  ..args,
)

#figure(
diagram(
  spacing: 14pt,
  edge-stroke: 1pt,
  edge-corner-radius: 5pt,
  mark-scale: 70%,

  blob((0,0), [*Sep 23* Project takeover ]),
  blob((1,0), [*Oct 23* Get familiar with the codebase ]),
  blob((2,0), [*Nov 23* Write USART debug utility]),
  blob((3,0), [*Des 23* Write IMU driver]),
  blob((0,1), [*Jan 24* Test IMU driver]),
  blob((1,1), [*Feb 24* Write magnetometer driver & SD card driver]),
  blob((2,1), [*Mar 24* Write pressure sensor driver]),
  blob((3,1), [*Apr 24* New PCB design]),
  blob((0,2), [*May 24* Kalman filter implementation done]),
  blob((1,2), [*Jun 24* Final testing]),
))
