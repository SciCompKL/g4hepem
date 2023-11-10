
#ifndef G4HepEmRunUtils_HH
#define G4HepEmRunUtils_HH

#include "G4HepEmMacros.hh"

// Roate the direction [u,v,w] given in the scattering frame to the lab frame.
// Details: scattering is described relative to the [0,0,1] direction (i.e. scattering
// frame). Therefore, after the new direction is computed relative to this [0,0,1]
// original direction, the real original direction [u1,u2,u3] in the lab frame
// needs to be accounted and the final new direction, i.e. in the lab frame is
// computed.
G4HepEmHostDevice
void RotateToReferenceFrame(G4double &u, G4double &v, G4double &w, const G4double* refDir);

G4HepEmHostDevice
void RotateToReferenceFrame(G4double* dir, const G4double* refDir);

// get spline interpolation of y(x) between (x1, x2) given y_N = y(x_N), y''N(x_N) 
G4HepEmHostDevice
G4double GetSpline(G4double x1, G4double x2, G4double y1, G4double y2, G4double secderiv1, G4double secderiv2, G4double x);

// get spline interpolation over a log-spaced xgrid previously prepared by
// PrepareSpline (separate storrage of ydata and second deriavtive)
// use the improved, robust spline interpolation that I put in G4 10.6
G4HepEmHostDevice
G4double GetSplineLog(int ndata, G4double* xdata, G4double* ydata, G4double* secderiv, G4double x, G4double logx, G4double logxmin, G4double invLDBin);

// get spline interpolation over a log-spaced xgrid previously prepared by
// PrepareSpline (compact storrage of ydata and second deriavtive in ydata)
// use the improved, robust spline interpolation that I put in G4 10.6
G4HepEmHostDevice
G4double GetSplineLog(int ndata, G4double* xdata, G4double* ydata, G4double x, G4double logx, G4double logxmin, G4double invLDBin);

// get spline interpolation over a log-spaced xgrid previously prepared by
// PrepareSpline (compact storrage of xdata, ydata and second deriavtive in data)
// use the improved, robust spline interpolation that I put in G4 10.6
G4HepEmHostDevice
G4double GetSplineLog(int ndata, G4double* data, G4double x, G4double logx, G4double logxmin, G4double invLDBin);


// get spline interpolation over any xgrid: idx = i such  xdata[i] <= x < xdata[i+1]
// and x >= xdata[0] and x<xdata[ndata-1]
// PrepareSpline (separate storrage of ydata and second deriavtive)
// use the improved, robust spline interpolation that I put in G4 10.6
G4HepEmHostDevice
G4double GetSpline(G4double* xdata, G4double* ydata, G4double* secderiv, G4double x, int idx, int step=1);

// get spline interpolation if it was prepared with compact storrage of ydata
// and second deriavtive in ydata
// use the improved, robust spline interpolation that I put in G4 10.6
G4HepEmHostDevice
G4double GetSpline(G4double* xdata, G4double* ydata, G4double x, int idx);

// get spline interpolation if it was prepared with compact storrage of xdata,
// ydata and second deriavtive in data
G4HepEmHostDevice
G4double GetSpline(G4double* data, G4double x, int idx);

// finds the lower index of the x-bin in an ordered, increasing x-grid such
// that x[i] <= x < x[i+1]
G4HepEmHostDevice
int    FindLowerBinIndex(G4double* xdata, int num, G4double x, int step=1);


#endif // G4HepEmRunUtils_HH
