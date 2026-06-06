import torch
import torchvision.transforms as T
from PIL import Image
import cv2
import numpy as np
import os # For manual mask example later, if needed

model = torch.hub.load('pytorch/vision:v0.13.0', 'deeplabv3_resnet101', pretrained=True)
model.eval()

COCO_INSTANCE_CATEGORY_NAMES = [
    '__background__', 'person', 'bicycle', 'car', 'motorcycle', 'airplane', 'bus',
    'train', 'truck', 'boat', 'traffic light', 'fire hydrant', 'N/A', 'stop sign',
    'parking meter', 'bench', 'bird', 'cat', 'dog', 'horse', 'sheep', 'cow',
    'elephant', 'bear', 'zebra', 'giraffe', 'N/A', 'backpack', 'umbrella', 'N/A', 'N/A',
    'handbag', 'tie', 'suitcase', 'frisbee', 'skis', 'snowboard', 'sports ball',
    'kite', 'baseball bat', 'baseball glove', 'skateboard', 'surfboard', 'tennis racket',
    'bottle', 'N/A', 'wine glass', 'cup', 'fork', 'knife', 'spoon', 'bowl',
    'banana', 'apple', 'sandwich', 'orange', 'broccoli', 'carrot', 'hot dog', 'pizza',
    'donut', 'cake', 'chair', 'couch', 'potted plant', 'bed', 'N/A', 'dining table',
    'N/A', 'N/A', 'toilet', 'N/A', 'tv', 'laptop', 'mouse', 'remote', 'keyboard', 'cell phone',
    'microwave', 'oven', 'toaster', 'sink', 'refrigerator', 'N/A', 'book',
    'clock', 'vase', 'scissors', 'teddy bear', 'hair drier', 'toothbrush'
]

TARGET_CLASS_INDICES = [
    # COCO_INSTANCE_CATEGORY_NAMES.index('truck'), # Should be 8
    # COCO_INSTANCE_CATEGORY_NAMES.index('bus'),   # Should be 6
    COCO_INSTANCE_CATEGORY_NAMES.index('train'),   # Should be 3
]
print(f"Targeting class indices: {TARGET_CLASS_INDICES} for classes: {[COCO_INSTANCE_CATEGORY_NAMES[i] for i in TARGET_CLASS_INDICES]}")


preprocess = T.Compose([
    T.ToTensor(),
    T.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
])

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
model.to(device)
print(f"Using device: {device}")

DEBUG_SEGMENTATION = True

def segment_frame(frame, model, target_indices):
    img_pil = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
    input_tensor = preprocess(img_pil)
    input_batch = input_tensor.unsqueeze(0).to(device)

    with torch.no_grad():
        output = model(input_batch)['out'][0]
    output_predictions = output.argmax(0) # (H, W)

    if DEBUG_SEGMENTATION:
        detected_classes_in_frame = torch.unique(output_predictions).cpu().numpy()
        print(f"  [DEBUG] Model detected class indices in this frame: {detected_classes_in_frame}")
        detected_names = [COCO_INSTANCE_CATEGORY_NAMES[i] for i in detected_classes_in_frame if i < len(COCO_INSTANCE_CATEGORY_NAMES)]
        print(f"  [DEBUG] Model detected class names: {detected_names}")
        print(f"  [DEBUG] We are looking for indices: {target_indices}")

    # Create initial binary mask for target classes
    initial_binary_mask_torch = torch.zeros_like(output_predictions, dtype=torch.uint8)
    found_any_target_pixel = False
    for target_idx in target_indices:
        if torch.any(output_predictions == target_idx):
            initial_binary_mask_torch[output_predictions == target_idx] = 255
            found_any_target_pixel = True
            if DEBUG_SEGMENTATION:
                print(f"  [DEBUG] Found pixels for target index: {target_idx} ({COCO_INSTANCE_CATEGORY_NAMES[target_idx]})")
    
    if DEBUG_SEGMENTATION and not found_any_target_pixel:
        print(f"  [DEBUG] NO pixels found for any target class in {target_indices}.")


    mask_np_raw = initial_binary_mask_torch.cpu().numpy()

    if DEBUG_SEGMENTATION:
        cv2.imshow("Debug: 0 - Raw Mask (Before Post-processing)", mask_np_raw)

    if not found_any_target_pixel:
        if DEBUG_SEGMENTATION:
            print("  [DEBUG] Skipping post-processing as no target pixels in raw mask.")
        return mask_np_raw # This will be an all-black mask

    # --- Post-processing ---
    processed_mask = mask_np_raw.copy() # Start with the raw mask

    kernel_close_size = 7 # Adjust kernel size as needed (e.g., 5, 7, 9)
    kernel_close = np.ones((kernel_close_size, kernel_close_size), np.uint8)
    if np.any(processed_mask): # Only apply if there's something to close
        processed_mask_closed = cv2.morphologyEx(processed_mask, cv2.MORPH_CLOSE, kernel_close)
        if DEBUG_SEGMENTATION:
            print(f"  [DEBUG] Applied Closing with kernel {kernel_close_size}x{kernel_close_size}.")
            cv2.imshow("Debug: 1 - Mask After Closing", processed_mask_closed)
            # cv2.waitKey(0)
        processed_mask = processed_mask_closed
    elif DEBUG_SEGMENTATION:
        print("  [DEBUG] Skipped Closing as mask was already empty.")


    kernel_open_size = 5 # Adjust kernel size as needed (e.g., 3, 5, 7)
    kernel_open = np.ones((kernel_open_size, kernel_open_size), np.uint8)
    if np.any(processed_mask): # Only apply if there's something to open
        processed_mask_opened = cv2.morphologyEx(processed_mask, cv2.MORPH_OPEN, kernel_open)
        if DEBUG_SEGMENTATION:
            print(f"  [DEBUG] Applied Opening with kernel {kernel_open_size}x{kernel_open_size}.")
            cv2.imshow("Debug: 2 - Mask After Opening", processed_mask_opened)
            # cv2.waitKey(0)
        processed_mask = processed_mask_opened
    elif DEBUG_SEGMENTATION:
        print("  [DEBUG] Skipped Opening as mask was already empty.")


    if np.any(processed_mask): # Only apply if there's something to find components in
        num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(processed_mask, connectivity=8)
        if DEBUG_SEGMENTATION:
            print(f"  [DEBUG] LCC: num_labels found (incl. background): {num_labels}")
            if num_labels > 1:
                areas = stats[1:, cv2.CC_STAT_AREA]
                print(f"  [DEBUG] LCC: Areas of components (excluding bg 0): {areas}")

        if num_labels > 1: # More than just the background
            largest_label_component_idx = 1 + np.argmax(stats[1:, cv2.CC_STAT_AREA]) # This is the label ID
            if DEBUG_SEGMENTATION:
                print(f"  [DEBUG] LCC: Largest component label ID: {largest_label_component_idx}, Area: {stats[largest_label_component_idx, cv2.CC_STAT_AREA]}")
            
            refined_mask = np.zeros_like(processed_mask)
            refined_mask[labels == largest_label_component_idx] = 255
            processed_mask = refined_mask
            if DEBUG_SEGMENTATION:
                 cv2.imshow("Debug: 3 - Mask After LCC", processed_mask)
                 # cv2.waitKey(0)
        elif num_labels <= 1 and DEBUG_SEGMENTATION: # Only background or empty mask before LCC
            print("  [DEBUG] LCC: No foreground components found, mask remains as is (likely empty).")
            # processed_mask is already what it was before LCC (potentially empty)
    elif DEBUG_SEGMENTATION:
        print("  [DEBUG] Skipped LCC as mask was already empty.")
        
    return processed_mask


# --- Video Processing --
video_path = "./test.mp4"
cap = cv2.VideoCapture(video_path)

if not cap.isOpened():
    print(f"Error: Could not open video {video_path}")
    exit()

output_video_path = "segmented_truck_debug.mp4"
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
fps = int(cap.get(cv2.CAP_PROP_FPS))
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
out_video = cv2.VideoWriter(output_video_path, fourcc, fps, (width, height), isColor=False)

frame_count = 0
while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame_count += 1
    print(f"\nProcessing frame {frame_count}...")
    
    truck_mask = segment_frame(frame.copy(), model, TARGET_CLASS_INDICES)

    cv2.imshow("Final Truck Mask", truck_mask)
    out_video.write(truck_mask)

    key_pressed = -1
    if DEBUG_SEGMENTATION:
        print("  [DEBUG] Press 'c' to continue to next frame, 'q' to quit. (Focus on a cv2 window)")
        key_pressed = cv2.waitKey(0) & 0xFF
    else:
        key_pressed = cv2.waitKey(1) & 0xFF

    if key_pressed == ord('q'):
        break
    elif DEBUG_SEGMENTATION and key_pressed == ord('c'):
        cv2.destroyWindow("Debug: 0 - Raw Mask (Before Post-processing)")
        cv2.destroyWindow("Debug: 1 - Mask After Closing")
        cv2.destroyWindow("Debug: 2 - Mask After Opening")
        cv2.destroyWindow("Debug: 3 - Mask After LCC")
        continue
    elif DEBUG_SEGMENTATION and key_pressed != ord('c'): # If any other key in debug, treat as quit
        print("  [DEBUG] Non-'c' key pressed in debug mode. Quitting.")
        break


cap.release()
out_video.release()
cv2.destroyAllWindows()
print(f"Processed video saved to {output_video_path}")
