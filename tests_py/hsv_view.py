import cv2
import numpy as np

# Global variables
selected_points = []
new_selection = False

# Mouse callback function
def select_region(event, x, y, flags, param):
    global selected_points, new_selection
    if event == cv2.EVENT_LBUTTONDOWN:
        if new_selection:
            selected_points = [(x, y)]
            new_selection = False
        else:
            selected_points = [(x, y)]
            cv2.imshow('image', image)  # Show the original image to remove previous selection

    elif event == cv2.EVENT_LBUTTONUP:
        selected_points.append((x, y))
        cv2.rectangle(image, selected_points[0], selected_points[1], (0, 255, 0), 2)
        cv2.imshow('image', image)

# Read the input image
image = cv2.imread('/home/hat082/coursework/tests_py/sample_line.png')
cv2.imshow('image', image)
cv2.setMouseCallback('image', select_region)

# Main loop to calculate HSV range
while True:
    if len(selected_points) == 2:
        roi = image[selected_points[0][1]:selected_points[1][1], selected_points[0][0]:selected_points[1][0]]
        hsv_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        h, s, v = cv2.split(hsv_roi)
        h_min, h_max = np.min(h), np.max(h)
        s_min, s_max = np.min(s), np.max(s)
        v_min, v_max = np.min(v), np.max(v)
        print(f"Hue Range: {h_min}-{h_max}, Saturation Range: {s_min}-{s_max}, Value Range: {v_min}-{v_max}")
        selected_points = []  # Reset selected points for new selection
        new_selection = True  # Set flag for new selection
    cv2.waitKey(10)


