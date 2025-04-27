#!/usr/bin/env python

import sys
import numpy as np
from scipy.spatial import Delaunay
import random

x_range = [-100, 100]
y_range = [-100, 100]

if len(sys.argv) < 2:
    sys.stderr.write("Usage: generator.py num_points\n")
    sys.exit(1)

n_points = int(sys.argv[1])
points = []

for _ in range(n_points):
    x = np.random.randint(x_range[0], x_range[1])
    y = np.random.randint(y_range[0], y_range[1])
    points.append([x, y])

points = np.array(points)
tri = Delaunay(points)

# --- Extract edges from Delaunay triangles ---
edges = set()
for t in tri.simplices:
    pairs = [(t[0], t[1]), (t[1], t[2]), (t[2], t[0])]
    for i, j in pairs:
        edge = tuple(sorted((i, j)))
        edges.add(edge)

edges = list(edges)

# --- Sample a subset of edges for general subdivision ---
subset_ratio = 0.7  # choose 70% of the edges (tweakable)
subset_size = int(len(edges) * subset_ratio)
selected_edges = random.sample(edges, subset_size)

with open("input.txt", "w") as f:
    f.write(f"{len(selected_edges)}\n")
    for i, j in selected_edges:
        x1, y1 = points[i]
        x2, y2 = points[j]
        f.write(f"{x1} {y1} {x2} {y2}\n")

    # Generate query point
    xq = np.random.randint(x_range[0] // 2, x_range[1] // 2)
    yq = np.random.randint(y_range[0] // 2, y_range[1] // 2)
    f.write(f"{xq} {yq}\n")
