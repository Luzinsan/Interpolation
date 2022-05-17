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
        std::string error_message = "������������ ��������, ���������� ��� ���.\n-> ");
    double getDouble(double min = -DBL_MAX,
        double max = DBL_MAX,
        std::string notification_message = "",
        std::string error_message = "������������ ��������, ���������� ��� ���.\n-> ");
    template<class T> T* getGridX(T* temp_array, int size,
        std::string notification_message,
        std::string error_message = "������������ ��������, ���������� ��� ���.");

    template<class T>
    class Interpolation
    {
    private:
        std::streambuf* _original_cin;
        char _method;  // ��������� ����� ���������������� 
                       // (1 � ������� �������,
                       //  2 � ������� ��������);
        int _k;        //  ������� ����������� 
                       // (0 � ����������� ��� �������, 
                       //  1 � ��� ������ �����������,
                       //  2 � ��� ������ �����������)
        int _n;        // ������� ��������;
        char _s;       // ������, �������� ��� �������� ����� 			 	   
                       // (u � uniform grid     � ����������� �����,
                       //  n � non-uniform grid � ������������� �����);
        T _a;
        T _b;          // ������� ������� (��� ����������� �����);
        Vector<T> _x0; // ���� �����(���� ��� �������������);
        Vector<T> _y0; // �������� ������� � ����� �����;
        int _m;        // ���������� ���������� � �������������� �����
                       // (�.�.���������� ����� � m + 1,
                       //  ��� ������� ��� ���������� � ������ �������� �����);
        Vector<T> _res_x; // ���� �������������� �����;
        char _t;          // ������, ����������, �������� ��� ��� ������������� ��������� ��� ������� f(x)
                          // (y - ������������� ��������� ��������,
                          //  n - ������������� ��������� ����������);
        std::string _f;   // ������������� ��������� ��� ������� (���� ��� ��������).
    public:

        Interpolation()
            : _original_cin{ std::cin.rdbuf() },
            _method(1), _k(0), _n(-1), _s('u'), _a(0), _b(0),
            _x0(Vector<T>()), _y0(Vector<T>()), _m(0),
            _res_x(Vector<T>()), _t('n'), _f("")
        {
        }
        // ��������� ������ �����
        std::ifstream* setInputDevice(char input_method)
        {
            std::ifstream* fin = NULL;
            switch (input_method)
            {
            case '1': return NULL;
            case '2':
            {
                std::string filename;
                std::cout << "\n\t������� ��� �������� �����:\n-> ";
                getline(std::cin, filename);
                fin = new std::ifstream(filename);
                //��������� ������ ����� ������ � ����� � �������������� ����������� ����� �� ���������������� ����
                _original_cin = redirectInput(fin);
                if (!_original_cin)  return NULL;
                break;
            }
            case '3':
                fin = new std::ifstream("input_non-uniform_grid.txt");
                //��������� ������ ����� � �������������� ����������� ����� �� ���� input.txt
                _original_cin = redirectInput(fin);
                if (!_original_cin) return NULL;
                break;
            default:
                throw std::invalid_argument("\n\t\t��� ����������� ������ ����� ������...\n");
            }
            return fin;
        }
        // ���������� ������ �� �������� ������ �����
        void inputData(std::ifstream* in)
        {
            if (!in)
            {
                _method = getSymbol({ '1', '2' }, "\n\t������� ��������� ����� ����������������:"
                    "\n\t1 � ������� �������;"
                    "\n\t2 � ������� ��������.\n-> ");
                _k = getSymbol({ '0','1','2' }, "\n\t������� �����������:"
                    "\n\t0 � ����������� ��� �������;"
                    "\n\t1 � ��� ������ �����������;"
                    "\n\t2 � ��� ������ �����������.\n-> ") - '0';
                _n = getDouble(0, INT_MAX, "\n\t������� ������� ��������:\n-> ");
                _s = getSymbol({ 'u','n' }, "\n\t������� ������, �������� ��� �������� �����:"
                    "\n\tu � uniform grid     � ����������� �����;"
                    "\n\tn � non-uniform grid � ������������� �����).\n-> ");

                T* temp_array = new T[_n+1];
                if (_s == 'u')
                {
                    _a = getDouble(-INT_MAX, INT_MAX, "\n\t������� ����� ������� ���������������� �������, �� ������� ����� ����������������� �������:\n-> ");
                    _b = getDouble(-INT_MAX, INT_MAX, "\n\t������� ������ ������� ���������������� �������, �� ������� ����� ����������������� �������:\n-> ");
                }
                else
                {
                    _x0 = Vector<T>(_n+1,
                            true, getGridX(temp_array, _n + 1,
                                "\n\t������� �������� ����� ���������������� �����:\n",
                                "\n\t�������� ����� ������ ���� ������ �� �����������. ������� ������ ��������."));
                    delete[] temp_array;
                }

                temp_array = new T[_n + 1];
                std::cout << "\n\t������� �������� ������� � ����� ���������������� �����:\n";
                for (unsigned i = 0; i <= _n; i++)
                    temp_array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << i << "]: ").str());
                _y0 = Vector<T>(_n+1, true, temp_array);
                delete[] temp_array;

                _m = getDouble(0, INT_MAX, "\n\t������� ���������� ���������� � �������������� �����:\n");
                temp_array = new T[_m+1];
                _res_x = Vector<T>(_m + 1,
                        true, getGridX(temp_array, _m + 1,
                            "\n\t������� �������� ����� �������������� ���������������� �����:\n",
                            "\n\t\t\t�������� ����� ������ ���� ������ �� �����������. ������� ������ ��������.\n"));
                delete[] temp_array;

                _t = getSymbol({ 'y', 'n' }, "\n\t�������� �� ������������� ��������� ��� ������� f(x)?"
                    "\n\ty � ��;"
                    "\n\tn � ���\n-> ");
                if (_t == 'y')
                {
                    char choice = 'y';
                    while (choice == 'y')
                    {
                        std::cout << "\n\t������� ��������� ��� �������:\n-> ";
                        getline(std::cin, _f);
                        if (_f.empty())
                        {
                            std::cerr << "\n\t������ ���������� ������ ������.";
                            choice == getSymbol({ 'y','n' }, "\n\t����������� ��� ���? (y/n)\n-> ");
                        }
                        else choice = 'n';
                    }
                }
                if (in)
                {
                    std::cin.rdbuf(_original_cin); // ���������� �� ������������ ����� � ����������
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
            Vector<Vector<T>> dividedDifferences(_n + 1, true); // ������-�������
            dividedDifferences[0] = Vector<T>(_n + 1, false);   // �������� ������-������ (y0,...,yn)
            for (unsigned i = 0; i <= _n; i++)
                dividedDifferences[0][i] = _y0[i];

            for (unsigned i = 1; i <= _n; i++)
            {
                dividedDifferences[i] = Vector<T>(_n + 1 - i, false); // �������� ������-������ ���������� ��������� [x_i,...,x_(i+k)]
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
        //        << "\n������� �����������: " << expr._eps
        //        << "\n���������� ����������� ����������: " << expr._eps_new << std::endl;
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
                "������ ���� �� ����� ���� ������, ���� �� ����������. ����������� ��� ���?\n1) ��\n2) �����\n->");
            if (choice == '1')
            {
                std::cout << "������� ��� �����:\n->";
                getline(std::cin, filename);
            }
            else return NULL;
            fin->open(filename);
        }
        //�������������� ����������� ����� ����� �� ���������� ����
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



