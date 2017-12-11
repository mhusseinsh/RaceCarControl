#!/usr/bin/python

import os
import sys
import csv
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt

from keras.preprocessing.image import img_to_array, load_img
from keras.models import Sequential, model_from_json
from keras.layers import Dense, Activation, Conv2D, MaxPooling2D, Dropout, Flatten
from keras.utils import np_utils


# global variables
cursor_index = 0
images = list()
labels_list = list()
count = 0
fps = 0
fig = 0 # figure object for veiwing frames and navigation

# function that handels csv data supported
# -----------------------------------------------------------------------------
def process_dataset(images_path, labels_path):
    global images
    global labels_list
    with open(labels_path) as f:
        read_file = csv.reader(f, delimiter=',')
        for row in read_file:
            labels_list.append([row[0], row[2], row[3], row[4]])
            # print(row[1])
    image_names = [x[0] for x in labels_list]

    for image_name in image_names:
        for filename in os.listdir(images_path):
            # print(os.path.splitext(filename)[0])
            if filename.endswith(".jpg") and (os.path.splitext(filename)[0]) == image_name:
                images.append(img_to_array(load_img(images_path + '/' + filename)))
                break

    for row in labels_list:
    	del row[0]
    # print(labels_list)
    # print('--------------------------------------------------------')
    # print('--------------------------------------------------------')
    # print('--------------------------------------------------------')
    # print('--------------------------------------------------------')
    # print('--------------------------------------------------------')
    # print(images[0].shape)
    # print('--------------------------------------------------------')
    # print('--------------------------------------------------------')
    # print('--------------------------------------------------------')
def split_train_valid():
    shuffled_index = np.random.permutation(len(images))

    indices_train = shuffled_index[0:int(0.8*len(images))]
    indices_valid = shuffled_index[int(0.8*len(images)):len(images)]

    train_data = [images[i] for i in indices_train]
    train_labels = [labels_list[i] for i in indices_train]

    valid_data = [images[i] for i in indices_valid]
    valid_labels = [labels_list[i] for i in indices_valid]

    return train_data, train_labels, valid_data, valid_labels

class cnn():
    def __init__(self):
        self.model = Sequential()



def train_network():
    x, y, x_val, y_val = split_train_valid()
    rows = x[0].shape[0]
    cols = x[0].shape[1]
    channels = x[0].shape[2]

    # print(np.array(y).shape)
    # y = np.array(y).reshape((-1, 1))
    # y_val = np.array(y_val).reshape((-1, 1))

    print (rows, cols, channels)

    cnn_m = cnn()
    # Create cnn
    cnn_m.model.add(Conv2D(32, kernel_size=5, padding='same', data_format="channels_last", input_shape=(rows, cols, channels)))
    cnn_m.model.add(Activation('relu'))
    cnn_m.model.add(MaxPooling2D(pool_size=(2, 2)))
    cnn_m.model.add(Conv2D(16, kernel_size=3, padding='same'))
    cnn_m.model.add(Activation('relu'))
    cnn_m.model.add(MaxPooling2D(pool_size=(2, 2)))
    cnn_m.model.add(Conv2D(8, kernel_size=3, padding='same'))
    cnn_m.model.add(Activation('relu'))
    cnn_m.model.add(MaxPooling2D(pool_size=(2, 2)))
    cnn_m.model.add(Dropout(0.25))

    cnn_m.model.add(Flatten())
    cnn_m.model.add(Dense(512))
    cnn_m.model.add(Activation('relu'))
    cnn_m.model.add(Dropout(0.5))
    cnn_m.model.add(Dense(3))
    cnn_m.model.add(Activation('linear'))

    # Define attributes of the cnn; categorial, optimizer_type, performance metrics
    cnn_m.model.compile(loss='mean_squared_error', optimizer='adam', metrics=['accuracy'])

    # Fit the model to the training data
    history = cnn_m.model.fit(np.array(x), np.array(y), epochs=50, batch_size=20, validation_data=(np.array(x_val), np.array(y_val)), shuffle=True)

    # 2. save your trained model
    # serialize model to JSON
    model_json = cnn_m.model.to_json()
    with open("model.json", "w") as json_file:
        json_file.write(model_json)
    # serialize weights to HDF5
    cnn_m.model.save_weights("model.h5")
    print("Saved model to disk")

    # list all data in history
    print(history.history.keys())

    # summarize history for accuracy
    plt.plot(history.history['acc'])
    plt.plot(history.history['val_acc'])
    plt.title('model accuracy')
    plt.ylabel('accuracy')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()

    # summarize history for loss
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()


# # function responsible for processing the video and extracting the frames
# # -----------------------------------------------------------------------------
# def process_video(video_path):#, timestamp, events_id):
#     # variables initialization
#     global count
#     global fps
#     success = True
#     # creating directory for new images
#     video_name = os.path.basename(os.path.splitext(video_path)[0])
#     print("Start processing: " + video_name)
#     if not os.path.isdir(video_name):
#         os.mkdir(video_name)
#     # initialize captured video and retrieve the frames per second for the video
#     vidcap = cv2.VideoCapture(video_path)
#     fps = vidcap.get(cv2.CAP_PROP_FPS)
#     success, image = vidcap.read() # images are numpy.ndarray

#     # saving video frames into images list
#     while success:
#         images.append((timestamp_list[count], image))
#         # save frame as JPEG file, uncomment if needed
#         # cv2.imwrite(os.path.join(video_name, "frame%d.jpg" % count), image)
#         count += 1
#         success, image = vidcap.read()

#     print("finished reading %d frames" %count)


# main function
# -----------------------------------------------------------------------------
def main():
    if len(sys.argv) == 3:
        images_path = sys.argv[1]
        labels_path = sys.argv[2]
        process_dataset(images_path, labels_path)
        train_network()
    else:
        print('Wrong number of arguments')

if __name__ == '__main__':
    main()
