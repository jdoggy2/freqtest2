
#include "ti_msp_dl_config.h"
#include <math.h>
#include <stdint.h>
#include "dacINIT.h"

#define SINE_N        256u
#define SINE_AMPL     0.5f
#define MID_CODE      32768u
#define SAMPLE_DELAY  160u
#define NUM_CHANNELS  8
#define ACTUAL_FS     38000.0f

#define LUT_BITS   8u // 2^8 = 256
#define LUT_MASK   (SINE_N - 1)

/* Debug */
volatile uint32_t dbg_loopCount = 0;
volatile uint16_t dbg_lastVal0  = 0;
volatile uint16_t dbg_idx0      = 0;

static const float freqs[NUM_CHANNELS] = {
    1000.0f, 2000.0f, 3000.0f, 4000.0f,5000.0f,6000.0f,7000.0f,10000.0f  // start at 100 Hz
};

/* DDS struct */
typedef struct {
    uint32_t phase;
    uint32_t step;
} DDS_t;

static DDS_t dds[NUM_CHANNELS];

/* LUT */
static uint16_t sine_lut[SINE_N];

static void build_sine(void)
{
    for (uint32_t i = 0; i < SINE_N; i++)
    {
        float s = sinf(2.0f * 3.1415926f * (float)i / (float)SINE_N);
        float x = (float)MID_CODE + s * 32767.0f;

        if (x < 0.0f)     x = 0.0f;
        if (x > 65535.0f) x = 65535.0f;

        sine_lut[i] = (uint16_t)(x + 0.5f);
    }
}

static void init_DDS(void)
{
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        dds[i].phase = 0;
        dds[i].step  = (uint32_t)((freqs[i] * 4294967296.0f) / ACTUAL_FS);
    }
}

int main(void)
{
    SYSCFG_DL_init();

    DAC_CLR_HIGH();
    delay_cycles(32000u);
    delay_cycles(320000u);

    DAC80508_soft_reset();
    delay_cycles(320000u);

    DAC_CLR_HIGH();
    delay_cycles(32000u);

    DAC80508_write_reg(DAC80508_REG_CONFIG, 0x0000u);
    DAC80508_write_reg(DAC80508_REG_GAIN,   0x01FFu);
    DAC80508_write_reg(DAC80508_REG_SYNC,   0x0000u);

    for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++)
        dacSetChannel(ch, MID_CODE);

    build_sine();
    init_DDS();

    ledON();
    extLedON();
    delay_cycles(3200000);  // ~0.1s visible blink

   
    

    while (1)
    {
        dbg_loopCount++;

        for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++)
        {
            dds[ch].phase += dds[ch].step;
            uint16_t idx = (dds[ch].phase >> (32 - LUT_BITS)) & LUT_MASK;
            uint16_t val = sine_lut[idx];

            dacSetChannel(ch, val);

            if (ch == 0)
            {
                dbg_lastVal0 = val;
                dbg_idx0     = idx;
            }
        }

        if ((dbg_loopCount % 500) == 0)
        delay_cycles(SAMPLE_DELAY);

        extLedTOGGLE();
    }
}