#!/usr/bin/env python3
 
import os
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.patches import Arc
from matplotlib.path import Path
 
import yaml
from argparse import ArgumentParser
 
# The entire pulse animation can be parameterized by the the number of wavelengths (n) in
#   the pulse and the distances (Xtgt) to the targets from which the signal will be reflected
#   - all lengths are scaled by the the wavelength
#   - all times are scaled by the pulse duration
#   This leads to the following observations about the reduced(*) physical properties:
#   - Speed of light (c) is equal to n
#   - Pulse frequency (f) is also equal to n
#   - Distance to target is simply number of wavelengths (aka phase shift)
#   - Round trip time for a pulse is twice the ratio between reduced distance to target and n
#   (*) reduced means de-dimensionalized
#
 
class Pulse:
    def __init__(self, n):
        """
        Args:
            - n (int): number of wavelengths in the pulse (easier to think about than pulse frequency)
        """
        self.n = n
 
    def signal_limits_x(self,t,trimmed=False):
        xo = self.n * (t-1)
        xe = xo + self.n
        if trimmed:
            if xe < 0:
                return None
            xo = max(0,xo)
        return (xo, xe)
 
    def signal_limits_t(self,x):
        return (x/self.n, 1 + x/self.n)
 
    def signal(self,t,x):
        #return np.sin(2*np.pi*(self.n*t-x))
        return (2*(t-x/self.n)-1)
 
    def echo_limits_x(self,t,tgt, trimmed=False):
        xo = 2*tgt.X - self.n * t
        xe = xo + self.n
        if trimmed:
            if xe < 0 or xo > tgt.X:
                return None
            xo = max(xo,0)
            xe = min(xe,tgt.X)
        return (xo, xe)
 
    def echo_limits_t(self,x,tgt):
        to = (2*tgt.X-x)/self.n
        return (to, 1+to)
 
    def echo(self,t,x,tgt):
        #return tgt.A * np.sin(2*np.pi*(self.n*t-2*tgt.X+x))
        return (2*(t-(2*tgt.X-x)/self.n) - 1)
 
class Target:
    def __init__(self, X, *, rcs=1, color='orange'):
        """
        Args:
            - X (float): distance from antenna to target (same unit as c)
            - rcs (float): "radar cross section" bigger number has stronger return
            - color (str): color used to draw the target and its return signal
        """
        self.X = X
        self.A = rcs
        self.col = color

    def trim_xlims(self, xlims):
        if xlims[1] < 0:
            return None
        if xlims[0] > self.X:
            return None
        return( max(0,xlims[0]), min(self.X,xlims[1]) )
 
 
class PulseAnimation:
    def __init__(self, name, n=8, targets=[], nsamp=100, figsize=(15,2)):
        """
        Args:
            - name (str): name used for both output mp4 file and folder containing intermediate frames
            - n (int): number of wavelengths in the pulse (easier to think about than pulse frequency)
            - targets (list): list of Targets in the scene
            - nsamp (float): number of signal samples per wavelength
            - figsize (tuple): width and height of animation frames (matplotlib units)
        """
        self.name = name
        self.n = n
        self.targets = targets
        self.nsamp = n * nsamp
        self.figsize = figsize
 
        self.frame_dir = f"{name}_frames"
 
        self.max_tgt_x = max(tgt.X for tgt in self.targets)
        self.xlims = (-10, self.max_tgt_x + 3*n)
 
        self.pulse = Pulse(n)
 
        rtt = 1 + 2 * max(tgt.X for tgt in self.targets) / self.n
 
        self.T_anim = 2 + rtt
 
    def gen_frames(self, nframes=128):
        """
        Generates the frames (png files) to create the animation
        Args:
            - nframes (int): number of frames to be included in the animation
        """
 
        # setup frame output directory
        if os.path.isdir(self.frame_dir):
            for f in os.listdir(self.frame_dir):
                os.unlink(f"{self.frame_dir}/{f}")
        else:
            os.mkdir(self.frame_dir)
 
       
        # generate the frames
        for frame in range(nframes):
            print(f"Generating frame {frame+1}")
 
            fig,ax = plt.subplots(figsize=self.figsize)
            ax.set_xlim(self.xlims)
            ax.set_ylim(-2,2)
            ax.tick_params(axis='x',which='both',bottom=False, top=False, labelbottom=False)
            ax.tick_params(axis='y',which='both',left=False, right=False, labelleft=False)
 
            t = self.T_anim * frame/(nframes-1)
 
            # add signal pulse
            xlim = self.pulse.signal_limits_x(t)
            dx = (xlim[1]-xlim[0])/(self.nsamp-1)
            pulse_x = [xlim[0] + i*dx for i in range(self.nsamp)]
            pulse_x = [x for x in pulse_x if x >= 0.]
            if len(pulse_x):
                pulse_y = [self.pulse.signal(t,x) for x in pulse_x]
                ax.plot(pulse_x, pulse_y, 'w')
 
            # add target echos
            echos = [(tgt, self.pulse.echo_limits_x(t,tgt,trimmed=True)) for
                        tgt in self.targets
                    ]
            echos = [ {'tgt':e[0], 'xo':e[1][0], 'xe':e[1][1]} 
                     for e in echos
                     if e[1] is not None ]

            if echos:
                dx = self.n / self.nsamp
                for echo in echos:
                    echo_x = np.arange(echo['xo'],echo['xe'],dx)
                    echo_y = np.array([self.pulse.echo(t,x,echo['tgt']) for x in echo_x])
                    ax.plot(echo_x, echo_y, echo['tgt'].col,
                            linewidth=3*echo['tgt'].A)
 
 
            # add transmitter graphic
            ant = Arc((0,0),2,2, theta1=90, theta2=270, linewidth=3, color='yellow')
            base = Rectangle((-3, -.1), 2, .2, color='yellow')
            ax.add_patch(ant)
            ax.add_patch(base)
 
            # add target graphics
            for tgt in self.targets:
                ax.add_patch(Rectangle((tgt.X,-tgt.A), 1, 2*tgt.A, color=tgt.col))
 
            ax.set(facecolor='black')
            fig.savefig(f"{self.frame_dir}/frame_{frame+1:03}.png")
            plt.close(fig)
 
    def gen_animation(self, ext="mpeg"):
        """
        Generates the animation from the frames created in gen_frames()
        Args:
            - ext (str): animation file extension
 
        The filename extension informs ImageMagick what type of movie file to create.
        """
        filename = f"{self.name}.{ext}"
        print(f"Generating {filename} from frames")
        if os.path.exists(filename):
            os.unlink(filename)
        for convert_bin in ("/usr/local/bin","usr/bin"):
            convert = f"{convert_bin}/convert"
            if os.path.exists(convert):
                os.system(f"{convert} {self.frame_dir}/*.png {filename}")
        print("Done")
 
pa_sets = dict()
with open("animation.yaml","r") as file:
    pa_sets = yaml.load(file,Loader=yaml.loader.SafeLoader)
 
def gen_anim(name):
    print(f"Generating animation: {name}")
    pa_set = pa_sets[name]
 
    pa_args = dict()
    for arg in ('n','nsamp','figsize'):
        if arg in pa_set:
            pa_args[arg] = pa_set[arg]
 
    assert 'targets' in pa_set, f"Preset {name} has no targets (check yaml file)"
    targets = list()
    for tgt in pa_set['targets']:
        assert 'X' in tgt, f"Preset {name} target {1+len(targets)} is missing an X value"
        tgt_args = dict()
        for arg in ('rcs','color'):
            if arg in tgt:
                tgt_args[arg] = tgt[arg]
        targets.append(Target(tgt['X'],**tgt_args))
 
    gf_args = dict()
    for arg in ('nframes'):
        if arg in pa_set:
            gf_args[arg] = pa_set[arg]
 
    ga_args = dict()
    for arg in ('ext'):
        if arg in pa_set:
            ga_args[arg] = pa_set[arg]
 
    pa = PulseAnimation(name, targets=targets, **pa_args)
    pa.gen_frames(**gf_args)
    pa.gen_animation(**ga_args)
 
if __name__ == '__main__':
    parser = ArgumentParser(description="Generates animations to illustrate SAR pulses")
    parser.add_argument("name",
            choices=list(pa_sets.keys()) + ['all'],
            help="Predefined animation set")
 
    args = parser.parse_args()
 
    if args.name == 'all':
        for name in pa_sets.keys():
            gen_anim(name)
            print("\n")
    else:
        gen_anim(args.name)
 
 
