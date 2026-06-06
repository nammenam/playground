import cv2
import os

video_path = "test.mp4"
output_frames_dir = "images" # COLMAP expects images in an 'images' subdir
output_masks_dir = "masks"
os.makedirs(output_frames_dir, exist_ok=True)
os.makedirs(output_masks_dir, exist_ok=True)

cap = cv2.VideoCapture(video_path)
frame_idx = 0
while True:
    ret, frame = cap.read()
    if not ret:
        break
    cv2.imwrite(os.path.join(output_frames_dir, f"frame_{frame_idx:04d}.png"), frame)
    frame_idx += 1
cap.release()
print(f"Extracted {frame_idx} frames.")
