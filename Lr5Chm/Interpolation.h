#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "Vector.h"
#include "Polynomial.h"
#include "PolStr.h"

namespace luMath
{
    std::streambuf* redirectInput(std::ifstream* fin = NULL);
    char getSymbol(std::initializer_list<char> list,
        std::string notification_message = "",
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.\n-> ");
    double getDouble(double min = -DBL_MAX,
        double max = DBL_MAX,
        std::string notification_message = "",
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.\n-> ");
    template<class T> T* getGridX(T* temp_array, int size,
        std::string notification_message,
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.");

    template<class T>
    class Interpolation
    {
    private:
        std::streambuf* _original_cin;
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
            : _original_cin{ std::cin.rdbuf() },
            _method(1), _k(0), _n(-1), _s('u'), _a(0), _b(0),
            _x0(Vector<T>()), _y0(Vector<T>()), _m(0),
            _res_x(Vector<T>()), _t('n'), _f("")
        {
        }
        // Установка потока ввода
        std::ifstream* setInputDevice(char input_method)
        {
            std::ifstream* fin = NULL;
            switch (input_method)
            {
            case '1': return NULL;
            case '2':
            {
                std::string filename;
                std::cout << "\n\tВведите имя входного файла:\n-> ";
                getline(std::cin, filename);
                fin = new std::ifstream(filename);
                //сохраняем старый поток вывода и ввода и перенаправляем стандартный поток на пользовательский файл
                _original_cin = redirectInput(fin);
                if (!_original_cin)  return NULL;
                break;
            }
            case '3':
                fin = new std::ifstream("input_non-uniform_grid.txt");
                //сохраняем старый поток и перенаправляем стандартный поток на файл input.txt
                _original_cin = redirectInput(fin);
                if (!_original_cin) return NULL;
                break;
            default:
                throw std::invalid_argument("\n\t\tНет подходящего метода ввода данных...\n");
            }
            return fin;
        }
        // Считывание данных из текущего потока ввода
        void inputData(std::ifstream* in)
        {
            if (!in)
            {
                _method = getSymbol({ '1', '2' }, "\n\tВведите требуемый метод интерполирования:"
                    "\n\t1 – полином Ньютона;"
                    "\n\t2 – полином Лагранжа.\n-> ");
                _k = getSymbol({ '0','1','2' }, "\n\tПорядок производной:"
                    "\n\t0 – вычисляется сам полином;"
                    "\n\t1 – его первая производная;"
                    "\n\t2 – его вторая производная.\n-> ") - '0';
                _n = getDouble(0, INT_MAX, "\n\tВведите порядок полинома:\n-> ");
                _s = getSymbol({ 'u','n' }, "\n\tВведите символ, задающий тип исходной сетки:"
                    "\n\tu – uniform grid     – равномерная сетка;"
                    "\n\tn – non-uniform grid – неравномерная сетка).\n-> ");

                T* temp_array = new T[_n+1];
                if (_s == 'u')
                {
                    _a = getDouble(-INT_MAX, INT_MAX, "\n\tВведите левую границу рассматриваемого отрезка, на котором будет интерполироваться функция:\n-> ");
                    _b = getDouble(-INT_MAX, INT_MAX, "\n\tВведите правую границу рассматриваемого отрезка, на котором будет интерполироваться функция:\n-> ");
                }
                else
                {
                    _x0 = Vector<T>(_n+1,
                            true, getGridX(temp_array, _n + 1,
                                "\n\tВведите значения узлов интерполяционной сетки:\n",
                                "\n\tЗначения узлов должны идти строго по возрастанию. Введите другое значение."));
                    delete[] temp_array;
                }

                temp_array = new T[_n + 1];
                std::cout << "\n\tВведите значения функции в узлах интерполяционной сетки:\n";
                for (unsigned i = 0; i <= _n; i++)
                    temp_array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << i << "]: ").str());
                _y0 = Vector<T>(_n+1, true, temp_array);
                delete[] temp_array;

                _m = getDouble(0, INT_MAX, "\n\tВведите количество интервалов в результирующей сетке:\n");
                temp_array = new T[_m+1];
                _res_x = Vector<T>(_m + 1,
                        true, getGridX(temp_array, _m + 1,
                            "\n\tВведите значения узлов результирующей интерполяционной сетки:\n",
                            "\n\t\t\tЗначения узлов должны идти строго по возрастанию. Введите другое значение.\n"));
                delete[] temp_array;

                _t = getSymbol({ 'y', 'n' }, "\n\tИзвестно ли аналитическое выражение для функции f(x)?"
                    "\n\ty – да;"
                    "\n\tn – нет\n-> ");
                if (_t == 'y')
                {
                    char choice = 'y';
                    while (choice == 'y')
                    {
                        std::cout << "\n\tВведите выражение для функции:\n-> ";
                        getline(std::cin, _f);
                        if (_f.empty())
                        {
                            std::cerr << "\n\tНельзя обработать пустую строку.";
                            choice == getSymbol({ 'y','n' }, "\n\tПопробовать ещё раз? (y/n)\n-> ");
                        }
                        else choice = 'n';
                    }
                }
                if (in)
                {
                    std::cin.rdbuf(_original_cin); // сбрасываем до стандартного ввода с клавиатуры
                    in->close();
                }
            }
            else
            {
                std::cin >> _method >> _k >> _n >> _s;
                T* temp_array = new T[_n + 1];
                if (_s == 'u')
                    std::cin >> _a >> _b;
                else
                {
                    for (unsigned i = 0; i <= _n; i++)
                        std::cin >> temp_array[i];
                    _x0 = Vector<T>(_n + 1, true, temp_array);
                    delete[] temp_array;
                }
                temp_array = new T[_n + 1];
                for (unsigned i = 0; i <= _n; i++)
                    std::cin >> temp_array[i];
                _y0 = Vector<T>(_n + 1, true, temp_array);
                delete[] temp_array;

                std::cin >> _m;
                temp_array = new T[_m + 1];
                for (unsigned i = 0; i <= _m; i++)
                    std::cin >> temp_array[i];
                _res_x = Vector<T>(_m + 1, true, temp_array);
                delete[] temp_array;

                std::cin >> _t;
                if (_t == 'y') 
                {
                    std::cin.seekg(2, std::ios_base::cur);
                    getline(std::cin, _f);
                }
            }

        }
        ~Interpolation()
        {   
        }
        
        void  NewtonInterpolation() 
        {
            Vector<Vector<T>> dividedDifferences(_n + 1, true); // вектор-столбец
            dividedDifferences[0] = Vector<T>(_n + 1, false);   // выделяем вектор-строку (y0,...,yn)
            for (unsigned i = 0; i <= _n; i++)
                dividedDifferences[0][i] = _y0[i];

            for (unsigned i = 1; i <= _n; i++)
            {
                dividedDifferences[i] = Vector<T>(_n + 1 - i, false); // выделяем вектор-строки разделённых разностей [x_i,...,x_(i+k)]
                for (unsigned j = 0; j <= _n - i; j++)
                    dividedDifferences[i][j] = (dividedDifferences[i - 1][j + 1] - dividedDifferences[i - 1][j])
                                             / (_x0[i + j] - _x0[j]);
            }

            Polynomial<T> P;
            for (int i = 0; i <= _n; i++) 
            {
                Polynomial<T> mult((T)1);
                for (int j = 0; j <= i-1; j++)
                {
                    std::cout << Polynomial<T>({ -_x0[j], 1 }) << "\n";
                    mult *= Polynomial<T>({ -_x0[j], 1 });
                    std::cout << mult << "\n";
                }
                P += mult * dividedDifferences[i][0];
                std::cout << P << "\n";
            }





        
        }

        void LagrangeInterpolation() 
        {
        
        
        
        }



        //friend std::ostream& operator<<(std::ostream& out, const Interpolation& expr)
        //{
        //    std::streamsize precision = std::cout.precision();
        //    out << expr.getType() << ": \n"
        //        << expr._expr
        //        << "\nx' = " << std::setprecision(precision) << expr._res << "\n"
        //        << "f(x') = " << EvalPolStr(expr._pstr, expr._res, 0)
        //        << "\nВедённая погрешность: " << expr._eps
        //        << "\nПолученная погрешность вычислений: " << expr._eps_new << std::endl;
        //    return out;
        //}
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
        array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << i << "]: ").str());
        i++;
        do
        {
            array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << i << "]: ").str());
            if (array[i] < array[i - 1]) std::cout << error_message;
            else i++;
        } while (i <= size);
        return array;
    }
}
#endif



