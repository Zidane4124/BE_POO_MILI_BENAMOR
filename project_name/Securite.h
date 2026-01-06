#ifndef SECURITE_H
#define SECURITE_H

#include <iostream>
#include <vector>
#include <string>

class Securite {
public:
    virtual ~Securite() {}
};

class CapteurMouvement : public Securite {
protected:
    int distance;
    int pinTrigger;
    int pinEcho;

public:
    CapteurMouvement(int trigger, int echo);
    int detection_mouvement();
};

#endif