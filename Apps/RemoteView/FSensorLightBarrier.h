#pragma once
#ifndef FSensorLightBarrier_h
#define FSensorLightBarrier_h

// ----------------------------------------------------------------------------

/** @brief Light barrier sensor */
class FSensorLightBarrier : public FCompSensor
{
	FCLASS(FSensorLightBarrier, FCompSensor);

protected:
public:
	//! Constructor
	/** @param [in] Parent The parent component */
	OFB_API FSensorLightBarrier(FComponent* Parent);

	//! Destructor	
	OFB_API ~FSensorLightBarrier();

	// Returns a description
	const FString GetDescription() override { return "Photo transistor ft-153870"; }
};

// ----------------------------------------------------------------------------

#endif // of FSensorLightBarrier_h
