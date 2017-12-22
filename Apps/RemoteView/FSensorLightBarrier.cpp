#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FSensorLightBarrier::FSensorLightBarrier(FComponent* Parent) :
	FCompSensor(Parent, FSensorInputType::Analog5kOhm)
{

}

// ............................................................................

// Destructor	
FSensorLightBarrier::~FSensorLightBarrier()
{

}
