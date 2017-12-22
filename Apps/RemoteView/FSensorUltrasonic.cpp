#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FSensorUltrasonic::FSensorUltrasonic(FComponent* Parent) :
	FCompSensor(Parent, FSensorInputType::Ultrasonic)
{
	SetValueRange(0, 1000);
}

// ............................................................................

// Destructor
FSensorUltrasonic::~FSensorUltrasonic()
{

}
