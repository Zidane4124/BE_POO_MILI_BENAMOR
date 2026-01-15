#pragma once
#include <iostream>
#include "Securite.h"
#include <exception>
#include <string>

class SecuriteException : public std::exception {
  private:
    std::string _msg;
  public:
    SecuriteException(std::string msg) : _msg(msg) {}
    virtual const char* what() const throw() override { //what : polymorphisme
        return _msg.c_str();
    }
};