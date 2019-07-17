/******************************************************************************
*
* Copyright (C) 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMANGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*
*
******************************************************************************/

/*
 * CONTENT
 * File is specific for each PU instance and must exist in order to
 * port Power Management code for some new PU.
 * Contains PU specific macros and macros to be defined depending on
 * the execution environment.
 */

#ifndef PM_CLIENT_H
#define PM_CLIENT_H

#include "xil_exception.h"
#include "xil_io.h"
#include "pm_apu.h"
#include "pm_defs.h"
#include "pm_common.h"

<<<<<<< HEAD
<<<<<<< HEAD
#ifdef __cplusplus
extern "C" {
#endif
=======

>>>>>>> Xilpm: Fixed MISRA-C:2012 R.2.5 in APU pm_client.h

#define pm_print(MSG, ...)	xil_printf("APU: "MSG,##__VA_ARGS__)

<<<<<<< HEAD
=======
#ifdef __cplusplus
extern "C" {
#endif

#define pm_print(MSG, ...)	xil_printf("APU: "MSG,##__VA_ARGS__)

>>>>>>> xilpm: Extern C added to header files
#ifdef __cplusplus
}
#endif

<<<<<<< HEAD
=======
>>>>>>> Xilpm: Fixed MISRA-C:2012 R.21.1, R.21.2 in APU pm_client.h
=======
>>>>>>> xilpm: Extern C added to header files
#endif /* PM_CLIENT_H */