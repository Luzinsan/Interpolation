#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>
#include <sstream>
#include "Vector.h"

namespace luMath
{
    std::streambuf* redirectInput(std::ifstream* fin = NULL);
    char getSymbol(std::initializer_list<char> list,
        std::string notification_message = "",
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.\n->");
    double getDouble(double min = -DBL_MAX,
        double max = DBL_MAX,
        std::string notification_message = "",
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.\n->");
    template<class T> T* getGridX(T* temp_array, int size,
        std::string notification_message,
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.\n->");

    template<class T>
    class Interpolation
    {
    private:
        std::streambuf* _original_cin;
        std::ifstream* _fin;
        char _method;  // требуемый метод интерполирования 
                       // (1 – полином Ноютона,
                       //  2 – полином Лагранжа);
        int _k;        //  порядок производной 
                       // (0 – вычисляется сам полином, 
                       //  1 – его первая производная,
                       //  2 – его вторая производная)
        int _n;        // порядок полинома;
        char _s;       // символ, задающий тип исходной сетки 			 	   
                       // (u – uniform grid     – равномерная сетка,
                       //  n – non-uniform grid – неравномерная сетка);
        T _a;
        T _b;          // границы отрезка (при равномерной сетке);
        Vector<T> _x0; // узлы сетки(если она неравномерная);
        Vector<T> _y0; // значения функции в узлах сетки;
        int _m;        // количество интервалов в результирующей сетке
                       // (т.е.количество узлов – m + 1,
                       //  что сделано для унификации с узлами исходной сетки);
        Vector<T> _res_x; // узлы результирующей сетки;
        char _t;          // символ, сообщающий, известно или нет аналитическое выражение для функции f(x)
                          // (y - аналитическое выражение известно,
                          //  n - аналитическое выражение неизвестно);
        std::string _f;   // аналитическое выражение для функции (если оно известно).
    public:

        Interpolation()
            : _original_cin{ std::cin.rdbuf() }, _fin{ NULL },
            _method(1), _k(0), _n(-1), _s('u'), _a(0), _b(0),
            _x0(Vector<T>()), _y0(Vector<T>()), _m(0),
            _res_x(Vector<T>()), _t('n'), _f("")
        {
        }

        // Установка потока ввода
        Interpolation(char input_method) : Interpolation()
        {
            switch (input_method)
            {
            case '2':
            {
                std::string filename;
                std::cout << "Введите имя входного файла:\n->";
                getline(std::cin, filename);
                _fin = new std::ifstream(filename);
                //сохраняем старый поток вывода и ввода и перенаправляем стандартный поток на пользовательский файл
                _original_cin = redirectInput(_fin);
                if (!_original_cin)  input_method = '4';
                break;
            }
            case '3':
                _fin = new std::ifstream("input_non-uniform_grid.txt");
                //сохраняем старый поток и перенаправляем стандартный поток на файл input.txt
                _original_cin = redirectInput(_fin);
                if (!_original_cin) input_method = '4';
                break;
            case '4': return;
            default:
                throw std::invalid_argument("Нет подходящего метода ввода данных...\n");
            }
            Interpolation(_fin);
        }


        // Считывание данных из текущего потока ввода
        Interpolation(std::ifstream* in)
        {
            std::string string;
            if (std::cin.rdbuf() == _original_cin)
            {
                _method = getSymbol({ '1', '2' }, "\n\tВведите требуемый метод интерполирования:"
                    "\n\t(1 – полином Ньютона;"
                    "\n\t 2 – полином Лагранжа)");
                _k      = getSymbol({ '0','1','2' }, "\n\tПорядок производной:"
                    "\n\t(0 – вычисляется сам полином; "
                    "\n\t 1 – его первая производная;"
                    "\n\t 2 – его вторая производная") - '0';
                _n = getDouble(0, INT_MAX, "\n\tВведите порядок полинома:");
                _s = getSymbol({ 'u','n' }, "\n\tВведите символ, задающий тип исходной сетки"
                    "\n\t(u – uniform grid     – равномерная сетка,"
                    "\n\t n – non-uniform grid – неравномерная сетка):");
                    
                T* temp_array = new T[_n];
                if (_s == 'u')
                {
                    _a = getDouble(-INT_MAX, INT_MAX, "\n\tВведите левую границу рассматриваемого отрезка, на котором будет интерполироваться функция:");
                    _b = getDouble(-INT_MAX, INT_MAX, "\n\tВведите правую границу рассматриваемого отрезка, на котором будет интерполироваться функция:");
                }
                else 
                {
                    _x0 = Vector<T>(_n, getGridX(temp_array, _n,
                        "\n\tВведите значения узлов интерполяционной сетки:\n", 
                        "\n\tЗначения узлов должны идти строго по возрастанию. Введите другое значение.\n"), 
                        true);
                    delete[] temp_array;
                }

                std::cout << "Введите значения функции в узлах интерполяционной сетки:\n";
                for (unsigned i = 0; i <= _n; i++)
                    temp_array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream("[") << i << "]: ").str());
                _y0 = Vector<T>(_n, temp_array, true);
                delete[] temp_array;

                _m = getDouble(0, INT_MAX, "\n\tВведите количество интервалов в результирующей сетке:");
                _res_x = Vector<T>(_m, getGridX(temp_array, _m,
                    "\n\tВведите значения узлов результирующей интерполяционной сетки : \n",
                    "\n\tЗначения узлов должны идти строго по возрастанию. Введите другое значение.\n"),
                    true);
                delete[] temp_array;
                    
                _t = getSymbol({ '1', '2' }, "\n\tИзвестно ли аналитическое выражение для функции f(x)?"
                    "\n\t(y – да;"
                    "\n\t n – нет)");
                if (_t == 'y') 
                {
                    char choice = 'y';
                    while (choice == 'y')
                    {
                        std::cout << "\n\tВведите выражение для функции:\n-> ";
                        getline(*in, _f);
                        if (_f.empty())
                        {
                            std::cerr << "\n\tНельзя обработать пустую строку.";
                            choice == getSymbol({'y','n'},"\n\tПопробовать ещё раз? (y/n)\n-> ");
                        }
                    }
                }
            }
            else
            {
                while (true)
                {
                    string.clear();
                    getline(std::cin, string);
                    if (!string.empty()) { _expr.push_back(string); continue; }
                    if (_expr.empty())
                        std::cerr << "Нельзя обработать пустую строку.\n";
                    break;
                }
                _method = _expr[0][0];
                std::stringstream stream(_expr[2]);
                getline(stream, string, ' ');
                _a = std::stod(string);
                getline(stream, string, ' ');
                _b = std::stod(string);
                _eps = std::stod(_expr[3]);
                _NAfterComma = setNAfterComma(_eps);
                // взяли все нужные данные, теперь переместим выражение в начало вектора и отрежем всё ненужное
                // это нужно, так как при считывании с клавиатуры, expr может хранить больше выражений,
                // нежели считывать из файла только одно
                _expr[0] = _expr[1];
                _expr.resize(1);
                break;
            }
        }
    /*
        ~Interpolation()
        {
            if (_original_cin)
                std::cin.rdbuf(_original_cin); // сбрасываем до стандартного ввода с клавиатуры
            if (_fin)
                _fin->close();
        }

        bool isEmptyVectorString()
        {
            return _expr.empty();
        }
        std::vector<std::string> getVectorString()
        {
            return _expr;
        }

        double getLeft() { return _a; }
        double getRight() { return _b; }
        double getEps() { return _eps; }
        int getNAfterComma() { return _NAfterComma; }
        char getMethod() { return _method; }

        void setMethod(char method)
        {
            if (method < '1' || method > '6') throw std::invalid_argument("Недопустимое значение метода");
            _method = method;
        }

        int setNAfterComma(double eps = 0.0)
        {
            if (!eps)
                _eps = getDouble(0, 1,
                    "Введите погрешность вычислений (0 < eps < 1) (с разделяющей запятой ',')\n->",
                    "Погрешность не удовлетворяет условию. Попробуйте ещё раз.\n");

            return -std::ceil(std::log10(_eps));

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
        */
        
        

    };

    std::streambuf* redirectInput(std::ifstream* fin)
    {
        std::streambuf* original_cin = std::cin.rdbuf();
        while (!*fin)
        {
            std::string filename;
            char choice;
            choice = getSymbol({ '1', '2' },
                "Данный файл не может быть открыт, либо не существует. Попробовать ещё раз?\n1) да\n2) выйти\n->");
            if (choice == '1')
            {
                std::cout << "Введите имя файла:\n->";
                getline(std::cin, filename);
            }
            else return NULL;
            fin->open(filename);
        }
        //перенаправляем стандартный поток ввода на переданный файл
        std::cin.rdbuf(fin->rdbuf());
        return original_cin;
    }

    char getSymbol(std::initializer_list<char> list,
        std::string notification_message,
        std::string error_message)
    {
        char choice;
        std::cout << notification_message;

        bool flag = true;
        do {
            choice = _getche();
            std::cout << std::endl;
            for (auto it = list.begin(); it != list.end(); it++)
                if (it[0] == choice) { flag = false; break; }

            if (flag) std::cerr << error_message;
        } while (flag);
        return choice;
    }

    double getDouble(double min, double max, 
        std::string notification_message, 
        std::string error_message)
    {
        std::string epsstr;
        double eps;
        do {
            std::cout << notification_message;
            std::cin >> epsstr;

            bool point = false, flag = false;
            auto it = epsstr.begin();
            if (it[0] == '-') it++;
            for (; it != epsstr.end(); it++)
                if (!isdigit(it[0]) && (it[0] != ',' || point))
                {
                    std::cout << error_message;
                    flag = true;
                    break;
                }
                else if (it[0] == ',' && !point) point = true;

            if (flag) continue;
            eps = std::stod(epsstr);
            if (eps <= min || eps >= max)
                std::cout << error_message;
            else { std::cin.ignore(32256, '\n'); break; }
        } while (true);
        return eps;
    }

    template<class T> T* getGridX(T* array, int size,
        std::string notification_message,
        std::string error_message)
    {
        std::cout << notification_message;
        unsigned i = 0;
        array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream("[") << i << "]: ").str());
        i++;
        do
        {
            array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream("[") << i << "]: ").str());
            if (array[i] < array[i - 1]) std::cout << error_message;
            else i++;
        } while (i <= size);
        return array;
    }
}
#endif



