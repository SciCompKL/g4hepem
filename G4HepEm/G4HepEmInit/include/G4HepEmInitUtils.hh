
#ifndef G4HepEmInitUtils_HH
#define G4HepEmInitUtils_HH

//
// Utility methods used during the initialisation.
//

class G4HepEmInitUtils {
public:
  G4HepEmInitUtils() = delete;

  // Fills the input vector arguments with `npoints` GL integral abscissas and 
  // weights values (on [0,1] by default)
  static void GLIntegral(int npoints, G4double* abscissas, G4double* weights, G4double min=0.0, G4double max=1.0);
                  
  
  
  // receives `npoint` x,y data arrays and fills the `secderiv` array with the 
  // second derivatives that can be used for a spline interpolation
  static void   PrepareSpline(int npoint, G4double* xdata, G4double* ydata, G4double* secderiv);

  // same as above, but both the ydata and second derivatives are stored in the 
  // ydata array (compact [...,y_i, sd_i, y_{i+1}, sd_{i+1},...] with 2x`npoint`)
  static void   PrepareSpline(int npoint, G4double* xdata, G4double* ydata);                


  // get spline interpolation over a log-spaced xgrid previously prepared by 
  // PrepareSpline (separate storrage of ydata and second deriavtive)  
  // use the improved, robust spline interpolation that I put in G4 10.6
  static G4double GetSplineLog(int ndata, G4double* xdata, G4double* ydata, G4double* secderiv, G4double x, G4double logx, G4double logxmin, G4double invLDBin);

  // get spline interpolation over a log-spaced xgrid previously prepared by 
  // PrepareSpline (compact storrage of ydata and second deriavtive in ydata)  
  // use the improved, robust spline interpolation that I put in G4 10.6
  static G4double GetSplineLog(int ndata, G4double* xdata, G4double* ydata, G4double x, G4double logx, G4double logxmin, G4double invLDBin);

  // get spline interpolation over a log-spaced xgrid previously prepared by 
  // PrepareSpline (compact storrage of xdata, ydata and second deriavtive in data)  
  // use the improved, robust spline interpolation that I put in G4 10.6
  static G4double GetSplineLog(int ndata, G4double* data, G4double x, G4double logx, G4double logxmin, G4double invLDBin);


  // get spline interpolation over any xgrid: idx = i such  xdata[i] <= x < xdata[i+1]
  // and x >= xdata[0] and x<xdata[ndata-1]
  // PrepareSpline (separate storrage of ydata and second deriavtive)  
  // use the improved, robust spline interpolation that I put in G4 10.6
  static G4double GetSpline(G4double* xdata, G4double* ydata, G4double* secderiv, G4double x, int idx, int step=1);

  // get spline interpolation if it was prepared with compact storrage of ydata 
  // and second deriavtive in ydata
  // use the improved, robust spline interpolation that I put in G4 10.6
  static G4double GetSpline(G4double* xdata, G4double* ydata, G4double x, int idx);

  // get spline interpolation if it was prepared with compact storrage of xdata,
  // ydata and second deriavtive in data
  static G4double GetSpline(G4double* data, G4double x, int idx);
  
  
  // finds the lower index of the x-bin in an ordered, increasing x-grid such 
  // that x[i] <= x < x[i+1]
  static int    FindLowerBinIndex(G4double* xdata, int num, G4double x, int step=1);

   /**
   * Fills a pre-existing array with G4doubles uniformly spaced in log(x)
   *
   * @param[in] emin Inclusive lower bound
   * @param[in] emax Inclusive upper bound
   * @param[in] npoints Number of points in array, including upper/lower bounds
   * @param[out] log_min_value Logarithm of @param emin
   * @param[out] inverse_log_delta Inverse of logarithmic spacing between points
   * @param[out] grid pointer to array
   *
   * @pre @param grid must not by `nullptr` and have capacity for at least @param npoints `G4double`s
   * @post The [0,npoints-1] elements of @param grid will contain the data
   */
  static void FillLogarithmicGrid(const G4double emin, const G4double emax, const int npoints,
                                  G4double& log_min_value, G4double& inverse_log_delta, G4double* grid);
}; 

#endif //  G4HepEmInitUtils_HH