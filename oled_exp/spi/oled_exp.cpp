#include "dev/oled_ssd130x.h"
#include "oled_exp.h"

#ifndef SAMPLE_RATE
// #define SAMPLE_RATE DSY_AUDIO_SAMPLE_RATE
#define SAMPLE_RATE 48014.f
#endif

using namespace daisy;

// # Rev3 and Rev4 with newest pinout.
// Compatible with Seed Rev3 and Rev4
constexpr Pin SW_1_PIN = seed::D27;
constexpr Pin SW_2_PIN = seed::D28;
constexpr Pin SW_3_PIN = seed::D12;

constexpr Pin ENC_A_PIN = seed::D26;
constexpr Pin ENC_B_PIN = seed::D25;
constexpr Pin ENC_CLICK_PIN = seed::D13;

constexpr Pin LED_1_R_PIN = seed::D20;
constexpr Pin LED_1_G_PIN = seed::D19;
constexpr Pin LED_1_B_PIN = seed::D18;
constexpr Pin LED_2_R_PIN = seed::D17;
constexpr Pin LED_2_G_PIN = seed::D24;
constexpr Pin LED_2_B_PIN = seed::D23;

constexpr Pin KNOB_1_PIN = seed::D21;
constexpr Pin KNOB_2_PIN = seed::D15;
constexpr Pin KNOB_3_PIN = seed::D16;
constexpr Pin KNOB_4_PIN = seed::D22;

constexpr Pin OLED_DC_PIN = seed::D9;
constexpr Pin OLED_RESET_PIN = seed::D11;

using namespace daisy;

void OledExp::Init(bool boost)
{
  // Set Some numbers up for accessors.
  // Initialize the hardware.
  seed.Configure();
  seed.Init(boost);
  InitButtons();
  InitEncoder();
  InitLeds();
  InitKnobs();
  InitMidi();
  InitDisplay();
  SetAudioBlockSize(48);
}

void OledExp::DelayMs(size_t del) { seed.DelayMs(del); }

void OledExp::SetHidUpdateRates()
{
  for (int i = 0; i < KNOB_LAST; i++) {
    knobs[i]->SetSampleRate(AudioCallbackRate());
  }
}

void OledExp::StartAudio(AudioHandle::InterleavingAudioCallback cb) { seed.StartAudio(cb); }

void OledExp::StartAudio(AudioHandle::AudioCallback cb) { seed.StartAudio(cb); }

void OledExp::ChangeAudioCallback(AudioHandle::InterleavingAudioCallback cb) { seed.ChangeAudioCallback(cb); }

void OledExp::ChangeAudioCallback(AudioHandle::AudioCallback cb) { seed.ChangeAudioCallback(cb); }

void OledExp::StopAudio() { seed.StopAudio(); }

void OledExp::SetAudioBlockSize(size_t size)
{
  seed.SetAudioBlockSize(size);
  SetHidUpdateRates();
}

size_t OledExp::AudioBlockSize() { return seed.AudioBlockSize(); }

void OledExp::SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate)
{
  seed.SetAudioSampleRate(samplerate);
  SetHidUpdateRates();
}

float OledExp::AudioSampleRate() { return seed.AudioSampleRate(); }

float OledExp::AudioCallbackRate() { return seed.AudioCallbackRate(); }

void OledExp::StartAdc() { seed.adc.Start(); }

void OledExp::StopAdc() { seed.adc.Stop(); }

void OledExp::ProcessAnalogControls()
{
  knob1.Process();
  knob2.Process();
  knob3.Process();
  knob4.Process();
}

float OledExp::GetKnobValue(Knob k)
{
  size_t idx;
  idx = k < KNOB_LAST ? k : KNOB_1;
  return knobs[idx]->Value();
}

void OledExp::ProcessDigitalControls()
{
  encoder.Debounce();
  button1.Debounce();
  button2.Debounce();
  button3.Debounce();
}

void OledExp::ClearLeds()
{
  // Using Color
  Color c;
  c.Init(Color::PresetColor::OFF);
  led1.SetColor(c);
  led2.SetColor(c);
  // Without
  // led1.Set(0.0f, 0.0f, 0.0f);
  // led2.Set(0.0f, 0.0f, 0.0f);
}

void OledExp::UpdateLeds()
{
  led1.Update();
  led2.Update();
}

void OledExp::InitButtons()
{
  button1.Init(SW_1_PIN);
  button2.Init(SW_2_PIN);
  button3.Init(SW_3_PIN);

  buttons[BUTTON_1] = &button1;
  buttons[BUTTON_2] = &button2;
  buttons[BUTTON_3] = &button3;
}

void OledExp::InitEncoder() { encoder.Init(ENC_A_PIN, ENC_B_PIN, ENC_CLICK_PIN); }

void OledExp::InitLeds()
{
  led1.Init(LED_1_R_PIN, LED_1_G_PIN, LED_1_B_PIN, true);
  led2.Init(LED_2_R_PIN, LED_2_G_PIN, LED_2_B_PIN, true);

  ClearLeds();
  UpdateLeds();
}

void OledExp::InitKnobs()
{
  // Configure the ADC channels using the desired pin
  AdcChannelConfig knob_init[KNOB_LAST];
  knob_init[KNOB_1].InitSingle(KNOB_1_PIN);
  knob_init[KNOB_2].InitSingle(KNOB_2_PIN);
  knob_init[KNOB_3].InitSingle(KNOB_3_PIN);
  knob_init[KNOB_4].InitSingle(KNOB_4_PIN);
  // Initialize with the knob init struct w/ 2 members
  // Set Oversampling to 32x
  seed.adc.Init(knob_init, KNOB_LAST);
  // Make an array of pointers to the knobs.
  knobs[KNOB_1] = &knob1;
  knobs[KNOB_2] = &knob2;
  knobs[KNOB_3] = &knob3;
  knobs[KNOB_4] = &knob4;
  for (int i = 0; i < KNOB_LAST; i++) {
    knobs[i]->Init(seed.adc.GetPtr(i), seed.AudioCallbackRate());
  }
}

void OledExp::InitMidi()
{
  MidiUartHandler::Config midi_config;
  midi.Init(midi_config);
}

void OledExp::InitDisplay()
{
  MyOledDisplay::Config disp_cfg;
  disp_cfg.driver_config.transport_config.pin_config.dc = OLED_DC_PIN;
  disp_cfg.driver_config.transport_config.pin_config.reset = OLED_RESET_PIN;
  display.Init(disp_cfg);
}