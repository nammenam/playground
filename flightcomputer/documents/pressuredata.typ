#import "@preview/cetz:0.2.2": canvas, plot ,draw, vector, matrix
// #set page(width: auto, height: auto, margin: .5cm)

#figure(
canvas(length: 1cm, {
  let points = ((1406, 190), (1311, 2.2), (1318, 12), (1330, 30), (1351, 54),
         (1381, 100), (1432, 220), (1455, 250), (1485, 300), (1497, 340),
         (1539, 400), (1543, 460), (1581, 540), (1628, 630), (1663, 740),
         (1720, 860), (1786, 1000))

  plot.plot(size: (14, 6),x-max:1800,y-max:1200, y-min:0, x-min:1300,
    {
        plot.add(points, mark:"o", style:(stroke:0pt))
        plot.add(((1200,-200),(1900,1150)),style:(stroke:1pt))
    })
}),
caption: "pressure data",
)
