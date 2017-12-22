#pragma once
#ifndef FEngineEncoder_h
#define FEngineEncoder_h

// ----------------------------------------------------------------------------

/** @brief Encoder engine handling class */
class FEngineEncoder : public FCompEngine
{
	FCLASS(FEngineEncoder, FCompEngine);

protected:
public:
	//! Constructor
	/** @param [in] Parent The parent component */
	OFB_API FEngineEncoder(FComponent* Parent);

	//! Destructor
	OFB_API ~FEngineEncoder();
};

// ----------------------------------------------------------------------------

#endif // of FEngineEncoder_h
