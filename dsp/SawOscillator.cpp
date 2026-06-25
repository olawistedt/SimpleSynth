#include "SawOscillator.h"
#include <cmath> // För fmod

// PolyBLEP: band-limited correction applied around the waveform's
// discontinuity to suppress aliasing. t is the phase [0,1), dt the phase
// increment per sample.
static double
polyBlep(double t, double dt)
{
  if (dt <= 0.0)
    return 0.0;

  if (t < dt)
  {
    t /= dt;
    return t + t - t * t - 1.0;
  }
  else if (t > 1.0 - dt)
  {
    t = (t - 1.0) / dt;
    return t * t + t + t + 1.0;
  }
  return 0.0;
}

SawOscillator::SawOscillator() {}

void SawOscillator::SetSampleRate(double sampleRate)
{
  m_sampleRate = sampleRate;
  UpdatePhaseIncrement();
}

void SawOscillator::SetFrequency(double freq)
{
  m_frequency = freq;
  UpdatePhaseIncrement();
}

void SawOscillator::UpdatePhaseIncrement()
{
  m_phaseIncrement = m_frequency / m_sampleRate;
}

void SawOscillator::ResetPhase()
{
  m_phase = 0.0;
}

double SawOscillator::Process()
{
  // Naiv stigande sågtand från -1 till 1 ...
  double sample = 2.0 * m_phase - 1.0;
  // ... bandbegränsad med PolyBLEP runt diskontinuiteten vid fasomslaget.
  sample -= polyBlep(m_phase, m_phaseIncrement);

  m_phase += m_phaseIncrement;
  m_phase = std::fmod(m_phase, 1.0); // Håll fasen mellan 0 och 1

  return sample;
}