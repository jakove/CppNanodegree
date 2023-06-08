from tensorflow.keras.models import load_model
import cv2
import numpy as np

import onnx


if __name__ == "__main__":


    onnx_model = onnx.load("model.onnx")


    model = load_model("mnist")
    model.summary()

    numbers = np.arange(0, 34)

    for i in numbers:

        image = cv2.imread(f"test_images/{i}.png", 0)
        cv2.imshow("", image)
        

        image = np.array(image) / 255.0
        print(image.shape)
        image = np.reshape(image, [1, 28, 28, 1])
        print(image.shape)
        res = model.predict(image)
        print(np.argmax(res))

        cv2.waitKey(0)