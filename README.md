mdx-m3-optimizer
================

Optimizes MDX and M3 files used by the games Warcraft 3 and Starcraft 2 respectively.

For MDX files, it does the following:

    Changes the precision of all the big floating point data sequences (VRTX, NRMS, UVBS, PIVT, all chunks of the form K***).
    Removes useless key frames.
    Changes all bezier and hermite keyframes to linear keyframes.

For M3 files, it does the following:

    Changes the precision of all the big floating point data sequences (vertex positions, REAL, VEC2, VEC3, VEC4, QUAT, BNDS, IREF).

This tool is a command-line tool, meaning there is no graphical user interface.
You can either run it through a console, which gives you full control over the options (run it with no arguments to see the list of options), or simply grab files or directories and drop them into it, which will use the default values.

The default values are:

    Floating point precision: 16.
    Threshold: 0.001.
    Force linear keyframes: false.

Floating point precision goes from 10 to 32, where 32 does nothing, and 10 will probably make your model useless.
Note that this wont directly reduce the size of the file, however any map that imports that file will be smaller than the same map importing the original file.
This idea was taken from [MDX Squisher](http://www.wc3c.net/showthread.php?p=800246).

The threshold is used to compare keyframes, you can make it bigger to remove more keyframes, at the risk of getting bad animation data.

Forcing keyframes to be linear means deleting their in/out tangents, which makes every hermite/bezier keyframe take a third of the original size.
This can cause gigantic size reductions for models with many hermite/bezier keyframes, at the cost of a lower quality animation (though in most cases our eyes can't really see the difference).

To compile, simply run make.
