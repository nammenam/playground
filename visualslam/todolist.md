# To-Do List TEK5030 Project
We'll be focusing on the code from lab 11, since it runs.

## Goals
Our main goals are 
1. Export images to the AI and use the response from the AI in the output visualisation.
2. Track the position of the camera and visualise the tracking in a rudimentary 2D map.

### For goal 1, our objectives are:
1. Being able to change the image source to a pre-recorded video.
2. Being able to extract images from a live stream or pre-recorded video.
3. Being able to alter images from a live stream or pre-recorded video.
4. Being able to inject images from a live stream or pre-recorded video.

### For goal 2, our objectives are:
1. Automatically use every n-th frame as a keyframe. 
2. Create a suitable coordinate system.
3. Reliably determine the position of the camera for each frame.
4. Plot the camera position for each keyframe in the coordinate system or 2D map.


## Tasks
- [x] Find out where each image is temporarily saved.
  - Saved in the tracking_frame variable (comman_lab_utils.frame object). tracking_frame.colour_image gives a Ny\*Nx\*3 array with the RGB values.
- [x] Create a function which returns an RGB or BW version of each n-th image.
  - Done in export_image.
- [x] Create a function to alter an image.
  - Done in modify_image.
- [x] Create a function to replace an image with a saved image.
  - Done in modify_image.
- [ ] Find out which coordinate system is used in the tracker.
- [ ] Find out how to access the coordinate system
- [ ] Find out how to place our own markers in the coordinate system
- [ ] Find out where the position/coordinates of the camera are stored
- [ ] Create function to extract camera coordinates in the tracker coordinate system.
- [ ] Create a function to place a marker with camera coordinates in the tracker


## New Tasks for COLMAP
- [x] Find out what type of format colmap exports.
- [x] Find out how to modify point cloud manually to only the car.
- [x] Create bounding box
- [x] Find 3D coordinates of the corners of the bounding box
- [ ] Find 2D coordinates of the bounding box corners in image
- [ ] Plot bounding box in image by modifying the image
- [ ] Repeat for each image
- [ ] Create video from modified images