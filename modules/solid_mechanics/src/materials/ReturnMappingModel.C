#include "ReturnMappingModel.h"

#include "SymmIsotropicElasticityTensor.h"

template<>
InputParameters validParams<ReturnMappingModel>()
{
  InputParameters params = validParams<ConstitutiveModel>();

  // Sub-Newton Iteration control parameters
  params.addParam<unsigned int>("max_its", 30, "Maximum number of sub-newton iterations");
  params.addParam<bool>("output_iteration_info", false, "Set true to output sub-newton iteration information");
   params.addParam<bool>("output_iteration_info_on_error", false, "Set true to output sub-newton iteration information when a step fails");
  params.addParam<Real>("relative_tolerance", 1e-5, "Relative convergence tolerance for sub-newtion iteration");
  params.addParam<Real>("absolute_tolerance", 1e-20, "Absolute convergence tolerance for sub-newtion iteration");

  return params;
}


ReturnMappingModel::ReturnMappingModel( const std::string & name,
                                                  InputParameters parameters )
  :ConstitutiveModel( name, parameters ),
   _max_its(parameters.get<unsigned int>("max_its")),
   _output_iteration_info(getParam<bool>("output_iteration_info")),
   _output_iteration_info_on_error(getParam<bool>("output_iteration_info_on_error")),
   _relative_tolerance(parameters.get<Real>("relative_tolerance")),
   _absolute_tolerance(parameters.get<Real>("absolute_tolerance"))
{
}


void
ReturnMappingModel::computeStress( const Elem & current_elem,
                                   unsigned qp, const SymmElasticityTensor & elasticityTensor,
                                   const SymmTensor & stress_old, SymmTensor & strain_increment,
                                   SymmTensor & stress_new )
{
  // Given the stretching, compute the stress increment and add it to the old stress. Also update the creep strain
  // stress = stressOld + stressIncrement
  if (_t_step == 0) return;

  stress_new = elasticityTensor * strain_increment;
  stress_new += stress_old;

  SymmTensor inelastic_strain_increment;
  computeStress( current_elem, qp, elasticityTensor, stress_old,
                 strain_increment, stress_new, inelastic_strain_increment );

}

void
ReturnMappingModel::computeStress( const Elem & /*current_elem*/, unsigned qp,
                                   const SymmElasticityTensor & elasticityTensor,
                                   const SymmTensor & stress_old,
                                   SymmTensor & strain_increment,
                                   SymmTensor & stress_new,
                                   SymmTensor & inelastic_strain_increment )
{

  // compute deviatoric trial stress
  SymmTensor dev_trial_stress(stress_new);
  dev_trial_stress.addDiag( -dev_trial_stress.trace()/3.0 );

  // compute effective trial stress
  Real dts_squared = dev_trial_stress.doubleContraction(dev_trial_stress);
  Real effective_trial_stress = std::sqrt(1.5 * dts_squared);

  computeStressInitialize(qp, effective_trial_stress, elasticityTensor);

  // Use Newton sub-iteration to determine inelastic strain increment

  Real scalar = 0;
  unsigned int it = 0;
  Real residual = 10;
  Real norm_residual = 10;
  Real first_norm_residual = 10;

  std::stringstream iter_output;

  while (it < _max_its &&
        norm_residual > _absolute_tolerance &&
        (norm_residual/first_norm_residual) > _relative_tolerance)
  {
    iterationInitialize( qp, scalar );

    residual = computeResidual(qp, effective_trial_stress, scalar);
    norm_residual = std::abs(residual);
    if (it == 0)
    {
      first_norm_residual = norm_residual;
      if (first_norm_residual == 0)
      {
        first_norm_residual = 1;
      }
    }

    scalar -= residual / computeDerivative(qp, effective_trial_stress, scalar);

    if (_output_iteration_info == true ||
        _output_iteration_info_on_error == true)
    {
      iter_output
        << " it="       << it
        << " trl_strs=" << effective_trial_stress
        << " scalar="   << scalar
        << " rel_res="  << norm_residual/first_norm_residual
        << " rel_tol="  << _relative_tolerance
        << " abs_res="  << norm_residual
        << " abs_tol="  << _absolute_tolerance
        << std::endl;
    }

    iterationFinalize( qp, scalar );

    ++it;
  }

  if (_output_iteration_info)
    _console << iter_output.str();


  if (it == _max_its &&
     norm_residual > _absolute_tolerance &&
     (norm_residual/first_norm_residual) > _relative_tolerance)
  {
    if (_output_iteration_info_on_error)
    {
      Moose::err << iter_output.str();
    }
    mooseError("Max sub-newton iteration hit during nonlinear constitutive model solve!");
  }

  // compute inelastic and elastic strain increments (avoid potential divide by zero - how should this be done)?
  if (effective_trial_stress < 0.01)
  {
    effective_trial_stress = 0.01;
  }

  inelastic_strain_increment = dev_trial_stress;
  inelastic_strain_increment *= (1.5*scalar/effective_trial_stress);

  strain_increment -= inelastic_strain_increment;

  // compute stress increment
  stress_new = elasticityTensor * strain_increment;

  // update stress
  stress_new += stress_old;

  computeStressFinalize(qp, inelastic_strain_increment);

}
