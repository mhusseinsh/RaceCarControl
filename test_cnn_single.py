#!/usr/bin/python
# import the necessary packages
import os
import sys
import csv
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt

from keras.preprocessing.image import img_to_array, load_img
from keras.models import Sequential, model_from_json
from keras.layers import Dense, Activation, Conv2D, MaxPooling2D, Dropout, Flatten
from keras.utils import plot_model

# load model
print("Loading Model")
json_file = open('model.json','r')
model = json_file.read()
json_file.close()
ml = model_from_json(model)
ml.load_weights("model.h5")
print("Model loaded")

filename = sys.argv[1]
image = np.array(img_to_array(load_img(filename)))
image = image.reshape(1, image.shape[0], image.shape[1], image.shape[2])
print(image.shape)

# predicting image
x = ml.predict(image)
print('x = ', x[0][0])
print('y = ', x[0][1])
print('psi = ', x[0][2])
