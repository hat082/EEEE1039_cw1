# 35 max
# 0 55 
# 0 94

import cv2
import numpy as np

def onTrackbarChange(pos):
    pass

def main():
    # Read the input image
    input_image = cv2.imread('/home/hat082/coursework/tests_py/image.png')

    # Convert the input image to HSV color space
    hsv_image = cv2.cvtColor(input_image, cv2.COLOR_BGR2HSV)

    # Initialize the HSV threshold values
    h_min, h_max = 0, 179  # Hue range
    s_min, s_max = 0, 255  # Saturation range
    v_min, v_max = 0, 255  # Value range

    # Initialize the blur and dilation parameters
    dilation_kernel = np.ones((5, 5), np.uint8)

    # Create a window to display the input and output images
    cv2.namedWindow('Color Extraction')

    # Create trackbars to adjust the HSV threshold values
    cv2.createTrackbar('H_min', 'Color Extraction', h_min, 179, onTrackbarChange)
    cv2.createTrackbar('H_max', 'Color Extraction', h_max, 179, onTrackbarChange)
    cv2.createTrackbar('S_min', 'Color Extraction', s_min, 255, onTrackbarChange)
    cv2.createTrackbar('S_max', 'Color Extraction', s_max, 255, onTrackbarChange)
    cv2.createTrackbar('V_min', 'Color Extraction', v_min, 255, onTrackbarChange)
    cv2.createTrackbar('V_max', 'Color Extraction', v_max, 255, onTrackbarChange)

    # Create trackbars to adjust blur and dilation parameters
    cv2.createTrackbar('Dilation Size', 'Color Extraction', 1, 20, onTrackbarChange)


    while True:
        # Get the current HSV threshold values from the trackbars
        h_min = cv2.getTrackbarPos('H_min', 'Color Extraction')
        h_max = cv2.getTrackbarPos('H_max', 'Color Extraction')
        s_min = cv2.getTrackbarPos('S_min', 'Color Extraction')
        s_max = cv2.getTrackbarPos('S_max', 'Color Extraction')
        v_min = cv2.getTrackbarPos('V_min', 'Color Extraction')
        v_max = cv2.getTrackbarPos('V_max', 'Color Extraction')

        dilation_size = cv2.getTrackbarPos('Dilation Size', 'Color Extraction')

        # Create a numpy array for the lower and upper HSV threshold values
        lower_bound = np.array([h_min, s_min, v_min])
        upper_bound = np.array([h_max, s_max, v_max])

        # Create a mask based on the HSV threshold values
        mask = cv2.inRange(hsv_image, lower_bound, upper_bound)

        # Apply dilation to the mask
        dilated_mask = cv2.dilate(mask, np.ones((dilation_size, dilation_size), np.uint8), iterations=1)
        dilated_mask = cv2.erode(dilated_mask,np.ones((dilation_size, dilation_size), np.uint8), iterations=1)

        cv2.imshow('Color Extraction', dilated_mask)
        # # Bitwise AND the mask with the input image to extract the color
        # extracted_color = cv2.bitwise_and(input_image, input_image, mask=mask)

        # # Display the input image and the binary image with the extracted color
        # cv2.imshow('Color Extraction', np.hstack([input_image, extracted_color]))

        # Break the loop if the 'q' key is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release the window and camera resources
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
