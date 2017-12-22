#pragma once
#ifndef FVisualElement_h
#define FVisualElement_h

// ----------------------------------------------------------------------------

/** @brief Enumeration of visual element tiles */
enum class FVisualElementType
{
	None,		///< Undefined
	Quad,		///< A flat quad
	Box			///< A box
};

// ----------------------------------------------------------------------------

/** @brief Defines the visualization quality */
class FVisualQualityOptions
{

};

// ----------------------------------------------------------------------------

/** @brief Defines a single visualized quad */
class FVisualizedQuad
{
public:
	FVector			mavEdges[4];				///< The quad's edges
	FColor			mclColor;					///< The quad's color	
	bool			mbPreferWireframe = false;	///< Defines if wire framed visualization is preferred

	//! Constructor
	/** @param [in] A The quad's first point
	* @param [in] B The quad's second point
	* @param [in] C The quad's third point
	* @param [in] D The quad's fourth point
	* @param [in] Color The quad's color */
	inline FVisualizedQuad(const FVector &A, const FVector &B, const FVector &C, const FVector &D, const FColor &Color)
	{
		mavEdges[0] = A;
		mavEdges[1] = B;
		mavEdges[2] = C;
		mavEdges[3] = D;
		mclColor = Color;
	}

	//! Constructor
	/** @param [in] Points The quad's points
	  * @param [in] Color The quad's color  */
	inline FVisualizedQuad(const FVector* Points, const FColor &Color)
	{
		mavEdges[0] = Points[0];
		mavEdges[1] = Points[1];
		mavEdges[2] = Points[2];
		mavEdges[3] = Points[3];
		mclColor = Color;
	}
};

// ----------------------------------------------------------------------------

/** @brief Visual element base class */
class FVisualElement
{
protected:
	FVisualElementType	mType;				///< The element's type
	FColor				mclPreferredColor;	///< The preferred color

public:
	//! Constructor
	/** @param [in] Type The element's type */
	OFB_API FVisualElement(const FVisualElementType Type);

	//! Destructor
	OFB_API virtual ~FVisualElement();

	//! Returns the element's type
	/** @return The type */
	OFB_API const FVisualElementType GetType() const;

	//! Requests to fill the passed data array with all quads provided by this element
	/** @param [in][out] Quads The quad array
	* @param [in] Options The visual quality options */
	OFB_API virtual void GetQuads(FList<FVisualizedQuad> &Quads, const FVisualQualityOptions &Options) const = NULL;
};

// ----------------------------------------------------------------------------

/** @brief Defines a component's (flat) bounding quad */
class FQuadElement : public FVisualElement
{
public:
	FVector		mavEdges[4];		///< The quad's edges

	//! Constructs the quad of four points
	/** @param [in] A The quad's first point
	  * @param [in] B The quad's second point
	  * @param [in] C The quad's third point
	  * @param [in] D The quad's fourth point
	  * @param [in] Color The color */
	OFB_API FQuadElement(const FVector &A, const FVector &B, const FVector &C, const FVector &D, const FColor &Color);

	//! Destructor
	OFB_API ~FQuadElement();

	// Requests to fill the passed data array with all quads provided by this element
	OFB_API void GetQuads(FList<FVisualizedQuad> &Quads, const FVisualQualityOptions &Options) const override;
};

// ----------------------------------------------------------------------------

/** @brief Defines a box */
class FBoxElement : public FVisualElement
{
public:
	FVector		mvOrigin;			///< The box origin
	FVector		mvSize;				///< The size (width, height, depth)

	//! Constructor
	/** @param [in] Origin The box origin
	  * @param [in] Size The box size
	  * @param [in] Color The color */
	OFB_API FBoxElement(const FVector &Origin, const FVector &Size, const FColor &Color);

	//! Destructor
	OFB_API ~FBoxElement();

	// Requests to fill the passed data array with all quads provided by this element
	OFB_API void GetQuads(FList<FVisualizedQuad> &Quads, const FVisualQualityOptions &Options) const override;
};

// ----------------------------------------------------------------------------

#endif	// of FVisualElement_h