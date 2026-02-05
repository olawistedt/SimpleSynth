#include "SawOscillator.h"
#include <cmath> // För fmod

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
  double sample = 2.0 * (m_phase - std::floor(m_phase + 0.5)); // Sawtooth från -1 till 1

  m_phase += m_phaseIncrement;
  m_phase = std::fmod(m_phase, 1.0); // Håll fasen mellan 0 och 1

  return sample;
}