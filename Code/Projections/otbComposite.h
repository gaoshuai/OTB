/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbComposite_h
#define __otbComposite_h

#include <iostream>
#include <sstream>
#include <stdio.h>
#include "itkTransform.h"
#include "itkMacro.h"
#include "otbMapProjection.h"

namespace otb
{


/** \class Composite

 * \brief Class for switching from a Map Projection coordinates to other Map Projection coordinates.
 * It converts MapProjection1 coordinates to MapProjection2 coordinates by using MapProjection methods.
 * It takes a point in input.
 * (X_1, Y_1) -> (lat, lon) -> (X_2, Y_2)
 * \ingroup Transform 
 */
template <class TInputMapProjection,
					class TOutputMapProjection,
					class TScalarType=double, 
					unsigned int NInputDimensions=2,
					unsigned int NOutputDimensions=2>
class ITK_EXPORT Composite: public itk::Transform<TScalarType,       // Data type for scalars 
																									NInputDimensions,  // Number of dimensions in the input space
																									NOutputDimensions> // Number of dimensions in the output space
{
public :
  

	/** Standard class typedefs */
  typedef itk::Transform< TScalarType,
                  				NInputDimensions,
				                  NOutputDimensions >  				Superclass;
  typedef Composite                            				Self;
  typedef itk::SmartPointer<Self>              				Pointer;
  typedef itk::SmartPointer<const Self>        				ConstPointer;

  typedef TInputMapProjection                  				InputMapProjectionType;
  typedef TOutputMapProjection                 				OutputMapProjectionType;
  typedef itk::Point<TScalarType,NInputDimensions >   InputPointType;
  typedef itk::Point<TScalarType,NOutputDimensions >  OutputPointType;        
	


	/** Method for creation through the object factory. */
	itkNewMacro( Self );
	
	/** Run-time type information (and related methods). */
	itkTypeMacro( Composite, itk::Transform );
	
  itkStaticConstMacro(InputSpaceDimension, unsigned int, NInputDimensions);
  itkStaticConstMacro(OutputSpaceDimension, unsigned int, NOutputDimensions);
  itkStaticConstMacro(SpaceDimension, unsigned int, NInputDimensions);
  itkStaticConstMacro(ParametersDimension, unsigned int,NInputDimensions*(NInputDimensions+1));

	/** Set MapProjection1. */  
	itkSetObjectMacro(InputMapProjection,InputMapProjectionType); 
	
	/** Set MapProjection2. */ 
	itkSetObjectMacro(OutputMapProjection,OutputMapProjectionType);

	/** Compute MapProjection1 coordinates to MapProjection2 coordinates. */ 
	OutputPointType ComputeProjection1ToProjection2(const InputPointType &point1);

	/** Compute MapProjection1 coordinates to MapProjection2 coordinates. */ 
	InputPointType ComputeProjection2ToProjection1(const OutputPointType &point2);

protected:
	Composite();
	~Composite();
	
	TInputMapProjection* m_InputMapProjection;
	TOutputMapProjection* m_OutputMapProjection;  
	
private:
  Composite(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
	
};

} // namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbComposite.txx"
#endif

#endif
