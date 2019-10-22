# -*- coding: utf-8 -*-
from extract_cnn_vgg16_keras import VGGNet

import numpy as np
import h5py

import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import argparse

# query one image
def query(path, queryDir, result):
    # read in indexed images' feature vectors and corresponding image names
    h5f = h5py.File(path, 'r')
    feats = h5f['dataset_1'][:]
    imgNames = h5f['dataset_2'][:]
    h5f.close()

    # read and show query image
    queryImg = mpimg.imread(queryDir)
    plt.title("Query Image")
    plt.imshow(queryImg)
    plt.show()

    # init VGGNet16 model
    model = VGGNet()

    # extract query image's feature, compute simlarity score and sort
    queryVec = model.features(queryDir)
    scores = np.dot(queryVec, feats.T)
    rank_ID = np.argsort(scores)[::-1]
    rank_score = scores[rank_ID]
    # print rank_ID
    # print rank_score

    # number of top retrieved images to show
    maxres = 3
    imlist = [imgNames[index] for i, index in enumerate(rank_ID[0:maxres])]
    print("top %d images in order are: " % maxres, imlist)

    # show top #maxres retrieved result one by one
    for i, im in enumerate(imlist):
        image = mpimg.imread(result+"/"+str(im, encoding='utf-8'))
        plt.title("search output %d" % (i+1))
        plt.imshow(image)
        plt.show()


if __name__ == "__main__":
    # 使用database文件夹内测试图片，在database内以featureCNN.h5进行近似图片查找，并显示最近似的3张图片
    # python query_online.py -query database/0001.jpg -index featureCNN.h5 -result database
    ap = argparse.ArgumentParser()
    ap.add_argument("-query", required=False,
                    help="Path to query which contains image to be queried",
                    default="D:\\照片\\2019【趣探索】中秋节\\1-23.jpg")
    ap.add_argument("-index", required=False,
                    help="Path to index", default="featureCNN.h5")
    ap.add_argument("-result", required=False, help="Path for output retrieved images",
                    default="D:\\照片\\2019【趣探索】中秋节")
    args = vars(ap.parse_args())

    print("--------------------------------------------------")
    print("               searching starts")
    print("--------------------------------------------------")

    query(args["index"], args["query"], args["result"])
