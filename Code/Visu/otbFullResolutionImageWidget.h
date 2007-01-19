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
#ifndef _otbFullResolutionImageWidget_h
#define _otbFullResolutionImageWidget_h

#include "otbImageWidgetBase.h"

namespace otb
{
/** \class FullResolutionImageWidget
 * \brief 
 *
 */
template <class TPixel>
class FullResolutionImageWidget
  : public ImageWidgetBase<TPixel>
{
 public:
  /** Standard class typedefs */
  typedef FullResolutionImageWidget Self;
  typedef ImageWidgetBase<TPixel> Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  
  /** Method for creation through the object factory */
  itkNewMacro(Self);
  
  /** Runtime information */
  itkTypeMacro(FullResolutionImageWidget,ImageWidgetBase);

  /** Superclass typedefs */
  typedef typename Superclass::SizeType SizeType;
  typedef typename Superclass::IndexType IndexType;
  typedef typename Superclass::RegionType RegionType;

  /** Unlarge OpenGlBuffer */
  virtual void UpdateOpenGlBufferedRegion(void);
  /** Test if the buffer has to be enlarged */
  virtual bool UpdateOpenGlBufferedRegionRequested(void);
  /** Unlarge OpenGlBuffer for image overlay */
  virtual void UpdateOpenGlImageOverlayBufferedRegion(void);
  /** Test if the buffer has to be enlarged */
  virtual bool UpdateOpenGlImageOverlayBufferedRegionRequested(void);

  /** Initialize the widget */
  virtual void Init(int x, int y, int w, int h, const char * l);
  /** Resize the widget */
  virtual void resize(int x, int y, int w, int h);

  /**
   * Set upper right corner position in image.
   * \param index the upper right corner index.
   */
  void SetUpperRightCorner(IndexType index);
  itkGetMacro(UpperRightCorner,IndexType);

 protected:
  /** Constructor. */
    FullResolutionImageWidget();
    /** Destructor. */
    ~FullResolutionImageWidget();

 private:
    FullResolutionImageWidget(const Self&);// purposely not implemented
    void operator=(const Self&);// purposely not implemented

    IndexType m_UpperRightCorner;
};
} // end namespace otb
#ifndef OTB_MANUAL_INSTANTIATION
#include "otbFullResolutionImageWidget.txx"
#endif

#endif
