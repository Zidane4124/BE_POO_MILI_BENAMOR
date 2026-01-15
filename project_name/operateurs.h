#pragma once
#include <iostream>
#include "Securite.h"
std::ostream& operator<<(std::ostream& os, Porte& p) {
    os << "=== ETAT DE LA PORTE ===\n";
    os << "Physiquement : " << (p.estOuverte() ? "OUVERTE" : "FERMEE") << "\n";
    os << "Verrouillage : " << (p.estVerrouillee() ? "VERROUILLEE" : "DEVERROUILLEE") << "\n";
    os << "========================";
    return os;
}