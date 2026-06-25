#pragma once

// Multi-mode State Variable Filter (TPT / trapezoidal-integrated, after
// Andrew Simper / Vadim Zavalishin). Selectable LP/HP/BP/Notch response,
// resonance that can self-oscillate, and an input drive stage.
class Filter
{
public:
  enum EMode
  {
    kLowPass = 0,
    kHighPass,
    kBandPass,
    kNotch,
    kNumModes
  };

  Filter();
  ~Filter();

  void init();

  double process(double input);

  void setSampleRate(double fs);
  double getSampleRate();

  void setBaseCutoff(double cutoff);
  double getBaseCutoff();

  void setCutoff(double c);
  double getCutoff();

  void setResonance(double r);  // 0..1
  double getRes();

  void setMode(int mode);   // EMode
  void setDrive(double d);  // 0..1

private:
  void calc();

  double mSampleRate = 44100.0;
  double mBaseCutoff = 1000.0;  // knob value before envelope modulation
  double mCutoff = 1000.0;      // running cutoff actually used
  double mRes = 0.0;            // 0..1
  double mDrive = 0.0;          // 0..1
  int mMode = kLowPass;

  // TPT coefficients
  double g = 0.0;
  double k = 2.0;
  double a1 = 0.0, a2 = 0.0, a3 = 0.0;

  // integrator states
  double ic1eq = 0.0;
  double ic2eq = 0.0;
};
