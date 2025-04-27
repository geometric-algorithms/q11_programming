import matplotlib.pyplot as plt
import matplotlib.patches as patches

def clip(val, lower=-100, upper=100):
    return max(lower, min(upper, val))

def line_intersection_with_vertical(x, x1, y1, x2, y2):
    """Returns intersection (x, y) of vertical line x with segment (x1, y1)-(x2, y2)"""
    if x1 == x2:
        return (x, y1)  # vertical segment
    slope = (y2 - y1) / (x2 - x1)
    y = y1 + slope * (x - x1)
    return (x, y)

# --- Read data from input.txt ---
segments = []
trap_top = trap_bot = trap_left = trap_right = None
query_point = None

with open("data.txt", "r") as f:
    for line in f:
        tokens = line.strip().split()
        if tokens[0] == "SEG":
            x1, y1, x2, y2 = map(float, tokens[1:])
            segments.append(((x1, y1), (x2, y2)))
        elif tokens[0] == "Above":
            trap_top = list(map(float, tokens[1:]))
        elif tokens[0] == "Below":
            trap_bot = list(map(float, tokens[1:]))
        elif tokens[0] == "Left":
            trap_left = list(map(float, tokens[1:]))
        elif tokens[0] == "Right":
            trap_right = list(map(float, tokens[1:]))
        elif tokens[0] == "QUERY":
            query_point = list(map(float, tokens[1:]))

# --- Plotting ---
fig, ax = plt.subplots(figsize=(8, 8))

# Plot segments
for (x1, y1), (x2, y2) in segments:
    x1, y1 = clip(x1), clip(y1)
    x2, y2 = clip(x2), clip(y2)
    ax.plot([x1, x2], [y1, y2], color="black", linewidth=1)

# Draw filled trapezoid using intersections
if trap_top and trap_bot and trap_left and trap_right:
    lx = clip(trap_left[0])
    rx = clip(trap_right[0])

    # Get intersection points with top segment
    tx1, ty1, tx2, ty2 = trap_top
    top_left = line_intersection_with_vertical(lx, tx1, ty1, tx2, ty2)
    top_right = line_intersection_with_vertical(rx, tx1, ty1, tx2, ty2)

    # Get intersection points with bottom segment
    bx1, by1, bx2, by2 = trap_bot
    bot_left = line_intersection_with_vertical(lx, bx1, by1, bx2, by2)
    bot_right = line_intersection_with_vertical(rx, bx1, by1, bx2, by2)

    # Clip all points
    trapezoid_polygon = [
        (clip(bot_left[0]), clip(bot_left[1])),
        (clip(bot_right[0]), clip(bot_right[1])),
        (clip(top_right[0]), clip(top_right[1])),
        (clip(top_left[0]), clip(top_left[1])),
    ]

    ax.add_patch(patches.Polygon(trapezoid_polygon, closed=True, color="orange", alpha=0.3))

# Draw query point
if query_point:
    xq, yq = clip(query_point[0]), clip(query_point[1])
    ax.plot(xq, yq, marker='*', markersize=12, color='red', label='Query Point')

ax.set_xlim(-100, 100)
ax.set_ylim(-100, 100)
ax.set_aspect('equal')
ax.set_title("Trapezoid Region from Intersections")
ax.legend()
plt.grid(True)
plt.show()
