#include "DetunedSawOscillator.h"
#include <numeric> // För std::accumulate (om vi vill summera)

DetunedSawOscillator::DetunedSawOscillator()
: m_gen(std::random_device()()), // Initialisera slumpgeneratorn
  m_detuneDist(-1.0, 1.0)        // Detuning från -1.0 till 1.0 (multipliceras med m_detuneAmount)
{
  SetNumUnisonVoices(1); // Standard en röst
}

void DetunedSawOscillator::SetSampleRate(double sampleRate)
{
  m_sampleRate = sampleRate;
  for (auto& osc : m_unisonOscillators)
  {
    osc.SetSampleRate(sampleRate);
  }
  UpdateUnisonFrequencies(); // Uppdatera frekvenser om samplingsfrekvensen ändras
}

void DetunedSawOscillator::SetBaseFrequency(double freq)
{
  m_baseFrequency = freq;
  UpdateUnisonFrequencies();
}

void DetunedSawOscillator::SetNumUnisonVoices(int numVoices)
{
  m_numUnisonVoices = std::max(1, numVoices); // Se till att det är minst 1 röst
  
  // Ändra storlek på vektorn av oscillatorer
  m_unisonOscillators.resize(m_numUnisonVoices);
  
  // Sätt samplingsfrekvensen för alla nya oscillatorer
  for (auto& osc : m_unisonOscillators)
  {
    osc.SetSampleRate(m_sampleRate);
  }
  UpdateUnisonFrequencies();
}

void DetunedSawOscillator::SetDetuneAmount(double detuneHz)
{
  m_detuneAmount = detuneHz;
  UpdateUnisonFrequencies();
}

void DetunedSawOscillator::UpdateUnisonFrequencies()
{
  // Fördela detuningen jämnt eller slumpmässigt
  // Här använder vi en kombination: centrera en röst, och detuna resten slumpmässigt runt den
  
  if (m_numUnisonVoices == 1)
  {
    m_unisonOscillators[0].SetFrequency(m_baseFrequency);
  }
  else
  {
    // För en jämnare spridning, detunar vi rösterna runt basfrekvensen.
    // Vi kan ha en röst på basfrekvensen, och sedan sprida de andra.
    // Alternativt, detuna alla jämnt runt basfrekvensen.
    
    // För detta exempel: den första rösten är ren, resten är slumpmässigt detunade.
    // En mer avancerad variant skulle sprida detuningen systematiskt.

    m_unisonOscillators[0].SetFrequency(m_baseFrequency); // Central röst
    
    for (int i = 1; i < m_numUnisonVoices; ++i)
    {
      double randomDetune = m_detuneDist(m_gen) * m_detuneAmount; // Slumpmässig detuning mellan -detuneAmount och +detuneAmount
      m_unisonOscillators[i].SetFrequency(m_baseFrequency + randomDetune);
    }
  }
}

double DetunedSawOscillator::Process()
{
  double mixedSample = 0.0;
  for (auto& osc : m_unisonOscillators)
  {
    mixedSample += osc.Process();
  }
  
  // Normalisera summan för att undvika klippning, beroende på antal röster
  // En enkel normalisering är att dela med antalet röster.
  return mixedSample / m_numUnisonVoices;
}