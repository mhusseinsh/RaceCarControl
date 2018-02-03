#!/usr/bin/python
# import the necessary packages
import os
import sys
import csv
import time
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import shutil

from keras.preprocessing.image import img_to_array, load_img
from keras.models import Sequential, model_from_json
from keras.layers import Dense, Activation, Conv2D, MaxPooling2D, Dropout, Flatten

images = list()
labels_list = list()
image_names = list()

# function that handels csv data supported
# processing dataset to extract test images as arrays as well as labels
# ---------------------------------------------------------------------
def test_batch(images_path, labels_path):
    global images
    global labels_list
    global image_names
    with open(labels_path) as f:
        read_file = csv.reader(f, delimiter=',')
        for row in read_file:
            labels_list.append([row[0], row[2], row[3], row[4]])
    image_names = [x[0] for x in labels_list]
    
    for image_name in image_names:
        for filename in os.listdir(images_path):
            if filename.endswith(".jpg") and (os.path.splitext(filename)[0]) == image_name:
                images.append(img_to_array(load_img(images_path + '/' + filename)))
                break
    for row in labels_list:
        del row[0]


    return images, labels_list

# main function
# -----------------------------------------------------------------------------
def main():
    # load model
    print("Loading Model")
    json_file = open('model.json','r')
    model = json_file.read()
    json_file.close()
    ml = model_from_json(model)
    ml.load_weights("model.h5")
    print("Model loaded")
    x_error = []
    y_error = []
    psi_error = []
    time_diff = []
    if len(sys.argv) == 3:
        images_path = sys.argv[1]
        labels_path = sys.argv[2]
        x_test, y_test = test_batch(images_path, labels_path)
        x_test = np.array(x_test)
        y_test = np.array(y_test)
        # print size and shape of the test set
        print(x_test.shape, y_test.shape)
        y_test = np.array(y_test).astype(float)
        # loop on all test set to calculate the average error in the 3 dimmensions; x, y and psi
        for image, label in zip(x_test, y_test):
            image = image.reshape(1, image.shape[0], image.shape[1], image.shape[2])
            t0 = time.time()
            result = ml.predict(image)
            t1 = time.time()
            x = result[0][0]
            y = result[0][1]
            psi = result[0][2]
            x_error.append(abs(x-label[0]))
            y_error.append(abs(y-label[1]))
            psi_error.append(abs(psi-label[2]))
            time_diff.append(abs(t1-t0))
        x_average = np.mean(x_error)
        y_average = np.mean(y_error)
        psi_average = np.mean(psi_error)
        time_diff = time_diff[1:]
        time_average = np.mean(time_diff)

        print('x_average', x_average)
        print('y_average', y_average)
        print('psi_average', psi_average)
        print('time_average', time_average)
    else:
        print('Wrong number of arguments')

if __name__ == '__main__':
    main()
