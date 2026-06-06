#import "@preview/fletcher:0.5.0" as fletcher: diagram, node, edge
#import fletcher.shapes: house, hexagon
// #set page(width: auto, height: auto, margin: .5cm)


#let moore(pos, label, tint: white, ..args) = node(
  pos, align(center, label),
  width: 20mm,
  height: 12mm,
  fill: tint.lighten(60%),
  stroke: 1pt + tint.darken(20%),
  corner-radius: 4pt,
  ..args,
)


#let mealy(pos, label, tint: orange, ..args) = node(
  pos, align(center, label),
  width: 20mm,
  height: 12mm,
  fill: tint.lighten(60%),
  stroke: 1pt + tint.darken(20%),
  corner-radius: 18pt,
  ..args,
)

#let desicion(pos, label, tint: yellow, ..args) = node(
  pos, align(center, label),
  width: 20mm,
  height: 12mm,
  fill: tint.lighten(60%),
  stroke: 1pt + tint.darken(20%),
  corner-radius: 4pt,
  shape: hexagon,
  ..args,
)

#let state(pos, label,  ..args) = node(
  pos, align(top + center , label),
  width: 20mm,
  height: 12mm,
  fill: none,
  stroke: (dash: "dashed"),
  corner-radius: 4pt,
  inset: 16pt,
  ..args,
)


#figure(
diagram(
  spacing: 20pt,
  cell-size: (8mm, 10mm),
  edge-stroke: 1pt,
  edge-corner-radius: 5pt,
  mark-scale: 70%,

  moore((0,0), [x->9] ),
  desicion((0,2), [compute] ),
  mealy((0,3), [update] ),
  desicion((0,1), $K$),
  state((0,-1), [init], enclose: ((0,-1), (0,3))),
))
