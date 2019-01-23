import numpy as np

tower_config_1 = {
    "name": "Horizontal Bundled Tower Configuration",
    "lines": 3,
    "conductors": np.array([
        [0, 25],
        [11.47, 25.75],
        [22.94, 25.0]
    ]),
    "conductor_radius": 1.6,
    "bundled_conductor": {
        "bundled": True,
        "N": 2,
        "diameter": 45
    },

    "ground_resistivity": 100,
    "conductor_resistivity": 4.0858e-8
}

tower_config_2 = {
    "name": "Vertical Tower Configuration",
    "lines": 3,
    "conductors": np.array([
        [0, 30.72],
        [0, 36.64],
        [0.4, 24.8]
    ]),
    "conductor_radius": 1.21,
    "bundled_conductor": {
        "bundled": False
    },

    "ground_resistivity": 100,
    "conductor_resistivity": 4.394e-8
}
