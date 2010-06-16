[Mesh]
  dim = 2
  file = square.e
  uniform_refine = 4
[]

[Variables]
  active = 'u v'

  [./u]
    order = FIRST
    family = LAGRANGE
  [../]

  [./v]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  active = 'u_ie example_diff conv v_ie diff'

  [./u_ie]
    type = ImplicitEuler
    variable = u
  [../]

  [./example_diff]
    # This Kernel uses "diffusivity" from the active material 
    type = ExampleDiffusion
    variable = u
  [../]

  [./conv]
    type = Convection
    variable = u
    velocity_vector = v
  [../]

  [./v_ie]
    type = ImplicitEuler
    variable = v
  [../]

  [./diff]
    type = Diffusion
    variable = v
  [../]
[]

[BCs]
  active = 'left_u right_u left_v right_v'

  [./left_u]
    type = DirichletBC
    variable = u
    boundary = '1'
    value = 0
  [../]

  [./right_u]
    type = DirichletBC
    variable = u
    boundary = '2'
    value = 1

    some_var = v
  [../]

  [./left_v]
    type = DirichletBC
    variable = v
    boundary = '1'
    value = 0
  [../]

  [./right_v]
    type = DirichletBC
    variable = v
    boundary = '2'
    value = 1
  [../]

[]

[Materials]
  active = empty

  [./empty]
    type = ExampleMaterial
    block = 1
    diffusivity = 0.1
  [../]
[]

[Executioner]
  type = Transient
  perf_log = true
  petsc_options = '-snes_mf_operator'

  num_steps = 10
  dt = 1.0
[]

[Output]
  file_base = out
  interval = 1
  exodus = true
[]
   
    
