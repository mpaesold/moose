# Density User objects give the correct value
# 
# If you want to add another test for another UserObject
# then add the UserObject, add a Function defining the expected result,
# add an AuxVariable and AuxKernel that will record the UserObject's value
# and finally add a NodalL2Error that compares this with the Function

[UserObjects]
  [./PPNames]
    type = RichardsPorepressureNames
    porepressure_vars = pressure
  [../]
  [./DensityConstBulk]
    type = RichardsDensityConstBulk
    dens0 = 1000
    bulk_mod = 2E6
  [../]
  [./DensityIdeal]
    type = RichardsDensityIdeal
    p0 = 33333
    slope = 1.1E-2
  [../]

  # following are unimportant in this test
  [./SeffVG]
    type = RichardsSeff1VG
    m = 0.8
    al = 1E-6
  [../]
  [./RelPermPower]
    type = RichardsRelPermPower
    simm = 0.10101
    n = 2
  [../]
  [./Saturation]
    type = RichardsSat
    s_res = 0.054321
    sum_s_res = 0.054321
  [../]
  [./SUPGstandard]
    type = RichardsSUPGstandard
    p_SUPG = 1E5
  [../]
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = x
  [../]
  [./answer_DensityConstBulk]
    type = ParsedFunction
    value = dens0*exp(x/bulk_mod)
    vars = 'dens0 bulk_mod'
    vals = '1000 2E6'
  [../]
  [./answer_dDensityConstBulk]
    type = GradParsedFunction
    direction = '1 0 0'
    value = dens0*exp(x/bulk_mod)
    vars = 'dens0 bulk_mod'
    vals = '1000 2E6'
  [../]
  [./answer_d2DensityConstBulk]
    type = Grad2ParsedFunction
    direction = '1 0 0'
    value = dens0*exp(x/bulk_mod)
    vars = 'dens0 bulk_mod'
    vals = '1000 2E6'
  [../]

  [./answer_DensityIdeal]
    type = ParsedFunction
    value = slope*(x-p0)
    vars = 'p0 slope'
    vals = '33333 1.1E-2'
  [../]
  [./answer_dDensityIdeal]
    type = GradParsedFunction
    direction = '1 0 0'
    value = slope*(x-p0)
    vars = 'p0 slope'
    vals = '33333 1.1E-2'
  [../]
  [./answer_d2DensityIdeal]
    type = Grad2ParsedFunction
    direction = '1 0 0'
    value = slope*(x-p0)
    vars = 'p0 slope'
    vals = '33333 1.1E-2'
  [../]
[]

[AuxVariables]
  [./DensityConstBulk_Aux]
  [../]
  [./dDensityConstBulk_Aux]
  [../]
  [./d2DensityConstBulk_Aux]
  [../]

  [./DensityIdeal_Aux]
  [../]
  [./dDensityIdeal_Aux]
  [../]
  [./d2DensityIdeal_Aux]
  [../]

  [./check_Aux]
  [../]
[]

[AuxKernels]
  [./DensityConstBulk_AuxK]
    type = RichardsDensityAux
    variable = DensityConstBulk_Aux
    density_UO = DensityConstBulk
    pressure_var = pressure
  [../]
  [./dDensityConstBulk_AuxK]
    type = RichardsDensityPrimeAux
    variable = dDensityConstBulk_Aux
    density_UO = DensityConstBulk
    pressure_var = pressure
  [../]
  [./d2DensityConstBulk_AuxK]
    type = RichardsDensityPrimePrimeAux
    variable = d2DensityConstBulk_Aux
    density_UO = DensityConstBulk
    pressure_var = pressure
  [../]

  [./DensityIdeal_AuxK]
    type = RichardsDensityAux
    variable = DensityIdeal_Aux
    density_UO = DensityIdeal
    pressure_var = pressure
  [../]
  [./dDensityIdeal_AuxK]
    type = RichardsDensityPrimeAux
    variable = dDensityIdeal_Aux
    density_UO = DensityIdeal
    pressure_var = pressure
  [../]
  [./d2DensityIdeal_AuxK]
    type = RichardsDensityPrimePrimeAux
    variable = d2DensityIdeal_Aux
    density_UO = DensityIdeal
    pressure_var = pressure
  [../]

  [./check_AuxK]
    type = FunctionAux
    variable = check_Aux
    function = answer_d2DensityConstBulk
  [../]
[]

[Postprocessors]
  [./cf_DensityConstBulk]
    type = NodalL2Error
    function = answer_DensityConstBulk
    variable = DensityConstBulk_Aux
  [../]
  [./cf_dDensityConstBulk]
    type = NodalL2Error
    function = answer_dDensityConstBulk
    variable = dDensityConstBulk_Aux
  [../]
  [./cf_d2DensityConstBulk]
    type = NodalL2Error
    function = answer_d2DensityConstBulk
    variable = d2DensityConstBulk_Aux
  [../]

  [./cf_DensityIdeal]
    type = NodalL2Error
    function = answer_DensityIdeal
    variable = DensityIdeal_Aux
  [../]
  [./cf_dDensityIdeal]
    type = NodalL2Error
    function = answer_dDensityIdeal
    variable = dDensityIdeal_Aux
  [../]
  [./cf_d2DensityIdeal]
    type = NodalL2Error
    function = answer_d2DensityIdeal
    variable = d2DensityIdeal_Aux
  [../]
[]
    


#############################################################################
#
# Following is largely unimportant as we're not running an actual similation
#
#############################################################################
[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 100
  xmin = -5E6
  xmax = 5E6
[]

[Variables]
  [./pressure]
    order = FIRST
    family = LAGRANGE
    [./InitialCondition]
      type = FunctionIC
      function = initial_pressure
    [../]
  [../]
[]
  
[Kernels]
  active = 'richardsf richardst'
  [./richardst]
    type = RichardsMassChange
    porepressureNames_UO = PPNames
    variable = pressure
  [../]
  [./richardsf]
    type = RichardsFlux
    porepressureNames_UO = PPNames
    variable = pressure
  [../]
[]

[Materials]
  [./unimportant_material]
    type = RichardsMaterial
    block = 0
    mat_porosity = 0.1
    mat_permeability = '1E-20 0 0  0 1E-20 0  0 0 1E-20'
    porepressureNames_UO = PPNames
    density_UO = DensityConstBulk
    relperm_UO = RelPermPower
    sat_UO = Saturation
    seff_UO = SeffVG
    SUPG_UO = SUPGstandard
    viscosity = 1E-3
    gravity = '0 0 -10'
    linear_shape_fcns = true
  [../]
[]

[Preconditioning]
  [./does_nothing]
    type = SMP
    full = true
    petsc_options = '-snes_converged_reason'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E50 1E50 10000'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  num_steps = 1
  dt = 1E-100
[]

[Output]
  linear_residuals = false
  file_base = uo2
  interval = 1
  exodus = false
  postprocessor_csv = true
  perf_log = false
  hidden_variables = pressure
[]
