
# Probelm 5, using example code given in link.
# Paul Bischoff

import numpy as np
from sgp4.api import Satrec
from matplotlib import pyplot as plt

def main():
    s1 = '1 45917U 98067RQ  21030.44366121  .00120197  00000-0  83178-3 0  9999'
    s2 = '1 45917U 98067RQ  21030.44366121  .03220197  00000-0  83178-3 0  9999'
    get_Alt(s1, 1)
    get_Alt(s2, 2)

def get_Alt(s, x):
    t = '2 45917  51.6342 290.9269 0007560 141.0241 219.1304 15.73595660 31312'
    jd = np.linspace(2459246,2459376,num=130)
    fr = 0
    satellite = Satrec.twoline2rv(s, t)
    #satellite2 = Satrec.twoline2rv(s2, t)
    rEarth = 6371
    alt = []
    for i in jd:
         e, r, v = satellite.sgp4(i, fr)
         alt.append(np.linalg.norm(r) - rEarth)
    plot_Alt(jd, alt, x)


def plot_Alt(jd, alt, x):
    plt.figure(x)
    plt.title("Orbital Alt.")
    plt.xlabel("JD")
    plt.ylabel("Orbital Alt. (km)")
    plt.plot(jd,alt)
    plt.show()



if __name__ == '__main__':
    main()
