import numpy as np
from scipy.spatial import Delaunay
import random

def generate_non_intersecting_segments(N_points=20, seed=42):
    np.random.seed(seed)
    random.seed(seed)

    points = np.random.rand(N_points, 2) * 10
    tri = Delaunay(points)

    edges = set()
    for triangle in tri.simplices:
        indices = [(0, 1), (1, 2), (2, 0)]
        for i, j in indices:
            p1 = tuple(points[triangle[i]])
            p2 = tuple(points[triangle[j]])
            edge = tuple(sorted((p1, p2)))
            edges.add(edge)

    return list(edges)

def save_segments_to_file(segments, filename="input.txt"):
    with open(filename, "w") as f:
        f.write(f"{len(segments)}\n")
        for (x1, y1), (x2, y2) in segments:
            f.write(f"{x1} {y1} {x2} {y2}\n")
        xq, yq = np.random.rand(2) * 100
        f.write(f"{xq} {yq}\n")

segments = generate_non_intersecting_segments(N_points=25)
save_segments_to_file(segments)
