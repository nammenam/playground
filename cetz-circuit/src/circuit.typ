#import "cetz.typ"

// =============================================================================
// 1. MATH HELPERS
// =============================================================================
#let vec-add(a, b) = (a.at(0) + b.at(0), a.at(1) + b.at(1))
#let vec-sub(a, b) = (a.at(0) - b.at(0), a.at(1) - b.at(1))
#let vec-scale(a, s) = (a.at(0) * s, a.at(1) * s)
#let vec-len(a) = calc.sqrt(calc.pow(a.at(0), 2) + calc.pow(a.at(1), 2))
#let angle-between(a, b) = {
  let d = vec-sub(b, a)
  calc.atan2(d.at(0), d.at(1))
}

// =============================================================================
// 2. WIRE FUNCTION (Rounded Corners + Gaps)
// =============================================================================
#let wire(
    start, 
    ..steps,
    radius: 0.25, 
    gap: 2pt, 
    background: white, 
) = {
    import cetz.draw: *
    
    get-ctx(ctx => {
        // 1. Resolve Start Point
        let (ctx, curr) = cetz.coordinate.resolve(ctx, start)
        let pts = (curr,)

        // 2. Process Steps (The "Turtle" Logic)
        for step in steps.pos() {
            // Check if step is a dictionary of commands (e.g. (l: 2, u: 1))
            if type(step) == dictionary {
                let x = curr.at(0)
                let y = curr.at(1)
                
                // Relative Moves (Directions)
                if "l" in step { x -= step.l }
                if "left" in step { x -= step.left }
                if "r" in step { x += step.r }
                if "right" in step { x += step.right }
                if "u" in step { y += step.u }
                if "up" in step { y += step.up }
                if "d" in step { y -= step.d }
                if "down" in step { y -= step.down }
                
                // Generic Relative
                if "dx" in step { x += step.dx }
                if "dy" in step { y += step.dy }
                
                // Axis Locking (Keep one, change other)
                if "x" in step { x = step.x }
                if "y" in step { y = step.y }
                
                curr = (x, y)
            } else {
                // If it's just a coordinate/string, resolve it as absolute
                let res = cetz.coordinate.resolve(ctx, step)
                ctx = res.at(0)
                curr = res.at(1)
            }
            pts.push(curr)
        }

        if pts.len() < 2 { return }

        // 3. Normalize Gap
        let scale = ctx.at("length", default: 1cm)
        let gap-val = if type(gap) == length { float(gap / scale) } else { float(gap) }
        
        // 4. Geometry Drawing Logic
        let draw-geom(points) = {
            line(points.at(0), points.at(0)) 
            for i in range(1, points.len() - 1) {
                let prev = points.at(i - 1)
                let curr = points.at(i)
                let next = points.at(i + 1)

                let theta-in = angle-between(prev, curr)
                let theta-out = angle-between(curr, next)
                let deg = calc.rem((theta-out - theta-in).deg(), 360.0)
                let delta = if deg > 180 { (deg - 360) } 
                            else if deg <= -180 { (deg + 360) } 
                            else { deg } * 1deg

                if calc.abs(delta) > 0.01deg {
                    let sign = if delta > 0deg { 1 } else { -1 }
                    let dist = radius * calc.abs(calc.tan(delta / 2))
                    
                    let d-prev = vec-len(vec-sub(curr, prev))
                    let d-next = vec-len(vec-sub(next, curr))
                    let limit = calc.min(d-prev, d-next) / 2
                    let actual-dist = calc.min(dist, limit)
                    let r = if dist > 0 { radius * (actual-dist / dist) } else { 0 }

                    let cut-vec = (calc.cos(theta-in) * actual-dist, calc.sin(theta-in) * actual-dist)
                    let arc-start = vec-sub(curr, cut-vec)
                    let theta-center = theta-in + delta/2 + sign * 90deg
                    let dist-center = r / calc.cos(delta/2)
                    let arc-center = (
                        curr.at(0) + dist-center * calc.cos(theta-center),
                        curr.at(1) + dist-center * calc.sin(theta-center)
                    )
                    let start-angle = theta-in - sign * 90deg

                    line((), arc-start)
                    arc(arc-center, radius: r, start: start-angle, delta: delta, anchor: "origin")
                } else {
                    line((), curr)
                }
            }
            line((), points.last())
        }

        // 5. Draw Background Halo
        group({
            let thickness = 1pt
            if "stroke" in ctx.style {
                let s = ctx.style.stroke
                if type(s) == stroke { thickness = s.thickness }
                else if type(s) == dictionary and "thickness" in s { thickness = s.thickness }
            }
            
            let bg-pts = pts
            if pts.len() >= 2 {
               let start-vec = vec-sub(pts.at(1), pts.at(0))
               let end-vec = vec-sub(pts.last(), pts.at(pts.len()-2))
               if vec-len(start-vec) > gap-val {
                   let shift = vec-scale(start-vec, gap-val / vec-len(start-vec))
                   bg-pts.at(0) = vec-add(bg-pts.at(0), shift)
               }
               if vec-len(end-vec) > gap-val {
                   let shift = vec-scale(end-vec, gap-val / vec-len(end-vec))
                   bg-pts.at(-1) = vec-sub(bg-pts.at(-1), shift)
               }
            }

            set-style(stroke: (thickness: thickness + 2 * gap, paint: background, cap: "butt"), fill: none)
            merge-path(draw-geom(bg-pts))
        })

        // 6. Draw Foreground
        merge-path(draw-geom(pts))
    })
}
// =============================================================================
// 3. IC SHAPE (Logic)
// =============================================================================
#let ic-shape(node, extrude) = {
    import cetz.draw: *
    let (w, h) = node.size
    let stroke = node.stroke
    let fill = node.fill
    
    let pins = node.at("pins", default: 8)
    let pin-len = node.at("pin-length", default: 0.25) // Default 0.25 aligns to grid/4
    let label-inset = 0.1
    
    rect((-w/2, -h/2), (w/2, h/2), stroke: stroke, fill: fill, radius:.1)
    arc((0, h/2), radius: 0.15, start: 180deg, delta: 180deg, stroke: stroke, fill: none)
    
    let draw-pin(name, pos, angle) = {
        let anchor-pt = (
            pos.at(0) + calc.cos(angle) * pin-len, 
            pos.at(1) + calc.sin(angle) * pin-len
        )
        line(pos, anchor-pt, stroke: stroke)
        anchor(str(name), anchor-pt)
    }

    if type(pins) == int {
        let count = pins
        let per-side = int(count / 2)
        let spacing = h / (per-side + 1)
        
        for i in range(per-side) {
            let y = h/2 - (i + 1) * spacing
            draw-pin(i + 1, (-w/2, y), 180deg)
            draw-pin(count - i, (w/2, y), 0deg)
        }
    }
}

// =============================================================================
// 4. IC WRAPPER (Layout)
// =============================================================================

// =============================================================================
// 4. IC WRAPPER (Fixed: 'stroke' renamed to 'outline')
// =============================================================================
#let ic(pos, pins: 8, width: 1.5, pin-pitch: 0.5, pin-length: 0.25, name: none, label: none, outline: 1.5pt + black) = {
    import cetz.draw: *
    
    let count = pins
    let per-side = int(count / 2)
    let height = (per-side + 1) * pin-pitch

    group(name: name, {
        translate(pos)
        // Safe: We pass 'outline' to the key 'stroke'
        let node = (
            size: (width, height),
            stroke: outline, 
            fill: white,
            pins: pins,
            "pin-length": pin-length
        )
        ic-shape(node, 0)
        
        if label != none {
            content((0,0), label)
        }
    })
}

// =============================================================================
// 5. COMPONENT: RESISTOR (European & Compact)
// =============================================================================
#let resistor(start, end, name: none, label: none, value: none, style: "eu", outline: auto) = {
  import cetz.draw: *
  
  get-ctx(ctx => {
    let (ctx, a) = cetz.coordinate.resolve(ctx, start)
    let (ctx, b) = cetz.coordinate.resolve(ctx, end)

    let diff = vec-sub(b, a)
    let dist = vec-len(diff)
    let angle = calc.atan2(diff.at(0), diff.at(1))

    let body-len = 0.6
    let body-h   = 0.2
    let lead-len = (dist - body-len) / 2

    group(name: name, {
      translate(a)
      rotate(angle)

      // Leads
      line((0, 0), (lead-len, 0), stroke: outline)
      line((dist - lead-len, 0), (dist, 0), stroke: outline)

      if style == "eu" {
         let mid = dist / 2
         rect(
           (mid - body-len/2, -body-h/2), 
           (mid + body-len/2, body-h/2), 
           fill: white,
           stroke: outline
         )
      } else {
         let x = lead-len
         let step = body-len / 6
         line(
            (x, 0),
            (x + step * 0.5, body-h),
            (x + step * 1.5, -body-h),
            (x + step * 2.5, body-h),
            (x + step * 3.5, -body-h),
            (x + step * 4.5, body-h),
            (x + step * 5.5, -body-h),
            (x + body-len, 0),
            stroke: outline
         )
      }

      anchor("start", (0, 0))
      anchor("end", (dist, 0))
      anchor("center", (dist / 2, 0))

      let txt-offset = body-h/2 + 0.15
      if label != none {
        content((dist / 2, txt-offset), label, angle: -angle, anchor: "south") 
      }
      if value != none {
        content((dist / 2, -txt-offset), value, angle: -angle, anchor: "north") 
      }
    })
  })
}
// =============================================================================
// 6. DIODE SHAPE HELPER
// =============================================================================
#let diode-shape(kind: "standard", fill_color: white) = {
  import cetz.draw: *
  
  let h = 0.3 
  let w = 0.3

  // 1. Base Triangle
  line(
    (-w/2, -h/2), (w/2, 0), (-w/2, h/2), 
    close: true, 
    fill: fill_color,
    radius:.1
  )

  // 2. Cathode Bar
  let bar-x = w/1.8
  line((bar-x, -h/1.5), (bar-x, h/1.5))

  // 3. Variations
  if kind == "zener" {
    line((bar-x*1.11, h/1.7), (bar-x - 0.1, h/1.4))
    line((bar-x*0.89, -h/1.7), (bar-x + 0.1, -h/1.4))
  } 
  else if kind == "schottky" {
    line((bar-x, h/1.7), (bar-x + 0.08, h/1.7), (bar-x + 0.08, h/1.7 - 0.08))
    line((bar-x, -h/1.7), (bar-x - 0.08, -h/1.7), (bar-x - 0.08, -h/1.7 + 0.08))
  }
  else if kind == "led" {
    let arrow-start-x = -.1
    let arrow-start-y = h/1.8 + 0.1
    for i in (0, 1) {
      let dx = i * 0.15
      let dy = i * 0.05
      line(
        (arrow-start-x + dx, arrow-start-y - dy), 
        (arrow-start-x + dx + 0.2, arrow-start-y - dy + 0.2), 
        mark: (end: ">", scale: 0.5), 
      )
    }
  }
}

// =============================================================================
// 7. COMPONENT: DIODE
// =============================================================================
#let diode(start, end, kind: "standard", name: none, label: none, value: none) = {
  import cetz.draw: *
  
  get-ctx(ctx => {
    let (ctx, a) = cetz.coordinate.resolve(ctx, start)
    let (ctx, b) = cetz.coordinate.resolve(ctx, end)

    let diff = vec-sub(b, a)
    let dist = vec-len(diff)
    let angle = calc.atan2(diff.at(0), diff.at(1))

    let body-len = 0.6 
    let lead-len = (dist - body-len) / 1.6

    group(name: name, {
      translate(a)
      rotate(angle)

      line((0, 0), (lead-len, 0))
      line((dist - lead-len, 0), (dist, 0))

      translate((dist/2, 0))
      // Pass 'stroke' (arg) to 'stroke-arg' (param)
      diode-shape(kind: kind)
      translate((-dist/2, 0)) 

      anchor("start", (0, 0))
      anchor("end", (dist, 0))
      anchor("A", (dist/2 - body-len/2, 0)) 
      anchor("K", (dist/2 + body-len/2, 0))

      let txt-offset = if kind == "led" { 0.5 } else { 0.3 }
      if label != none {
        content((dist / 2, txt-offset), label, angle: -angle, anchor: "south") 
      }
      if value != none {
        content((dist / 2, -0.3), value, angle: -angle, anchor: "north") 
      }
    })
  })
}

// Convenience wrappers
#let zener(..args) = diode(..args, kind: "zener")
#let led(..args) = diode(..args, kind: "led")
#let schottky(..args) = diode(..args, kind: "schottky")
