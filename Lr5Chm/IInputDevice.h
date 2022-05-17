#ifndef IINPUTDEVICE_H
#define IINPUTDEVICE_H
#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <conio.h>
#include <vector>
#include <sstream>
#include <cfloat>
#include <cmath>

namespace luzinsan
{
    char getSymbol(std::initializer_list<char> list,
        std::string notification_message = "",
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.\n->");

    double getDouble(double min = -DBL_MAX,
        double max = DBL_MAX,
        std::string notification_message = "",
        std::string error_message = "Недопустимое значение, попробуйте ещё раз.\n->");



    class IInputDevice
    {
    private:
        std::streambuf* _original_cin;
        std::ifstream* _fin;
        std::vector<std::string> _expr;
        char _method;
        int _NAfterComma;
        double _eps;
        double _a, _b;
    public:

        IInputDevice(char choice = '1')
            : _original_cin{ NULL },
            _fin{ NULL }
        {
            switch (choice)
            {
            case '1':
                _a = getDouble(-DBL_MAX, DBL_MAX, "Введите левую границу интервала\n-> ");
                _b = getDouble(-DBL_MAX, DBL_MAX, "Введите правую границу интервала\n-> ");
                _NAfterComma = setNAfterComma();
                break;
            case '2':
            {
                std::string filename;
                std::cout << "Введите имя входного файла:\n->";
                getline(std::cin, filename);
                _fin = new std::ifstream(filename);
                //сохраняем старый поток вывода и ввода и перенаправляем стандартный поток на пользовательский файл
                _original_cin = redirectInput(_fin);
                if (!_original_cin)  choice = '4';
                break;
            }
            case '3':
                _fin = new std::ifstream("input.txt");
                //сохраняем старый поток и перенаправляем стандартный поток на файл input.txt
                _original_cin = redirectInput(_fin);
                if (!_original_cin) choice = '4';
                break;
            case '4': break;
            default:
                throw std::invalid_argument("Нет подходящего метода ввода данных...\n");
            }
            InputData(choice);
        }

        void InputData(char choice)
        {
            std::string string;
            switch (choice)
            {
            case '1':
                while (true)
                {
                    std::cout << "Введите выражение:\n-> ";

                    getline(std::cin, string);
                    if (string.empty())
                        std::cerr << "Нельзя обработать пустую строку.\n";
                    else _expr.push_back(string);
                    choice = getSymbol({ '1','2' }, "1) продолжить;\n2) завершить ввод данных\n-> ");
                    if (choice == '1') continue;
                    else break;
                }
                break;
            case '2': case '3':
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

        std::streambuf* redirectInput(std::ifstream* fin = NULL)
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



        ~IInputDevice()
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
    };
}
#endif
