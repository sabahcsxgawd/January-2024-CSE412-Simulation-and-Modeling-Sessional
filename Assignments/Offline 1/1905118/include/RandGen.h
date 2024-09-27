#ifndef RandGen_h
#define RandGen_h

class RandGen {
public:
    RandGen();    
    double get(double mean);

private:
    double mean;
};

#endif // RandGen_h