import numpy as np
import pycolmap
import cv2
from sklearn.cluster import DBSCAN
from sklearn.decomposition import PCA
from itertools import product
import open3d as o3d

# Load reconstruction
reconstruction = pycolmap.Reconstruction("sparse/1/")
print(reconstruction.summary())


# Iterate through cameras, images, and 3D points
valid_points_id = []
for image_id, image in reconstruction.images.items():
    mask = cv2.imread(f"masks/mask_{image_id:04d}.png", cv2.IMREAD_GRAYSCALE)
    if mask is None:
        print(f"No mask found. Skipping image {image_id}.")
        continue
    # print(f"Image {image.name} has {image.num_points2D()} features")
    # print(f"images/frame{image_id}.png")
    print(f"Mask found. Segmenting 2D-points in image {image_id}.")
    for point2D in image.points2D:
        if point2D.has_point3D():
            y, x = point2D.xy
            if mask[int(x), int(y)] == 255:
                valid_points_id.append(point2D.point3D_id)


point3D_ids = list(reconstruction.points3D.keys())
valid_points_id_arr = np.array(valid_points_id)
valid_points_3d = []
print("\nRunning DBSCAN to remove outliers")
for pid in valid_points_id_arr:
    point = reconstruction.points3D[pid].xyz
    if point is not None:
        valid_points_3d.append(point)


db = DBSCAN(eps=0.1, min_samples=60).fit(valid_points_3d)
labels = db.labels_

# Retain only points that are not labeled as noise
inlier_ids = [pid for pid, label in zip(valid_points_id_arr, labels) if label != -1]
inlier_ids = np.array(inlier_ids)

print(f"\nRemoved {valid_points_id_arr.shape[0] - inlier_ids.shape[0]} points after DBSCAN")

points_xyz = np.array([reconstruction.points3D[pid].xyz for pid in inlier_ids])


print("\nMaking point cloud")
for point3D_id in point3D_ids:
    if point3D_id not in inlier_ids:
        reconstruction.delete_point3D(point3D_id)


# Center the point cloud
points_centered = points_xyz - points_xyz.mean(axis=0)

# PCA for orientation
pca = PCA(n_components=3)
pca.fit(points_centered)

# Project to PCA space
points_pca = pca.transform(points_centered)

# Get AABB in PCA space
min_pca = points_pca.min(axis=0)
max_pca = points_pca.max(axis=0)

# Generate corner points in PCA space
obb_corners_pca = np.array(list(product(*zip(min_pca, max_pca))))

# Transform corners back to original space
obb_corners = pca.inverse_transform(obb_corners_pca) + points_xyz.mean(axis=0)

print("Oriented Bounding Box corners (world coordinates):")
print(obb_corners)


print(f"Number of remaining points: {reconstruction.num_points3D()}")




# === Prepare 3D points from valid inlier IDs ===
points_xyz = np.array([reconstruction.points3D[pid].xyz for pid in inlier_ids])

# === Create Open3D point cloud ===
pcd = o3d.geometry.PointCloud()
pcd.points = o3d.utility.Vector3dVector(points_xyz)
pcd.paint_uniform_color([0.1, 0.7, 0.1])  # Green car

# === Oriented Bounding Box using PCA ===
obb = pcd.get_oriented_bounding_box()
obb.color = (0, 0, 1)  # Blue box

# === Visualize ===
o3d.visualization.draw_geometries([pcd, obb])

new_filename = "car1_masked_v2"
reconstruction.write("car1_masked_v2")
print(f"\nSaved new point cloud to '{new_filename}'")


