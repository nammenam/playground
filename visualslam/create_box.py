import numpy as np
import pycolmap
import cv2
from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('TkAgg')


def create_bounding_box(reconstruction, p0=0.1, p1=0.9):
    bbox = np.array(reconstruction.compute_bounding_box(p0=p0, p1=p1))
    vtx0 = np.array((bbox[0, 0], bbox[0, 1], bbox[0, 2]))
    vtx1 = np.array((bbox[0, 0], bbox[0, 1], bbox[1, 2]))
    vtx2 = np.array((bbox[0, 0], bbox[1, 1], bbox[1, 2]))
    vtx3 = np.array((bbox[0, 0], bbox[1, 1], bbox[0, 2]))
    vtx4 = np.array((bbox[1, 0], bbox[0, 1], bbox[0, 2]))
    vtx5 = np.array((bbox[1, 0], bbox[0, 1], bbox[1, 2]))
    vtx6 = np.array((bbox[1, 0], bbox[1, 1], bbox[1, 2]))
    vtx7 = np.array((bbox[1, 0], bbox[1, 1], bbox[0, 2]))
    return np.stack((vtx0, vtx1, vtx2, vtx3, vtx4, vtx5, vtx6, vtx7))


def project_vertexes_to_image(image_id, image, vertexes):
    image_file = cv2.imread(f"images/frame_{image_id - 1:04d}.png")

    image_points = []
    for vertex in vertexes:
        try:
            a = image.project_point(vertex)
            b = np.round(a)
            c = np.int_(b)
            image_points.append(c)
        except:
            continue
    image_points_arr = np.array(image_points)

    for point in image_points_arr:
        cv2.circle(image_file, (point[1], point[0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[0, 1], image_points[0, 0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[1, 1], image_points[1, 0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[2, 1], image_points[2, 0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[3, 1], image_points[3, 0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[4, 1], image_points[4, 0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[5, 1], image_points[5, 0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[6, 1], image_points[6, 0]), radius=5, color=(0, 0, 255), thickness=2)
    # cv2.circle(image_file, (image_points[7, 1], image_points[7, 0]), radius=5, color=(0, 0, 255), thickness=2)
    cv2.imshow("Keypoint", image_file)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


def plot_vertexes(vertexes):
    fig = plt.figure()
    ax = plt.axes(projection='3d')

    x = vertexes[:, 0]
    y = vertexes[:, 1]
    z = vertexes[:, 2]
    ax.scatter(x, y, z)

    # syntax for plotting
    ax.set_title("Bounding Box vertexes")
    plt.show()


def plot_all_points(reconstruction, image_id, image, vertexes):
    image_file = cv2.imread(f"images/frame_{image_id - 1:04d}.png")
    camera = image.camera
    # for point3D in reconstruction.point
    image_points = np.int_(np.round(camera.img_from_cam(image.cam_from_world * vertexes)))
    print(min(image_points[:, 0]), max(image_points[:, 0]))
    print(min(image_points[:, 1]), max(image_points[:, 1]))
    cv2.circle(image_file, (image_points[0, 1], image_points[0, 0]), radius=5, color=(0, 0, 255), thickness=2)


if __name__ == "__main__":
    # Load reconstruction
    reconstruction = pycolmap.Reconstruction("car1_masked_v2")
    print(reconstruction.summary())

    # Creating Bounding Box
    vertexes = create_bounding_box(reconstruction)

    # plot_vertexes(vertexes)
    vertexes1 = []
    for point3D in reconstruction.points3D.values():
        vertexes1.append(point3D.xyz)
    vertexes2 = np.array(vertexes1)

    for image_id, image in reconstruction.images.items():
        project_vertexes_to_image(image_id, image, vertexes2)
