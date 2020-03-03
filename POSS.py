import matplotlib.pyplot as plt
from scipy import ndimage
from scipy import sparse
from scipy import misc
import numpy as np
from numpy import linalg as LA
import skimage as sk
import skimage.io as skio
from sklearn import preprocessing
import copy
from skimage.color import rgb2gray




def mask_check(y, x, mask):
	
	return mask[y][x] == 1.

def calculate_vars(height, width, mask):
	i = 0;

	for y in range(height):
		for x in range(width):
			if mask_check(y, x, mask):
				i = i + 1
	return i

def calc_adjacent(x, y, height, width, mask, A , b, e, im2var, source, source_i, target):
	if mask_check(y, x, mask):
		A[e, im2var[y][x]] = -1
		b[e] = source_i - source[y][x]
	else:
		b[e] = target[y][x]

def calc_all_sides(x, y, height, width, mask, A , b, e, im2var, source, target):
	#calc up
	calc_adjacent(x, y + 1, height, width, mask, A, b, e, im2var, source, source[y][x], target)
	A[e, im2var[y][x]] = 1
	e = e + 1
	#calc down
	calc_adjacent(x, y - 1, height, width, mask, A, b, e, im2var, source, source[y][x], target)
	A[e, im2var[y][x]] = 1
	e = e + 1
	#calc_right
	calc_adjacent(x + 1, y, height, width, mask, A, b, e, im2var, source, source[y][x], target)
	A[e, im2var[y][x]] = 1
	e = e + 1
	#calc_left
	val = calc_adjacent(x - 1, y, height, width, mask, A, b, e, im2var, source, source[y][x], target)
	A[e, im2var[y][x]] = 1
	e = e + 1

	return e

def poisson_blend(source, target, mask):
	height = len(source)
	width = len(source[0])
	num_vars = calculate_vars(height, width, mask)
	im2var = np.zeros((height, width))

	k = 0
	for y in range(height):
		for x in range(width):
			if mask_check(y, x, mask):
				im2var[y][x] = k	
				k = k + 1
			else:
				im2var[y][x] = -1

	A = sparse.lil_matrix(((num_vars * 4), num_vars), dtype = np.float32)
	b = np.zeros(((num_vars * 4), 1))
	e = 0

	for y in range(height):
		for x in range(width):
			if mask_check(y, x, mask):
				e = calc_all_sides(x, y, height, width, mask, A , b, e, im2var, source, target)		
			
	A = sparse.csr_matrix(A)

	result = sparse.linalg.lsqr(A, b)[0]
	# if result.min() < 0:
	# 	result -= result.min()
	# result /= result.max()
	return np.clip(sparse.linalg.lsqr(A, b)[0], 0, 1)

def pixel_replace(result_vec, source, target, mask):
	k = 0
	for y in range(len(source)):
		for x in range(len(source[0])):
			if mask_check(y, x, mask):
				
				target[y][x] = result_vec[k]
				k = k + 1
	return target

def gray_blend(source, target, mask):
	result_vec = poisson_blend(source, target, mask)
	target = pixel_replace(result_vec, source, target, mask)

	return target

def split_RGB(im):
	red = im[:,:,2]
	green = im[:,:,1]
	blue = im[:,:,0]
	return red, green, blue

def color_blend(source, target, mask):
	red_source, green_source, blue_source = split_RGB(source)
	red_target, green_target, blue_target = split_RGB(target)

	result_vec_red = poisson_blend(red_source, red_target, mask)
	result_vec_green = poisson_blend(green_source, green_target, mask)
	result_vec_blue = poisson_blend(blue_source, blue_target, mask)

	pixel_replace(result_vec_red, red_source, red_target, mask)
	pixel_replace(result_vec_green, green_source, green_target, mask)
	pixel_replace(result_vec_blue, blue_source, blue_target, mask)

	return np.dstack([blue_target, green_target, red_target])


def AlignImages(mask, source, target, offset):
    sourceHeight=len(source)
    sourceWidth=len(source[0])
    targetHeight=len(target)
    targetWidth= len(target[0])
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



def main():
	source =skio.imread('D://WZQ//Images//source_05.jpg')/255.
	# misc.imsave("source_image_gray.png", source)
	target = skio.imread('D://WZQ//Images//target_05.jpg')/255.
	# misc.imsave("target_image_gray.png", target)
	mask = skio.imread('D://WZQ//Images//mask_05.jpg')/255.
	

	source, mask=AlignImages(mask,source,target,[-40,90])
	mask=rgb2gray(mask)
	
	target = color_blend(source, target, mask)

	skio.imsave("D://WZQ//Images//res22ff231.jpg", target)

main()
