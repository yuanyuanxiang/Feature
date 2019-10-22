# -*- coding: utf-8 -*-
import os
import h5py
import numpy as np
import argparse

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
        img = image.load_img(img_path, target_size=(
            self.input_shape[0], self.input_shape[1]))
        img = image.img_to_array(img)
        img = np.expand_dims(img, axis=0)
        img = preprocess_input(img)
        feat = self.model.predict(img)
        norm_feat = feat[0]/LA.norm(feat[0])
        return norm_feat

    def featureAll(self, img_dir):
        img_list = get_imlist(img_dir)
        feats = []
        names = []
        for i, img_path in enumerate(img_list):
            norm_feat = self.features(img_path)
            img_name = os.path.split(img_path)[1]
            feats.append(norm_feat)
            names.append(img_name.encode())
            print("extracting feature from image No. %d / %d" %((i+1), len(img_list)))

        feats = np.array(feats)
        return names, feats


'''
 Returns a list of filenames for all jpg images in a directory. 
'''


def get_imlist(path):
    return [os.path.join(path, f) for f in os.listdir(path) if f.endswith('.jpg')]


'''
 Extract features and index the images
 # 对database文件夹内图片进行特征提取，建立索引文件featureCNN.h5
 # python index.py -database database -index featureCNN.h5
'''
if __name__ == "__main__":

    ap = argparse.ArgumentParser()
    ap.add_argument("-database", required=False,
                    help="Path to database which contains images to be indexed",
                    default="D:\\照片\\2019【趣探索】中秋节")
    ap.add_argument("-index", required=False,
                    help="Name of index file", default="featureCNN.h5")
    args = vars(ap.parse_args())
    # directory of images
    db = args["database"]
    # directory for storing extracted features
    output = args["index"]

    model = VGGNet()
    names, feats = model.featureAll(db)

    h5f = h5py.File(output, 'w')
    h5f.create_dataset('dataset_1', data=feats)
    h5f.create_dataset('dataset_2', data=names)
    h5f.close()
    print("Finish")
