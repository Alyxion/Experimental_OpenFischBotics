#pragma once
#ifndef FVisualizer_h
#define FVisualizer_h

// ----------------------------------------------------------------------------

typedef ASQCanvas FCanvas;

// ----------------------------------------------------------------------------

/** @brief The different visualization sides */
enum class FVisualSide
{
	Front,
	Back,
	Left,
	Right,
	Top,
	Bottom
};

// ----------------------------------------------------------------------------

typedef AMatrix3DD FMatrix3DD;

/** @brief Visualization class, helps visualizing a model in different modes */
class FVisualizer : public FObject
{
protected:
	//! Collects all quadrangles and transforms them recursive
	/** @param [in] Component The current start object
	  * @param [in] Matrix The transformation matrix
	  * @param [in] Quads The quad list
	  * @param [in] Options The visual quality options */
	OFB_API void CollectQuadsRecursive(FComponent* Component, const AMatrix3DD &Matrix, AList<FVisualizedQuad> &Quads, const FVisualQualityOptions &Options);

public:
	//! Constructor
	OFB_API FVisualizer();

	//! Destructor
	OFB_API virtual ~FVisualizer();

	//! Visualizes given component
	/** @param [in] Canvas The target canvas
	  * @param [in] Component The root component
	  * @param [in] Offset The 2D offset
	  * @param [in] Scaling The 2D scaling factor
	  * @param [in] Side The visualization side */
	OFB_API void Visualize(FCanvas* Canvas, FComponent *Component, const FVector Offset, const float Scaling, const FVisualSide Side);
};

// ----------------------------------------------------------------------------

#endif