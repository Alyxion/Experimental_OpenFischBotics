#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FCompEngine::FCompEngine(FComponent* Parent) :
	FComponent(Parent, FComponentType::Engine)
{

}

// ............................................................................

// Destructor
FCompEngine::~FCompEngine()
{

}

// ............................................................................

// Connects the engine to given controller and input port
void FCompEngine::Connect(FController *Controller, const int MajorPort, const int ClockwisePort, const int CcwPort, const int CounterPort)
{
	SetController(Controller);
	if (Controller)
	{
		Controller->SetupEngine(this, MajorPort, ClockwisePort, CcwPort, CounterPort);
	}
}

// ............................................................................

// Direct power control
void FCompEngine::SetOutputPower(float Value)
{
	if (mfOutputPower == Value)
		return;

	mfOutputPower = Value;
	mxiLastModification = ATime::GetTick();
}
