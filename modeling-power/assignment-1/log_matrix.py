import numpy as np

# Build the earth log matrix to calculate Le
def e_matrix(cond, cond_img, radius, p = 0):
    x = 0
    y = 1

    # temp
    D = 0
    Dp = 0
    r = 0
    d = 0

    # Allocate size for distance matrix
    (m, _) = cond.shape
    log_mat = np.zeros((m, m))

    # loop for each element of the matrix
    for row in np.arange(m):
        for col in np.arange(m):

            # if identity elements, ex P(1,1), P(2,2), P(3,3), etc
            if row == col:

                Dp = get_image_dist_eye(\
                    cond[row, x],\
                    cond[row, y], p )

                D = get_image_dist_eye(\
                    cond[row, x],\
                    cond[row, y] )

                log_mat[row, col] = np.log( Dp / D )

            # Else calculate log of the other elements
            else:

                Dp = get_image_dist(\
                    cond[row, x],\
                    cond[row, y],\
                    cond_img[col, x],\
                    cond_img[col, y], p )

                D = get_image_dist(\
                    cond[row, x],\
                    cond[row, y],\
                    cond_img[col, x],\
                    cond_img[col, y] )

                log_mat[row, col] = np.log( Dp / D )

    return log_mat

# Build the P matrix of the conductors
def p_matrix(cond, cond_m, radius, p = 0):
    x = 0
    y = 1

    # temp
    D = 0
    r = 0
    d = 0

    # Allocate size for distance matrix
    (m, n) = cond.shape
    log_mat = np.zeros((m, m))

    # loop for each element of the matrix
    for row in np.arange(m):
        for col in np.arange(m):

            # if identity elements, ex P(1,1), P(2,2), P(3,3), etc
            if row == col:

                D = get_image_dist_eye(\
                    cond[row, x],\
                    cond[row, y], p )

                r = radius[row]

                log_mat[row, col] = np.log( D / r )

            # Else calculate log of the other elements
            else:

                D = get_image_dist(\
                    cond[row, x],\
                    cond[row, y],\
                    cond_m[col, x],\
                    cond_m[col, y], p )

                d = get_real_dist(\
                    cond[row, x],\
                    cond[row, y],\
                    cond[col, x],\
                    cond[col, y] )

                log_mat[row, col] = np.log( D / d )

    return log_mat

# Calculate the diagonal distance between real and image conductor
def get_image_dist_eye(xi, yi, p = 0):
    return 2 * ( yi + np.abs(p) )

# Calculate the distance between real conductor and other image conductors
def get_image_dist(xi, yi, xj, yj, p = 0):

    return np.sqrt(\
        np.power( xi - xj, 2 ) + \
        np.power( np.abs(yi) + np.abs(yj) + ( 2 * np.abs(p) ), 2 )\
    )

# Calculate the distance between two real conductors
def get_real_dist(xi, yi, xj, yj):

    return np.sqrt(\
        np.power( xi - xj, 2 ) + \
        np.power( yi - yj, 2 )\
    )