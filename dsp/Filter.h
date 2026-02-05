#pragma once

class Filter
{
public:
  Filter();
  void init();
  void calc();
  double process(double x);
  ~Filter();
  double getCutoff();
  void setBaseCutoff(double cutoff);
  double getBaseCutoff();
  void setCutoff(double c);
  double getRes();
  void setResonance(double r);
  void setSampleRate(double fs);
  double getSampleRate();

protected:
  double mBaseCutoff;
  double cutoff;
  double res;
  double fs;
  double y1, y2, y3, y4;
  double oldx;
  double oldy1, oldy2, oldy3;
  double x;
  double r;
  double p;
  double k;
};
