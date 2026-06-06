import collections

# The Physical Graph: 0 --- 1 --- 2
N = 3
adj = {0: [1], 1: [0, 2], 2: [1]}

COP_TURN = 0
ROBBER_TURN = 1

# State tracking
out_degree = {}
win_state = {}
bfs_step = {}  # Tracks the exact step a state was proven to be a Win
edges_drawn = []  # Stores the topological arrows for Graphviz

queue = collections.deque()

# 1. Initialize states
for c in range(N):
    for r in range(N):
        # Cop can move or wait (+1)
        out_degree[(c, r, COP_TURN)] = len(adj[c]) + 1
        # Robber MUST move
        out_degree[(c, r, ROBBER_TURN)] = len(adj[r])

        win_state[(c, r, COP_TURN)] = False
        win_state[(c, r, ROBBER_TURN)] = False

# 2. Base Cases (The Traps)
for i in range(N):
    for turn in [COP_TURN, ROBBER_TURN]:
        state = (i, i, turn)
        win_state[state] = True
        bfs_step[state] = 0  # Step 0: The Origin
        queue.append(state)

# 3. Retrograde BFS (The Multiverse Flood-Fill)
current_step = 1

while queue:
    # Process everything currently in the queue as a single "Step" in time
    level_size = len(queue)
    for _ in range(level_size):
        c, r, turn = queue.popleft()
        curr_state = (c, r, turn)

        if turn == COP_TURN:
            # Look backward: Robber moved here
            for prev_r in adj[r]:
                prev_state = (c, prev_r, ROBBER_TURN)
                if not win_state[prev_state]:
                    out_degree[prev_state] -= 1

                    # Draw a dotted arrow showing out_degree being chipped away
                    edges_drawn.append(
                        f'  "{curr_state}" -> "{prev_state}" [style="dashed", color="gray"];'
                    )

                    if out_degree[prev_state] == 0:
                        win_state[prev_state] = True
                        bfs_step[prev_state] = current_step
                        queue.append(prev_state)
                        # Draw a solid bold arrow showing the trap snapping shut
                        edges_drawn.append(
                            f'  "{curr_state}" -> "{prev_state}" [style="bold", color="red"];'
                        )

        else:
            # Look backward: Cop moved here (or waited)
            prev_cop_positions = adj[c] + [c]  # Include waiting
            for prev_c in prev_cop_positions:
                prev_state = (prev_c, r, COP_TURN)
                if not win_state[prev_state]:
                    win_state[prev_state] = True
                    bfs_step[prev_state] = current_step
                    queue.append(prev_state)
                    # Cop only needs one move, trap snaps shut instantly
                    edges_drawn.append(
                        f'  "{curr_state}" -> "{prev_state}" [style="bold", color="blue"];'
                    )

    current_step += 1

# =====================================================================
# 4. Generate Graphviz DOT Output
# =====================================================================
with open("multiverse.dot", "w") as f:
    f.write("digraph StateSpace {\n")
    f.write('  rankdir="LR";\n')
    f.write('  node [shape="box", style="filled", fontname="Courier"];\n\n')

    # Draw Nodes
    for c in range(N):
        for r in range(N):
            for turn in [COP_TURN, ROBBER_TURN]:
                state = (c, r, turn)
                turn_str = "Cop" if turn == COP_TURN else "Rob"
                label = f"C:{c} R:{r}\\n{turn_str} Turn"

                if win_state[state]:
                    step = bfs_step[state]
                    if step == 0:
                        color = "gold"  # Base Case
                    elif step == 1:
                        color = "lightgreen"
                    elif step == 2:
                        color = "skyblue"
                    else:
                        color = "plum1"
                    f.write(
                        f'  "{state}" [label="{label}\\n(Step {step})", fillcolor="{color}"];\n'
                    )
                else:
                    # Unreachable loops / safe states
                    f.write(
                        f'  "{state}" [label="{label}\\nESCAPES", fillcolor="grey40", fontcolor="white"];\n'
                    )

    f.write("\n")
    # Draw Edges
    for edge in edges_drawn:
        f.write(edge + "\n")

    f.write("}\n")

print("Generated multiverse.dot!")
print("Run: dot -Tpng multiverse.dot -o multiverse.png")
