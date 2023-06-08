import numpy as np
import cv2
from tensorflow import keras
import tensorflow as tf
from tensorflow.python.framework.convert_to_constants import convert_variables_to_constants_v2
import numpy as np
from pathlib import Path
from glob import glob





def prepare_custom_data(parent_folder, test_size = 0.2, image_type = "png", image_width = 28, image_height = 28):

    # find all subfolders in parent_folder
    paths = []
    for path in Path(parent_folder).iterdir():
        if path.is_dir():
            paths.append(path)

    # load all images in subfolder and get categoryname by corresponding foldername
    images = []
    labels = []

    print(paths)
    for path in paths:
        path = str(path)
        y = int(path.split("/")[-1])
        
        image_paths = glob(path + "/*." + image_type)
        for image_path in image_paths:
            
            image = cv2.imread(image_path, 0)
            image = cv2.resize(image, (image_height, image_width))
            image = np.array(image) / 255.0
            image = np.reshape(image, [image_height, image_width, 1])
            images.append(image)
            
            label_row = np.zeros(shape=(len(paths)))
            label_row[y] = 1.0
            labels.append(label_row)
            
    images = np.array(images)
    labels = np.array(labels)

    print(images.shape)

    #shuffle data and split train and test data
    p = np.random.permutation(len(images))
    images = images[p]
    labels = labels[p]
    train_max_idx = int(len(images) * (1.0 - test_size))

    x_train = images[: train_max_idx]
    y_train = labels[: train_max_idx]
    x_test = images[train_max_idx :]
    y_test = labels[train_max_idx :]

    return x_train, y_train, x_test, y_test


if __name__ == "__main__":

    model = keras.models.load_model('training/python/mnist')

    batch_size = 64
    epochs = 10
    
    x_train, y_train, x_test, y_test = prepare_custom_data("resources/custom_dataset")

    print(y_test)


    model.compile(loss="categorical_crossentropy",
                optimizer="adam", metrics=["accuracy"])

    model.fit(x_train, y_train, batch_size=batch_size,
            epochs=epochs, validation_split=0.1)
    
    score = model.evaluate(x_test, y_test, verbose=0)
    print("Test loss:", score[0])
    print("Test accuracy:", score[1])

    model.save("training/python/mnist_retrained")

