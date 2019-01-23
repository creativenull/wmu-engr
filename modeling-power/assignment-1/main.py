import numpy as np

import log_matrix as LM
import positions as COND

from tower_configurations import tower_config_1, tower_config_2

# run the main program
def main():

    # frequencies to run calculations
    freq = [ 60, 10e3, 200e3 ]

    # calculate and display the results
    run_calculations( tower_config_1, freq[0] )
    run_calculations( tower_config_1, freq[1] )
    run_calculations( tower_config_1, freq[2] )

    run_calculations( tower_config_2, freq[0] )
    run_calculations( tower_config_2, freq[1] )
    run_calculations( tower_config_2, freq[2] )


# perform calculations for each freq and tower config
def run_calculations(tower_conf, freq):

    ri = 0
    rb = 0
    num_lines = tower_conf["lines"]
    radius = np.zeros( num_lines )

    # angular freq
    w = 2 * np.pi * freq

    # Display the current tower configuration
    print("Tower: {}".format( tower_conf["name"] ))
    print("f = {}".format( freq ))
    print("Number of lines, n = {}".format( num_lines ))
    print()

    # Free space
    e = 8.854e-12
    u = 1.257e-6

    # Conductor and Ground resistivity
    pc = tower_conf["conductor_resistivity"]
    pe = tower_conf["ground_resistivity"]

    # Skin Depth
    delta = np.sqrt( pc / (1j * w * u ))
    print("skin depth, delta = {0:.4e}".format( delta ))

    # Penetration Depth
    p = np.sqrt( pe / (1j * w * u) )
    print("penetration depth, p = {0:.4f}".format( p ))

    if tower_conf["bundled_conductor"]["bundled"] == True:
        N = tower_conf["bundled_conductor"]["N"]
        db = tower_conf["bundled_conductor"]["diameter"]
        ri = tower_conf["conductor_radius"]

        # convert to meters
        ri = ri / 100.0
        rb = db / 2
        rb = rb / 100.0

        # get the GMR of bundled conductor
        gmr = COND.get_gmr( ri, rb, N )

        # radius
        for i in np.arange(num_lines):
            radius[i] = gmr
        
        print("Conductor radius = {} m".format( ri ))
        print("Bundled Conductor radius = {} m".format( rb ))

        print("GMR = {0:.4f} m".format( gmr ))
        print()

    else:
        # convert to meters
        ri = tower_conf["conductor_radius"]
        ri = ri / 100.0

        # radius
        for i in np.arange(num_lines):
            radius[i] = ri

        print("Conductor radius = {0:.4f} m".format( ri ))
        print()

    # get the conductors positions
    conductors_pos = tower_conf["conductors"]
    conductors_img_pos = COND.image_pos( conductors_pos )

    ## MATRIX
    # -------
    # matrix for admittance
    p_mat = LM.p_matrix( conductors_pos, conductors_img_pos, radius )

    # matrix for impedance
    pg_mat = LM.p_matrix( conductors_pos, conductors_img_pos, radius )
    e_mat = LM.e_matrix( conductors_pos, conductors_img_pos, radius, p )

    # Setting print options for better value readability
    np.set_printoptions(formatter = {
        "complexfloat": "{0:.3e}".format,
        "float": "{0:.3e}".format
    })

    ## ADMITTANCE
    # -----------
    # Calculate the P matrix to obtain the capacitance
    P = ( 1 / (2 * np.pi * e) ) * p_mat
    C = np.linalg.inv( P )
    print("C = \n{}".format( C ))
    print()

    # Calculate the Admittance
    Y = 1j * w * C
    print("Y = \n{}".format( Y ))
    print()

    ## IMPEDANCE
    # ----------
    # Conductor impedance
    Rdc = pc / ( np.pi * np.power( radius, 2 ) )
    Zhf = pc / ( (2 * np.pi * radius * delta) - (np.pi * np.power( delta, 2 )))
    Zc = np.sqrt( np.power( Rdc, 2 ) + np.power( Zhf, 2 ) )

    Zc = Zc * np.eye( num_lines )

    # Geometrical Inductance
    Lg = (u / (2 * np.pi) ) * pg_mat
    Zg = 1j * w * Lg

    # Earth's return Inductance
    Le = ( u / (2 * np.pi) ) * e_mat
    Ze = 1j * w * Le

	# Calculate the impedance
    Z = Ze + Zg + Zc
    print("Z = \n{}".format( Z ))
    print("=========================")

def compute_P( ln_mat, e = 8.854e-12 ):
    return ( 1 / (2 * np.pi * e) ) * ln_mat

def compute_C( p_mat ):
    return np.linalg.inv( p_mat )

def compute_Zc( cond_resist, delta, radius, num_lines ):

    Rdc = cond_resist / ( np.pi * np.power( radius , 2 ) )
    Zhf = cond_resist / ( 2 * np.pi * radius * delta ) +\
        ( np.pi * np.power( delta , 2 ) )
    Zc = np.sqrt( np.power( Rdc, 2) + np.power( Zhf, 2) )

    return Zc * np.eye( num_lines )

def compute_Lg( geo_mat, u = 1.257e-6 ):
    return ( u / (2 * np.pi) ) * geo_mat

def compute_Le( earth_mat, u = 1.257e-6 ):
    return ( u / (2 * np.pi) ) * earth_mat

if __name__ == "__main__":
    main()
