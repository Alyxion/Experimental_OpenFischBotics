#pragma once
#ifndef FSensorUltrasonic_h
#define FSensorUltrasonic_h

// ----------------------------------------------------------------------------

/** @brief Defines an ultrasonic distance meter */
class FSensorUltrasonic : public FCompSensor
{
	FCLASS(FSensorUltrasonic, FCompSensor);

protected:
public:
	//! Constructor
	/** @param [in] Parent The parent component */
	OFB_API FSensorUltrasonic(FComponent* Parent);

	//! Destructor
	OFB_API ~FSensorUltrasonic();

	// Returns a description
	const FString GetDescription() override { return "Ultrasonic Sensor ft-133009"; }
};

// ----------------------------------------------------------------------------

#endif // of FSensorUltrasonic_h
