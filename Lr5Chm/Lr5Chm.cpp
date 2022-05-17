#include <iostream>
#include "Timer.h"
#include "Interpolation.h"

using namespace luMath;

std::streambuf* redirectOutput(std::ofstream* fout);

int main()
{
    system("chcp 1251 > null");
    char choice;


    do {
        system("pause");
        system("cls");
        choice = getSymbol({ '1','2','3','4' },
            "Введите способ ввода данных:\n1) с клавиатуры;\n2) из файла, заданного пользователем\n3) стандартный (из файла input_non-uniform_grid.txt)\n4) завершить программу\n-> ");
        choice = '3';
        if (choice == '4') break;
        Interpolation<double> data;
        std::ifstream* fin = data.setInputDevice(choice);
        if (fin) data.inputData(fin);
        else break;
        data.NewtonInterpolation();

        
    } while (choice);
    system("pause");
}

std::streambuf* redirectOutput(std::ofstream* fout)
{
    std::streambuf* original_cout = std::cout.rdbuf();
    while (!*fout)
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
        fout->open(filename);
    }
    //перенаправляем стандартный поток вывода на переданный файл
    std::cout.rdbuf(fout->rdbuf());
    return original_cout;
}