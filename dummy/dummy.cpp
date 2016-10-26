#include "Thread.h"
#include "mbed_debug.h"

namespace rtos {
osStatus Thread::wait(unsigned long a) {
    debug("%s called.\n", __FUNCTION__);
}

Thread::~Thread() {
    debug("%s called.\n", __FUNCTION__);
}

osStatus Thread::start(mbed::Callback<void()> task) {
    debug("%s called.\n", __FUNCTION__);
}

void Thread::constructor(osPriority priority=osPriorityNormal,
                     uint32_t stack_size=DEFAULT_STACK_SIZE,
                     unsigned char *stack_pointer=NULL) {
    debug("%s called.\n", __FUNCTION__);
}
}

/* mbed USBHost Library
 * Copyright (c) 2006-2013 ARM Limited
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

#include "mbed.h"
#include "USBHALHost.h"
#include "dbg.h"

USBHALHost * USBHALHost::instHost;

USBHALHost::USBHALHost() {
}

void USBHALHost::init() {
}

uint32_t USBHALHost::controlHeadED() {
    return 0;
}

uint32_t USBHALHost::bulkHeadED() {
    return 0;
}

uint32_t USBHALHost::interruptHeadED() {
    return 0;
}

void USBHALHost::updateBulkHeadED(uint32_t addr) {
}


void USBHALHost::updateControlHeadED(uint32_t addr) {
}

void USBHALHost::updateInterruptHeadED(uint32_t addr) {
}


void USBHALHost::enableList(ENDPOINT_TYPE type) {
}


bool USBHALHost::disableList(ENDPOINT_TYPE type) {
    return false;
}


void USBHALHost::memInit() {
}

volatile uint8_t * USBHALHost::getED() {
    return NULL; //Could not alloc ED
}

volatile uint8_t * USBHALHost::getTD() {
    return NULL; //Could not alloc TD
}


void USBHALHost::freeED(volatile uint8_t * ed) {
}

void USBHALHost::freeTD(volatile uint8_t * td) {
}


void USBHALHost::resetRootHub() {
}


void USBHALHost::_usbisr(void) {
}

void USBHALHost::UsbIrqhandler() {
}

#include "USBHost.h"
extern "C"
int main() {
    debug("%s called.\r\n", __FUNCTION__);
    auto host = USBHost::getHostInst();
}

// TODO: modify upstream USBHost
extern "C"
void wait_ms(int ms) {
    Thread::wait(ms);
}
