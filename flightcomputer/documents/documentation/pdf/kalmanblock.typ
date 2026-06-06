#import "@preview/fletcher:0.5.0" as fletcher: diagram, node, edge
#import fletcher.shapes: house, hexagon
// #set page(width: auto, height: auto, margin: .5cm)

#let blob(pos, label, tint: white, ..args) = node(
  pos, align(center, label),
  width: 20mm,
  height: 12mm,
  fill: tint.lighten(60%),
  stroke: 1pt + tint.darken(20%),
  corner-radius: 5pt,
  ..args,
)

#figure(
diagram(
  spacing: 10pt,
  cell-size: (2mm, 5mm),
  edge-stroke: 1pt,
  edge-corner-radius: 5pt,
  mark-scale: 70%,

  edge((-2,0),"rrr", "-|>"),
  blob((1,0), $F$),
  edge("rrr", "-|>"),
  edge("rr,d,l", "-|>"),
  blob((2, 1), $H$),
  edge("ll", "-|>"),
  blob((0, 1), $K$),
  edge("l,u", "-|>"),
))
