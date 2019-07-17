/******************************************************************************
*
* Copyright (C) 2019 Xilinx, Inc.  All rights reserved.
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

#include "xpm_common.h"
#include "xpm_psfpdomain.h"
#include "xpm_bisr.h"
#include "xpm_regs.h"

static XStatus FpdInitStart(u32 *Args, u32 NumOfArgs)
{
	XStatus Status = XST_SUCCESS;
	u32 Payload[PAYLOAD_ARG_CNT] = {0};

	(void)Args;
	(void)NumOfArgs;

	/* Check vccint_fpd first to make sure power is on */
	if (XST_SUCCESS != XPmPower_CheckPower(PMC_GLOBAL_PWR_SUPPLY_STATUS_VCCINT_FPD_MASK)) {
		/* TODO: Request PMC to power up VCCINT_FP rail and wait for the acknowledgement.*/
		goto done;
	}

	Payload[0] = PSM_API_FPD_HOUSECLEAN;
	Payload[1] = FUNC_INIT_START;

	Status = XPm_IpiSend(PSM_IPI_INT_MASK, Payload);
	if (XST_SUCCESS != Status) {
		goto done;
	}

	Status = XPm_IpiReadStatus(PSM_IPI_INT_MASK);
	if (XST_SUCCESS != Status) {
		goto done;
	}
	/* Release POR for PS-FPD */
	Status = XPmReset_AssertbyId(POR_RSTID(XPM_NODEIDX_RST_FPD_POR),
				     PM_RESET_ACTION_RELEASE);
done:
	return Status;
}

static XStatus FpdInitFinish(u32 *Args, u32 NumOfArgs)
{
	XStatus Status = XST_SUCCESS;

	(void)Args;
	(void)NumOfArgs;

	return Status;
}

static XStatus FpdHcComplete(u32 *Args, u32 NumOfArgs)
{
	XStatus Status = XST_SUCCESS;
	u32 Payload[PAYLOAD_ARG_CNT] = {0};

	(void)Args;
	(void)NumOfArgs;

	/* Release SRST for PS-FPD - in case Bisr and Mbist are skipped */
	Status = XPmReset_AssertbyId(SRST_RSTID(XPM_NODEIDX_RST_FPD),
				     PM_RESET_ACTION_RELEASE);

	Payload[0] = PSM_API_FPD_HOUSECLEAN;
	Payload[1] = FUNC_INIT_FINISH;

	Status = XPm_IpiSend(PSM_IPI_INT_MASK, Payload);
	if (XST_SUCCESS != Status) {
		goto done;
	}

	Status = XPm_IpiReadStatus(PSM_IPI_INT_MASK);
	if (Status != XST_SUCCESS)
		goto done;

	/* Remove FPD SOC domains isolation */
	Status = XPmDomainIso_Control(XPM_NODEIDX_ISO_FPD_SOC, FALSE);
	if (Status != XST_SUCCESS)
		goto done;

done:
	return Status;
}

static XStatus FpdScanClear(u32 *Args, u32 NumOfArgs)
{
	XStatus Status = XST_SUCCESS;
	u32 Payload[PAYLOAD_ARG_CNT] = {0};

	(void)Args;
	(void)NumOfArgs;

        if (PLATFORM_VERSION_SILICON != Platform) {
                Status = XST_SUCCESS;
                goto done;
        }

        /* Trigger scan clear */
        PmRmw32(PSM_GLOBAL_SCAN_CLEAR_FPD, PSM_GLOBAL_SCAN_CLEAR_TRIGGER,
                       PSM_GLOBAL_SCAN_CLEAR_TRIGGER);

        Status = XPm_PollForMask(PSM_GLOBAL_SCAN_CLEAR_FPD,
                                        PSM_GLOBAL_SCAN_CLEAR_DONE_STATUS,
                                        0x10000U);
        if (XST_SUCCESS != Status) {
                goto done;
        }

        Status = XPm_PollForMask(PSM_GLOBAL_SCAN_CLEAR_FPD,
                                        PSM_GLOBAL_SCAN_CLEAR_PASS_STATUS,
                                        0x10000U);
        if (XST_SUCCESS != Status) {
                goto done;
        }

done:
        return Status;
}

static XStatus FpdBisr(u32 *Args, u32 NumOfArgs)
{
	XStatus Status = XST_SUCCESS;
	u32 Payload[PAYLOAD_ARG_CNT] = {0};

	(void)Args;
	(void)NumOfArgs;

	/* Release SRST for PS-FPD */
	Status = XPmReset_AssertbyId(SRST_RSTID(XPM_NODEIDX_RST_FPD),
				     PM_RESET_ACTION_RELEASE);

	/* Call PSM to execute pre bisr requirements */
	Payload[0] = PSM_API_FPD_HOUSECLEAN;
	Payload[1] = FUNC_BISR;

	Status = XPm_IpiSend(PSM_IPI_INT_MASK, Payload);
	if (XST_SUCCESS != Status) {
		goto done;
	}

	Status = XPm_IpiReadStatus(PSM_IPI_INT_MASK);
	if (XST_SUCCESS != Status) {
		goto done;
	}

	/* Trigger Bisr repair */
	Status = XPmBisr_Repair(FPD_TAG_ID);

done:
	return Status;
}

static XStatus FpdMbistClear(u32 *Args, u32 NumOfArgs)
{
        XStatus Status = XST_SUCCESS;
        u32 Payload[PAYLOAD_ARG_CNT] = {0};

	(void)Args;
	(void)NumOfArgs;

	/* Release SRST for PS-FPD */
	Status = XPmReset_AssertbyId(SRST_RSTID(XPM_NODEIDX_RST_FPD),
				     PM_RESET_ACTION_RELEASE);

        Payload[0] = PSM_API_FPD_HOUSECLEAN;
        Payload[1] = FUNC_MBIST_CLEAR;

        Status = XPm_IpiSend(PSM_IPI_INT_MASK, Payload);
        if (XST_SUCCESS != Status) {
                goto done;
        }

        Status = XPm_IpiReadStatus(PSM_IPI_INT_MASK);

        if (PLATFORM_VERSION_SILICON != Platform) {
                Status = XST_SUCCESS;
                goto done;
        }

        PmRmw32(PSM_GLOBAL_MBIST_RST, PSM_GLOBAL_MBIST_RST_FPD_MASK,
                       PSM_GLOBAL_MBIST_RST_FPD_MASK);

        PmRmw32(PSM_GLOBAL_MBIST_SETUP, PSM_GLOBAL_MBIST_SETUP_FPD_MASK,
                       PSM_GLOBAL_MBIST_SETUP_FPD_MASK);

        PmRmw32(PSM_GLOBAL_MBIST_PG_EN, PSM_GLOBAL_MBIST_PG_EN_FPD_MASK,
                       PSM_GLOBAL_MBIST_PG_EN_FPD_MASK);

        Status = XPm_PollForMask(PSM_GLOBAL_MBIST_DONE,
                                        PSM_GLOBAL_MBIST_DONE_FPD_MASK,
                                        0x10000U);
        if (XST_SUCCESS != Status) {
                goto done;
        }

        if (PSM_GLOBAL_MBIST_GO_FPD_MASK !=
            (XPm_In32(PSM_GLOBAL_MBIST_GO) &
             PSM_GLOBAL_MBIST_GO_FPD_MASK)) {
                Status = XST_FAILURE;
        }

done:
        return Status;
}

struct XPm_PowerDomainOps FpdOps = {
	.InitStart = FpdInitStart,
	.InitFinish = FpdInitFinish,
	.ScanClear = FpdScanClear,
	.Bisr = FpdBisr,
	.Mbist = FpdMbistClear,
	.HcComplete = FpdHcComplete,
};

XStatus XPmPsFpDomain_Init(XPm_PsFpDomain *PsFpd, u32 Id, u32 BaseAddress,
			   XPm_Power *Parent)
{
	XPmPowerDomain_Init(&PsFpd->Domain, Id, BaseAddress, Parent, &FpdOps);

	return XST_SUCCESS;
}