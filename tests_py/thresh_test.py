import cv2
import numpy as np

# Load the HSV image
hsv_image = cv2.imread('./sample_line.png')
hsv_image = cv2.cvtColor(hsv_image, cv2.COLOR_BGR2HSV)

# Split the HSV image into its components (H, S, V)
h, s, v = cv2.split(hsv_image)

# Apply adaptive thresholding to the S and V components and combine the results
s_threshold = cv2.adaptiveThreshold(s, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 11, 2)
v_threshold = cv2.adaptiveThreshold(v, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 11, 2)

# Combine the thresholded S and V components
combined_threshold = cv2.bitwise_and(s_threshold, v_threshold)

# Display the combined thresholding result
cv2.imshow('Combined Threshold', combined_threshold)
cv2.waitKey(0)
cv2.destroyAllWindows()
