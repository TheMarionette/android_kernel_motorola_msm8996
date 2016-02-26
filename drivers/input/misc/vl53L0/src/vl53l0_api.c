/*******************************************************************************
 Copyright � 2016, STMicroelectronics International N.V.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of STMicroelectronics nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
 IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "vl53l0_api.h"
#include "vl53l0_tuning.h"
#include "vl53l0_api_core.h"
#include "vl53l0_api_histogram.h"
#include "vl53l0_api_calibration.h"
#include "vl53l0_api_strings.h"

#ifndef __KERNEL__
#include <stdlib.h>
#endif
#define LOG_FUNCTION_START(fmt, ...) \
	_LOG_FUNCTION_START(TRACE_MODULE_API, fmt, ##__VA_ARGS__)
#define LOG_FUNCTION_END(status, ...) \
	_LOG_FUNCTION_END(TRACE_MODULE_API, status, ##__VA_ARGS__)
#define LOG_FUNCTION_END_FMT(status, fmt, ...) \
	_LOG_FUNCTION_END_FMT(TRACE_MODULE_API, status, fmt, ##__VA_ARGS__)

#ifdef VL53L0_LOG_ENABLE
#define trace_print(level, ...) trace_print_module_function(TRACE_MODULE_API, \
	level, TRACE_FUNCTION_NONE, ##__VA_ARGS__)
#endif

/* Group PAL General Functions */

VL53L0_Error VL53L0_GetVersion(VL53L0_Version_t *pVersion)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	pVersion->major = VL53L0_IMPLEMENTATION_VER_MAJOR;
	pVersion->minor = VL53L0_IMPLEMENTATION_VER_MINOR;
	pVersion->build = VL53L0_IMPLEMENTATION_VER_SUB;

	pVersion->revision = VL53L0_IMPLEMENTATION_VER_REVISION;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetPalSpecVersion(VL53L0_Version_t *pPalSpecVersion)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	pPalSpecVersion->major = VL53L0_SPECIFICATION_VER_MAJOR;
	pPalSpecVersion->minor = VL53L0_SPECIFICATION_VER_MINOR;
	pPalSpecVersion->build = VL53L0_SPECIFICATION_VER_SUB;

	pPalSpecVersion->revision = VL53L0_SPECIFICATION_VER_REVISION;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetProductRevision(VL53L0_DEV Dev,
	uint8_t *pProductRevisionMajor, uint8_t *pProductRevisionMinor)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t revision_id;

	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_IDENTIFICATION_REVISION_ID,
		&revision_id);
	*pProductRevisionMajor = 1;
	*pProductRevisionMinor = (revision_id & 0xF0) >> 4;

	LOG_FUNCTION_END(Status);
	return Status;

}

VL53L0_Error VL53L0_GetDeviceInfo(VL53L0_DEV Dev,
	VL53L0_DeviceInfo_t *pVL53L0_DeviceInfo)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_device_info(Dev, pVL53L0_DeviceInfo);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetDeviceErrorStatus(VL53L0_DEV Dev,
	VL53L0_DeviceError *pDeviceErrorStatus)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t RangeStatus;

	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_RESULT_RANGE_STATUS,
		&RangeStatus);

	*pDeviceErrorStatus = (VL53L0_DeviceError)((RangeStatus & 0x78) >> 3);

	LOG_FUNCTION_END(Status);
	return Status;
}


VL53L0_Error VL53L0_GetDeviceErrorString(VL53L0_DeviceError ErrorCode,
	char *pDeviceErrorString)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	Status = VL53L0_get_device_error_string(ErrorCode, pDeviceErrorString);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetRangeStatusString(uint8_t RangeStatus,
	char *pRangeStatusString)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_range_status_string(RangeStatus,
		pRangeStatusString);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetPalErrorString(VL53L0_Error PalErrorCode,
	char *pPalErrorString)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_pal_error_string(PalErrorCode, pPalErrorString);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetPalStateString(VL53L0_State PalStateCode,
	char *pPalStateString)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_pal_state_string(PalStateCode, pPalStateString);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetPalState(VL53L0_DEV Dev, VL53L0_State *pPalState)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	*pPalState = PALDevDataGet(Dev, PalState);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetPowerMode(VL53L0_DEV Dev, VL53L0_PowerModes PowerMode)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	/* Only level1 of Power mode exists */
	if ((PowerMode != VL53L0_POWERMODE_STANDBY_LEVEL1)
		&& (PowerMode != VL53L0_POWERMODE_IDLE_LEVEL1)) {
		Status = VL53L0_ERROR_MODE_NOT_SUPPORTED;
	} else if (PowerMode == VL53L0_POWERMODE_STANDBY_LEVEL1) {
		/* set the standby level1 of power mode */
		Status = VL53L0_WrByte(Dev, 0x80, 0x00);
		if (Status == VL53L0_ERROR_NONE) {
			/* Set PAL State to standby */
			PALDevDataSet(Dev, PalState, VL53L0_STATE_STANDBY);
			PALDevDataSet(Dev, PowerMode,
				VL53L0_POWERMODE_STANDBY_LEVEL1);
		}

	} else {
		/* VL53L0_POWERMODE_IDLE_LEVEL1 */
		Status = VL53L0_WrByte(Dev, 0x80, 0x00);
		if (Status == VL53L0_ERROR_NONE)
			Status = VL53L0_StaticInit(Dev);

		if (Status == VL53L0_ERROR_NONE)
			PALDevDataSet(Dev, PowerMode,
				VL53L0_POWERMODE_IDLE_LEVEL1);

	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetPowerMode(VL53L0_DEV Dev, VL53L0_PowerModes *pPowerMode)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Byte;
	LOG_FUNCTION_START("");

	/* Only level1 of Power mode exists */
	Status = VL53L0_RdByte(Dev, 0x80, &Byte);

	if (Status == VL53L0_ERROR_NONE) {
		if (Byte == 1) {
			PALDevDataSet(Dev, PowerMode,
				VL53L0_POWERMODE_IDLE_LEVEL1);
		} else {
			PALDevDataSet(Dev, PowerMode,
				VL53L0_POWERMODE_STANDBY_LEVEL1);
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetOffsetCalibrationDataMicroMeter(VL53L0_DEV Dev,
	int32_t OffsetCalibrationDataMicroMeter)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_set_offset_calibration_data_micro_meter(Dev,
		OffsetCalibrationDataMicroMeter);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetOffsetCalibrationDataMicroMeter(VL53L0_DEV Dev,
	int32_t *pOffsetCalibrationDataMicroMeter)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_offset_calibration_data_micro_meter(Dev,
		pOffsetCalibrationDataMicroMeter);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetLinearityCorrectiveGain(VL53L0_DEV Dev,
	int16_t LinearityCorrectiveGain)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	if ((LinearityCorrectiveGain < 0) || (LinearityCorrectiveGain > 1000))
		Status = VL53L0_ERROR_INVALID_PARAMS;
	else {
		PALDevDataSet(Dev, LinearityCorrectiveGain,
			LinearityCorrectiveGain);

		if (LinearityCorrectiveGain != 1000) {
			/* Disable FW Xtalk */
			Status = VL53L0_WrWord(Dev,
			VL53L0_REG_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS, 0);
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetLinearityCorrectiveGain(VL53L0_DEV Dev,
	uint16_t *pLinearityCorrectiveGain)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	*pLinearityCorrectiveGain = PALDevDataGet(Dev, LinearityCorrectiveGain);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetGroupParamHold(VL53L0_DEV Dev, uint8_t GroupParamHold)
{
	VL53L0_Error Status = VL53L0_ERROR_NOT_IMPLEMENTED;
	LOG_FUNCTION_START("");

	/* not implemented on VL53L0 */

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetUpperLimitMilliMeter(VL53L0_DEV Dev,
	uint16_t *pUpperLimitMilliMeter)
{
	VL53L0_Error Status = VL53L0_ERROR_NOT_IMPLEMENTED;
	LOG_FUNCTION_START("");

	/* not implemented on VL53L0 */

	LOG_FUNCTION_END(Status);
	return Status;
}
/* End Group PAL General Functions */

/* Group PAL Init Functions */
VL53L0_Error VL53L0_SetDeviceAddress(VL53L0_DEV Dev, uint8_t DeviceAddress)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_WrByte(Dev, VL53L0_REG_I2C_SLAVE_DEVICE_ADDRESS,
		DeviceAddress / 2);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_DataInit(VL53L0_DEV Dev)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	VL53L0_DeviceParameters_t CurrentParameters;

	int i;

	LOG_FUNCTION_START("");

	/* by default the I2C is running at 1V8 if you want to change it you
	 * need to include this define at compilation level. */
#ifdef USE_I2C_2V8
	Status = VL53L0_UpdateByte(Dev,
		VL53L0_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
		0xFE,
		0x01);
#endif

	/* Set I2C standard mode */
	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_WrByte(Dev, 0x88, 0x00);

	VL53L0_SETDEVICESPECIFICPARAMETER(Dev, ReadDataFromDeviceDone, 0);

#ifdef USE_IQC_STATION
	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_apply_offset_adjustment(Dev);
#endif

	/* Default value is 1000 for Linearity Corrective Gain */
	PALDevDataSet(Dev, LinearityCorrectiveGain, 1000);

	/* Dmax default Parameter */
	PALDevDataSet(Dev, DmaxCalRangeMilliMeter, 600);
	PALDevDataSet(Dev, DmaxCalSignalRateRtnMegaCps,
		(FixPoint1616_t)((2 * 65536)));
	PALDevDataSet(Dev, DmaxCalBlindAmbient, 589824); /* 9 Mcps */

	/* Set Default static parameters
	 *set first temporary values 9.44MHz * 65536 = 618660 */
	VL53L0_SETDEVICESPECIFICPARAMETER(Dev, OscFrequencyMHz, 618660);

	/* Set Default XTalkCompensationRateMegaCps to 0  */
	VL53L0_SETPARAMETERFIELD(Dev, XTalkCompensationRateMegaCps, 0);

	/* Get default parameters */
	Status = VL53L0_GetDeviceParameters(Dev, &CurrentParameters);
	if (Status == VL53L0_ERROR_NONE) {
		/* initialize PAL values */
		CurrentParameters.DeviceMode = VL53L0_DEVICEMODE_SINGLE_RANGING;
		CurrentParameters.HistogramMode = VL53L0_HISTOGRAMMODE_DISABLED;
		PALDevDataSet(Dev, CurrentParameters, CurrentParameters);
	}

	/* Sigma estimator variable */
	PALDevDataSet(Dev, SigmaEstRefArray, 100);
	PALDevDataSet(Dev, SigmaEstEffPulseWidth, 900);
	PALDevDataSet(Dev, SigmaEstEffAmbWidth, 500);
	PALDevDataSet(Dev, targetRefRate, 0x0A00); /* 20 MCPS in 9:7 format */

	/* Use internal default settings */
	PALDevDataSet(Dev, UseInternalTuningSettings, 1);

	/* Enable all check */
	for (i = 0; i < VL53L0_CHECKENABLE_NUMBER_OF_CHECKS; i++) {
		if (Status == VL53L0_ERROR_NONE)
			Status |= VL53L0_SetLimitCheckEnable(Dev, i, 1);
		else
			break;

	}

	/* Disable the following checks */
	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetLimitCheckEnable(Dev,
			VL53L0_CHECKENABLE_SIGNAL_REF_CLIP, 0);

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetLimitCheckEnable(Dev,
			VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 0);

	/* Limit default values */
	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_SetLimitCheckValue(Dev,
			VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE,
				(FixPoint1616_t)(18 * 65536));
	}
	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_SetLimitCheckValue(Dev,
			VL53L0_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
				(FixPoint1616_t)(25 * 65536 / 100));
				/* 0.25 * 65536 */
	}

	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_SetLimitCheckValue(Dev,
			VL53L0_CHECKENABLE_SIGNAL_REF_CLIP,
				(FixPoint1616_t)(35 * 65536));
	}

	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_SetLimitCheckValue(Dev,
			VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
				(FixPoint1616_t)(0 * 65536));
	}

	if (Status == VL53L0_ERROR_NONE) {

		PALDevDataSet(Dev, SequenceConfig, 0xFF);
		Status = VL53L0_WrByte(Dev, VL53L0_REG_SYSTEM_SEQUENCE_CONFIG,
			0xFF);

		/* Set PAL state to tell that we are waiting for call to
		 * VL53L0_StaticInit */
		PALDevDataSet(Dev, PalState, VL53L0_STATE_WAIT_STATICINIT);
	}

	if (Status == VL53L0_ERROR_NONE)
		VL53L0_SETDEVICESPECIFICPARAMETER(Dev, RefSpadsInitialised, 0);


	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetTuningSettingBuffer(VL53L0_DEV Dev,
	uint8_t *pTuningSettingBuffer, uint8_t UseInternalTuningSettings)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	if (UseInternalTuningSettings == 1) {
		/* Force use internal settings */
		PALDevDataSet(Dev, UseInternalTuningSettings, 1);
	} else {

		/* check that the first byte is not 0 */
		if (*pTuningSettingBuffer != 0) {
			PALDevDataSet(Dev, pTuningSettingsPointer,
				pTuningSettingBuffer);
			PALDevDataSet(Dev, UseInternalTuningSettings, 0);

		} else {
			Status = VL53L0_ERROR_INVALID_PARAMS;
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetTuningSettingBuffer(VL53L0_DEV Dev,
	uint8_t **ppTuningSettingBuffer, uint8_t *pUseInternalTuningSettings)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	*ppTuningSettingBuffer = PALDevDataGet(Dev, pTuningSettingsPointer);
	*pUseInternalTuningSettings = PALDevDataGet(Dev,
		UseInternalTuningSettings);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_StaticInit(VL53L0_DEV Dev)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	VL53L0_DeviceParameters_t CurrentParameters = {0};
	uint8_t *pTuningSettingBuffer;
	uint16_t tempword = 0;
	uint8_t tempbyte = 0;
	uint8_t UseInternalTuningSettings = 0;
	uint32_t count = 0;
	uint8_t isApertureSpads = 0;
	uint32_t refSpadCount = 0;
	uint8_t ApertureSpads = 0;

	LOG_FUNCTION_START("");

	Status = VL53L0_get_info_from_device(Dev, 1);

	/* set the ref spad from NVM */
	count   = (uint32_t)VL53L0_GETDEVICESPECIFICPARAMETER(Dev,
		ReferenceSpadCount);
	ApertureSpads = VL53L0_GETDEVICESPECIFICPARAMETER(Dev,
		ReferenceSpadType);

	/* NVM value invalid */
	if ((ApertureSpads > 1) ||
		((ApertureSpads == 1) && (count > 32)) ||
		((ApertureSpads == 0) && (count > 12)))
		Status = VL53L0_perform_ref_spad_management(Dev, &refSpadCount,
			&isApertureSpads);
	else
		Status = VL53L0_set_reference_spads(Dev, count, ApertureSpads);


	/* Initialize tuning settings buffer to prevent compiler warning. */
	pTuningSettingBuffer = DefaultTuningSettings;

	if (Status == VL53L0_ERROR_NONE) {
		UseInternalTuningSettings = PALDevDataGet(Dev,
			UseInternalTuningSettings);

		if (UseInternalTuningSettings == 0)
			pTuningSettingBuffer = PALDevDataGet(Dev,
				pTuningSettingsPointer);
		else
			pTuningSettingBuffer = DefaultTuningSettings;

	}

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_load_tuning_settings(Dev, pTuningSettingBuffer);

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_WrByte(Dev, 0x80, 0x00);


	/* Set interrupt config to new sample ready */
	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_SetGpioConfig(Dev, 0, 0,
		VL53L0_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY,
		VL53L0_INTERRUPTPOLARITY_LOW);
	}

	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_WrByte(Dev, 0xFF, 0x01);
		Status |= VL53L0_RdWord(Dev, 0x84, &tempword);
		Status |= VL53L0_WrByte(Dev, 0xFF, 0x00);
	}

	if (Status == VL53L0_ERROR_NONE) {
		VL53L0_SETDEVICESPECIFICPARAMETER(Dev, OscFrequencyMHz,
			VL53L0_FIXPOINT412TOFIXPOINT1616(tempword));
	}

	/* After static init, some device parameters may be changed,
	 * so update them */
	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_GetDeviceParameters(Dev, &CurrentParameters);


	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_RdByte(Dev, VL53L0_REG_SYSTEM_RANGE_CONFIG,
			&tempbyte);
		PALDevDataSet(Dev, RangeFractionalEnable, tempbyte);
	}

	if (Status == VL53L0_ERROR_NONE)
		PALDevDataSet(Dev, CurrentParameters, CurrentParameters);


	/* read the sequence config and save it */
	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_RdByte(Dev,
		VL53L0_REG_SYSTEM_SEQUENCE_CONFIG, &tempbyte);
		if (Status == VL53L0_ERROR_NONE)
			PALDevDataSet(Dev, SequenceConfig, tempbyte);

	}

	/* Disable MSRC and TCC by default */
	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetSequenceStepEnable(Dev,
					VL53L0_SEQUENCESTEP_TCC, 0);


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetSequenceStepEnable(Dev,
		VL53L0_SEQUENCESTEP_MSRC, 0);


	/* Set PAL State to standby */
	if (Status == VL53L0_ERROR_NONE)
		PALDevDataSet(Dev, PalState, VL53L0_STATE_IDLE);


	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_WaitDeviceBooted(VL53L0_DEV Dev)
{
	VL53L0_Error Status = VL53L0_ERROR_NOT_IMPLEMENTED;
	LOG_FUNCTION_START("");

	/* not implemented on VL53L0 */

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_ResetDevice(VL53L0_DEV Dev)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Byte;
	LOG_FUNCTION_START("");

	/* Set reset bit */
	Status = VL53L0_WrByte(Dev, VL53L0_REG_SOFT_RESET_GO2_SOFT_RESET_N,
		0x00);

	/* Wait for some time */
	if (Status == VL53L0_ERROR_NONE) {
		do {
			Status = VL53L0_RdByte(Dev,
			VL53L0_REG_IDENTIFICATION_MODEL_ID, &Byte);
		} while (Byte != 0x00);
	}

	/* Release reset */
	Status = VL53L0_WrByte(Dev, VL53L0_REG_SOFT_RESET_GO2_SOFT_RESET_N,
		0x01);

	/* Wait until correct boot-up of the device */
	if (Status == VL53L0_ERROR_NONE) {
		do {
			Status = VL53L0_RdByte(Dev,
			VL53L0_REG_IDENTIFICATION_MODEL_ID, &Byte);
		} while (Byte == 0x00);
	}

	/* Set PAL State to VL53L0_STATE_POWERDOWN */
	if (Status == VL53L0_ERROR_NONE)
		PALDevDataSet(Dev, PalState, VL53L0_STATE_POWERDOWN);


	LOG_FUNCTION_END(Status);
	return Status;
}
/* End Group PAL Init Functions */

/* Group PAL Parameters Functions */
VL53L0_Error VL53L0_SetDeviceParameters(VL53L0_DEV Dev,
	const VL53L0_DeviceParameters_t *pDeviceParameters)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	int i;
	LOG_FUNCTION_START("");
	Status = VL53L0_SetDeviceMode(Dev, pDeviceParameters->DeviceMode);

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetHistogramMode(Dev,
			pDeviceParameters->HistogramMode);


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetInterMeasurementPeriodMilliSeconds(Dev,
			pDeviceParameters->InterMeasurementPeriodMilliSeconds);


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetXTalkCompensationRateMegaCps(Dev,
			pDeviceParameters->XTalkCompensationRateMegaCps);


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetOffsetCalibrationDataMicroMeter(Dev,
			pDeviceParameters->RangeOffsetMicroMeters);


	for (i = 0; i < VL53L0_CHECKENABLE_NUMBER_OF_CHECKS; i++) {
		if (Status == VL53L0_ERROR_NONE)
			Status |= VL53L0_SetLimitCheckEnable(Dev, i,
				pDeviceParameters->LimitChecksEnable[i]);
		else
			break;

		if (Status == VL53L0_ERROR_NONE)
			Status |= VL53L0_SetLimitCheckValue(Dev, i,
				pDeviceParameters->LimitChecksValue[i]);
		else
			break;

	}

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetWrapAroundCheckEnable(Dev,
			pDeviceParameters->WrapAroundCheckEnable);

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_SetMeasurementTimingBudgetMicroSeconds(Dev,
			pDeviceParameters->MeasurementTimingBudgetMicroSeconds);


	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetDeviceParameters(VL53L0_DEV Dev,
	VL53L0_DeviceParameters_t *pDeviceParameters)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	int i;

	LOG_FUNCTION_START("");

	Status = VL53L0_GetDeviceMode(Dev, &(pDeviceParameters->DeviceMode));

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_GetHistogramMode(Dev,
			&(pDeviceParameters->HistogramMode));

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_GetInterMeasurementPeriodMilliSeconds(Dev,
		&(pDeviceParameters->InterMeasurementPeriodMilliSeconds));


	if (Status == VL53L0_ERROR_NONE)
		pDeviceParameters->XTalkCompensationEnable = 0;

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_GetXTalkCompensationRateMegaCps(Dev,
			&(pDeviceParameters->XTalkCompensationRateMegaCps));


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_GetOffsetCalibrationDataMicroMeter(Dev,
			&(pDeviceParameters->RangeOffsetMicroMeters));


	if (Status == VL53L0_ERROR_NONE) {
		for (i = 0; i < VL53L0_CHECKENABLE_NUMBER_OF_CHECKS; i++) {
			/* get first the values, then the enables.
			 * VL53L0_GetLimitCheckValue will modify the enable
			 * flags
			 */
			if (Status == VL53L0_ERROR_NONE) {
				Status |= VL53L0_GetLimitCheckValue(Dev, i,
				&(pDeviceParameters->LimitChecksValue[i]));
			} else {
				break;
			}
			if (Status == VL53L0_ERROR_NONE) {
				Status |= VL53L0_GetLimitCheckEnable(Dev, i,
				&(pDeviceParameters->LimitChecksEnable[i]));
			} else {
				break;
			}
		}
	}

	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_GetWrapAroundCheckEnable(Dev,
			&(pDeviceParameters->WrapAroundCheckEnable));
	}

	/* Need to be done at the end as it uses VCSELPulsePeriod */
	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_GetMeasurementTimingBudgetMicroSeconds(Dev,
		&(pDeviceParameters->MeasurementTimingBudgetMicroSeconds));
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetDeviceMode(VL53L0_DEV Dev, VL53L0_DeviceModes DeviceMode)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("%d", (int)DeviceMode);

	switch (DeviceMode) {
	case VL53L0_DEVICEMODE_SINGLE_RANGING:
	case VL53L0_DEVICEMODE_CONTINUOUS_RANGING:
	case VL53L0_DEVICEMODE_CONTINUOUS_TIMED_RANGING:
	case VL53L0_DEVICEMODE_SINGLE_HISTOGRAM:
	case VL53L0_DEVICEMODE_GPIO_DRIVE:
	case VL53L0_DEVICEMODE_GPIO_OSC:
		/* Supported modes */
		VL53L0_SETPARAMETERFIELD(Dev, DeviceMode, DeviceMode);
		break;
	default:
		/* Unsupported mode */
		Status = VL53L0_ERROR_MODE_NOT_SUPPORTED;
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetDeviceMode(VL53L0_DEV Dev,
	VL53L0_DeviceModes *pDeviceMode)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	VL53L0_GETPARAMETERFIELD(Dev, DeviceMode, *pDeviceMode);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetHistogramMode(VL53L0_DEV Dev,
	VL53L0_HistogramModes HistogramMode)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("%d", (int)HistogramMode);

	Status = VL53L0_set_histogram_mode(Dev, HistogramMode);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetHistogramMode(VL53L0_DEV Dev,
	VL53L0_HistogramModes *pHistogramMode)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_histogram_mode(Dev, pHistogramMode);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetMeasurementTimingBudgetMicroSeconds(VL53L0_DEV Dev,
	uint32_t MeasurementTimingBudgetMicroSeconds)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_set_measurement_timing_budget_micro_seconds(Dev,
		MeasurementTimingBudgetMicroSeconds);

	LOG_FUNCTION_END(Status);

	return Status;
}

VL53L0_Error VL53L0_GetMeasurementTimingBudgetMicroSeconds(VL53L0_DEV Dev,
	uint32_t *pMeasurementTimingBudgetMicroSeconds)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_measurement_timing_budget_micro_seconds(Dev,
		pMeasurementTimingBudgetMicroSeconds);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetVcselPulsePeriod(VL53L0_DEV Dev,
	VL53L0_VcselPeriod VcselPeriodType, uint8_t VCSELPulsePeriodPCLK)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_set_vcsel_pulse_period(Dev, VcselPeriodType,
		VCSELPulsePeriodPCLK);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetVcselPulsePeriod(VL53L0_DEV Dev,
	VL53L0_VcselPeriod VcselPeriodType, uint8_t *pVCSELPulsePeriodPCLK)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_vcsel_pulse_period(Dev, VcselPeriodType,
		pVCSELPulsePeriodPCLK);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetSequenceStepEnable(VL53L0_DEV Dev,
	VL53L0_SequenceStepId SequenceStepId, uint8_t SequenceStepEnabled)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t SequenceConfig = 0;
	uint8_t SequenceConfigNew = 0;
	uint32_t MeasurementTimingBudgetMicroSeconds;
	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_SYSTEM_SEQUENCE_CONFIG,
		&SequenceConfig);

	SequenceConfigNew = SequenceConfig;

	if (Status == VL53L0_ERROR_NONE) {
		if (SequenceStepEnabled == 1) {

			/* Enable requested sequence step
			 */
			switch (SequenceStepId) {
			case VL53L0_SEQUENCESTEP_TCC:
				SequenceConfigNew |= 0x10;
				break;
			case VL53L0_SEQUENCESTEP_DSS:
				SequenceConfigNew |= 0x28;
				break;
			case VL53L0_SEQUENCESTEP_MSRC:
				SequenceConfigNew |= 0x04;
				break;
			case VL53L0_SEQUENCESTEP_PRE_RANGE:
				SequenceConfigNew |= 0x40;
				break;
			case VL53L0_SEQUENCESTEP_FINAL_RANGE:
				SequenceConfigNew |= 0x80;
				break;
			default:
				Status = VL53L0_ERROR_INVALID_PARAMS;
			}
		} else {
			/* Disable requested sequence step
			 */
			switch (SequenceStepId) {
			case VL53L0_SEQUENCESTEP_TCC:
				SequenceConfigNew &= 0xef;
				break;
			case VL53L0_SEQUENCESTEP_DSS:
				SequenceConfigNew &= 0xd7;
				break;
			case VL53L0_SEQUENCESTEP_MSRC:
				SequenceConfigNew &= 0xfb;
				break;
			case VL53L0_SEQUENCESTEP_PRE_RANGE:
				SequenceConfigNew &= 0xbf;
				break;
			case VL53L0_SEQUENCESTEP_FINAL_RANGE:
				SequenceConfigNew &= 0x7f;
				break;
			default:
				Status = VL53L0_ERROR_INVALID_PARAMS;
			}
		}
	}

	if (SequenceConfigNew != SequenceConfig) {
		/* Apply New Setting */
		if (Status == VL53L0_ERROR_NONE) {
			Status = VL53L0_WrByte(Dev,
			VL53L0_REG_SYSTEM_SEQUENCE_CONFIG, SequenceConfigNew);
		}
		if (Status == VL53L0_ERROR_NONE)
			PALDevDataSet(Dev, SequenceConfig, SequenceConfigNew);


		/* Recalculate timing budget */
		if (Status == VL53L0_ERROR_NONE) {
			VL53L0_GETPARAMETERFIELD(Dev,
				MeasurementTimingBudgetMicroSeconds,
				MeasurementTimingBudgetMicroSeconds);

			VL53L0_SetMeasurementTimingBudgetMicroSeconds(Dev,
				MeasurementTimingBudgetMicroSeconds);
		}
	}

	LOG_FUNCTION_END(Status);

	return Status;
}

VL53L0_Error sequence_step_enabled(VL53L0_DEV Dev,
	VL53L0_SequenceStepId SequenceStepId, uint8_t SequenceConfig,
	uint8_t *pSequenceStepEnabled)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	*pSequenceStepEnabled = 0;
	LOG_FUNCTION_START("");

	switch (SequenceStepId) {
	case VL53L0_SEQUENCESTEP_TCC:
		*pSequenceStepEnabled = (SequenceConfig & 0x10) >> 4;
		break;
	case VL53L0_SEQUENCESTEP_DSS:
		*pSequenceStepEnabled = (SequenceConfig & 0x08) >> 3;
		break;
	case VL53L0_SEQUENCESTEP_MSRC:
		*pSequenceStepEnabled = (SequenceConfig & 0x04) >> 2;
		break;
	case VL53L0_SEQUENCESTEP_PRE_RANGE:
		*pSequenceStepEnabled = (SequenceConfig & 0x40) >> 6;
		break;
	case VL53L0_SEQUENCESTEP_FINAL_RANGE:
		*pSequenceStepEnabled = (SequenceConfig & 0x80) >> 7;
		break;
	default:
		Status = VL53L0_ERROR_INVALID_PARAMS;
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetSequenceStepEnable(VL53L0_DEV Dev,
	VL53L0_SequenceStepId SequenceStepId, uint8_t *pSequenceStepEnabled)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t SequenceConfig = 0;
	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_SYSTEM_SEQUENCE_CONFIG,
		&SequenceConfig);

	if (Status == VL53L0_ERROR_NONE) {
		Status = sequence_step_enabled(Dev, SequenceStepId,
			SequenceConfig, pSequenceStepEnabled);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetSequenceStepEnables(VL53L0_DEV Dev,
	VL53L0_SchedulerSequenceSteps_t *pSchedulerSequenceSteps)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t SequenceConfig = 0;
	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_SYSTEM_SEQUENCE_CONFIG,
		&SequenceConfig);

	if (Status == VL53L0_ERROR_NONE) {
		Status = sequence_step_enabled(Dev,
		VL53L0_SEQUENCESTEP_TCC, SequenceConfig,
			&pSchedulerSequenceSteps->TccOn);
	}
	if (Status == VL53L0_ERROR_NONE) {
		Status = sequence_step_enabled(Dev,
		VL53L0_SEQUENCESTEP_DSS, SequenceConfig,
			&pSchedulerSequenceSteps->DssOn);
	}
	if (Status == VL53L0_ERROR_NONE) {
		Status = sequence_step_enabled(Dev,
		VL53L0_SEQUENCESTEP_MSRC, SequenceConfig,
			&pSchedulerSequenceSteps->MsrcOn);
	}
	if (Status == VL53L0_ERROR_NONE) {
		Status = sequence_step_enabled(Dev,
		VL53L0_SEQUENCESTEP_PRE_RANGE, SequenceConfig,
			&pSchedulerSequenceSteps->PreRangeOn);
	}
	if (Status == VL53L0_ERROR_NONE) {
		Status = sequence_step_enabled(Dev,
		VL53L0_SEQUENCESTEP_FINAL_RANGE, SequenceConfig,
			&pSchedulerSequenceSteps->FinalRangeOn);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetNumberOfSequenceSteps(VL53L0_DEV Dev,
	uint8_t *pNumberOfSequenceSteps)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	*pNumberOfSequenceSteps = VL53L0_SEQUENCESTEP_NUMBER_OF_CHECKS;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetSequenceStepsInfo(VL53L0_SequenceStepId SequenceStepId,
	char *pSequenceStepsString)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_sequence_steps_info(
			SequenceStepId,
			pSequenceStepsString);

	LOG_FUNCTION_END(Status);

	return Status;
}

VL53L0_Error VL53L0_SetSequenceStepTimeout(VL53L0_DEV Dev,
	VL53L0_SequenceStepId SequenceStepId, FixPoint1616_t TimeOutMilliSecs)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	VL53L0_Error Status1 = VL53L0_ERROR_NONE;
	uint32_t TimeoutMicroSeconds = ((TimeOutMilliSecs * 1000) + 0x8000)
		>> 16;
	uint32_t MeasurementTimingBudgetMicroSeconds;
	FixPoint1616_t OldTimeOutMicroSeconds;

	LOG_FUNCTION_START("");

	/* Read back the current value in case we need to revert back to this.
	 */
	Status = get_sequence_step_timeout(Dev, SequenceStepId,
		&OldTimeOutMicroSeconds);

	if (Status == VL53L0_ERROR_NONE) {
		Status = set_sequence_step_timeout(Dev, SequenceStepId,
			TimeoutMicroSeconds);
	}

	if (Status == VL53L0_ERROR_NONE) {
		VL53L0_GETPARAMETERFIELD(Dev,
			MeasurementTimingBudgetMicroSeconds,
			MeasurementTimingBudgetMicroSeconds);

		/* At this point we don't know if the requested value is valid,
		 therefore proceed to update the entire timing budget and
		 if this fails, revert back to the previous value.
		 */
		Status = VL53L0_SetMeasurementTimingBudgetMicroSeconds(Dev,
			MeasurementTimingBudgetMicroSeconds);

		if (Status != VL53L0_ERROR_NONE) {
			Status1 = set_sequence_step_timeout(Dev, SequenceStepId,
				OldTimeOutMicroSeconds);

			if (Status1 == VL53L0_ERROR_NONE) {
				Status1 =
				VL53L0_SetMeasurementTimingBudgetMicroSeconds(
					Dev,
					MeasurementTimingBudgetMicroSeconds);
			}

			Status = Status1;
		}
	}

	LOG_FUNCTION_END(Status);

	return Status;
}

VL53L0_Error VL53L0_GetSequenceStepTimeout(VL53L0_DEV Dev,
	VL53L0_SequenceStepId SequenceStepId, FixPoint1616_t *pTimeOutMilliSecs)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint32_t TimeoutMicroSeconds;
	uint32_t WholeNumber_ms = 0;
	uint32_t Fraction_ms = 0;
	LOG_FUNCTION_START("");

	Status = get_sequence_step_timeout(Dev, SequenceStepId,
		&TimeoutMicroSeconds);
	if (Status == VL53L0_ERROR_NONE) {
		WholeNumber_ms = TimeoutMicroSeconds / 1000;
		Fraction_ms = TimeoutMicroSeconds - (WholeNumber_ms * 1000);
		*pTimeOutMilliSecs = (WholeNumber_ms << 16)
			+ (((Fraction_ms * 0xffff) + 500) / 1000);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetInterMeasurementPeriodMilliSeconds(VL53L0_DEV Dev,
	uint32_t InterMeasurementPeriodMilliSeconds)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint16_t osc_calibrate_val;
	uint32_t IMPeriodMilliSeconds;

	LOG_FUNCTION_START("");

	Status = VL53L0_RdWord(Dev, VL53L0_REG_OSC_CALIBRATE_VAL,
		&osc_calibrate_val);

	if (Status == VL53L0_ERROR_NONE) {
		if (osc_calibrate_val != 0) {
			IMPeriodMilliSeconds =
				InterMeasurementPeriodMilliSeconds
					* osc_calibrate_val;
		} else {
			IMPeriodMilliSeconds =
				InterMeasurementPeriodMilliSeconds;
		}
		Status = VL53L0_WrDWord(Dev,
		VL53L0_REG_SYSTEM_INTERMEASUREMENT_PERIOD,
			IMPeriodMilliSeconds);
	}

	if (Status == VL53L0_ERROR_NONE) {
		VL53L0_SETPARAMETERFIELD(Dev,
			InterMeasurementPeriodMilliSeconds,
			InterMeasurementPeriodMilliSeconds);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetInterMeasurementPeriodMilliSeconds(VL53L0_DEV Dev,
	uint32_t *pInterMeasurementPeriodMilliSeconds)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint16_t osc_calibrate_val;
	uint32_t IMPeriodMilliSeconds;

	LOG_FUNCTION_START("");

	Status = VL53L0_RdWord(Dev, VL53L0_REG_OSC_CALIBRATE_VAL,
		&osc_calibrate_val);

	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_RdDWord(Dev,
		VL53L0_REG_SYSTEM_INTERMEASUREMENT_PERIOD,
			&IMPeriodMilliSeconds);
	}

	if (Status == VL53L0_ERROR_NONE) {
		if (osc_calibrate_val != 0) {
			*pInterMeasurementPeriodMilliSeconds =
				IMPeriodMilliSeconds / osc_calibrate_val;
		}
		VL53L0_SETPARAMETERFIELD(Dev,
			InterMeasurementPeriodMilliSeconds,
			*pInterMeasurementPeriodMilliSeconds);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetXTalkCompensationEnable(VL53L0_DEV Dev,
	uint8_t XTalkCompensationEnable)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	FixPoint1616_t TempFix1616;
	uint16_t LinearityCorrectiveGain;

	LOG_FUNCTION_START("");

	LinearityCorrectiveGain = PALDevDataGet(Dev, LinearityCorrectiveGain);

	if ((XTalkCompensationEnable == 0)
		|| (LinearityCorrectiveGain != 1000)) {
		TempFix1616 = 0;
	} else {
		VL53L0_GETPARAMETERFIELD(Dev, XTalkCompensationRateMegaCps,
			TempFix1616);
	}

	/* the following register has a format 3.13 */
	Status = VL53L0_WrWord(Dev,
	VL53L0_REG_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS,
		VL53L0_FIXPOINT1616TOFIXPOINT313(TempFix1616));

	if (Status == VL53L0_ERROR_NONE) {
		if (XTalkCompensationEnable == 0) {
			VL53L0_SETPARAMETERFIELD(Dev, XTalkCompensationEnable,
				0);
		} else {
			VL53L0_SETPARAMETERFIELD(Dev, XTalkCompensationEnable,
				1);
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetXTalkCompensationEnable(VL53L0_DEV Dev,
	uint8_t *pXTalkCompensationEnable)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Temp8;
	LOG_FUNCTION_START("");

	VL53L0_GETPARAMETERFIELD(Dev, XTalkCompensationEnable, Temp8);
	*pXTalkCompensationEnable = Temp8;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetXTalkCompensationRateMegaCps(VL53L0_DEV Dev,
	FixPoint1616_t XTalkCompensationRateMegaCps)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Temp8;
	uint16_t LinearityCorrectiveGain;
	uint16_t data;
	LOG_FUNCTION_START("");

	VL53L0_GETPARAMETERFIELD(Dev, XTalkCompensationEnable, Temp8);
	LinearityCorrectiveGain = PALDevDataGet(Dev, LinearityCorrectiveGain);

	if (Temp8 == 0) { /* disabled write only internal value */
		VL53L0_SETPARAMETERFIELD(Dev, XTalkCompensationRateMegaCps,
			XTalkCompensationRateMegaCps);
	} else {
		/* the following register has a format 3.13 */
		if (LinearityCorrectiveGain == 1000) {
			data = VL53L0_FIXPOINT1616TOFIXPOINT313(
				XTalkCompensationRateMegaCps);
		} else {
			data = 0;
		}

		Status = VL53L0_WrWord(Dev,
		VL53L0_REG_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS, data);

		if (Status == VL53L0_ERROR_NONE) {
			VL53L0_SETPARAMETERFIELD(Dev,
				XTalkCompensationRateMegaCps,
				XTalkCompensationRateMegaCps);
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetXTalkCompensationRateMegaCps(VL53L0_DEV Dev,
	FixPoint1616_t *pXTalkCompensationRateMegaCps)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint16_t Value;
	FixPoint1616_t TempFix1616;

	LOG_FUNCTION_START("");

	Status = VL53L0_RdWord(Dev,
	VL53L0_REG_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS, (uint16_t *)&Value);
	if (Status == VL53L0_ERROR_NONE) {
		if (Value == 0) {
			/* the Xtalk is disabled return value from memory */
			VL53L0_GETPARAMETERFIELD(Dev,
				XTalkCompensationRateMegaCps, TempFix1616);
			*pXTalkCompensationRateMegaCps = TempFix1616;
			VL53L0_SETPARAMETERFIELD(Dev, XTalkCompensationEnable,
				0);
		} else {
			TempFix1616 = VL53L0_FIXPOINT313TOFIXPOINT1616(Value);
			*pXTalkCompensationRateMegaCps = TempFix1616;
			VL53L0_SETPARAMETERFIELD(Dev,
				XTalkCompensationRateMegaCps, TempFix1616);
			VL53L0_SETPARAMETERFIELD(Dev, XTalkCompensationEnable,
				1);
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetRefCalibration(VL53L0_DEV Dev, uint8_t VhvSettings,
	uint8_t PhaseCal)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_set_ref_calibration(Dev, VhvSettings, PhaseCal);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetRefCalibration(VL53L0_DEV Dev, uint8_t *pVhvSettings,
	uint8_t *pPhaseCal)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_ref_calibration(Dev, pVhvSettings, pPhaseCal);

	LOG_FUNCTION_END(Status);
	return Status;
}

/*
 * CHECK LIMIT FUNCTIONS
 */

VL53L0_Error VL53L0_GetNumberOfLimitCheck(uint16_t *pNumberOfLimitCheck)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	*pNumberOfLimitCheck = VL53L0_CHECKENABLE_NUMBER_OF_CHECKS;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetLimitCheckInfo(VL53L0_DEV Dev, uint16_t LimitCheckId,
	char *pLimitCheckString)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	Status = VL53L0_get_limit_check_info(Dev, LimitCheckId,
		pLimitCheckString);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetLimitCheckStatus(VL53L0_DEV Dev, uint16_t LimitCheckId,
	uint8_t *pLimitCheckStatus)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Temp8;

	LOG_FUNCTION_START("");

	if (LimitCheckId >= VL53L0_CHECKENABLE_NUMBER_OF_CHECKS) {
		Status = VL53L0_ERROR_INVALID_PARAMS;
	} else {

		VL53L0_GETARRAYPARAMETERFIELD(Dev, LimitChecksStatus,
			LimitCheckId, Temp8);

		*pLimitCheckStatus = Temp8;

	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetLimitCheckEnable(VL53L0_DEV Dev, uint16_t LimitCheckId,
	uint8_t LimitCheckEnable)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	FixPoint1616_t TempFix1616 = 0;
	uint8_t LimitCheckEnableInt;

	LOG_FUNCTION_START("");

	if (LimitCheckId >= VL53L0_CHECKENABLE_NUMBER_OF_CHECKS) {
		Status = VL53L0_ERROR_INVALID_PARAMS;
	} else {
		if (LimitCheckEnable == 0) {
			TempFix1616 = 0;
			LimitCheckEnableInt = 0;
		} else {
			VL53L0_GETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
				LimitCheckId, TempFix1616);
			/* this to be sure to have either 0 or 1 */
			LimitCheckEnableInt = 1;
		}

		switch (LimitCheckId) {

		case VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE:
			/* internal computation: */
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksEnable,
				VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE,
				LimitCheckEnableInt);

			break;

		case VL53L0_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE:

			Status = VL53L0_WrWord(Dev,
			VL53L0_REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT,
				VL53L0_FIXPOINT1616TOFIXPOINT97(TempFix1616));

			break;

		case VL53L0_CHECKENABLE_SIGNAL_REF_CLIP:

			/* internal computation: */
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksEnable,
				VL53L0_CHECKENABLE_SIGNAL_REF_CLIP,
				LimitCheckEnableInt);

			break;

		case VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD:

			/* internal computation: */
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksEnable,
				VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
				LimitCheckEnableInt);

			break;

		default:
			Status = VL53L0_ERROR_INVALID_PARAMS;

		}

	}

	if (Status == VL53L0_ERROR_NONE) {
		if (LimitCheckEnable == 0) {
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksEnable,
				LimitCheckId, 0);
		} else {
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksEnable,
				LimitCheckId, 1);
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetLimitCheckEnable(VL53L0_DEV Dev, uint16_t LimitCheckId,
	uint8_t *pLimitCheckEnable)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Temp8;

	LOG_FUNCTION_START("");

	if (LimitCheckId >= VL53L0_CHECKENABLE_NUMBER_OF_CHECKS)
		Status = VL53L0_ERROR_INVALID_PARAMS;


	VL53L0_GETARRAYPARAMETERFIELD(Dev, LimitChecksEnable, LimitCheckId,
		Temp8);
	*pLimitCheckEnable = Temp8;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetLimitCheckValue(VL53L0_DEV Dev, uint16_t LimitCheckId,
	FixPoint1616_t LimitCheckValue)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Temp8;

	LOG_FUNCTION_START("");

	VL53L0_GETARRAYPARAMETERFIELD(Dev, LimitChecksEnable, LimitCheckId,
		Temp8);

	if (Temp8 == 0) { /* disabled write only internal value */
		VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
			LimitCheckId, LimitCheckValue);
	} else {

		switch (LimitCheckId) {

		case VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE:
			/* internal computation: */
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
				VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE,
				LimitCheckValue);
			break;

		case VL53L0_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE:

			Status = VL53L0_WrWord(Dev,
			VL53L0_REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT,
				VL53L0_FIXPOINT1616TOFIXPOINT97(
					LimitCheckValue));

			break;

		case VL53L0_CHECKENABLE_SIGNAL_REF_CLIP:

			/* internal computation: */
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
				VL53L0_CHECKENABLE_SIGNAL_REF_CLIP,
				LimitCheckValue);

			break;

		case VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD:

			/* internal computation: */
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
				VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
				LimitCheckValue);

			break;

		default:
			Status = VL53L0_ERROR_INVALID_PARAMS;

		}

		if (Status == VL53L0_ERROR_NONE) {
			VL53L0_SETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
				LimitCheckId, LimitCheckValue);
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetLimitCheckValue(VL53L0_DEV Dev, uint16_t LimitCheckId,
	FixPoint1616_t *pLimitCheckValue)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t EnableZeroValue = 0;
	uint16_t Temp16;
	FixPoint1616_t TempFix1616;

	LOG_FUNCTION_START("");

	switch (LimitCheckId) {

	case VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE:
		/* internal computation: */
		VL53L0_GETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
			VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE, TempFix1616);
		EnableZeroValue = 0;
		break;

	case VL53L0_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE:
		Status = VL53L0_RdWord(Dev,
		VL53L0_REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT,
			&Temp16);
		if (Status == VL53L0_ERROR_NONE)
			TempFix1616 = VL53L0_FIXPOINT97TOFIXPOINT1616(Temp16);


		EnableZeroValue = 1;
		break;

	case VL53L0_CHECKENABLE_SIGNAL_REF_CLIP:
		/* internal computation: */
		VL53L0_GETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
			VL53L0_CHECKENABLE_SIGNAL_REF_CLIP, TempFix1616);
		EnableZeroValue = 0;
		break;

	case VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD:
		/* internal computation: */
		VL53L0_GETARRAYPARAMETERFIELD(Dev, LimitChecksValue,
			VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD, TempFix1616);
		EnableZeroValue = 0;
		break;

	default:
		Status = VL53L0_ERROR_INVALID_PARAMS;

	}

	if (Status == VL53L0_ERROR_NONE) {

		if (EnableZeroValue == 1) {

			if (TempFix1616 == 0) {
				/* disabled: return value from memory */
				VL53L0_GETARRAYPARAMETERFIELD(Dev,
					LimitChecksValue, LimitCheckId,
					TempFix1616);
				*pLimitCheckValue = TempFix1616;
				VL53L0_SETARRAYPARAMETERFIELD(Dev,
					LimitChecksEnable, LimitCheckId, 0);
			} else {
				*pLimitCheckValue = TempFix1616;
				VL53L0_SETARRAYPARAMETERFIELD(Dev,
					LimitChecksValue, LimitCheckId,
					TempFix1616);
				VL53L0_SETARRAYPARAMETERFIELD(Dev,
					LimitChecksEnable, LimitCheckId, 1);
			}
		} else {
			*pLimitCheckValue = TempFix1616;
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;

}

VL53L0_Error VL53L0_GetLimitCheckCurrent(VL53L0_DEV Dev, uint16_t LimitCheckId,
	FixPoint1616_t *pLimitCheckCurrent)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	VL53L0_RangingMeasurementData_t LastRangeDataBuffer;

	LOG_FUNCTION_START("");

	if (LimitCheckId >= VL53L0_CHECKENABLE_NUMBER_OF_CHECKS) {
		Status = VL53L0_ERROR_INVALID_PARAMS;
	} else {
		switch (LimitCheckId) {
		case VL53L0_CHECKENABLE_SIGMA_FINAL_RANGE:
			/* Need to run a ranging to have the latest values */
			*pLimitCheckCurrent = PALDevDataGet(Dev, SigmaEstimate);

			break;

		case VL53L0_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE:
			/* Need to run a ranging to have the latest values */
			LastRangeDataBuffer = PALDevDataGet(Dev,
				LastRangeMeasure);
			*pLimitCheckCurrent =
				LastRangeDataBuffer.SignalRateRtnMegaCps;

			break;

		case VL53L0_CHECKENABLE_SIGNAL_REF_CLIP:
			/* Need to run a ranging to have the latest values */
			*pLimitCheckCurrent = PALDevDataGet(Dev,
				LastSignalRefMcps);

			break;

		case VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD:
			/* Need to run a ranging to have the latest values */
			LastRangeDataBuffer = PALDevDataGet(Dev,
				LastRangeMeasure);
			*pLimitCheckCurrent =
				LastRangeDataBuffer.SignalRateRtnMegaCps;

			break;

		default:
			Status = VL53L0_ERROR_INVALID_PARAMS;
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;

}

/*
 * WRAPAROUND Check
 */
VL53L0_Error VL53L0_SetWrapAroundCheckEnable(VL53L0_DEV Dev,
	uint8_t WrapAroundCheckEnable)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Byte;
	uint8_t WrapAroundCheckEnableInt;

	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_SYSTEM_SEQUENCE_CONFIG, &Byte);
	if (WrapAroundCheckEnable == 0) {
		/* Disable wraparound */
		Byte = Byte & 0x7F;
		WrapAroundCheckEnableInt = 0;
	} else {
		/*Enable wraparound */
		Byte = Byte | 0x80;
		WrapAroundCheckEnableInt = 1;
	}

	Status = VL53L0_WrByte(Dev, VL53L0_REG_SYSTEM_SEQUENCE_CONFIG, Byte);

	if (Status == VL53L0_ERROR_NONE) {
		PALDevDataSet(Dev, SequenceConfig, Byte);
		VL53L0_SETPARAMETERFIELD(Dev, WrapAroundCheckEnable,
			WrapAroundCheckEnableInt);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetWrapAroundCheckEnable(VL53L0_DEV Dev,
	uint8_t *pWrapAroundCheckEnable)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t data;

	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_SYSTEM_SEQUENCE_CONFIG, &data);
	if (Status == VL53L0_ERROR_NONE) {
		PALDevDataSet(Dev, SequenceConfig, data);
		if (data & (0x01 << 7))
			*pWrapAroundCheckEnable = 0x01;
		else
			*pWrapAroundCheckEnable = 0x00;
	}
	if (Status == VL53L0_ERROR_NONE) {
		VL53L0_SETPARAMETERFIELD(Dev, WrapAroundCheckEnable,
			*pWrapAroundCheckEnable);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetDmaxCalParameters(VL53L0_DEV Dev,
	uint16_t RangeMilliMeter, FixPoint1616_t SignalRateRtnMegaCps,
	FixPoint1616_t DmaxCalBlindAmbient)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	PALDevDataSet(Dev, DmaxCalRangeMilliMeter, RangeMilliMeter);
	PALDevDataSet(Dev, DmaxCalSignalRateRtnMegaCps, SignalRateRtnMegaCps);
	PALDevDataSet(Dev, DmaxCalBlindAmbient, DmaxCalBlindAmbient);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetDmaxCalParameters(VL53L0_DEV Dev,
	uint16_t *pRangeMilliMeter, FixPoint1616_t *pSignalRateRtnMegaCps,
	FixPoint1616_t *pDmaxCalBlindAmbient)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	*pRangeMilliMeter = PALDevDataGet(Dev, DmaxCalRangeMilliMeter);
	*pSignalRateRtnMegaCps = PALDevDataGet(Dev,
		DmaxCalSignalRateRtnMegaCps);
	*pDmaxCalBlindAmbient = PALDevDataGet(Dev, DmaxCalBlindAmbient);

	LOG_FUNCTION_END(Status);
	return Status;
}

/* End Group PAL Parameters Functions */

/* Group PAL Measurement Functions */
VL53L0_Error VL53L0_PerformSingleMeasurement(VL53L0_DEV Dev)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	VL53L0_DeviceModes DeviceMode;

	LOG_FUNCTION_START("");

	/* Get Current DeviceMode */
	Status = VL53L0_GetDeviceMode(Dev, &DeviceMode);

	/* Start immediately to run a single ranging measurement in case of
	 * single ranging or single histogram */
	if (Status == VL53L0_ERROR_NONE
		&& DeviceMode == VL53L0_DEVICEMODE_SINGLE_RANGING)
		Status = VL53L0_StartMeasurement(Dev);


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_measurement_poll_for_completion(Dev);


	/* Change PAL State in case of single ranging or single histogram */
	if (Status == VL53L0_ERROR_NONE
		&& DeviceMode == VL53L0_DEVICEMODE_SINGLE_RANGING)
		PALDevDataSet(Dev, PalState, VL53L0_STATE_IDLE);


	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_PerformSingleHistogramMeasurement(VL53L0_DEV Dev,
	VL53L0_HistogramMeasurementData_t *pHistogramMeasurementData)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_perform_single_histogram_measurement(Dev,
		pHistogramMeasurementData);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_PerformRefCalibration(VL53L0_DEV Dev, uint8_t *pVhvSettings,
	uint8_t *pPhaseCal)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_perform_ref_calibration(Dev, pVhvSettings,
		pPhaseCal, 1);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_PerformXTalkCalibration(VL53L0_DEV Dev,
	FixPoint1616_t XTalkCalDistance,
	FixPoint1616_t *pXTalkCompensationRateMegaCps)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_perform_xtalk_calibration(Dev, XTalkCalDistance,
		pXTalkCompensationRateMegaCps);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_PerformOffsetCalibration(VL53L0_DEV Dev,
	FixPoint1616_t CalDistanceMilliMeter, int32_t *pOffsetMicroMeter)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_perform_offset_calibration(Dev, CalDistanceMilliMeter,
		pOffsetMicroMeter);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_StartMeasurement(VL53L0_DEV Dev)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	VL53L0_DeviceModes DeviceMode;
	uint8_t Byte;
	uint8_t StartStopByte = VL53L0_REG_SYSRANGE_MODE_START_STOP;
	uint32_t LoopNb;
	LOG_FUNCTION_START("");

	/* Get Current DeviceMode */
	VL53L0_GetDeviceMode(Dev, &DeviceMode);

	switch (DeviceMode) {
	case VL53L0_DEVICEMODE_SINGLE_RANGING:
		Status = VL53L0_WrByte(Dev,
		VL53L0_REG_SYSRANGE_START,
		VL53L0_REG_SYSRANGE_MODE_SINGLESHOT | StartStopByte);
		break;
	case VL53L0_DEVICEMODE_CONTINUOUS_RANGING:
		/* Back-to-back mode */
		Status = VL53L0_WrByte(Dev,
		VL53L0_REG_SYSRANGE_START,
		VL53L0_REG_SYSRANGE_MODE_BACKTOBACK | StartStopByte);
		if (Status == VL53L0_ERROR_NONE) {
			/* Set PAL State to Running */
			PALDevDataSet(Dev, PalState, VL53L0_STATE_RUNNING);
		}
		break;
	case VL53L0_DEVICEMODE_CONTINUOUS_TIMED_RANGING:
		/* Continuous mode */
		Status = VL53L0_WrByte(Dev,
		VL53L0_REG_SYSRANGE_START,
		VL53L0_REG_SYSRANGE_MODE_TIMED | StartStopByte);
		if (Status == VL53L0_ERROR_NONE) {
			/* Set PAL State to Running */
			PALDevDataSet(Dev, PalState, VL53L0_STATE_RUNNING);
		}
		break;
	default:
		/* Selected mode not supported */
		Status = VL53L0_ERROR_MODE_NOT_SUPPORTED;
	}

	Byte = StartStopByte;
	if (Status == VL53L0_ERROR_NONE) {
		/* Wait until start bit has been cleared */
		LoopNb = 0;
		do {
			if (LoopNb > 0)
				Status = VL53L0_RdByte(Dev,
				VL53L0_REG_SYSRANGE_START, &Byte);
			LoopNb = LoopNb + 1;
		} while (((Byte & StartStopByte) == StartStopByte)
			&& (Status == VL53L0_ERROR_NONE)
			&& (LoopNb < VL53L0_DEFAULT_MAX_LOOP));

		if (LoopNb >= VL53L0_DEFAULT_MAX_LOOP)
			Status = VL53L0_ERROR_TIME_OUT;

	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_StopMeasurement(VL53L0_DEV Dev)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_WrByte(Dev, VL53L0_REG_SYSRANGE_START,
	VL53L0_REG_SYSRANGE_MODE_SINGLESHOT);

	if (Status == VL53L0_ERROR_NONE) {
		/* Set PAL State to Idle */
		PALDevDataSet(Dev, PalState, VL53L0_STATE_IDLE);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetMeasurementDataReady(VL53L0_DEV Dev,
	uint8_t *pMeasurementDataReady)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t SysRangeStatusRegister;
	uint8_t InterruptConfig;
	uint32_t InterruptMask;
	LOG_FUNCTION_START("");

	InterruptConfig = VL53L0_GETDEVICESPECIFICPARAMETER(Dev,
		Pin0GpioFunctionality);

	if (InterruptConfig ==
		VL53L0_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY) {
		Status = VL53L0_GetInterruptMaskStatus(Dev, &InterruptMask);
		if (InterruptMask ==
		VL53L0_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY)
			*pMeasurementDataReady = 1;
		else
			*pMeasurementDataReady = 0;
	} else {
		Status = VL53L0_RdByte(Dev, VL53L0_REG_RESULT_RANGE_STATUS,
			&SysRangeStatusRegister);
		if (Status == VL53L0_ERROR_NONE) {
			if (SysRangeStatusRegister & 0x01)
				*pMeasurementDataReady = 1;
			else
				*pMeasurementDataReady = 0;
		}
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_WaitDeviceReadyForNewMeasurement(VL53L0_DEV Dev,
	uint32_t MaxLoop)
{
	VL53L0_Error Status = VL53L0_ERROR_NOT_IMPLEMENTED;
	LOG_FUNCTION_START("");

	/* not implemented for VL53L0 */

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_compute_dmax(VL53L0_DEV Dev, uint16_t *pDmaxVal,
	uint16_t AmbientRate)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint32_t DmaxVal;
	uint32_t DmaxVal2;
	uint32_t DmaxVal2_sqrt;
	uint32_t DmaxCalBlindAmbient97;
	uint16_t DmaxCalRangeMilliMeter;
	FixPoint1616_t DmaxCalSignalRateRtnMegaCps;
	FixPoint1616_t LimitCheckValueRIT;
	FixPoint1616_t LimitCheckValueSR;
	FixPoint1616_t MinSignalNeeded;
	FixPoint1616_t DmaxCalBlindAmbient;
	uint32_t AmbientDiff97;
	uint32_t AmbientRatio;
	uint32_t AmbientRatio_sqr;
	LOG_FUNCTION_START("");

	/* Note that AmbientRate is in format 9.7 */

	Status |= VL53L0_GetDmaxCalParameters(Dev, &DmaxCalRangeMilliMeter,
		&DmaxCalSignalRateRtnMegaCps, &DmaxCalBlindAmbient);

	DmaxCalBlindAmbient97 = VL53L0_FIXPOINT1616TOFIXPOINT97(
			DmaxCalBlindAmbient);

	/* this limit is already per spad */
	Status |= VL53L0_GetLimitCheckValue(Dev,
	VL53L0_CHECKENABLE_RANGE_IGNORE_THRESHOLD, &LimitCheckValueRIT);

	Status |= VL53L0_GetLimitCheckValue(Dev,
	VL53L0_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, &LimitCheckValueSR);

	/* MinSignalNeeded=Max(SignalLimit,RIT-XTalkComp) */
	if (LimitCheckValueSR > LimitCheckValueRIT)
		MinSignalNeeded = LimitCheckValueSR;
	else
		MinSignalNeeded = LimitCheckValueRIT;


	if ((MinSignalNeeded > 0)
		&& (DmaxCalSignalRateRtnMegaCps >= 0)
		&& (AmbientRate < DmaxCalBlindAmbient97)) {

		/* multiply by 65536 to give more precision */
		AmbientDiff97 = (DmaxCalBlindAmbient97
					- AmbientRate) ;

		AmbientRatio = (uint32_t)((65536 * AmbientDiff97)
				/ DmaxCalBlindAmbient97);

		/* The following value is x 16 */
		AmbientRatio_sqr = VL53L0_isqrt(AmbientRatio);

		/* sqrt(16^2*x) = 16*sqrt(x) need to divide by 16 */
		DmaxVal2_sqrt = VL53L0_isqrt((256 *
			DmaxCalSignalRateRtnMegaCps) / MinSignalNeeded);

		DmaxVal2 = DmaxVal2_sqrt * AmbientRatio_sqr;


	} else
		DmaxVal2 = 0;


	DmaxVal = DmaxCalRangeMilliMeter * DmaxVal2;

	/* need to divide by 4096 = 16 * 256 */
	*pDmaxVal = (uint16_t)(DmaxVal / 4096);

	LOG_FUNCTION_END(Status);
	return Status;
}



VL53L0_Error VL53L0_GetRangingMeasurementData(VL53L0_DEV Dev,
	VL53L0_RangingMeasurementData_t *pRangingMeasurementData)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t DeviceRangeStatus;
	uint8_t RangeFractionalEnable;
	uint8_t PalRangeStatus;
	uint8_t XTalkCompensationEnable;
	uint16_t AmbientRate;
	FixPoint1616_t SignalRate;
	uint16_t XTalkCompensationRateMegaCps;
	uint16_t EffectiveSpadRtnCount;
	uint16_t tmpuint16;
	uint16_t XtalkRangeMilliMeter;
	uint16_t LinearityCorrectiveGain;
	uint8_t localBuffer[12];
	VL53L0_RangingMeasurementData_t LastRangeDataBuffer;

	uint16_t DmaxVal16;

	LOG_FUNCTION_START("");

	/*
	 * use multi read even if some registers are not useful, result will
	 * be more efficient
	 * start reading at 0x14 dec20
	 * end reading at 0x21 dec33 total 14 bytes to read
	 */
	Status = VL53L0_ReadMulti(Dev, 0x14, localBuffer, 12);

	if (Status == VL53L0_ERROR_NONE) {

		pRangingMeasurementData->ZoneId = 0; /* Only one zone */
		pRangingMeasurementData->TimeStamp = 0; /* Not Implemented */

		tmpuint16 = VL53L0_MAKEUINT16(localBuffer[11], localBuffer[10]);
		/* cut1.1 if SYSTEM__RANGE_CONFIG if 1 range is 2bits fractional
		 *(format 11.2) else no fractional
		 */

		pRangingMeasurementData->MeasurementTimeUsec = 0;

		SignalRate = VL53L0_FIXPOINT97TOFIXPOINT1616(
			VL53L0_MAKEUINT16(localBuffer[7], localBuffer[6]));
		/* peak_signal_count_rate_rtn_mcps */
		pRangingMeasurementData->SignalRateRtnMegaCps = SignalRate;

		AmbientRate = VL53L0_MAKEUINT16(localBuffer[9], localBuffer[8]);
		pRangingMeasurementData->AmbientRateRtnMegaCps =
			VL53L0_FIXPOINT97TOFIXPOINT1616(AmbientRate);

		EffectiveSpadRtnCount = VL53L0_MAKEUINT16(localBuffer[3],
			localBuffer[2]);
		/* EffectiveSpadRtnCount is 8.8 format */
		pRangingMeasurementData->EffectiveSpadRtnCount =
			EffectiveSpadRtnCount;

		DeviceRangeStatus = localBuffer[0];

		/* Get Linearity Corrective Gain */
		LinearityCorrectiveGain = PALDevDataGet(Dev,
			LinearityCorrectiveGain);

		/* Get ranging configuration */
		RangeFractionalEnable = PALDevDataGet(Dev,
			RangeFractionalEnable);

		if (LinearityCorrectiveGain != 1000) {

			tmpuint16 = (uint16_t)((LinearityCorrectiveGain
				* tmpuint16 + 500) / 1000);

			/* Implement Xtalk */
			VL53L0_GETPARAMETERFIELD(Dev,
				XTalkCompensationRateMegaCps,
				XTalkCompensationRateMegaCps);
			VL53L0_GETPARAMETERFIELD(Dev, XTalkCompensationEnable,
				XTalkCompensationEnable);

			if (XTalkCompensationEnable) {

				if ((SignalRate
					- ((XTalkCompensationRateMegaCps
					* EffectiveSpadRtnCount) >> 8))
					<= 0) {
					if (RangeFractionalEnable)
						XtalkRangeMilliMeter = 8888;
					else
						XtalkRangeMilliMeter = 8888
							<< 2;
				} else {
					XtalkRangeMilliMeter =
					(tmpuint16 * SignalRate)
						/ (SignalRate
						- ((XTalkCompensationRateMegaCps
						* EffectiveSpadRtnCount)
						>> 8));
				}

				tmpuint16 = XtalkRangeMilliMeter;
			}

		}

		if (RangeFractionalEnable) {
			pRangingMeasurementData->RangeMilliMeter =
				(uint16_t)((tmpuint16) >> 2);
			pRangingMeasurementData->RangeFractionalPart =
				(uint8_t)((tmpuint16 & 0x03) << 6);
		} else {
			pRangingMeasurementData->RangeMilliMeter = tmpuint16;
			pRangingMeasurementData->RangeFractionalPart = 0;
		}

		/* Dmax Computation */
		Status |= VL53L0_compute_dmax(Dev, &DmaxVal16, AmbientRate);

		pRangingMeasurementData->RangeDMaxMilliMeter = DmaxVal16;

		/*
		 * For a standard definition of RangeStatus, this should
		 * return 0 in case of good result after a ranging
		 * The range status depends on the device so call a device
		 * specific function to obtain the right Status.
		 */
		Status |= VL53L0_get_pal_range_status(Dev, DeviceRangeStatus,
			SignalRate, EffectiveSpadRtnCount,
			pRangingMeasurementData, &PalRangeStatus);

		if (Status == VL53L0_ERROR_NONE)
			pRangingMeasurementData->RangeStatus = PalRangeStatus;

	}

	if (Status == VL53L0_ERROR_NONE) {
		/* Copy last read data into Dev buffer */
		LastRangeDataBuffer = PALDevDataGet(Dev, LastRangeMeasure);

		LastRangeDataBuffer.RangeMilliMeter =
			pRangingMeasurementData->RangeMilliMeter;
		LastRangeDataBuffer.RangeFractionalPart =
			pRangingMeasurementData->RangeFractionalPart;
		LastRangeDataBuffer.RangeDMaxMilliMeter =
			pRangingMeasurementData->RangeDMaxMilliMeter;
		LastRangeDataBuffer.MeasurementTimeUsec =
			pRangingMeasurementData->MeasurementTimeUsec;
		LastRangeDataBuffer.SignalRateRtnMegaCps =
			pRangingMeasurementData->SignalRateRtnMegaCps;
		LastRangeDataBuffer.AmbientRateRtnMegaCps =
			pRangingMeasurementData->AmbientRateRtnMegaCps;
		LastRangeDataBuffer.EffectiveSpadRtnCount =
			pRangingMeasurementData->EffectiveSpadRtnCount;
		LastRangeDataBuffer.RangeStatus =
			pRangingMeasurementData->RangeStatus;

		PALDevDataSet(Dev, LastRangeMeasure, LastRangeDataBuffer);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetMeasurementRefSignal(VL53L0_DEV Dev,
	FixPoint1616_t *pMeasurementRefSignal)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	*pMeasurementRefSignal = PALDevDataGet(Dev, LastSignalRefMcps);

	LOG_FUNCTION_END(Status);
	return Status;

}

VL53L0_Error VL53L0_GetHistogramMeasurementData(VL53L0_DEV Dev,
	VL53L0_HistogramMeasurementData_t *pHistogramMeasurementData)
{
	VL53L0_Error Status = VL53L0_ERROR_NOT_IMPLEMENTED;
	LOG_FUNCTION_START("");

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_PerformSingleRangingMeasurement(VL53L0_DEV Dev,
	VL53L0_RangingMeasurementData_t *pRangingMeasurementData)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	/* This function will do a complete single ranging
	 * Here we fix the mode! */
	Status = VL53L0_SetDeviceMode(Dev, VL53L0_DEVICEMODE_SINGLE_RANGING);

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_PerformSingleMeasurement(Dev);


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_GetRangingMeasurementData(Dev,
			pRangingMeasurementData);


	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_ClearInterruptMask(Dev, 0);


	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetNumberOfROIZones(VL53L0_DEV Dev,
	uint8_t NumberOfROIZones)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	if (NumberOfROIZones != 1)
		Status = VL53L0_ERROR_INVALID_PARAMS;


	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetNumberOfROIZones(VL53L0_DEV Dev,
	uint8_t *pNumberOfROIZones)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	*pNumberOfROIZones = 1;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetMaxNumberOfROIZones(VL53L0_DEV Dev,
	uint8_t *pMaxNumberOfROIZones)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;

	LOG_FUNCTION_START("");

	*pMaxNumberOfROIZones = 1;

	LOG_FUNCTION_END(Status);
	return Status;
}

/* End Group PAL Measurement Functions */

VL53L0_Error VL53L0_SetGpioConfig(VL53L0_DEV Dev, uint8_t Pin,
	VL53L0_DeviceModes DeviceMode, VL53L0_GpioFunctionality Functionality,
	VL53L0_InterruptPolarity Polarity)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t data;

	LOG_FUNCTION_START("");

	if (Pin != 0) {
		Status = VL53L0_ERROR_GPIO_NOT_EXISTING;
	} else if (DeviceMode == VL53L0_DEVICEMODE_GPIO_DRIVE) {
		if (Polarity == VL53L0_INTERRUPTPOLARITY_LOW)
			data = 0x10;
		else
			data = 1;

		Status = VL53L0_WrByte(Dev,
		VL53L0_REG_GPIO_HV_MUX_ACTIVE_HIGH, data);

	} else if (DeviceMode == VL53L0_DEVICEMODE_GPIO_OSC) {

		Status |= VL53L0_WrByte(Dev, 0xff, 0x01);
		Status |= VL53L0_WrByte(Dev, 0x00, 0x00);

		Status |= VL53L0_WrByte(Dev, 0xff, 0x00);
		Status |= VL53L0_WrByte(Dev, 0x80, 0x01);
		Status |= VL53L0_WrByte(Dev, 0x85, 0x02);

		Status |= VL53L0_WrByte(Dev, 0xff, 0x04);
		Status |= VL53L0_WrByte(Dev, 0xcd, 0x00);
		Status |= VL53L0_WrByte(Dev, 0xcc, 0x11);

		Status |= VL53L0_WrByte(Dev, 0xff, 0x07);
		Status |= VL53L0_WrByte(Dev, 0xbe, 0x00);

		Status |= VL53L0_WrByte(Dev, 0xff, 0x06);
		Status |= VL53L0_WrByte(Dev, 0xcc, 0x09);

		Status |= VL53L0_WrByte(Dev, 0xff, 0x00);
		Status |= VL53L0_WrByte(Dev, 0xff, 0x01);
		Status |= VL53L0_WrByte(Dev, 0x00, 0x00);

	} else {

		if (Status == VL53L0_ERROR_NONE) {
			switch (Functionality) {
			case VL53L0_GPIOFUNCTIONALITY_OFF:
				data = 0x00;
				break;
			case VL53L0_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW:
				data = 0x01;
				break;
			case VL53L0_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH:
				data = 0x02;
				break;
			case VL53L0_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT:
				data = 0x03;
				break;
			case VL53L0_GPIOFUNCTIONALITY_NEW_MEASURE_READY:
				data = 0x04;
				break;
			default:
				Status =
				VL53L0_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED;
			}
		}

		if (Status == VL53L0_ERROR_NONE)
			Status = VL53L0_WrByte(Dev,
			VL53L0_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, data);

		if (Status == VL53L0_ERROR_NONE) {
			if (Polarity == VL53L0_INTERRUPTPOLARITY_LOW)
				data = 0;
			else
				data = (uint8_t)(1 << 4);

			Status = VL53L0_UpdateByte(Dev,
			VL53L0_REG_GPIO_HV_MUX_ACTIVE_HIGH, 0xEF, data);
		}

		if (Status == VL53L0_ERROR_NONE)
			VL53L0_SETDEVICESPECIFICPARAMETER(Dev,
				Pin0GpioFunctionality, Functionality);

		if (Status == VL53L0_ERROR_NONE)
			Status = VL53L0_ClearInterruptMask(Dev, 0);

	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetGpioConfig(VL53L0_DEV Dev, uint8_t Pin,
	VL53L0_DeviceModes *pDeviceMode,
	VL53L0_GpioFunctionality *pFunctionality,
	VL53L0_InterruptPolarity *pPolarity)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	VL53L0_GpioFunctionality GpioFunctionality;
	uint8_t data;

	LOG_FUNCTION_START("");

	/* pDeviceMode not managed by Ewok it return the current mode */

	Status = VL53L0_GetDeviceMode(Dev, pDeviceMode);

	if (Status == VL53L0_ERROR_NONE) {
		if (Pin != 0) {
			Status = VL53L0_ERROR_GPIO_NOT_EXISTING;
		} else {
			Status = VL53L0_RdByte(Dev,
			VL53L0_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, &data);
		}
	}

	if (Status == VL53L0_ERROR_NONE) {
		switch (data & 0x07) {
		case 0x00:
			GpioFunctionality = VL53L0_GPIOFUNCTIONALITY_OFF;
			break;
		case 0x01:
			GpioFunctionality =
			VL53L0_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW;
			break;
		case 0x02:
			GpioFunctionality =
			VL53L0_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH;
			break;
		case 0x03:
			GpioFunctionality =
			VL53L0_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT;
			break;
		case 0x04:
			GpioFunctionality =
			VL53L0_GPIOFUNCTIONALITY_NEW_MEASURE_READY;
			break;
		default:
			Status = VL53L0_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED;
		}
	}

	if (Status == VL53L0_ERROR_NONE)
		Status = VL53L0_RdByte(Dev, VL53L0_REG_GPIO_HV_MUX_ACTIVE_HIGH,
			&data);

	if (Status == VL53L0_ERROR_NONE) {
		if ((data & (uint8_t)(1 << 4)) == 0)
			*pPolarity = VL53L0_INTERRUPTPOLARITY_LOW;
		else
			*pPolarity = VL53L0_INTERRUPTPOLARITY_HIGH;
	}

	if (Status == VL53L0_ERROR_NONE) {
		*pFunctionality = GpioFunctionality;
		VL53L0_SETDEVICESPECIFICPARAMETER(Dev, Pin0GpioFunctionality,
			GpioFunctionality);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetInterruptThresholds(VL53L0_DEV Dev,
	VL53L0_DeviceModes DeviceMode, FixPoint1616_t ThresholdLow,
	FixPoint1616_t ThresholdHigh)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint16_t Threshold16;
	LOG_FUNCTION_START("");

	/* no dependency on DeviceMode for Ewok */
	/* Need to divide by 2 because the FW will apply a x2 */
	Threshold16 = (uint16_t)((ThresholdLow >> 17) & 0x00fff);
	Status = VL53L0_WrWord(Dev, VL53L0_REG_SYSTEM_THRESH_LOW, Threshold16);

	if (Status == VL53L0_ERROR_NONE) {
		/* Need to divide by 2 because the FW will apply a x2 */
		Threshold16 = (uint16_t)((ThresholdHigh >> 17) & 0x00fff);
		Status = VL53L0_WrWord(Dev, VL53L0_REG_SYSTEM_THRESH_HIGH,
			Threshold16);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetInterruptThresholds(VL53L0_DEV Dev,
	VL53L0_DeviceModes DeviceMode, FixPoint1616_t *pThresholdLow,
	FixPoint1616_t *pThresholdHigh)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint16_t Threshold16;
	LOG_FUNCTION_START("");

	/* no dependency on DeviceMode for Ewok */

	Status = VL53L0_RdWord(Dev, VL53L0_REG_SYSTEM_THRESH_LOW, &Threshold16);
	/* Need to multiply by 2 because the FW will apply a x2 */
	*pThresholdLow = (FixPoint1616_t)((0x00fff & Threshold16) << 17);

	if (Status == VL53L0_ERROR_NONE) {
		Status = VL53L0_RdWord(Dev, VL53L0_REG_SYSTEM_THRESH_HIGH,
			&Threshold16);
		/* Need to multiply by 2 because the FW will apply a x2 */
		*pThresholdHigh =
			(FixPoint1616_t)((0x00fff & Threshold16) << 17);
	}

	LOG_FUNCTION_END(Status);
	return Status;
}

/* Group PAL Interrupt Functions */
VL53L0_Error VL53L0_ClearInterruptMask(VL53L0_DEV Dev, uint32_t InterruptMask)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t LoopCount;
	uint8_t Byte;
	LOG_FUNCTION_START("");

	/* clear bit 0 range interrupt, bit 1 error interrupt */
	Status = VL53L0_WrByte(Dev, VL53L0_REG_SYSTEM_INTERRUPT_CLEAR, 0x01);
	LoopCount = 0;
	do {
		VL53L0_RdByte(Dev, VL53L0_REG_RESULT_INTERRUPT_STATUS, &Byte);
		LoopCount++;
	} while (((Byte & 0x07) != 0x00) && (LoopCount < 8));
	Status = VL53L0_WrByte(Dev, VL53L0_REG_SYSTEM_INTERRUPT_CLEAR, 0x00);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetInterruptMaskStatus(VL53L0_DEV Dev,
	uint32_t *pInterruptMaskStatus)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Byte;
	LOG_FUNCTION_START("");

	Status = VL53L0_RdByte(Dev, VL53L0_REG_RESULT_INTERRUPT_STATUS, &Byte);
	*pInterruptMaskStatus = Byte & 0x07;

	if (Byte & 0x18)
		Status = VL53L0_ERROR_RANGE_ERROR;

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_EnableInterruptMask(VL53L0_DEV Dev, uint32_t InterruptMask)
{
	VL53L0_Error Status = VL53L0_ERROR_NOT_IMPLEMENTED;
	LOG_FUNCTION_START("");

	/* not implemented for VL53L0 */

	LOG_FUNCTION_END(Status);
	return Status;
}

/* End Group PAL Interrupt Functions */

/* Group SPAD functions */

VL53L0_Error VL53L0_SetSpadAmbientDamperThreshold(VL53L0_DEV Dev,
	uint16_t SpadAmbientDamperThreshold)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	VL53L0_WrByte(Dev, 0xFF, 0x01);
	Status = VL53L0_WrWord(Dev, 0x40, SpadAmbientDamperThreshold);
	VL53L0_WrByte(Dev, 0xFF, 0x00);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetSpadAmbientDamperThreshold(VL53L0_DEV Dev,
	uint16_t *pSpadAmbientDamperThreshold)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	VL53L0_WrByte(Dev, 0xFF, 0x01);
	Status = VL53L0_RdWord(Dev, 0x40, pSpadAmbientDamperThreshold);
	VL53L0_WrByte(Dev, 0xFF, 0x00);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_SetSpadAmbientDamperFactor(VL53L0_DEV Dev,
	uint16_t SpadAmbientDamperFactor)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Byte;
	LOG_FUNCTION_START("");

	Byte = (uint8_t)(SpadAmbientDamperFactor & 0x00FF);

	VL53L0_WrByte(Dev, 0xFF, 0x01);
	Status = VL53L0_WrByte(Dev, 0x42, Byte);
	VL53L0_WrByte(Dev, 0xFF, 0x00);

	LOG_FUNCTION_END(Status);
	return Status;
}

VL53L0_Error VL53L0_GetSpadAmbientDamperFactor(VL53L0_DEV Dev,
	uint16_t *pSpadAmbientDamperFactor)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	uint8_t Byte;
	LOG_FUNCTION_START("");

	VL53L0_WrByte(Dev, 0xFF, 0x01);
	Status = VL53L0_RdByte(Dev, 0x42, &Byte);
	VL53L0_WrByte(Dev, 0xFF, 0x00);
	*pSpadAmbientDamperFactor = (uint16_t)Byte;

	LOG_FUNCTION_END(Status);
	return Status;
}

/* END Group SPAD functions */

/*****************************************************************************
 * Internal functions
 *****************************************************************************/

VL53L0_Error VL53L0_SetReferenceSpads(VL53L0_DEV Dev, uint32_t count,
	uint8_t isApertureSpads)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_set_reference_spads(Dev, count, isApertureSpads);

	LOG_FUNCTION_END(Status);

	return Status;
}

VL53L0_Error VL53L0_GetReferenceSpads(VL53L0_DEV Dev, uint32_t *pSpadCount,
	uint8_t *pIsApertureSpads)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_get_reference_spads(Dev, pSpadCount, pIsApertureSpads);

	LOG_FUNCTION_END(Status);

	return Status;
}

VL53L0_Error VL53L0_PerformRefSpadManagement(VL53L0_DEV Dev,
	uint32_t *refSpadCount, uint8_t *isApertureSpads)
{
	VL53L0_Error Status = VL53L0_ERROR_NONE;
	LOG_FUNCTION_START("");

	Status = VL53L0_perform_ref_spad_management(Dev, refSpadCount,
		isApertureSpads);

	LOG_FUNCTION_END(Status);

	return Status;
}
