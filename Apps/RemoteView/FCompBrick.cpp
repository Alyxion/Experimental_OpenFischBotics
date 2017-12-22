#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FCompBrick::FCompBrick(FComponent* Parent, const FBrickType Type, const float Length) :
	FComponent(Parent),
	mType(Type)
{
	double usedLength = Length == 0.0 ? 0.3 : Length;

	switch (Type)
	{
	case FBrickType::Black15:
	{
		mdWidth = 0.15; 
	} break;
	case FBrickType::Black30:
	{
		mdWidth = 0.3;
	} break;
	case FBrickType::Alu:
	{
		mdWidth = usedLength;
	} break;
	default:
	{

	}
	}

	AddVisualElement(FNew(FBoxElement)(FVector(0.0, 0.0, 0.0), FVector(mdWidth, mdHeight, mdDepth), aclBlack));
}

// ............................................................................

// Destructor
FCompBrick::~FCompBrick()
{

}
