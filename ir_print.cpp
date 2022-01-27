/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ir.h"
#include <cstdint>
#include <iostream>

struct Printer {
    virtual void PrintOpcode(std::ostream &stm, int opc) {
        stm << "opc." << opc;
    }
    virtual void PrintInput(std::ostream &stm, int in) {
        stm << "in." << in;
    }
    virtual void PrintInfo(std::ostream &stm, int in) {
        stm << "in." << in;
    }
};


struct Inst {
    void PrintInfo(std::ostream& stm) {
        stm << "Inst ";
    }
    int id{1};
    int opcode{2};
};

struct DynInst : public Inst {

    void PrintInfo(std::ostream& stm) {
        Inst::PrintInfo(stm);
        stm << "DynInst ";
    }
    uintptr_t* inputs;
};

struct FixInst : public Inst {

    void PrintInfo(std::ostream& stm) {
        Inst::PrintInfo(stm);
        stm << "FixInst ";
    }
    uintptr_t inputs[4];
};

struct MixinA  {
    void PrintInfo(std::ostream& stm) {
        stm << "MixinA ";
    }
    uintptr_t value;
};

struct MixinB  {

    void PrintInfo(std::ostream& stm) {
        stm << "MixinB ";
    }
    char c;
};

template<typename T>
struct MixinC : public T {

    void PrintInfo(std::ostream& stm) {
        T::PrintInfo(stm);
        stm << "MixinC ";
    }
    char c;
};

template<typename InstT, typename... Args>
struct Wrapper : public InstT, Args... {
    void Print(std::ostream& stm, Printer& printer) {
        stm << InstT::id << " ";
        printer.PrintOpcode(stm, InstT::opcode);
        stm <<  " {";
        InstT::PrintInfo(stm);
        (Args::PrintInfo(stm),...);
        stm << "}\n";
    }
};

template<typename InstT, typename... Args>
void PrintAll(std::ostream& stm, InstT* inst, Printer& printer) {
    stm << inst->id << " ";
    printer.PrintOpcode(stm, inst->opcode);
    stm <<  " {";
    (static_cast<Args*>(inst)->PrintInfo(stm),...);
    inst->PrintInfo(stm);
    stm << "}\n";
};

//struct InstA : public Wrapper<MixinC<FixInst>, MixinA> {
struct InstA : public MixinC<FixInst>, public MixinA {

    void PrintInfo(std::ostream& stm) {
        //Wrapper::Print(stm);
//        MixinC<FixInst>::PrintInfo(stm);
//        MixinA::PrintInfo(stm);
        stm << "InstA";
    }

    void Print(std::ostream& stm, Printer& printer) {
        PrintAll<InstA, MixinC<FixInst>, MixinA>(stm, this, printer);
    }

    uintptr_t* inputs;
};

using tp = std::tuple<MixinA, MixinB>;

struct PrinterA : public Printer {
    void PrintOpcode(std::ostream &stm, int opc) override {
        Printer::PrintOpcode(stm, opc);
        stm << "(хуй)";
    }

};

void test_ir() {
    InstA a;
    PrinterA printer;
    a.Print(std::cerr, printer);
}
