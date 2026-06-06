import os
import xml.etree.ElementTree as ET
import numpy as np
import cv2

# Configuration
xml_path = 'annotations.xml'
output_dir = './masks'
image_shape = (360, 640)  # height, width from <original_size>
os.makedirs(output_dir, exist_ok=True)

def decode_rle(rle_str, width, height):
    """Decode CVAT RLE into a binary mask."""
    counts = list(map(int, rle_str.strip().split(',')))
    values = []
    value = 0
    for count in counts:
        values.extend([value] * count)
        value = 1 - value  # toggle between 0 and 1
    mask = np.array(values, dtype=np.uint8).reshape((height, width))
    return mask

# Parse XML
tree = ET.parse(xml_path)
root = tree.getroot()

for track in root.findall('track'):
    label = track.attrib.get('label')
    if label != 'Truck':
        continue

    for mask_elem in track.findall('mask'):
        frame = int(mask_elem.attrib['frame'])
        left = int(mask_elem.attrib['left'])
        top = int(mask_elem.attrib['top'])
        width = int(mask_elem.attrib['width'])
        height = int(mask_elem.attrib['height'])
        rle = mask_elem.attrib['rle'].strip()

        # Decode RLE to binary mask
        mask = decode_rle(rle, width, height)

        # Place it on full-size canvas
        full_mask = np.zeros(image_shape, dtype=np.uint8)
        full_mask[top:top + height, left:left + width] = mask

        # Save as PNG
        filename = os.path.join(output_dir, f'mask_{frame:04}.png')
        cv2.imwrite(filename, full_mask * 255)

print("✅ Done: Masks saved to", output_dir)
