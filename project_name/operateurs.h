#pragma once
#include <iostream>
#include "Securite.h"
std::ostream& operator<<(std::ostream& os, Porte& p) {
    // Modification de l'operateur "<<" pour pouvoir afficher dans le terminal ou
    // sur le telephone l'état d'un objet porte
    os << "=== ETAT DE LA PORTE ===\n";
    os << "Physiquement : " << (p.estOuverte() ? "OUVERTE" : "FERMEE") << "\n";
    os << "Verrouillage : " << (p.estVerrouillee() ? "VERROUILLEE" : "DEVERROUILLEE") << "\n";
    os << "========================";
    return os;
}