/*****************************************/
/* Written by andrew.wilkins@csiro.au    */
/* Please contact me if you make changes */
/*****************************************/

#ifndef RICHARDSHALFGAUSSIANSINKFLUX_H
#define RICHARDSHALFGAUSSIANSINKFLUX_H

#include "SideIntegralVariablePostprocessor.h"
#include "RichardsVarNames.h"

//Forward Declarations
class RichardsHalfGaussianSinkFlux;

template<>
InputParameters validParams<RichardsHalfGaussianSinkFlux>();

/**
 * Postprocessor that records the mass flux from porespace to
 * a half-gaussian sink.  (Positive if fluid is being removed from porespace.)
 * flux out = max*exp((-0.5*(p - centre)/sd)^2) for p<centre, and flux out = max otherwise
 */
class RichardsHalfGaussianSinkFlux: public SideIntegralVariablePostprocessor
{
public:
  RichardsHalfGaussianSinkFlux(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  FEProblem & _feproblem;

  /// flux out = max*exp((-0.5*(p - centre)/sd)^2) for p<centre, and flux out = max otherwise
  Real _maximum;

  /// flux out = max*exp((-0.5*(p - centre)/sd)^2) for p<centre, and flux out = max otherwise
  Real _sd;

  /// flux out = max*exp((-0.5*(p - centre)/sd)^2) for p<centre, and flux out = max otherwise
  Real _centre;

  /**
   * holds info regarding the names of the Richards variables
   * and methods for extracting values of these variables
   */
  const RichardsVarNames & _richards_name_UO;

  /**
   * the index of this variable in the list of Richards variables
   * held by _richards_name_UO.  Eg
   * if richards_vars = 'pwater pgas poil' in the _richards_name_UO
   * and this kernel has variable = pgas, then _pvar = 1
   * This is used to index correctly into _viscosity, _seff, etc
   */
  unsigned int _pvar;

  /// porepressure (or porepressure vector for multiphase problems)
  MaterialProperty<std::vector<Real> > & _pp;

};

#endif
