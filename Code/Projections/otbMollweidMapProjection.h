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
#ifndef __otbMollweidMapProjection_h
#define __otbMollweidMapProjection_h

#include "projection/ossimMapProjection.h"
#include "projection/ossimMollweidProjection.h"
#include "otbMapProjection.h"

namespace otb
{

class ITK_EXPORT MollweidMapProjection : public MapProjection<ossimMollweidProjection> 
{
public :

	/** Standard class typedefs. */
	typedef MollweidMapProjection                                 Self;
  typedef MapProjection<ossimMollweidProjection>                Superclass;
  typedef itk::SmartPointer<Self>                    	      	  Pointer;
  typedef itk::SmartPointer<const Self>              	      	  ConstPointer;

	typedef Superclass::ScalarType  															ScalarType;
	typedef itk::Point<ScalarType,2 >   	      									InputPointType;
	typedef itk::Point<ScalarType,2 >  	      										OutputPointType;      

	/** Method for creation through the object factory. */
	itkNewMacro(Self);

	/** Run-time type information (and related methods). */
	itkTypeMacro( MollweidMapProjection, MapProjection );

	void SetFalseEasting(double falseEasting);

	void SetFalseNorthing(double falseNorthing);

	double GetFalseNorthing() const;

	double GetFalseEasting() const;

	void SetDefaults();

protected:

	MollweidMapProjection(); 
	virtual ~MollweidMapProjection();

	ossimMollweidProjection* m_MollweidProjection;

private :
	
  MollweidMapProjection(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
	
};

} // namespace otb


#endif
