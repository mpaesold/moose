/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "MultiAppPostprocessorTransfer.h"

// Moose
#include "MooseTypes.h"
#include "FEProblem.h"

// libMesh
#include "libmesh/meshfree_interpolation.h"
#include "libmesh/system.h"

template<>
InputParameters validParams<MultiAppPostprocessorTransfer>()
{
  InputParameters params = validParams<MultiAppTransfer>();
  params.addRequiredParam<PostprocessorName>("from_postprocessor", "The name of the Postprocessor in the Master to transfer the value from.");
  params.addRequiredParam<PostprocessorName>("to_postprocessor", "The name of the Postprocessor in the MultiApp to transfer the value to.  This should most likely be a Reporter Postprocessor.");
  MooseEnum reduction_type("average sum maximum minimum");
  params.addParam<MooseEnum>("reduction_type", reduction_type, "The type of reduction to perform to reduce postprocessor values from multiple SubApps to a single value");
  return params;
}

MultiAppPostprocessorTransfer::MultiAppPostprocessorTransfer(const std::string & name, InputParameters parameters) :
    MultiAppTransfer(name, parameters),
    _from_pp_name(getParam<PostprocessorName>("from_postprocessor")),
    _to_pp_name(getParam<PostprocessorName>("to_postprocessor")),
    _reduction_type(getParam<MooseEnum>("reduction_type"))
{
  if (_direction == FROM_MULTIAPP)
    if (!_reduction_type.isValid())
      mooseError("In MultiAppPostprocessorTransfer, must specify 'reduction_type' if direction = from_multiapp");
}

void
MultiAppPostprocessorTransfer::execute()
{
  switch (_direction)
  {
    case TO_MULTIAPP:
    {
      FEProblem & from_problem = *_multi_app->problem();

      Real pp_value = from_problem.getPostprocessorValue(_from_pp_name);

      for (unsigned int i=0; i<_multi_app->numGlobalApps(); i++)
        if (_multi_app->hasLocalApp(i))
          _multi_app->appProblem(i)->getPostprocessorValue(_to_pp_name) = pp_value;
      break;
    }
    case FROM_MULTIAPP:
    {
      FEProblem & to_problem = *_multi_app->problem();

      Real reduced_pp_value;
      Real default_init;

      switch (_reduction_type)
      {
        case AVERAGE:
        case SUM:
          reduced_pp_value = 0;
          default_init = -std::numeric_limits<Real>::max();
          break;
        case MAXIMUM:
          reduced_pp_value = -std::numeric_limits<Real>::max();
          default_init = -std::numeric_limits<Real>::max();
          break;
        case MINIMUM:
          reduced_pp_value = std::numeric_limits<Real>::max();
          default_init = std::numeric_limits<Real>::max();
          break;
        default:
          mooseError("Can't get here unless someone adds a new enum and fails to add it to this switch");
      }

      bool found_local = false;
      for (unsigned int i=0; i<_multi_app->numGlobalApps(); i++)
      {
        if (_multi_app->hasLocalApp(i) && _multi_app->isRootProcessor())
        {
          found_local = true;
          Real curr_pp_value = _multi_app->appProblem(i)->getPostprocessorValue(_from_pp_name);
          switch (_reduction_type)
          {
            case AVERAGE:
            case SUM:
              reduced_pp_value += curr_pp_value;
              break;
            case MAXIMUM:
              reduced_pp_value = std::max(curr_pp_value,reduced_pp_value);
              break;
            case MINIMUM:
              reduced_pp_value = std::min(curr_pp_value,reduced_pp_value);
              break;
          default:
            mooseError("Can't get here unless someone adds a new enum and fails to add it to this switch");
          }
        }
      }
      if (!found_local)
        reduced_pp_value = default_init;

      switch (_reduction_type)
      {
        case AVERAGE:
          _communicator.sum(reduced_pp_value);
          reduced_pp_value /= (Real) _multi_app->numGlobalApps();
          break;
        case SUM:
          _communicator.sum(reduced_pp_value);
          break;
        case MAXIMUM:
          _communicator.max(reduced_pp_value);
          break;
        case MINIMUM:
          _communicator.min(reduced_pp_value);
          break;
      default:
        mooseError("Can't get here unless someone adds a new enum and fails to add it to this switch");
      }

      to_problem.getPostprocessorValue(_to_pp_name) = reduced_pp_value;
      break;
    }
  }
}
