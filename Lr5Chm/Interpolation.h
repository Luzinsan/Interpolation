#pragma once
#include <iomanip>
#include <iostream>
#include "PolStr.h"
#include "IInputDevice.h"
#define MAX_ITER 100000

class Interpolation
{
private:
    double _a, _b, _eps;
    const char* _pstr;
    char _method;
    double _res;
    double _eps_new;
    std::string _expr;
    int _count;
public:
    Interpolation(const char* pstr, double a, double b, double eps, std::string expr, char method = '1') :
        _pstr{ pstr }, _a(a), _b(b), _eps(eps), _expr{ expr }, _method(method), _count(0), _eps_new(0), _res(0)
    {

    }

    Interpolation(IInputDevice& data)
    {

    }

    ~Interpolation()
    {
        

    }

    std::string getType() const
    {
        switch (_method)
        {
        case '1':
            return "Метод Дихотомии";
        case '2':
            return "Метод Хорд";
        default:
            return "Не Обнаружен";
        }
    }

    int getCount()
    {
        return _count;
    }

    friend std::ostream& operator<<(std::ostream& out, const Interpolation& expr)
    {
        std::streamsize precision = std::cout.precision();
        out << expr.getType() << ": \n"
            << expr._expr
            << "\nx' = " << std::setprecision(precision) << expr._res << "\n"
            << "f(x') = " << EvalPolStr(expr._pstr, expr._res, 0)
            << "\nВедённая погрешность: " << expr._eps
            << "\nПолученная погрешность вычислений: " << expr._eps_new << std::endl;
        return out;
    }

    void setResult(char choice)
    {
        _method = choice;
        switch (_method)
        {
        case '1':
            //setDichotomyMethod();
            break;
        case '2':
           // setChordMethod();
            break;
        default:
            throw std::invalid_argument("Не найден подходящий метод решения");
        }
    }

};

