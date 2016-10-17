/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   radio_timer.h
 * Author: fabiuz
 *
 * Created on October 11, 2016, 11:17 AM
 */

#include "high_resolution_timer_base.h"
#include "timer.h"

#ifndef RADIO_TIMER_H
#define RADIO_TIMER_H

namespace miosix{
    class RadioTimer : public TimerInterface {
        public:
            //transceiver::excChB //usato per la ricezione INPUT_CAPTURE
            //transceiver::stxon //usato per attivare la trasmissione OUTPUTCOMPARE
            static Thread *tWaitingRadio;
            static RadioTimer& instance();
            virtual ~RadioTimer();
            
            long long getValue() const;   
            
            void wait(long long tick);         
            bool absoluteWait(long long tick); 
            
            
            bool absoluteWaitTrigger(long long tick);
            
            bool waitTimeoutOrEvent(long long tick);
            bool absoluteWaitTimeoutOrEvent(long long tick);

            long long tick2ns(long long tick);
            long long ns2tick(long long ns);
            
            unsigned int getTickFrequency() const;
            
            long long getExtEventTimestamp() const;
            

        private:
            RadioTimer();
            HighResolutionTimerBase& b;
            TimeConversion tc;
    };
}



#endif /* RADIO_TIMER_H */

