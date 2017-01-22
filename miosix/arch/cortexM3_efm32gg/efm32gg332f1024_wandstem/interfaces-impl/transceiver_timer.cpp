/***************************************************************************
 *   Copyright (C) 2016 by Fabiano Riccardi                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include "transceiver_timer.h"
#include "gpioirq.h"

using namespace miosix;

//transceiver::excChB	usato per la ricezione INPUT_CAPTURE TIMER2_CC0 -> PA8
//transceiver::stxon	usato per attivare la trasmissione OUTPUTCOMPARE TIMER2_CC1 -> PA9

long long TransceiverTimer::getValue() const{
    FastInterruptDisableLock dLock;
    return b.IRQgetCurrentTick();
}

void TransceiverTimer::wait(long long tick){
    Thread::nanoSleep(tc.tick2ns(tick));
}

bool TransceiverTimer::absoluteWait(long long tick){
    if(b.getCurrentTick()>=tick){
	return true;
    }
    Thread::nanoSleepUntil(tc.tick2ns(tick));
    return false;
}

bool TransceiverTimer::absoluteWaitTrigger(long long tick){
    FastInterruptDisableLock dLock;
    
    b.setModeTransceiverTimer(false);
    if(b.IRQsetNextTransceiverInterrupt(tick)==WaitResult::WAKEUP_IN_THE_PAST){
	return true;
    }
    
    b.IRQtransceiverWait(tick,&dLock);
    return false;
}

bool TransceiverTimer::absoluteWaitTimeoutOrEvent(long long tick){
    FastInterruptDisableLock dLock;
    if(b.IRQsetTransceiverTimeout(tick)==WaitResult::WAKEUP_IN_THE_PAST){
        return true;
    }
    b.setModeTransceiverTimer(true);
    b.cleanBufferTrasceiver();
    b.enableCC0Interrupt(false);
    b.enableCC0InterruptTim2(true);
    
    Thread* tWaiting=b.IRQtransceiverWait(tick,&dLock);
    
    if(tWaiting==nullptr){
	return false;
    }else{
	return true;
    }
}

bool TransceiverTimer::waitTimeoutOrEvent(long long tick){
    return absoluteWaitTimeoutOrEvent(b.getCurrentTick()+tick);
}

long long TransceiverTimer::tick2ns(long long tick){
    return tc.tick2ns(tick);
}

long long TransceiverTimer::ns2tick(long long ns){
    return tc.ns2tick(ns);
}
            
unsigned int TransceiverTimer::getTickFrequency() const{
    return b.getTimerFrequency();
}
	    
long long TransceiverTimer::getExtEventTimestamp() const{
    return b.IRQgetSetTimeTransceiver()-stabilizingTime;
}
	 
TransceiverTimer::TransceiverTimer():b(HighResolutionTimerBase::instance()),tc(b.getTimerFrequency()) {
    registerGpioIrq(transceiver::excChB::getPin(),GpioIrqEdge::RISING,[](){});
}

TransceiverTimer& TransceiverTimer::instance(){
    static TransceiverTimer instance;
    return instance;
}

TransceiverTimer::~TransceiverTimer() {}

/// This parameter was obtained by connecting an output compare to an input
/// capture channel and computing the difference between the expected and
/// captured value. 
///
/// It is believed that it is caused by the internal flip-flop
/// in the input capture stage for resynchronizing the asynchronous input and
/// prevent metastability. The test has also been done on multiple boards.
/// The only open issue is whether this delay of 3 ticks is all at the input
/// capture stage or some of those ticks are in the output compare.
const int TransceiverTimer::stabilizingTime=3;