/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "PipelineProducer.h"
#include "Pin.h"

using namespace plv;

PipelineProducer::PipelineProducer()
{
}

PipelineProducer::~PipelineProducer()
{
}

/** implementation for definition in PipelineElement */
bool PipelineProducer::requiredPinsConnected() const
{
    return true;
}

bool PipelineProducer::__ready( unsigned int& serial )
{
    // serial ignored by producers
    Q_UNUSED(serial)

    return( getState() < PLE_DISPATCHED && this->readyToProduce() );
}

bool PipelineProducer::__process( unsigned int serial )
{
    QMutexLocker lock( &m_pleMutex );

    // TODO remove debug
    //unsigned int processingSerial = getProcessingSerial();
    //QString msg = QString("PipelineProducer: serial: %1 processing serial: %2").arg(serial).arg(processingSerial);
    //qDebug() << msg;

    assert( serial > getProcessingSerial() || serial == 0 );
    //if(!(serial > processingSerial || serial == 0)) {
    //    qDebug() << "HERE!!!!!! ======== <<<<<<<<<<";
    //}

    // set the serial number for this processing run
    setProcessingSerial( serial );

    // call pre processing callback on all output pins
    this->preOutput();

    // do the actual producing
    lock.unlock();
    bool retval = this->produce();
    lock.relock();

    // call post processing callback on all output pins
    this->postOutput();

    lock.unlock();
    if(!retval) setState(PLE_ERROR);

    return retval;
}
