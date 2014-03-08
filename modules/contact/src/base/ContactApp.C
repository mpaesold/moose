#include "ContactApp.h"
#include "Moose.h"
#include "AppFactory.h"

#include "ContactAction.h"
#include "ContactMaster.h"
#include "ContactPenetrationAuxAction.h"
#include "ContactPenetrationVarAction.h"
#include "ContactPressureAux.h"
#include "ContactPressureAuxAction.h"
#include "ContactPressureVarAction.h"
#include "NodalAreaAction.h"
#include "SlaveConstraint.h"
#include "OneDContactConstraint.h"
#include "MultiDContactConstraint.h"
#include "GluedContactConstraint.h"
#include "SparsityBasedGluedContactConstraint.h"
#include "FrictionalContactProblem.h"
#include "ReferenceResidualProblem.h"
#include "NodalArea.h"
#include "NodalAreaAction.h"
#include "NodalAreaVarAction.h"

template<>
InputParameters validParams<ContactApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

ContactApp::ContactApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(libMesh::processor_id());

  Moose::registerObjects(_factory);
  ContactApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ContactApp::associateSyntax(_syntax, _action_factory);
}

ContactApp::~ContactApp()
{
}

void
ContactApp::registerApps()
{
  registerApp(ContactApp);
}

void
ContactApp::registerObjects(Factory & factory)
{
  registerDiracKernel(ContactMaster);
  registerDiracKernel(SlaveConstraint);
  registerConstraint(OneDContactConstraint);
  registerConstraint(MultiDContactConstraint);
  registerConstraint(GluedContactConstraint);
  registerConstraint(SparsityBasedGluedContactConstraint);
  registerProblem(FrictionalContactProblem);
  registerProblem(ReferenceResidualProblem);
  registerUserObject(NodalArea);
  registerAux(ContactPressureAux);
}

void
ContactApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  syntax.registerActionSyntax("ContactAction", "Contact/*");

  syntax.registerActionSyntax("ContactPenetrationAuxAction", "Contact/*");
  syntax.registerActionSyntax("ContactPenetrationVarAction", "Contact/*");

  syntax.registerActionSyntax("ContactPressureAuxAction", "Contact/*");
  syntax.registerActionSyntax("ContactPressureVarAction", "Contact/*");

  syntax.registerActionSyntax("NodalAreaAction", "Contact/*");
  syntax.registerActionSyntax("NodalAreaVarAction", "Contact/*");

  registerAction(ContactAction, "add_dg_kernel");

  registerAction(ContactPenetrationAuxAction, "add_aux_bc");
  registerAction(ContactPenetrationVarAction, "add_aux_variable");

  registerAction(ContactPressureAuxAction, "add_aux_bc");
  registerAction(ContactPressureVarAction, "add_aux_variable");

  registerAction(NodalAreaAction, "add_user_object");
  registerAction(NodalAreaVarAction, "add_aux_variable");
}