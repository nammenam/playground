// #set page(width: auto, height: auto)
#import "/src/cetz.typ" as cetz
#import "/src/lib.typ": *

// =============================================================================
// 5. DEBUG CANVAS
// =============================================================================
#set text(font:"Geist", size: 10pt, weight: "medium")

#cetz.canvas({
    import cetz.draw: *
    
    // Grid: 0.5 steps (light) and 1.0 steps (dark)
    grid((-2,-2), (8,6), stroke: gray + 0.2pt, step: .5)
    
    set-style(stroke: 1.5pt + black)

    // 1. Place IC (Default: Width 1.5, Pin-Len 0.25 -> Total X Span 2.0)
    //    Center at (1, 2). Pins will land on x=0 and x=2.
    ic((1, 2), label: "555", pins: 24, name: "timer")
    
    // 2. Place LED
    circle((6, 3), radius: 0.3, name: "led", fill: white)
    
    // 3. Connect
    // Note: Timer Pin 3 is at (0, 1.75) -> Grid/4 alignment on Y axis
    // wire("timer.3", (l:1), (up:2),(r:7), "led", radius: .1)

    // Cross wire (shows white gap)
    wire((3, 5), (d:1),(r:1),  radius:.1)
    resistor((4,0),(4,2))

    diode((3, 2.5), (5, 2.5), name: "D1")

    zener((7, 4), (7, 2), name: "Z1")
    zener((5, 1), (7, 1), name: "Z1")
    led((5, 5), (7, 5), name: "L1")    
    schottky((5, 0), (7, 0), name: "S1")    
    // Connection dot
    circle((3, 3), radius: 0.08, fill: black)
})
