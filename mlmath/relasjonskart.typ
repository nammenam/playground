#import "@preview/fletcher:0.5.8" as fletcher: diagram, node, edge
#import "@preview/cetz:0.4.2"

#let relasjonskart(active: none, visited: ()) = {
  let node-style(target-id) = {
    if active == target-id {
      // Aktiv node
      (fill: green.lighten(65%), stroke: 2pt + green.darken(60%))
    } else if target-id in visited {
      // Besøkte noder
      (fill: yellow.lighten(65%), stroke: 2pt + yellow.darken(60%))
    } else {
      // Ubesøkte noder
      (fill: blue.lighten(65%), stroke: 2pt + blue.darken(60%))
    }
  }

  align(center)[
    #diagram(
      spacing: (2.5cm, 1.0cm),
      edge-stroke: 2pt + gray.darken(50%),
      mark-scale: 0,

      // ── Row 0: Foundation ──────────────────────────────────────────
      node(
        (1, 0),
        align(center)[
          *N-dimensjonale tabeller*\
          #text(size: 12pt)[Skalarer, vektorer, matriser]
        ],
        name: <objekter>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<objekter>),
      ),

      node(
        (1, 1),
        align(center)[
          *Matrisemultiplikasjon*\
          #text(size: 12pt)[Lineære transformasjoner]
        ],
        name: <matmul>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<matmul>),
      ),

      // ── Row 1: Representasjon & rom ───────────────────────────────
      node(
        (0, 0),
        align(center)[
          *Datarepresentasjon*\
          #text(size: 12pt)[Hvordan data kodes som tall]
        ],
        name: <data>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<data>),
      ),

      node(
        (2, 1),
        align(center)[
          *Vektorrom*\
          #text(size: 12pt)[Geometri og normer]
        ],
        name: <vektorrom>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<vektorrom>),
      ),

      // ── Row 2: Operasjoner ────────────────────────────────────────
      node(
        (1, 2),
        align(center)[
          *Vektet sum\ og prikkprodukt*\
          #text(size: 12pt)[]
        ],
        name: <vektsum>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<vektsum>),
      ),

      node(
        (2, 2),
        align(center)[
          *Avstand & likheter*\
          #text(size: 12pt)[Euklidsk, cosinus, Manhattan]
        ],
        name: <avstand>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<avstand>),
      ),

      // ── Row 3: Ikke-linearitet & feil ─────────────────────────────
      node(
        (1, 3),
        align(center)[
          *Aktiveringsfunksjoner*\
          #text(size: 12pt)[ReLU, sigmoid, tanh]
        ],
        name: <aktivering>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<aktivering>),
      ),

      node(
        (2, 3),
        align(center)[
          *Tapsfunksjoner*\
          #text(size: 12pt)[MSE, kryssentropi]
        ],
        name: <loss>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<loss>),
      ),

      // ── Row 4: Optimering ─────────────────────────────────────────
      node(
        (1, 4),
        align(center)[
          *Tapslandskap*\
          #text(size: 12pt)[Feiloverflate, minima]
        ],
        name: <landskap>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<landskap>),
      ),

      node(
        (0, 3),
        align(center)[
          *Gradient*\
          #text(size: 12pt)[Partiell derivert, retning]
        ],
        name: <gradient>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<gradient>),
      ),

      node(
        (0, 4),
        align(center)[
          *Gradientnedstigning*\
          #text(size: 12pt)[SGD, Adam, læringsrate]
        ],
        name: <gd>,
        shape: rect,
        corner-radius: 6pt,
        inset: 10pt,
        ..node-style(<gd>),
      ),

      // ── Kanter (ikke-direksjonelle) ────────────────────────────────

      // Grunnlag → representasjon og rom
      edge(<objekter>, <data>),
      edge(<objekter>, <vektorrom>),

      // Grunnlag & Vektorrom → matrisemultiplikasjon
      edge(<objekter>, <matmul>),
      edge(<vektorrom>, <matmul>),
      edge(<matmul>, <vektsum>),

      // Representasjon → operasjoner
      edge(<vektorrom>, <vektsum>),
      edge(<vektorrom>, <avstand>),

      // Operasjoner → ikke-linearitet
      edge(<vektsum>, <aktivering>),
      edge(<vektsum>, <avstand>),
      edge(<loss>, <aktivering>),

      // Avstand → tap
      edge(<avstand>, <loss>),

      // Aktivering + tap → tapslandskap
      // edge(<aktivering>, <landskap>),
      edge(<loss>, <landskap>),

      // Gradient og GD
      edge(<landskap>, <gradient>),
      edge(<gradient>, <gd>),
      edge(<landskap>, <gd>),
      edge(<vektsum>, <gradient>),
    )
  ]
}

#let tapslandskap() = {
cetz.canvas(length: 6cm, {
  import cetz.draw: *

  set-style(
    stroke: (thickness: 1.6pt, cap: "round", join: "round"),
    mark: (fill: black, scale: 1.0)
  )

  let get_height_color(i) = {
      // Ensure i is clamped between 0 and 1
      let t = calc.max(0.0, calc.min(1.0, i))

      let r = 0.0
      let g = 0.0
      let b = 0.0

      if (t < 0.33) {
          // Stage 1: Deep Blue to Sea Green
          let local_t = t / 0.33
          r = 0.0
          g = local_t * 180.0
          b = 130.0 + (local_t * 70.0)
      }
      else if (t < 0.66) {
          // Stage 2: Sea Green to Pure Yellow
          let local_t = (t - 0.33) / 0.33
          r = local_t * 255.0
          g = 180.0 + (local_t * 75.0)
          b = 200.0 - (local_t * 200.0)
      }
      else {
          // Stage 3: Yellow to Bright Orange
          let local_t = (t - 0.66) / 0.34
          r = 255.0
          g = 255.0 - (local_t * 155.0)
          b = 0.0
      }

      return rgb(int(r), int(g), int(b))
  }

  let franke_function(x, y) = {
      let term1 = 0.75 * calc.exp(-(calc.pow((9 * x - 2), 2)) / 4.0 - calc.pow((9 * y - 2), 2) / 4.0)
      let term2 = 0.75 * calc.exp(-(calc.pow((9 * x + 1), 2)) / 49.0 - (9 * y + 1) / 10.0)
      let term3 = 0.5 * calc.exp(-(calc.pow((9 * x - 7), 2)) / 4.0 - calc.pow((9 * y - 3), 2) / 4.0)
      let term4 = -0.2 * calc.exp(-(calc.pow((9 * x - 4), 2)) - calc.pow((9 * y - 7), 2))
      return term1 + term2 + term3 + term4
    }

  ortho(x: 45deg, y: -20deg, {
    // We use a smaller step to get a "mesh" look
    let steps = 15
    let delta = 1.0 / steps

    for i in range(0, steps) {
      for j in range(0, steps) {
        let x1 = i * delta
        let x2 = (i + 1) * delta
        let y1 = j * delta
        let y2 = (j + 1) * delta

        // Evaluate the 4 corners of the quad
        let z1 = franke_function(x1, y1)
        let z2 = franke_function(x2, y1)
        let z3 = franke_function(x2, y2)
        let z4 = franke_function(x1, y2)

        // Draw the quad face
        // We vary the fill color slightly based on Z to give it depth
        line(
          (x1, z1, y1),
          (x2, z2, y1),
          (x2, z3, y2),
          (x1, z4, y2),
          close: true,
          fill: get_height_color(z1),
          stroke: (thickness: 0.1pt, paint: gray.darken(50%))
        )
      }
    }

    // Optional: Add Axes for orientation
    line((0,0,0), (1.2,0,0), mark: (end: ">"))
    line((0,0,0), (0,1.2,0), mark: (end: ">"))
    line((0,0,0), (0,0,1.2), mark: (end: ">"))
    // on-xy(z: 0, {
    //   grid((-4, -4), (4, 4), stroke: gray.lighten(60%) + 0.5pt)
    //   // content((0, 4.5), text(size: 8pt, fill: gray.darken(30%), [$theta_1$ Axis]))
    //   // content((4.5, 0), text(size: 8pt, fill: gray.darken(30%), [$theta_2$ Axis]))
    // })
    // on-xz(y: -4, {
    //   grid((4, 0), (-4, 8), stroke: gray.lighten(60%) + 0.5pt)
    //   // content((0, 4.5), text(size: 8pt, fill: gray.darken(30%), [$theta_1$ Axis]))
    //   // content((4.5, 0), text(size: 8pt, fill: gray.darken(30%), [$theta_2$ Axis]))
    // })
    // on-yz(x: 4, {
    //   grid((0,-4), (8, 4), stroke: gray.lighten(60%) + 0.5pt)
    //   // content((0, 4.5), text(size: 8pt, fill: gray.darken(30%), [$theta_1$ Axis]))
    //   // content((4.5, 0), text(size: 8pt, fill: gray.darken(30%), [$theta_2$ Axis]))
    // })
  })
})
}




#let vectoravstand() = {
cetz.canvas(length: 1cm, {
  import cetz.draw: *

  set-style(
    stroke: (thickness: 2pt, cap: "round", join: "round"),
    mark: (fill: black, scale: 2.0)
  )

    line((0, 0), (6, 0), stroke: (dash: "dashed"))
    line((0, 0), (0, 5.0), stroke: (dash: "dashed"))
    line((0, 0), (6, 2), mark: (end: ">"))
    line((0, 0), (1.5, 3.0), mark: (end: ">"))

    arc((0,0), radius: 2, start:20deg, stop:62deg, anchor: "origin")
    line((6.0, 2.3), (1.56, 3.25), mark: (end:"|", start:"|"))
})
}
