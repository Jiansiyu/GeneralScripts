# vdcConstOpt.C
The Vertical Drift Chamber t000, y000, p000 constant Optimization Script For PRex and CRex experiment. 

In the standard Jefferson Lab Hall A Podd, the first three lines in vdc database are used for project the detector 
variable to the Focal Plane Variables. For the central momentum of the HRS, the focal_theta, focal_Phi, and focal_y 
term should be 0 as how the focal plane is defined. 

The idea of the script: Use the event in the central sieve, project the detector plane variable to the focal plane. 
Optimized the database variables until the projected focal plane variable close to 0. 

The beam Position on target should as close to (0,0) as possible since the code did not do any correction to the 
beam position. All the Dp scan runs should be used in the Optimization. 

[More about VDC..](https://hallaweb.jlab.org/publications/Technotes/files/2002/02-012.pdf)


# Usage
- Optimized the 't000' 'y00', 'p000' vdc correction term:

```c++
vdcConstOptimizer()
```

- calculate the focal plane variables with the Detector Plane Variables
```c++
// This function is mainly used for project the previous cut result to the focal plane variable to avoid cut \
sieve whole again after change the t000 y000, p000 term


```


# TODO 
* Y have not been added yet(How?)


# @ me
- [Siyu Jian @ uva](mailto:sj9va@virginia.edu)

- [defination of Varibles in the Root file]()