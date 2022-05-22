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
        if (choice == '4') break;
        Interpolation<double> data;
        std::ifstream* fin = data.setInputDevice(choice);
        data.inputData(fin);
        
        std::cout << std::setw(10);
        Interpolation<double>::print(std::cout, data);
        
        /*Polynomial<double> polNewton0(data.getNewtonInterPol(0));
        Polynomial<double> polNewton1(data.getNewtonInterPol(1));
        Polynomial<double> polNewton2(data.getNewtonInterPol(2));
        std::cout << "\n\n\n\tПолином Ньютона: \nполином:\t" << polNewton0
                  << "\nПроверка по исходной сетке:\n";
        data.checkRes(polNewton0);
        std::cout << "\nпроизводная первой степени:\t" << polNewton1
                  << "\nпроизводная второй степени:\t" << polNewton2 << "\n";
        
        Polynomial<double> polLagrange0(data.getLagrangeInterPol(0));
        Polynomial<double> polLagrange1(data.getLagrangeInterPol(1));
        Polynomial<double> polLagrange2(data.getLagrangeInterPol(2));
        std::cout << "\n\n\n\tПолином Лагранжа: \nполином:\t" << polLagrange0
                  << "\nПроверка по исходной сетке:\n";
        data.checkRes(polLagrange0);
        std::cout << "\nпроизводная первой степени:\t" << polLagrange1
                  << "\nпроизводная второй степени:\t" << polLagrange2 << "\n";*/
       
        
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