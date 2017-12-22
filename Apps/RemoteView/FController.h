#pragma once
#ifndef FController_h
#define FController_h

// ----------------------------------------------------------------------------

/** @brief Defines a registered component */
class FRegisteredComponent
{
public:
	FComponent		*mlpComponent = nullptr;		///< The component
	FComponentType	mType = FComponentType::None;	///< The component's type
	FSensorInputType mInputType = FSensorInputType::None; ///< The input type
	AInt64			mxiLastModification = 0;		///< The last modification tick
	int				miMajorIndex		= 0,		///< Major input / output index
					miMinorIndex		= -1,		///< Minor input / output index, -1 = undefined
					miSecondaryIndex	= -1;		///< Secondary input / output index (engine reverse for example), -1 = undefined
	int				miCounterIndex		= -1;		///< The encoder counter index, -1 = undefined
};

// ----------------------------------------------------------------------------

/** @brief Enumeration of component registration errors */
enum class FComponentRegistrationError
{
	None,			///< No error
	InputIndexInUse	///< Input index already in use
};

// ----------------------------------------------------------------------------

/** @brief Controller base interface class */
class FController : public FComponent
{
	FCLASS(FController, FComponent);

protected:
	FList<FRegisteredComponent*>		mlComponents;		///< List of registered components
	FInt64								mxiLastSynch = 0;	///< Last synchronization id

	//! Synchronization handler. Updates desired sensor modes, output voltages, counter and sensor inputs
	/** @param [in] CurTick The current data update counter / tick
	  * @return The new data update counter / tick. If matching the input value no data update was available */
	OFB_API virtual const FInt64 SynchInt(const FInt64 CurTick) = 0;

public:
	//! Constructor
	/** @param [in] Parent The parent component */
	OFB_API FController(FComponent* Parent);

	//! Destructor
	OFB_API ~FController();

	//! Tries to fetch new data from the real controller
	/** @return True if new data was available */
	OFB_API const bool Synchronize();

	//! Returns if a connection error occurred
	/** @return True if an error occurred */
	OFB_API virtual const bool GetErrorOccurred() { return false; }

	//! Returns the registered component count
	/** @return The component count */
	inline const int GetComponentCount() const { return ~mlComponents; }

	//! Returns the component at given index
	/** @param [in] Index The component's index 
	  * @return The component or nullptr if the index is invalid */
	inline FRegisteredComponent* GetComponentAt(const int Index) { return mlComponents[Index]; }

	//! Setups a sensor
	/** @param [in] Component The associated component
	  * @param [in] InputType The type of input awaited
	  * @param [in] MajorIndex The major input index (pass 0, reserved for future use)
	  * @param [in] MinorIndex The minor index (the input socket index, i.e. 0..7)
	  * @return None if the registration was successful, otherwise the error code */
	OFB_API const FComponentRegistrationError SetupSensor(FComponent* Component,  const FSensorInputType InputType, const int MajorIndex, const int MinorIndex);

	//! Setups an engine
	/** @param [in] Component The associated component
	* @param [in] MajorIndex The major input index (pass 0, reserved for future use)
	* @param [in] FirstOutput The major output index
	* @param [in] SecondOutput The reverse / secondary output index (-1 if none is available)
	* @param [in] CounterIndex The counter index (-1 if none is available)
	* @return None if the registration was successful, otherwise the error code */
	OFB_API const FComponentRegistrationError SetupEngine(FComponent* Component, const int MajorIndex, const int FirstOutput, const int SecondOutput, const int CounterIndex);

	//! Registers given component
	/** @param [in] Component The component to register */
	OFB_API virtual void RegisterComponent(FComponent* Component);

	//! Removes given component
	/** @param [in] Component The component to be removed */
	OFB_API virtual void RemoveComponent(FComponent* Component);

	// Casts the component to controller
	OFB_API FController* AsController() override { return this; }
};

// ----------------------------------------------------------------------------

#endif // of FController_h
