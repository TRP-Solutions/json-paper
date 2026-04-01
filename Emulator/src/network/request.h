//
// Created by mikke on 01/04/2026.
//

#ifndef E_PAPER_EMULATOR_REQUEST_H
#define E_PAPER_EMULATOR_REQUEST_H
#include <string>
#include <vector>

class Command;

class Request {
public:
    static std::vector<Command> RequestConfig(std::string addr);
};


#endif //E_PAPER_EMULATOR_REQUEST_H