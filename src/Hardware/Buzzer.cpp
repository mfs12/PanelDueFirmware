/*
 * Buzzer.cpp
 *
 * Created: 13/11/2014 22:56:18
 *  Author: David
 * The piezo sounder is connected to the complementary outputs of PWM channel 0, PWMH0 and PWML0, aka PB0 peripheral A and PB5 peripheral B.
 * The backlight control is included in this module because it also uses PWM. Output PWMH1 (aka PB1 peripheral A) drives the backlight pin.
 */

#include "ecv.h"
#undef array
#undef result
#undef value
#include "asf.h"
#include "stdint.h"
#include "Buzzer.hpp"
#include "SysTick.hpp"
#include "Configuration.hpp"

namespace Buzzer
{
	static const uint32_t pwmClockFrequency = 2000000;		// 2MHz clock (OK down to 30Hz PWM frequency)

#if IS_ER
	static const uint32_t backlightPwmFrequency = 20000;	// 20kHz is recommend by East Rising
#else
	static const uint32_t backlightPwmFrequency = 300;		// Working range is about 100Hz to 1KHz. MP3202 dataseet says use 1kHz or below due to soft start. Some frequencies causes flickering on the 4.3" display.
#endif

	static const uint32_t backlightPeriod = pwmClockFrequency/backlightPwmFrequency;

	static pwm_channel_t buzzer_pwm_channel_instance =
	{
		.channel = 0,
		.ul_prescaler = PWM_CMR_CPRE_CLKA,
		.alignment = PWM_ALIGN_LEFT
//		.b_deadtime_generator = false,
//		.b_pwmh_output_inverted = false,
//		.b_pwml_output_inverted = false,
//		.b_sync_ch = false,
//		.counter_event = 0,
//		.fault_id = (pwm_fault_id_t)0,
//		.output_selection = 0,
//		.polarity = 0,
//		.ul_duty = 0,
//		.ul_fault_output_pwmh = 0,
//		.ul_fault_output_pwml = 0,
//		.ul_period = 0,
//		.us_deadtime_pwml = 0,
//		.us_deadtime_pwmh = 0
	};
	static pwm_channel_t backlight_pwm_channel_instance =
	{
		.channel = 1,
		.ul_prescaler = PWM_CMR_CPRE_CLKA,
		.alignment = PWM_ALIGN_LEFT
//		.b_deadtime_generator = false,
//		.b_pwmh_output_inverted = false,
//		.b_pwml_output_inverted = false,
//		.b_sync_ch = false,
//		.counter_event = 0,
//		.fault_id = (pwm_fault_id_t)0,
//		.output_selection = 0,
//		.polarity = 0,
//		.ul_duty = 0,
//		.ul_fault_output_pwmh = 0,
//		.ul_fault_output_pwml = 0,
//		.ul_period = 0,
//		.us_deadtime_pwml = 0,
//		.us_deadtime_pwmh = 0
	};
	static uint32_t beepTicksToGo = 0;
	static bool inBuzzer = true;

	// Initialize the buzzer and the PWM system. Must be called before using the buzzer or backlight.
	void Init()
	{
		pwm_channel_disable(PWM, PWM_CHANNEL_0);					// make sure buzzer PWM is off
		pwm_channel_disable(PWM, PWM_CHANNEL_1);					// make sure backlight PWM is off
		pwm_clock_t clock_setting =
		{
			.ul_clka = pwmClockFrequency,
			.ul_clkb = 0,
			.ul_mck = SystemCoreClock
		};
		pwm_init(PWM, &clock_setting);								// set up the PWM clock
		pio_configure(PIOB, PIO_PERIPH_A, PIO_PB1, 0);				// enable HI output to backlight, but not to piezo yet
		pio_configure(PIOB, PIO_OUTPUT_0, PIO_PB0 | PIO_PB5, 0);	// set both piezo pins low

		beepTicksToGo = 0;
		inBuzzer = false;
	}

	static const uint32_t volumeTable[MaxVolume] = { 3, 9, 20, 40, 80 };

	// Generate a beep of the given length and frequency. The volume goes from 0 to MaxVolume.
	void Beep(uint32_t frequency, uint32_t ms, uint32_t volume)
	{
		if (volume != 0)
		{
			if (volume > MaxVolume)
			{
				volume = MaxVolume;
			}

			inBuzzer = true;		// tell the tick interrupt to leave us alone
			if (beepTicksToGo == 0)
			{
				uint32_t period = pwmClockFrequency/frequency;
				// To get the maximum fundamental component, we want the dead time to be 1/6 of the period.
				// Larger dead times reduce the volume, at the expense of generating more high harmonics.
				uint32_t onTime = (period * volumeTable[volume - 1])/200;
				uint16_t deadTime = period/2 - onTime;
				buzzer_pwm_channel_instance.ul_period = period;
				buzzer_pwm_channel_instance.ul_duty = period/2;
				pwm_channel_init(PWM, &buzzer_pwm_channel_instance);
				PWM->PWM_CH_NUM[PWM_CHANNEL_0].PWM_CMR |= PWM_CMR_DTE;
				PWM->PWM_CH_NUM[PWM_CHANNEL_0].PWM_DT = (deadTime << 16) | deadTime;
				PWM->PWM_CH_NUM[PWM_CHANNEL_0].PWM_DTUPD = (deadTime << 16) | deadTime;
				pwm_channel_enable(PWM, PWM_CHANNEL_0);
				pio_configure(PIOB, PIO_PERIPH_A, PIO_PB0, 0);				// enable HI PWM output to piezo
				pio_configure(PIOB, PIO_PERIPH_B, PIO_PB5, 0);				// enable LO PWM output to piezo
				beepTicksToGo = ms;
			}
			inBuzzer = false;
		}
	}

	// This is called from the tick ISR
	void Tick()
	{
		if (!inBuzzer && beepTicksToGo != 0)
		{
			--beepTicksToGo;
			if (beepTicksToGo == 0)
			{
				// Turn the buzzer off
				pwm_channel_disable(PWM, PWM_CHANNEL_0);
				pio_configure(PIOB, PIO_OUTPUT_0, PIO_PB0 | PIO_PB5, 0);		// set both piezo pins low to silence the piezo
			}
		}
	}

	// Return true if the buzzer is (or should be) still sounding
	bool Noisy()
	{
		return beepTicksToGo != 0;
	}

	struct backlight {
		uint32_t frequency;
		uint32_t period;
		uint32_t channel;
		uint32_t dimBrightness;
		uint32_t minBrightness;
		uint32_t maxBrightness;
		enum {
			BacklightStateDimmed = (1 << 0)
		} state;
		pwm_channel_t *pwm;

	};

	void BacklightInit(struct backlight *backlight, pwm_channel_t *pwm, uint32_t frequency, uint32_t dimBrightness, uint32_t minBrightness, uint32_t maxBrightness)
	{
		//assert(backlight);
		//assert(channel);
		//assert(pwmClockFrequency >= frequency);

		//backlight->pwm = pwm;
		backlight->pwm = &backlight_pwm_channel_instance;

		backlight->frequency = frequency;
		backlight->period = pwmClockFrequency / frequency - 1;

		backlight->dimBrightness = dimBrightness;
		backlight->minBrightness = minBrightness;
		backlight->maxBrightness = maxBrightness;

		backlight->pwm->ul_period = backlight->period;
		backlight->pwm->ul_duty = 0;

		pwm_channel_init(PWM, backlight->pwm);
		pwm_channel_disable(PWM, backlight->pwm->channel);
	}

	// Set the backlight brightness on a scale of 0 to MaxBrightness.
	// Must call Init before calling this.
	void SetBacklight(struct backlight *backlight, uint32_t brightness)
	{
		//assert(backlight);
		//assert(backlight->pwm);
		//assert(backlight->maxBrightness >= brightness);

		if (backlight->dimBrightness != brightness)
		{
			backlight->state &= ~BacklightStateDimmed;
		}

		backlight->pwm->ul_period = backlight->period;
		backlight->pwm->ul_duty =
			(backlight->period * (backlight->maxBrightness - brightness)) / backlight->maxBrightness;

		pwm_channel_init(PWM, backlight->pwm);
		pwm_channel_enable(PWM, backlight->pwm->channel);
	}

	void DimBrightness(struct backlight *backlight)
	{
		SetBacklight(backlight, backlight->dimBrightness);
		backlight->state |= BacklightStateDimmed;
	}
}

// End
