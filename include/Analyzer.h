#ifndef ANALYZER_H
#define ANALYZER_H

class Analyzer {
private:
    double initCash;

public:
    Analyzer(double cash);
    void report(double finalCash, int shares, double lastPrice);
};

#endif
