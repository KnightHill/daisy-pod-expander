#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include "oled_exp.h"

using namespace daisy;

static OledExp hw;

static char line1[128];
static char line2[128];
static float knob1_value;
static float knob2_value;
static bool sw1;

const uint16_t ScreenWidth = 128;
static float m_scopeBuffer[ScreenWidth];
static uint16_t m_bufferIndex;

void show_info()
{
  Rectangle topRowRect(hw.display.Width(), 20);
  Rectangle secondRowRect(0, 21, hw.display.Width(), 10);
  hw.display.Fill(false);
  hw.display.WriteStringAligned(line1, Font_11x18, topRowRect, Alignment::centered, true);
  hw.display.WriteStringAligned(line2, Font_7x10, secondRowRect, Alignment::centered, true);
  hw.display.Update();
  System::Delay(1000);
}

void show_logo()
{
  sprintf(line1, "KnightHill");
  sprintf(line2, "Engineering");
  show_info();
}

void show_product()
{
  sprintf(line1, "OLED SPI Exp");
  sprintf(line2, "version 1.1");
  show_info();
}

void draw_ui(float v1, float v2, bool sw1)
{
  uint16_t center = hw.display.Height() / 2;
  uint8_t pot1 = (uint8_t)(127.0f * v1);
  uint8_t pot2 = (uint8_t)(127.0f * v2);
  sprintf(line1, "v1:%d v2:%d sw:%s", pot1, pot2, sw1 ? "on" : "off");
  hw.display.Fill(false);
  hw.display.SetCursor(0, 0);
  hw.display.WriteString(line1, Font_7x10, true);

  for (uint16_t i = 0; i < ScreenWidth; i++) {
    hw.display.DrawPixel(i, m_scopeBuffer[i] * center * knob1_value * 10 + center, true);
  }
  hw.display.Update();
}

void Callback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    m_scopeBuffer[m_bufferIndex] = (IN_L[i] + IN_R[i]) / 2.0f;
    if (++m_bufferIndex >= ScreenWidth)
      m_bufferIndex = 0;
    OUT_L[i] = IN_L[i];
    OUT_R[i] = IN_R[i];
  }
}

int main(void)
{
  knob1_value = 0;
  knob2_value = 0;
  m_bufferIndex = 0;

  for (uint16_t i = 0; i < ScreenWidth; i++) {
    m_scopeBuffer[i] = 0;
  }

  hw.Init();
  hw.SetAudioBlockSize(8);

  hw.StartAdc();

  show_logo();
  show_product();

  hw.StartAudio(Callback);

  while (1) {
    hw.ProcessDigitalControls();

    knob1_value = hw.knob3.Process();
    knob2_value = hw.knob4.Process();

    if (hw.button3.RisingEdge()) {
      sw1 = !sw1;
    }

    draw_ui(knob1_value, knob2_value, sw1);

    // System::Delay(5);
  }
}
