#ifndef LOOP_H
#define LOOP_H


class Loop
{
    public:
        Loop();
        virtual ~Loop();
    bool hasToStop = false;
    void run();
};

#endif // LOOP_H
