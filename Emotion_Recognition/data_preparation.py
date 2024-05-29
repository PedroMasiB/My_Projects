import pandas as pd
import numpy as np
import cv2
import os

"""
class data_preparation():
    def __init__(self):
        self.data = pd.DataFrame()
        self.load_data()
        self.prepare_data()
        self.save_data()
    
    def load_data(self):
        image = cv2.imread('data/emotions.jpg')
"""

# Get the full path to the script's directory
script_dir = os.path.dirname(os.path.abspath(__file__))

# Construct the full path to the image file
image_path = os.path.join(script_dir, 'agustinlindo.jpeg')


print(image_path)
image = cv2.imread('agustinlindo.jpeg')
grey_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
scaled_image = 0.2
new_dim = (int(grey_image.shape[1] * scaled_image), int(grey_image.shape[0] * scaled_image))
grey_image = cv2.resize(grey_image, new_dim)
print(grey_image.shape)
flattened_image = grey_image.flatten()
df = pd.DataFrame(flattened_image)
print(df)



"""
cv2.imshow('Image', grey_image)
cv2.waitKey(0)
cv2.destroyAllWindows()
"""