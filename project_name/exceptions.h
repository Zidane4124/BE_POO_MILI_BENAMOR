#pragma once
#include <iostream>
#include "Securite.h"
#include <exception>
#include <string>

class SecuriteException : public std::exception {
  // Gestion des exceptions
  private:
    std::string _msg;
  public:
    // création de l'obbjet erreur. Un throw appelle ce constructeur.
    SecuriteException(std::string msg) 
      : _msg(msg) // Liste d'initialisation : on copie 'msg' dans l'attribut privé '_msg'
    {
      // Accolades vides car travail de réalisation deja fait au dessus
    }

    // Methode what. Redefinission (override) d'une méthode qui existe deja dans la
    // classe mère (std::exception).
    virtual const char* what() const throw() override { //what : polymorphisme
        return _msg.c_str();
    }
};