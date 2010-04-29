#include "CameraProducer.h"

#include <QMutexLocker>
#include <QDebug>

#include "OpenCVCamera.h"
#include "Pin.h"

using namespace plv;

#define OUTPUT_PIN_NAME "output"

CameraProducer::CameraProducer() :
        m_camera(new OpenCVCamera()),
        m_lastProcessedId( 0 )
{
    // we have one output pin
    m_outputPin = new TypedOutputPin<OpenCVImage>(OUTPUT_PIN_NAME, this );
    addOutputPin( m_outputPin.getPtr() );

    // connect the camera to this camera producer using Qt's signals
    // and slots mechanism.
    connect( m_camera.getPtr(),
             SIGNAL( newFrame( RefPtr<Data> ) ),
             this,
             SLOT( newFrame( RefPtr<Data> ) ) );
}

CameraProducer::~CameraProducer()
{
}

void CameraProducer::produce()
{
    qDebug() << "produce()";
    QMutexLocker lock(&m_frameMutex);

    if( m_lastFrame.isNotNull() )
    {
        qDebug() << "CP::put";
        m_outputPin->put( m_lastFrame.getPtr() );
        //m_lastProcessedId = m_lastFrame->getId();
    }

//    qDebug() << "Pin type: " << m_outputPin->getTypeInfo().name();
}

void CameraProducer::newFrame( RefPtr<Data> frame )
{
    QMutexLocker lock(&m_frameMutex);
    m_lastFrame = ref_ptr_dynamic_cast<OpenCVImage>( frame );
}

PlvPipelineElementState CameraProducer::init()
{
    if( m_camera->init() != 0 )
    {
        return PLV_PLE_STATE_NOT_READY;
    }

    m_camera->start();

    return PLV_PLE_STATE_READY;
}

PlvPipelineElementState CameraProducer::checkConfig()
{
    if( m_camera->getState() != OpenCVCamera::CAM_RUNNING )
    {
        return PLV_PLE_STATE_NOT_READY;
    }
    return PLV_PLE_STATE_READY;
}

