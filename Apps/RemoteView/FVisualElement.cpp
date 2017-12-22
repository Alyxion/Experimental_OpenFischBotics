#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FVisualElement::FVisualElement(const FVisualElementType Type) :
	mType(Type),
	mclPreferredColor(1.f, 1.f, 1.f, 1.f)
{

}

// ............................................................................

// Destructor
FVisualElement::~FVisualElement()
{

}

// ............................................................................

// Returns the element's type
const FVisualElementType FVisualElement::GetType() const
{
	return mType;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Constructs the quad of four points
FQuadElement::FQuadElement(const FVector &A, const FVector &B, const FVector &C, const FVector &D, const FColor &Color) :
	FVisualElement(FVisualElementType::Quad)
{
	mavEdges[0] = A;
	mavEdges[1] = B;
	mavEdges[2] = C;
	mavEdges[3] = D;
	mclPreferredColor = Color;
}

// ............................................................................

// Destructor
FQuadElement::~FQuadElement()
{

}

// ............................................................................

// Requests to fill the passed data array with all quads provided by this element
void FQuadElement::GetQuads(FList<FVisualizedQuad> &Quads, const FVisualQualityOptions &Options) const
{
	Quads.Add(FVisualizedQuad(mavEdges, mclPreferredColor));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Constructor
FBoxElement::FBoxElement(const FVector &Origin, const FVector &Size, const FColor &Color) :
	FVisualElement(FVisualElementType::Box),
	mvOrigin(Origin),
	mvSize(Size)
{
	mclPreferredColor = Color;
}

// ............................................................................

// Destructor
FBoxElement::~FBoxElement()
{

}

// ............................................................................

// Requests to fill the passed data array with all quads provided by this element
void FBoxElement::GetQuads(FList<FVisualizedQuad> &Quads, const FVisualQualityOptions &Options) const
{
	double	halfWidth = mvSize.mX / 2.0,
			halfHeight = mvSize.mY / 2.0,
			halfDepth = mvSize.mZ / 2.0;

	FVector frontLT(mvOrigin.mX - halfWidth, mvOrigin.mY + halfHeight, mvOrigin.mZ + halfDepth),	// left top
			frontRT(mvOrigin.mX + halfWidth, mvOrigin.mY + halfHeight, mvOrigin.mZ + halfDepth),	// right top
			frontLB(mvOrigin.mX - halfWidth, mvOrigin.mY - halfHeight, mvOrigin.mZ + halfDepth),	// left bottom
			frontRB(mvOrigin.mX + halfWidth, mvOrigin.mY - halfHeight, mvOrigin.mZ + halfDepth),	// right bottom
			backLT(mvOrigin.mX - halfWidth, mvOrigin.mY + halfHeight, mvOrigin.mZ - halfDepth),
			backRT(mvOrigin.mX + halfWidth, mvOrigin.mY + halfHeight, mvOrigin.mZ - halfDepth),
			backLB(mvOrigin.mX - halfWidth, mvOrigin.mY - halfHeight, mvOrigin.mZ - halfDepth),
			backRB(mvOrigin.mX + halfWidth, mvOrigin.mY - halfHeight, mvOrigin.mZ - halfDepth);

	// add the boxes 6 sides
	Quads.Reserve(6);
	Quads.Add(FVisualizedQuad(frontLT, frontRT, frontRB, frontLB, mclPreferredColor));	// front
	Quads.Add(FVisualizedQuad(backLB, backRB, backRT, backLT, mclPreferredColor));		// back
	Quads.Add(FVisualizedQuad(backLT, frontLT, frontLB, backLB, mclPreferredColor));	// left
	Quads.Add(FVisualizedQuad(frontRT, backRT, backRB, frontRB, mclPreferredColor));	// right
	Quads.Add(FVisualizedQuad(backLT, backRT, frontRT, frontLT, mclPreferredColor));	// top
	Quads.Add(FVisualizedQuad(frontLB, frontRB, backRB, backLB, mclPreferredColor));	// bottom
}
