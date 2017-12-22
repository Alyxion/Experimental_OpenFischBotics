#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FComponent::FComponent(FComponent *Parent, const FComponentType Type) :
	mType(Type)
{
	if (Parent)
	{
		SetParent(Parent);
	}
}

// ............................................................................

// Destructor
FComponent::~FComponent()
{
	if (mlpController)
	{
		mlpController->RemoveComponent(this);
	}

	SetParent(nullptr);		// remove from parent

	// destroy child components
	for (int i = ~mlChildren-1; i >= 0; --i)
	{
		if(i>=~mlChildren)	// automatism check
		{
			continue;
		}

		FComponent* cur = mlChildren[i];
		cur->mlpParent = nullptr;
		FDelete(cur);
		mlChildren.Delete(i);
	}

	ClearVisualElements();
}

// ............................................................................

// Returns the component's description name
const FString FComponent::GetDescription()
{
	return FString();
}

// ............................................................................

// Returns the component's status
const FString FComponent::GetStatus()
{
	return FString();
}

// ............................................................................

// Removes the controller (for example because it's destroyed)
void FComponent::_RemoveControllerInt()
{
	mlpController = nullptr;
}

// ............................................................................

// Returns the component's controller
FController* FComponent::GetController() const
{
	return mlpController;
}

// ............................................................................

// Sets the component's controller
void FComponent::SetController(FController* Controller)
{
	if (Controller != mlpController)
	{
		if (mlpController)	// remove from old controller if required
		{
			mlpController->RemoveComponent(this);
		}
		mlpController = Controller;
		if (mlpController)	// register to new controller if required
		{
			mlpController->RegisterComponent(this);
		}
	}
}

// ............................................................................

// Returns the position
const FVector& FComponent::GetPosition() const
{
	return mvPosition;
}

// ............................................................................

// Returns the rotation
const FVector& FComponent::GetRotation() const
{
	return mvRotation;
}

// ............................................................................

// Sets a new position and rotation
void FComponent::SetPosition(const FVector &Position, const FVector &Rotation)
{
	mvPosition = Position;
	mvRotation = Rotation;
}

// ............................................................................

// Sets a new position
void FComponent::SetPosition(const FVector &Position)
{
	FVector oldRotation = mvRotation;
	SetPosition(Position, oldRotation);
}

// ............................................................................

// Sets a new rotation
void FComponent::SetRotation(const FVector &Rotation)
{
	FVector oldPosition = mvPosition;
	SetPosition(oldPosition, Rotation);
}

// ............................................................................

// Registers a new child component
void FComponent::AddChild(FComponent* Child)
{
	mlChildren.Add(Child);
}

// ............................................................................

// Removes a child component
void FComponent::RemoveChild(FComponent* Child)
{
	mlChildren.Remove(Child);
}

// ............................................................................

// Clears the list of visual elements
void FComponent::ClearVisualElements()
{
	for (int i = ~mlVisualElements - 1; i >= 0; --i)
	{
		if (i >= ~mlVisualElements)	// automatism check
		{
			continue;
		}

		FVisualElement* cur = mlVisualElements[i];
		FDelete(cur);
		mlVisualElements.Delete(i);
	}
}

// ............................................................................

// Adds a visual element
void FComponent::AddVisualElement(FVisualElement* Element)
{
	mlVisualElements.Add(Element);
}

// ............................................................................

// Returns the components parent component
FComponent* FComponent::GetParent() const
{
	return mlpParent;
}

// ............................................................................

// Sets a new parent
void FComponent::SetParent(FComponent* Parent)
{
	if (Parent == mlpParent)		// do nothing if Parent already matches
	{
		return;
	}

	if (mlpParent)	// remove from parent component
	{
		mlpParent->RemoveChild(this);
		mlpParent = nullptr;
	}

	if (Parent)
	{
		Parent->AddChild(this);
		mlpParent = Parent;
	}
}

// ............................................................................

// Returns the count of child components
const int FComponent::GetChildCount() const
{
	return ~mlChildren;
}

// ............................................................................

// Returns the child at given index
FComponent* FComponent::GetChildAt(const int Index) const
{
	return Index >= 0 && Index < ~mlChildren ? mlChildren[Index] : nullptr;
}

// ............................................................................

// Returns the count of visual elements
const int FComponent::GetVisualElementCount() const
{
	return ~mlVisualElements;
}

// ............................................................................

// Returns the visual element at given index
const FVisualElement* FComponent::GetVisualElementAt(const int Index) const
{
	return Index >= 0 && Index < ~mlVisualElements ? mlVisualElements[Index] : nullptr;
}