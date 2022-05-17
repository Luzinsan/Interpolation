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
        std::string error_message = "������������ ��������, ���������� ��� ���.\n->");
    double getDouble(double min = -DBL_MAX,
        double max = DBL_MAX,
        std::string notification_message = "",
        std::string error_message = "������������ ��������, ���������� ��� ���.\n->");
    template<class T> T* getGridX(T* temp_array, int size,
        std::string notification_message,
        std::string error_message = "������������ ��������, ���������� ��� ���.\n->");

    template<class T>
    class Interpolation
    {
    private:
        std::streambuf* _original_cin;
        std::ifstream* _fin;
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
            : _original_cin{ std::cin.rdbuf() }, _fin{ NULL },
            _method(1), _k(0), _n(-1), _s('u'), _a(0), _b(0),
            _x0(Vector<T>()), _y0(Vector<T>()), _m(0),
            _res_x(Vector<T>()), _t('n'), _f("")
        {
        }

        // ��������� ������ �����
        Interpolation(char input_method) : Interpolation()
        {
            switch (input_method)
            {
            case '2':
            {
                std::string filename;
                std::cout << "������� ��� �������� �����:\n->";
                getline(std::cin, filename);
                _fin = new std::ifstream(filename);
                //��������� ������ ����� ������ � ����� � �������������� ����������� ����� �� ���������������� ����
                _original_cin = redirectInput(_fin);
                if (!_original_cin)  input_method = '4';
                break;
            }
            case '3':
                _fin = new std::ifstream("input_non-uniform_grid.txt");
                //��������� ������ ����� � �������������� ����������� ����� �� ���� input.txt
                _original_cin = redirectInput(_fin);
                if (!_original_cin) input_method = '4';
                break;
            case '4': return;
            default:
                throw std::invalid_argument("��� ����������� ������ ����� ������...\n");
            }
            Interpolation(_fin);
        }


        // ���������� ������ �� �������� ������ �����
        Interpolation(std::ifstream* in)
        {
            std::string string;
            if (std::cin.rdbuf() == _original_cin)
            {
                _method = getSymbol({ '1', '2' }, "\n\t������� ��������� ����� ����������������:"
                    "\n\t(1 � ������� �������;"
                    "\n\t 2 � ������� ��������)");
                _k      = getSymbol({ '0','1','2' }, "\n\t������� �����������:"
                    "\n\t(0 � ����������� ��� �������; "
                    "\n\t 1 � ��� ������ �����������;"
                    "\n\t 2 � ��� ������ �����������") - '0';
                _n = getDouble(0, INT_MAX, "\n\t������� ������� ��������:");
                _s = getSymbol({ 'u','n' }, "\n\t������� ������, �������� ��� �������� �����"
                    "\n\t(u � uniform grid     � ����������� �����,"
                    "\n\t n � non-uniform grid � ������������� �����):");
                    
                T* temp_array = new T[_n];
                if (_s == 'u')
                {
                    _a = getDouble(-INT_MAX, INT_MAX, "\n\t������� ����� ������� ���������������� �������, �� ������� ����� ����������������� �������:");
                    _b = getDouble(-INT_MAX, INT_MAX, "\n\t������� ������ ������� ���������������� �������, �� ������� ����� ����������������� �������:");
                }
                else 
                {
                    _x0 = Vector<T>(_n, getGridX(temp_array, _n,
                        "\n\t������� �������� ����� ���������������� �����:\n", 
                        "\n\t�������� ����� ������ ���� ������ �� �����������. ������� ������ ��������.\n"), 
                        true);
                    delete[] temp_array;
                }

                std::cout << "������� �������� ������� � ����� ���������������� �����:\n";
                for (unsigned i = 0; i <= _n; i++)
                    temp_array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream("[") << i << "]: ").str());
                _y0 = Vector<T>(_n, temp_array, true);
                delete[] temp_array;

                _m = getDouble(0, INT_MAX, "\n\t������� ���������� ���������� � �������������� �����:");
                _res_x = Vector<T>(_m, getGridX(temp_array, _m,
                    "\n\t������� �������� ����� �������������� ���������������� ����� : \n",
                    "\n\t�������� ����� ������ ���� ������ �� �����������. ������� ������ ��������.\n"),
                    true);
                delete[] temp_array;
                    
                _t = getSymbol({ '1', '2' }, "\n\t�������� �� ������������� ��������� ��� ������� f(x)?"
                    "\n\t(y � ��;"
                    "\n\t n � ���)");
                if (_t == 'y') 
                {
                    char choice = 'y';
                    while (choice == 'y')
                    {
                        std::cout << "\n\t������� ��������� ��� �������:\n-> ";
                        getline(*in, _f);
                        if (_f.empty())
                        {
                            std::cerr << "\n\t������ ���������� ������ ������.";
                            choice == getSymbol({'y','n'},"\n\t����������� ��� ���? (y/n)\n-> ");
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
                        std::cerr << "������ ���������� ������ ������.\n";
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
                // ����� ��� ������ ������, ������ ���������� ��������� � ������ ������� � ������� �� ��������
                // ��� �����, ��� ��� ��� ���������� � ����������, expr ����� ������� ������ ���������,
                // ������ ��������� �� ����� ������ ����
                _expr[0] = _expr[1];
                _expr.resize(1);
                break;
            }
        }
    /*
        ~Interpolation()
        {
            if (_original_cin)
                std::cin.rdbuf(_original_cin); // ���������� �� ������������ ����� � ����������
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
            if (method < '1' || method > '6') throw std::invalid_argument("������������ �������� ������");
            _method = method;
        }

        int setNAfterComma(double eps = 0.0)
        {
            if (!eps)
                _eps = getDouble(0, 1,
                    "������� ����������� ���������� (0 < eps < 1) (� ����������� ������� ',')\n->",
                    "����������� �� ������������� �������. ���������� ��� ���.\n");

            return -std::ceil(std::log10(_eps));

        }



        std::string getType() const
        {
            switch (_method)
            {
            case '1':
                return "����� ���������";
            case '2':
                return "����� ����";
            default:
                return "�� ���������";
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
                << "\n������� �����������: " << expr._eps
                << "\n���������� ����������� ����������: " << expr._eps_new << std::endl;
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
                throw std::invalid_argument("�� ������ ���������� ����� �������");
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



