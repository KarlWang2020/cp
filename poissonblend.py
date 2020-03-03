from scipy import sparse
import numpy as np
from matplotlib import pyplot as plt


# Read source, target and mask for a given id
def Read(id, path="D:\WZQ\Images"):
    source = plt.imread(path + "source_" + id + ".jpg") / 255
    target = plt.imread(path + "target_" + id + ".jpg") / 255
    mask = plt.imread(path + "mask_" + id + ".jpg") / 255

    return source, mask, target


# Adjust parameters, source and mask for negative offsets or out of bounds of offsets
def AlignImages(mask, source, target, offset):
    sourceHeight, sourceWidth, _ = source.shape
    targetHeight, targetWidth, _ = target.shape
    xOffset, yOffset = offset

    if (xOffset < 0):
        mask = mask[abs(xOffset):, :]
        source = source[abs(xOffset):, :]
        sourceHeight -= abs(xOffset)
        xOffset = 0
    if (yOffset < 0):
        mask = mask[:, abs(yOffset):]
        source = source[:, abs(yOffset):]
        sourceWidth -= abs(yOffset)
        yOffset = 0
    # Source image outside target image after applying offset
    if (targetHeight < (sourceHeight + xOffset)):
        sourceHeight = targetHeight - xOffset
        mask = mask[:sourceHeight, :]
        source = source[:sourceHeight, :]
    if (targetWidth < (sourceWidth + yOffset)):
        sourceWidth = targetWidth - yOffset
        mask = mask[:, :sourceWidth]
        source = source[:, :sourceWidth]

    maskLocal = np.zeros_like(target)
    maskLocal[xOffset:xOffset + sourceHeight, yOffset:yOffset + sourceWidth] = mask
    sourceLocal = np.zeros_like(target)
    sourceLocal[xOffset:xOffset + sourceHeight, yOffset:yOffset + sourceWidth] = source

    return sourceLocal, maskLocal


# Pyramid Blend
#def PyramidBlend(source, mask, target):
 #   return source * mask + target * (1 - mask)







# Poisson Blend for each channel
def PoissonBlend_each(source, mask, target):
    height = len(source)
    width = len(source[0])

    mat_dim=np.count_nonzero(mask)
    order = np.zeros_like(source)

    k = 0
    for y in range(height):
        for x in range(width):
            if mask[y,x]==1:
                order[y][x] = k
                k = k + 1
            else:
                order[y][x] = -1

    A = sparse.lil_matrix(((mat_dim * 4), mat_dim), dtype=np.float32)
    b = np.zeros(((mat_dim * 4), 1))
    i = 0

    for y in range(height):
        for x in range(width):
            if mask[y][x]==1:
                if mask[y+1][x]==1:
                    A[i, order[y+1][x]] = -1
                    b[i] = source[y][x] - source[y+1][x]
                else:
                    b[i] = target[y+1][x]
                A[i, order[y][x]] = 1
                i = i + 1

                if mask[y-1][x]==1:
                    A[i, order[y-1][x]] = -1
                    b[i] = source[y][x] - source[y-1][x]
                else:
                    b[i] = target[y-1][x]
                A[i, order[y][x]] = 1
                i = i + 1

                if mask[y][x+1]==1:
                    A[i, order[y][x+1]] = -1
                    b[i] = source[y][x] - source[y][x+1]
                else:
                    b[i] = target[y][x+1]
                A[i, order[y][x]] = 1
                i = i + 1


                if mask[y][x-1]==1:
                    A[i, order[y][x-1]] = -1
                    b[i] = source[y][x] - source[y][x-1]
                else:
                    b[i] = target[y][x-1]
                A[i, order[y][x]] = 1
                i = i + 1

    A = sparse.csr_matrix(A)

    result = sparse.linalg.lsqr(A, b)[0]

    return np.clip(sparse.linalg.lsqr(A, b)[0], 0, 1)


#replace pixel in target
def PoissonReplace(res, source, target, mask):
    k = 0
    for y in range(len(source)):
        for x in range(len(source[0])):
            if mask[y][x]==1:

                target[x,y] = res[k]
                k = k + 1
    return target

#for each channel R G B, implement Poisson Blending
def PoissonBlend(source,target,mask,ismix=False):

    sourceR= source[:,:,2]
    sourceG =source[:, :, 1]
    sourceB =source[:, :, 0]

    targetR=target[:,:,2]
    targetG=target[:,:,1]
    targetB=target[:,:,0]



    res_r = PoissonBlend_each(sourceR, targetR, mask)
    res_g = PoissonBlend_each(sourceG, targetG, mask)
    res_b = PoissonBlend_each(sourceB, targetB, mask)

    PoissonReplace(res_r, sourceR, targetR, mask)
    PoissonReplace(res_g, sourceG, targetG, mask)
    PoissonReplace(res_b, sourceB, targetB, mask)

    return np.dstack([targetB, targetG, targetR])


if __name__ == '__main__':
    # Setting up the input output paths
    inputDir = 'D://WZQ//Images//'
    outputDir = 'D://WZQ//Results//'

    # False for source gradient, true for mixing gradients
    isMix = False

    # Source offsets in target
    offsets = [[0, 0], [0, 0], [210, 10], [10, 28], [140, 80], [-40, 90], [60, 100], [20, 20], [-28, 88]]

    # main area to specify files and display blended image
    for index in range(1, len(offsets)):
        # Read data and clean mask
        source, maskOriginal, target = Read(str(index).zfill(2), inputDir)

        # Cleaning up the mask
        mask = np.ones_like(maskOriginal)
        mask[maskOriginal < 0.5] = 0

        # Align the source and mask using the provided offest
        source, mask = AlignImages(mask, source, target, offsets[index])

        ### The main part of the code ###



        # Implement the PyramidBlend function (Task 1)
        #pyramidOutput = PyramidBlend(source, mask, target)

        # Implement the PoissonBlend function (Task 2)
        poissonOutput = PoissonBlend(source, mask, target, isMix)

        # Writing the result

        #plt.imsave("{}pyramid_{}.jpg".format(outputDir, str(index).zfill(2)), pyramidOutput)

        if not isMix:
            plt.imsave("{}poisson_{}.jpg".format(outputDir, str(index).zfill(2)), poissonOutput)
        else:
            plt.imsave("{}poisson_{}_Mixing.jpg".format(outputDir, str(index).zfill(2)), poissonOutput)
