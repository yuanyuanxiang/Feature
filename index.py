# -*- coding: utf-8 -*-
import sys
import h5py
import numpy as np
from numpy import linalg as LA

from keras.applications.vgg16 import VGG16
from keras.preprocessing import image
from keras.applications.vgg16 import preprocess_input


class VGGNet:
    def __init__(self):
        # weights: 'imagenet'
        # pooling: 'max' or 'avg'
        # input_shape: (width, height, 3), width and height should >= 48
        self.input_shape = (224, 224, 3)
        self.weight = 'imagenet'
        self.pooling = 'max'
        self.model = VGG16(weights=self.weight, input_shape=(
            self.input_shape[0], self.input_shape[1], self.input_shape[2]),
            pooling=self.pooling, include_top=False)
        self.model.predict(np.zeros((1, 224, 224, 3)))

    '''
    Use vgg16 model to extract features
    Output normalized feature vector
    '''

    def features(self, img_path):
        img = image.load_img(img_path, target_size=(self.input_shape[0], self.input_shape[1]),
                             interpolation='bilinear')
        img = image.img_to_array(img)
        return self.featureSrc(img)

    # numpy array
    def featureSrc(self, image_np):
        image_np = np.expand_dims(image_np, axis=0)
        image_np = preprocess_input(image_np)
        feat = self.model.predict(image_np)
        norm_feat = feat[0]/LA.norm(feat[0]) # normlized
        return norm_feat


# VGG16
model = VGGNet()


# get feature from image data
def Feature(image_src):
    image_np = np.array(image_src).astype(np.float32)
    # BGR -> RGB
    b = np.copy(image_np[:, :, 0])
    image_np[:, :, 0] = image_np[:, :, 2]
    image_np[:, :, 2] = b
    result = model.featureSrc(image_np)
    return result


# active VGG16
model.features("image.JPG")


# main
if __name__ == "__main__":
    img = "image.JPG"
    if len(sys.argv) > 1:
        img = sys.argv[1]
    print("feature of ", img, ":\n", model.features(img))
