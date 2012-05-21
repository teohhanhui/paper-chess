#ifndef LINE_H
#define LINE_H

class Line{
public:
    Line(dot begin, dot end);
    dot getBegin();
    dot getEnd();

private:
    dot begin;
    dot end;

};

#endif // LINE_H
