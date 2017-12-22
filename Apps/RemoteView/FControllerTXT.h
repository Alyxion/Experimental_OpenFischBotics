#pragma once
#ifndef FControllerTXT_h
#define FControllerTXT_h

// ----------------------------------------------------------------------------

/** @brief fischertechnik TXT controller component */
class FControllerTXT : public FController
{
	FCLASS(FControllerTXT, FController);

protected:
	FTControllerConnection		*mlpConnection = nullptr;		///< The TXT controller connection
	bool						mbNewConnection = true;			///< Connection modified ?

	// Synchronization handling
	OFB_API const FInt64 SynchInt(const FInt64 CurTick) override;

public:
	//! Constructor
	/** @param [in] Parent The parent component */
	OFB_API FControllerTXT(FComponent* Parent);

	//! Destructor
	OFB_API ~FControllerTXT();

	// Tries to enable the controller
	OFB_API void SetEnabled(const bool State) override;

	// Returns the controller's state
	OFB_API const bool GetEnabled() override;

	// Returns if a connection error occurred
	OFB_API const bool GetErrorOccurred() override;
};

// ----------------------------------------------------------------------------

#endif // of FControllerTXT_h
