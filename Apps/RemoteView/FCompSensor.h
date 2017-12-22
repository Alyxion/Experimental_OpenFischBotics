#pragma once
#ifndef FCompSensor_h
#define FCompSensor_h

// ----------------------------------------------------------------------------

/** @brief Enumeration of sensor input types */
enum class FSensorInputType
{
	None,	///< Undefined
	Digital10v,		///< Digital 10 volt input
	Digital5kOhm,	///< Digital 5k ohm input
	Analog10v,		///< Analog 10 volt input
	Analog5kOhm,	///< Analog 5k ohm input
	Ultrasonic		///< Ultra sonic distance input
};

// ----------------------------------------------------------------------------

#define OFB_MAX_CACHED_SENSOR_VALUES 2048 ///< The maximum count of remembered entries

// ----------------------------------------------------------------------------

/** @brief Sensor component base class */
class FCompSensor : public FComponent
{
	FCLASS(FCompSensor, FComponent);

protected:
	FSensorInputType		mInputType;										///< The sensor's input type
	FInt64					maxiUpdateTimes[OFB_MAX_CACHED_SENSOR_VALUES];	///< The tick times of the last updates
	int						maiValues[OFB_MAX_CACHED_SENSOR_VALUES];		///< The most recent values
	int						miCurrentIndex = 0;								///< The current write index for the circle buffer
	int						miValidValues = 0;								///< Count of valid values
	int						miValueMinimum = 0,								///< Minimum valid sensor value
							miValueMaximum = 2000000000;					///< Maximum valid sensor value

public:
	//! Constructor
	/** @param [in] Parent The parent component
	  * @param [in] InputType The awaited sensor input data type */
	OFB_API FCompSensor(FComponent* Parent, const FSensorInputType InputType);

	//! Destructor
	OFB_API ~FCompSensor();

	//! Returns the sensor's input type
	/** @return The input type */
	inline const FSensorInputType GetInputType() const { return mInputType; }

	//! Sets the sensors valid value range
	/** @param [in] Min Minimum valid value
	  * @param [in] Max Maximum value value */
	OFB_API void SetValueRange(const int Min, const int Max);

	//! Connects the sensor to given controller and input port
	/** @param [in] Controller The controller providing the data
	  * @param [in] MajorPort The major port (reserved for future use, pass 0)
	  * @param [in] MinorPort The minor port (0..7) */
	OFB_API void Connect(FController *Controller, const int MajorPort, const int MinorPort);

	// Casts the object to sensor
	OFB_API FCompSensor* AsSensor() override { return this; }

	//! Adds new data to this sensor
	/** @param [in] Data The new sensor data
	  * @param [in] TimeStamps The time stamp for each passed value */
	OFB_API virtual void UpdateData(const FList<int> &Data, const FInt64 *TimeStamps);

	//! Returns the sensor's values
	/** @param [in] Maximum The maximum count of values to receive
	  * @param [in][out] List The list in which the values shall be stored
	  * @param [in][out] TimeStamps The time stamp history */
	OFB_API void GetValues(const int Maximum, FList<int> &List, FList<FInt64> &TimeStamps);
};

// ----------------------------------------------------------------------------

#endif // of FCompSensor_h
