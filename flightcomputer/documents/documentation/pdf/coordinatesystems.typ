#import "@preview/cetz:0.2.2": canvas, plot ,draw, vector, matrix
// #set page(width: auto, height: auto, margin: .5cm)


#set text(
  font: "DM sans",
  weight: 600,
)
#figure(
  grid(
        columns: 2,
        gutter: 4mm,
    [#canvas({
        import draw: *

        // Set up the transformation matrix
        set-transform(matrix.transform-rotate-dir((1, 1, -1.6), (0, 1, .3)))
        scale(x: 1.25, y:1.25, z: -1)

        grid((-2,-2), (2,2), stroke: gray + .5pt)
        // Draw the coordinate axes
        line((0, 0), (0, -2), 
          stroke: 2.5pt + green, name: "x", mark: (end: "o", fill: green))
        line((0, 0), (2, 0), stroke: 2.5pt + red, name: "y",
          mark: (end: "o", fill: red)) 
        line((0, 0), (0, 0, 2), stroke: 2.5pt + blue, name:"z",
          mark: (end: "o", fill: blue))
        // draw cube
        scale(x:0.5,y:0.5,z:0.5)
        line((-0.5,0.5,0.5),(0.5,0.5,0.5), (0.5,-0.5,0.5),(-0.5,-0.5,0.5), (-0.5,0.5,0.5),
            fill:blue, stroke: blue,
            close:true)
        line((-0.5,0.5,-0.5),(0.5,0.5,-0.5), (0.5,0.5,0.5),(-0.5,0.5,0.5), (-0.5,0.5,-0.5),
            fill:green, stroke: green,
            close:true)
        line((-0.5,0.5,-0.5),(-0.5,-0.5,-0.5),(-0.5,-0.5,0.5),(-0.5,0.5,0.5),(-0.5,0.5,-0.5),
            fill:red, stroke: red,
            close:true)
        
        scale(x: 1.25, y:1.25, z: -1)
        content((0, -4),[_North_])
        content((4, -0.1), [_East_])
      })
      --- ENU ---
    ],
    [#canvas({
        import draw: *

        // Set up the transformation matrix
        set-transform(matrix.transform-rotate-dir((1, 1, -1.6), (0, 1, .3)))
        scale(x: 1.25, y:1.25, z: -1)

        grid((-2,-2), (2,2), stroke: gray + .5pt)
        // Draw the coordinate axes
        line((0, 0), (0, -2), 
          stroke: 2.5pt + red, name: "x", mark: (end: "o", fill: red))
        line((0, 0), (2, 0), stroke: 2.5pt + green, name: "y",
          mark: (end: "o", fill: green)) 
        line((0, 0), (0, 0, -2), stroke: 2.5pt + blue, name:"z",
          mark: (end: "o", fill: blue))
        // draw cube
        scale(x:0.5,y:0.5,z:0.5)
        line((-0.5,0.5,0.5),(0.5,0.5,0.5), (0.5,-0.5,0.5),(-0.5,-0.5,0.5), (-0.5,0.5,0.5),
            fill:blue, stroke: blue,
            close:true)
        line((-0.5,0.5,-0.5),(0.5,0.5,-0.5), (0.5,0.5,0.5),(-0.5,0.5,0.5), (-0.5,0.5,-0.5),
            fill:red, stroke: red,
            close:true)
        line((-0.5,0.5,-0.5),(-0.5,-0.5,-0.5),(-0.5,-0.5,0.5),(-0.5,0.5,0.5),(-0.5,0.5,-0.5),
            fill:green, stroke: green,
            close:true)
        
        scale(x: 1.25, y:1.25, z: -1)
        content((0, -4),[_North_])
        content((4, -0.1), [_East_])
      })
      --- NED ---
    ],
    )
)
