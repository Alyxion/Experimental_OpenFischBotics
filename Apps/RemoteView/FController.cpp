#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FController::FController(FComponent* Parent) :
	FComponent(Parent, FComponentType::Controller)
{

}

// ............................................................................

// Destructor
FController::~FController()
{
	for (int i = 0; i < ~mlComponents; ++i)
	{
		FDelete(mlComponents[i]);
	}
	mlComponents.Clear();
}

// ............................................................................

// Tries to fetch new data from the real controller
const bool FController::Synchronize()
{
	FInt64 newTick = SynchInt(mxiLastSynch);
	if (newTick != mxiLastSynch)
	{
		mxiLastSynch = newTick;
		return true;
	}

	return false;
}

// ............................................................................

// Setups a sensor
const FComponentRegistrationError FController::SetupSensor(FComponent* Component, const FSensorInputType InputType, const int MajorIndex, const int MinorIndex)
{
	for (int i = 0; i < ~mlComponents; ++i)	// search for matching component
	{
		FRegisteredComponent* cur = mlComponents[i];
		if (cur->mlpComponent == Component)	// and update values
		{
			cur->miMajorIndex = MajorIndex;
			cur->miMinorIndex = MinorIndex;
			cur->mInputType = InputType;
			cur->mxiLastModification = mxiLastSynch;
			break;
		}
	}

	return FComponentRegistrationError::None;
}

// ............................................................................

// Setups an engine
const FComponentRegistrationError FController::SetupEngine(FComponent* Component, const int MajorIndex, const int FirstOutput, const int SecondOutput, const int CounterIndex)
{
	for (int i = 0; i < ~mlComponents; ++i)	// search for matching component
	{
		FRegisteredComponent* cur = mlComponents[i];
		if (cur->mlpComponent == Component)	// and update values
		{
			cur->miMajorIndex = MajorIndex;
			cur->miMinorIndex = FirstOutput;
			cur->miSecondaryIndex = SecondOutput;
			cur->miCounterIndex = CounterIndex;
			cur->mxiLastModification = mxiLastSynch;
			break;
		}
	}

	return FComponentRegistrationError::None;
}

// ............................................................................

// Registers given component
void FController::RegisterComponent(FComponent* Component)
{
	bool found = false;

	for (int i = 0; i < ~mlComponents; ++i)
	{
		if (mlComponents[i]->mlpComponent == Component)
		{
			found = true;
			break;
		}
	}

	// add if not found
	if (!found)
	{
		FRegisteredComponent* newComponent = FNew(FRegisteredComponent)();
		newComponent->mlpComponent = Component;
		newComponent->mType = Component->GetType();
		mlComponents.Add(newComponent);
	}
}

// ............................................................................

// Removes given component
void FController::RemoveComponent(FComponent* Component)
{
	// search and remove component
	for (int i = 0; i < ~mlComponents; ++i)
	{
		if (mlComponents[i]->mlpComponent == Component)
		{
			FDelete(mlComponents[i]);
			mlComponents.Delete(i);
			return;
		}
	}
}
