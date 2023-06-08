import numpy as np
import cv2
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D
from tensorflow.keras.layers import MaxPooling2D
from tensorflow.keras.layers import Activation
from tensorflow.keras.layers import Flatten
from tensorflow.keras.layers import Dense
from tensorflow.keras.layers import Dropout
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.datasets import mnist
from tensorflow.keras.utils import to_categorical


def prepare_mnist_data(num_classes):

    (x_train, y_train), (x_test, y_test) = mnist.load_data()
    x_train = x_train.astype("float32") / 255
    x_test = x_test.astype("float32") / 255
    x_train = np.expand_dims(x_train, -1)
    x_test = np.expand_dims(x_test, -1)

    y_train = to_categorical(y_train, num_classes)
    y_test = to_categorical(y_test, num_classes)

    return x_train, y_train, x_test, y_test


def get_network(width, height, depth, classes):

    # initialize the model
    network = Sequential()
    inputShape = (height, width, depth)

    # Conv2d
    network.add(Conv2D(32, (3, 3), padding="same",
                       input_shape=inputShape))
    network.add(Activation("relu"))
    network.add(MaxPooling2D(pool_size=(2, 2)))

    # Conv2D
    network.add(Conv2D(64, (3, 3), padding="same"))
    network.add(Activation("relu"))
    network.add(MaxPooling2D(pool_size=(2, 2)))

    # Fully Connected
    network.add(Flatten())
    network.add(Dense(128))
    network.add(Activation("relu"))
    network.add(Dropout(0.5))

    # Fully Connnected
    network.add(Dense(64))
    network.add(Activation("relu"))
    network.add(Dropout(0.5))

    # softmax
    network.add(Dense(classes))
    network.add(Activation("softmax"))

    return network


if __name__ == "__main__":
    batch_size = 128
    epochs = 15

    classes = 10
    
    x_train, y_train, x_test, y_test = prepare_mnist_data(classes)

    model = get_network(28, 28, 1, classes)

    model.compile(loss="categorical_crossentropy",
                optimizer="adam", metrics=["accuracy"])

    model.fit(x_train, y_train, batch_size=batch_size,
            epochs=epochs, validation_split=0.1)
    
    score = model.evaluate(x_test, y_test, verbose=0)
    print("Test loss:", score[0])
    print("Test accuracy:", score[1])

    model.save("mnist")

