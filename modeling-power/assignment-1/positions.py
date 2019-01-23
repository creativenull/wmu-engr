import numpy as np

# Create the image conductor positions
def image_pos(conductor_pos):
    x = 0
    y = 1

    (m, n) = conductor_pos.shape
    cond_img = np.zeros((m, n))

    for r in np.arange(m):
        for c in np.arange(n):

            # different sign for Y coordinate
            if c == y:
                cond_img[r, y] = -conductor_pos[r, y]
            else:
                cond_img[r, c] = conductor_pos[r, c]
    
    return cond_img

# calculate the GMR of the bundled conductor
def get_gmr(radius, bundle_radius, number_of_cond):
    return (number_of_cond * radius * (bundle_radius ** (number_of_cond - 1)))\
         ** ( 1./number_of_cond )