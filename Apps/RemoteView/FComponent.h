#pragma once
#ifndef FComponent_h
#define FComponent_h

// ----------------------------------------------------------------------------

class FComponent;
class FVisualElement;
class FController;
class FCompSensor;
class FCompEngine;

// ----------------------------------------------------------------------------

#define FCLASS ACLASS

typedef AInt64 FInt64;
typedef AObject FObject;
typedef AColor FColor;
typedef AString FString;
typedef AVector3DD FVector;
typedef ASQPoint FPoint;
#define FList AList
#define FDelete ADelete
#define FNew(X) ANew(X)
#define FUNUSED_ALWAYS(X) UNUSED_ALWAYS(X)
#define FLock(X) ALock(X)

typedef FList<FComponent*> FComponentList;

#define OFB_API

// ----------------------------------------------------------------------------

/** @brief Enumeration of major component types */
enum class FComponentType
{
	None,		///< Undefined
	Common,		///< A common component or brick
	Engine,		///< An engine
	Sensor,		///< A sensor
	Controller	///< A gear
};

// ----------------------------------------------------------------------------

/** @brief Defines a single model component */
class FComponent : public FObject
{
	FCLASS(FComponent, FObject);

private:
	FComponentType			mType = FComponentType::None;		///< The component's type
	FController				*mlpController = nullptr;			///< The component's associated controller
	FComponent				*mlpParent = nullptr;				///< The parent component
	FList<FComponent*>		mlChildren;							///< The child components

	FList<FVisualElement*>	mlVisualElements;					///< The list of visual elements

	FString					msDescription;						///< The component's description
	FVector					mvPosition = { 0.0, 0.0, 0.0 };		///< The position, relative to it's parent component
	FVector					mvRotation = { 0.0, 0.0, 0.0 };		///< The rotation around xyz, relative to it's parent component

protected:
	//! Registers a new child component
	/** @param [in] Child The new child component */
	OFB_API void AddChild(FComponent* Child);

	//! Removes a child component
	/** @param [in] Child The child component to be removed */
	OFB_API void RemoveChild(FComponent* Child);

	//! Clears the list of visual elements
	OFB_API void ClearVisualElements();

	//! Adds a visual element
	/** @param [in] Element The new element */
	OFB_API void AddVisualElement(FVisualElement* Element);

	//! Removes the controller (for example because it's destroyed)
	OFB_API void _RemoveControllerInt();

public:
	//! Constructor
	/** @param [in] Parent The parent component 
	  * @param [in][opt] Type The component's type */
	OFB_API FComponent(FComponent *Parent, const FComponentType Type = FComponentType::Common);

	//! Destructor
	OFB_API virtual ~FComponent();

	//! Returns the component's description name
	/** @return The component's description */
	OFB_API virtual const FString GetDescription();

	//! Returns the component's status
	/** @return The component's status string */
	OFB_API virtual const FString GetStatus();

	//! Returns the component's type
	/** @return The component's major type */
	inline const FComponentType GetType() const { return mType; }

	//! Returns the components parent component
	/** @return The parent component */
	OFB_API FComponent* GetParent() const;

	//! Sets a new parent
	/** @param [in] Parent The new parent component */
	OFB_API virtual void SetParent(FComponent* Parent);

	//! Returns the component's controller
	/** @return The controller */
	OFB_API FController* GetController() const;

	//! Sets the component's controller
	/** @param [in] Controller The new controller */
	OFB_API virtual void SetController(FController* Controller);

	//! Returns the count of child components
	/** @return The count */
	OFB_API const int GetChildCount() const;

	//! Returns the child at given index
	/** @param [in] Index The index
	* @return The child component object if Index is within range. nullptr otherwise */
	OFB_API FComponent* GetChildAt(const int Index) const;

	//! Changes the enabling state
	/** @param [in] State The new state */
	OFB_API virtual void SetEnabled(const bool State) { FUNUSED_ALWAYS(State); }

	//! Returns the enabling state. Note that this may return false even if SetEnabled has been called before as it returns the real current state
	/** @return The current state */
	OFB_API virtual const bool GetEnabled() { return false; }

	//! Returns the position
	/** @return The position */
	OFB_API const FVector& GetPosition() const;

	//! Returns the rotation
	/** @return The rotation */
	OFB_API const FVector& GetRotation() const;

	//! Sets a new position and rotation
	/** @param [in] Position The new position
	  * @param [in] Rotation The new rotation */
	OFB_API virtual void SetPosition(const FVector &Position, const FVector &Rotation);

	//! Sets a new position
	/** @param [in] Position The new position */	
	OFB_API void SetPosition(const FVector &Position);

	//! Sets a new rotation
	/** @param [in] Rotation The new rotation */
	OFB_API void SetRotation(const FVector &Rotation);

	//! Returns the count of visual elements
	/** @return The visual element count */
	OFB_API const int GetVisualElementCount() const;

	//! Returns the visual element at given index
	/** @return The visual element at given index if Index is within range. nullptr otherwise */
	OFB_API const FVisualElement* GetVisualElementAt(const int Index) const;

	//! Tries to cast this component to an engine
	/** @return The engine object or nullptr if this object is not an engine */
	OFB_API virtual FCompEngine* AsEngine() { return nullptr; }

	//! Tries to cast this component to a sensor
	/** @return The sensor object or nullptr if this object is not a sensor */
	OFB_API virtual FCompSensor* AsSensor() { return nullptr; }

	//! Tries to cast this component to a controller
	/** @return The controller object or nullptr if this object is not a controller */
	OFB_API virtual FController* AsController() { return nullptr; }
};

// ----------------------------------------------------------------------------

#endif // of FComponent