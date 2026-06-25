#include "Filter.h"

#define _USE_MATH_DEFINES  // To get M_PI
#include <cmath>      // For std::tan, std::tanh, M_PI
#include <algorithm>  // For std::clamp

Filter::Filter()
{
  init();
  calc();
}

Filter::~Filter() {}

void
Filter::init()
{
  // reset integrator states
  ic1eq = 0.0;
  ic2eq = 0.0;
}

void
Filter::calc()
{
  // Keep cutoff in a stable range below Nyquist (tan() blows up at fs/2).
  double fc = std::clamp(mCutoff, 10.0, 0.49 * mSampleRate);

  g = std::tan(M_PI * fc / mSampleRate);

  // Map resonance 0..1 to damping k (= 1/Q). k = 2 -> Q = 0.5 (no resonance);
  // k -> ~0 gives very high Q and self-oscillation at the top of the range.
  k = 2.0 - 1.98 * std::clamp(mRes, 0.0, 1.0);

  a1 = 1.0 / (1.0 + g * (g + k));
  a2 = g * a1;
  a3 = g * a2;
}

double
Filter::process(double input)
{
  // Drive: saturate the input. Transparent when drive == 0, increasingly
  // dirty/aggressive as it is turned up.
  if (mDrive > 0.0)
    input = std::tanh(input * (1.0 + mDrive * 9.0));

  // One TPT SVF step (Andrew Simper's trapezoidal SVF).
  double v3 = input - ic2eq;
  double v1 = a1 * ic1eq + a2 * v3;
  double v2 = ic2eq + a2 * ic1eq + a3 * v3;
  ic1eq = 2.0 * v1 - ic1eq;
  ic2eq = 2.0 * v2 - ic2eq;

  switch (mMode)
  {
    case kHighPass:
      return input - k * v1 - v2;
    case kBandPass:
      return v1;
    case kNotch:
      return input - k * v1;
    case kLowPass:
    default:
      return v2;
  }
}

void
Filter::setSampleRate(double fs)
{
  mSampleRate = fs;
  calc();
}

double
Filter::getSampleRate()
{
  return mSampleRate;
}

void
Filter::setBaseCutoff(double cutoff)
{
  mBaseCutoff = cutoff;
}

double
Filter::getBaseCutoff()
{
  return mBaseCutoff;
}

void
Filter::setCutoff(double c)
{
  mCutoff = c;
  calc();
}

double
Filter::getCutoff()
{
  return mCutoff;
}

void
Filter::setResonance(double r)
{
  mRes = r;
  calc();
}

double
Filter::getRes()
{
  return mRes;
}

void
Filter::setMode(int mode)
{
  mMode = std::clamp(mode, 0, (int)kNumModes - 1);
}

void
Filter::setDrive(double d)
{
  mDrive = std::clamp(d, 0.0, 1.0);
}
