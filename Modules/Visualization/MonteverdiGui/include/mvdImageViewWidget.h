/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef mvdImageViewWidget_h
#define mvdImageViewWidget_h

//
// Configuration include.
//// Included at first position before any other ones.
#include "ConfigureMonteverdi.h"

/*****************************************************************************/
/* INCLUDE SECTION                                                           */

//
// Qt includes (sorted by alphabetic order)
#include <QtOpenGL>

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)
#include "OTBMonteverdiGUIExport.h"
//
// Monteverdi includes (sorted by alphabetic order)
#include "mvdCore.h"
#include "mvdAbstractImageViewRenderer.h"
#include "mvdGui.h"


#define USE_XP_REGION_OPTIM 0

/*****************************************************************************/
/* PRE-DECLARATION SECTION                                                   */

//
// External classes pre-declaration.
namespace
{
}

namespace mvd
{
//
// Internal classes pre-declaration.
class AbstractImageViewManipulator;


/*****************************************************************************/
/* CLASS DEFINITION SECTION                                                  */

/** \class ImageViewWidget
 *
 * \ingroup OTBMonteverdiGUI
 *
 */
class OTBMonteverdiGUI_EXPORT ImageViewWidget :
    public QGLWidget
{

  /*-[ QOBJECT SECTION ]-----------------------------------------------------*/

  Q_OBJECT;

  /*-[ PUBLIC SECTION ]------------------------------------------------------*/

//
// Public types.
public:

//
// Public methods.
public:

  /** \brief Constructor. */
  ImageViewWidget( AbstractImageViewManipulator* manipulator,
                   AbstractImageViewRenderer* renderer,
                   QWidget* p =NULL,
                   const QGLWidget* shareWidget =NULL,
                   Qt::WindowFlags f =0 );

  /** \brief Constructor. */
  ImageViewWidget( AbstractImageViewManipulator* manipulator,
                   AbstractImageViewRenderer* renderer,
                   QGLContext* context,
                   QWidget* p =NULL,
                   const QGLWidget* shareWidget =NULL,
                   Qt::WindowFlags f =0 );

  /** \brief Constructor. */
  ImageViewWidget( AbstractImageViewManipulator* manipulator,
                   AbstractImageViewRenderer* renderer,
                   const QGLFormat& format,
                   QWidget* p =NULL,
                   const QGLWidget *shareWidget =NULL,
                   Qt::WindowFlags f =0 );

  /** \brief Destructor. */
  ~ImageViewWidget() ITK_OVERRIDE;

  /**
   */
  void SetLayerStack( StackedLayerModel * stackedLayerModel,
                      const PointType& center,
                      double scale );

  /**
   */
  void SetLayerStack( StackedLayerModel * );

  /**
   */
  inline const StackedLayerModel * GetLayerStack() const;

  /**
   */
  inline StackedLayerModel * GetLayerStack();

  /**
   * \brief Access the abstract view-manipualtor of this image-view.
   *
   * \return A pointer to the abstract view-manipulator of this image-view.
   */
  inline const AbstractImageViewManipulator* GetManipulator() const;

  /**
   * \brief Access the abstract model-renderer of this image-view.
   *
   * \return A pointer to the abstract model-renderer of this image-view.
   */
  inline const AbstractImageViewRenderer * GetRenderer() const;
  inline AbstractImageViewRenderer * GetRenderer();

  /**
   */
  bool SetBypassRenderingEnabled( bool );
  /**
   */
  bool IsBypassRenderingEnabled() const;
  /**
   */
  void SetPickingEnabled( bool );

  /**
   * This allows to set the fallback behaviour for picking
   */
  void SetPickingDefaultStatus( bool );
  /**
   */
  bool IsPickingEnabled() const;

  /*-[ PUBLIC SLOTS SECTION ]------------------------------------------------*/

//
// Public SLOTS.
public slots:
  /**
   */
  void UpdateScene();
  /**
   */
  void CenterOnSelected( const IndexType& index );
  /**
   */
  void CenterOn( const PointType& center, double scale );
  /**
   */
  void SaveScreenshot( bool );
  /**
   */
  void ZoomToExtent();
  /**
   */
  void ZoomToFullResolution();
  /**
   */
  void ZoomToLayerExtent();
  /**
   */
  void ZoomIn();
  /**
   */
  void ZoomOut();

  /*-[ SIGNALS SECTION ]-----------------------------------------------------*/

//
// SIGNALS.
signals:
  /**
   */
  void PhysicalCursorPositionChanged( const QPoint & screen,
                                      const PointType & view,
                                      const PointType & physical,
                                      const DefaultImageType::PixelType & );

  /**
   */
  void PixelInfoChanged( const QPoint & screen,
			 const PointType & view,
			 const PixelInfo::Vector & pixels );

  /**
   */
  void ReferenceActorShaderModeChanged(const std::string & mode);
  /**
   */
  void CenterRoiRequested( const PointType& center );
  /**
   */
  void RoiChanged( const PointType& origin,
                   const SizeType& size,
                   const SpacingType& spacing,
                   const PointType& center );
  /**
   */
  void RoiChanged( const PointType& center, double sx, double sy );
  /**
   */
  void CenterChanged( const PointType& center );
  /**
   */
  void ScaleChanged( double sx, double sy );
  /**
   */
  void ModelUpdated();

  /*-[ PROTECTED SECTION ]---------------------------------------------------*/

//
// Protected methods.
protected:

  //
  // Qt overloads.

  void initializeGL() ITK_OVERRIDE;
  void resizeGL( int widgth, int height ) ITK_OVERRIDE;
  void paintGL() ITK_OVERRIDE;

  void mouseMoveEvent( QMouseEvent* event ) ITK_OVERRIDE;
  void mouseReleaseEvent( QMouseEvent* event ) ITK_OVERRIDE;
  void mousePressEvent( QMouseEvent* event ) ITK_OVERRIDE;
  void mouseDoubleClickEvent( QMouseEvent * event ) ITK_OVERRIDE;

  void wheelEvent( QWheelEvent* event) ITK_OVERRIDE;

  void keyPressEvent( QKeyEvent* event ) ITK_OVERRIDE;
  void keyReleaseEvent( QKeyEvent* event ) ITK_OVERRIDE;

  void resizeEvent( QResizeEvent* event ) ITK_OVERRIDE;


//
// Protected attributes.
protected:
  /**
   */
  bool m_IsPickingEnabled : 1;
  bool m_PickingDefaultStatus : 1;

  /*-[ PRIVATE SECTION ]-----------------------------------------------------*/

//
// Private types.
private:

//
// Private methods.
private:

  /** Construction code (factorizes constructors initializations). */
  void Initialize( AbstractImageViewManipulator* manipulator,
                   AbstractImageViewRenderer* renderer );

  /**
   */
  void ListGlVersions() const;

  /**
   */
  void Center( const PointType& point, double sx, double sy );

  /**
   */
  void Center( ZoomType zoom =ZOOM_TYPE_NONE );

  /**
   */
  void Connect( AbstractLayerModel * );

  /**
   */
  void Disconnect( AbstractLayerModel * );

  /**
   */
  bool ApplyFixedZoomType();

//
// Private attributes.
private:
  /** Event handler pointer */
  AbstractImageViewManipulator* m_Manipulator;

  /** Model Renderer pointer */
  AbstractImageViewRenderer* m_Renderer;

  /** */
#if USE_XP_REGION_OPTIM
  PointType m_Position;
#endif // USE_XP_REGION_OPTIM

  /*-[ PRIVATE SLOTS SECTION ]-----------------------------------------------*/
private slots:
  /**
   */
  void OnRoiChanged( const PointType&,
                     const SizeType&,
                     const SpacingType&,
                     const PointType& );
  /**
   */
  void OnSelectFirstLayerRequested();
  /**
   */
  void OnSelectLastLayerRequested();
  /**
   */
  void OnSelectPreviousLayerRequested();
  /**
   */
  void OnSelectNextLayerRequested();
  /**
   */
  void OnLowerLayerRequested();
  /**
   */
  void OnRaiseLayerRequested();
  /**
   */
  void OnDeleteAllRequested();
  /**
   */
  void OnDeleteSelectedRequested();
  /**
   */
  void OnReferenceChanged( size_t );
  /**
   */
  void OnClearProjectionRequired();
  /**
   */
  void OnSetProjectionRequired();
  /**
   */
  void OnUpdateProjectionRequired();
  /**
   */
  void OnLayerAdded( size_t );
  /**
   */
  void OnLayerAboutToBeDeleted( size_t );
  /**
   */
  void OnToggleLayerVisibilityRequested( bool );
  /**
   */
  void OnResizeShaderRequested( double );
  /**
   */
  void OnReparamShaderRequested( double );
  /**
   */
  void OnShiftAlphaRequested( double );
  /**
   */
  void OnUpdateGammaRequested( double );
  /**
   */
  void OnShiftDynamicsRequested( double );
  /**
   */
  void OnScaleDynamicsRequested( double );
  /**
   */
  void OnResetQuantilesRequested( bool );
  /**
   */
  void OnShaderEffectRequested( Effect );
  /**
   */
  void OnApplyAllRequested();
  void OnResetEffectsRequested();
  /**
   */
  void OnSetReferenceRequested();
  /**
   */
  void OnContentChanged();
  /**
   */
  void OnContentReset();
  /**
   */
  void OnRefreshViewRequested();
  /**
   */
  void OnResetViewport();
};

}// end namespace 'mvd'

/*****************************************************************************/
/* INLINE SECTION                                                            */

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)

//
// Monteverdi includes (sorted by alphabetic order)
#include "mvdHistogramModel.h"
#include "mvdAbstractImageViewManipulator.h"

namespace mvd
{

/*****************************************************************************/
inline
const AbstractImageViewManipulator*
ImageViewWidget
::GetManipulator() const
{
  return m_Manipulator;
}

/*****************************************************************************/
inline
const AbstractImageViewRenderer*
ImageViewWidget
::GetRenderer() const
{
  return m_Renderer;
}

/*****************************************************************************/
inline
AbstractImageViewRenderer*
ImageViewWidget
::GetRenderer()
{
  return m_Renderer;
}

/*****************************************************************************/
inline
const StackedLayerModel *
ImageViewWidget
::GetLayerStack() const
{
  assert( m_Renderer!=NULL );

  return m_Renderer->GetLayerStack();
}

/*****************************************************************************/
inline
StackedLayerModel *
ImageViewWidget
::GetLayerStack()
{
  assert( m_Renderer!=NULL );

  return m_Renderer->GetLayerStack();
}

} // End of namespace 'mvd'

#endif // mvdImageViewWidget_h
