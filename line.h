#ifndef LINE_H
#define LINE_H

#include "dot.h"

class Line
{
public:
    Line(Dot &begin, Dot &end);
    Dot &getBegin();
    Dot &getEnd();

private:
    Dot &begin;
    Dot &end;
};

#endif // LINE_H
