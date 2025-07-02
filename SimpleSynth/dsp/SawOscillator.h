#pragma once

#include <cmath> // För M_PI

class SawOscillator
{
public:
  SawOscillator();
  void SetSampleRate(double sampleRate);
  void SetFrequency(double freq);
  double Process();
  void ResetPhase();

private:
  double m_sampleRate = 44100.0;
  double m_frequency = 440.0;
  double m_phase = 0.0;
  double m_phaseIncrement = 0.0;

  void UpdatePhaseIncrement();
};