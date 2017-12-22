#pragma once
#ifndef FCompEngine_h
#define FCompEngine_h

// ----------------------------------------------------------------------------

/** @brief Defines an engine or other manually powered component */
class FCompEngine : public FComponent
{
	FCLASS(FCompEngine, FComponent);

protected:
	int				miForwardsPort = -1,		///< The forwards movement port
					miBackwardsPort = -1,		///< The backwards movement port
					miCounterPort = -1;			///< The counter port
	float			mfOutputPower = 0.f;		///< The desired output power
	FInt64			mxiLastModification = 0;	///< The tick of the last modification of target counter or power value

public:
	//! Constructor
	/** @param [in] Parent The parent component */
	OFB_API FCompEngine(FComponent* Parent);

	//! Destructor
	OFB_API ~FCompEngine();

	//! Connects the engine to given controller and input port
	/** @param [in] Controller The controller providing the power and counter access
	* @param [in] MajorPort The major port (reserved for future use, pass 0)
	* @param [in] ClockwisePort The clockwise power port (0..7) or -1 if not available
	* @param [in] CcwPort The counter clockwise power port (0..7) or -1 if not available
	* @param [in] CounterPort The counter input port */
	OFB_API void Connect(FController *Controller, const int MajorPort, const int ClockwisePort, const int CcwPort, const int CounterPort);

	//! Direct power control
	/** @param [in] Value The powering value (> 0.0 .. 1.0 for clockwise movement, < 0.0 .. -1.0 for counter clockwise movement)  */
	OFB_API void SetOutputPower(float Value);

	//! Returns the current set output power value
	/** @return The current value */
	inline const float GetOutputPower() const { return mfOutputPower; }

	//! Returns the last modification tick value
	/** @return The tick value */
	inline const FInt64 GetLastModification() const { return mxiLastModification; }

	// Engine type cast
	OFB_API FCompEngine* AsEngine() override { return this; }
};

// ----------------------------------------------------------------------------

#endif // of FCompEngine_h
