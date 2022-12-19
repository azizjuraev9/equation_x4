// x4.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include<sstream>
#include<set>
#include<iterator>
#include <stdio.h>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <map>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

long double getX_1s(long double x_1, long double x_2, long double x_3, long double x_4) {
    return x_1 * x_1 + (2 * x_1 * x_2) + (2 * x_1 * x_3) + (2 * x_1 * x_4);
}

long double getX_2s(long double x_2, long double x_3, long double x_4) {
    return x_2 * x_2 + (2 * x_2 * x_3) + (2 * x_2 * x_4);
}

long double getX_3s(long double x_3, long double x_4) {
    return x_3 * x_3 + (2 * x_3 * x_4);
}

long double getX_4s(long double x_4) {
    return x_4 * x_4;
}

void resultsForOne(array<long double, 4> initValues, map<string, array<long double, 4>> &results, bool &limit, int &limitSize) {

    long double
        x_1 = initValues[0], 
        x_2 = initValues[1], 
        x_3 = initValues[2], 
        x_4 = initValues[3], 
    i = 0;


    limit = false;
    limitSize = 0;

    while (i < 50 && !limit) {
        i++;

        x_1 = getX_1s(x_1, x_2, x_3, x_4);
        x_2 = getX_2s(x_2, x_3, x_4);
        x_3 = getX_3s(x_3, x_4);
        x_4 = getX_4s(x_4);

        array<long double, 4> arr = {x_1, x_2, x_3, x_4 };
        string key = to_string(x_1) + "-" + to_string(x_2) + "-" + to_string(x_3) + "-" + to_string(x_4);

        map<string, array<long double, 4>>::iterator it;
        it = results.find(key);

        results.insert(pair<string, array<long double, 4>>(key, arr));
        
        if (it != results.end()) {
            limit = true;
            //limitSize = distance(it, results.end());
        }
    }

}

void threatF(int step, int start, int end, bool track = false) {

    long double x_1, x_2, x_3, x_4;
    map<string, array<long double, 4>> results;

    thread::id this_id = this_thread::get_id();

    for (x_1 = start; x_1 >= end; x_1 -= 1) {
        for (x_2 = step; x_2 >= 0; x_2 -= 1) {
            for (x_3 = step; x_3 >= 0; x_3 -= 1) {
                for (x_4 = step; x_4 >= 0; x_4 -= 1) {

                    if ((x_1 + x_2 + x_3 + x_4) != step) {
                        continue;
                    }

                    bool limit = false;
                    int limitSize = 0;

                    long double sx_1 = x_1 / step;
                    long double sx_2 = x_2 / step;
                    long double sx_3 = x_3 / step;
                    long double sx_4 = x_4 / step;

                    array<long double, 4> arr = { sx_1, sx_2, sx_3, sx_4};

                    string key = to_string(sx_1) + "-" + to_string(sx_2) + "-" + to_string(sx_3) + "-" + to_string(sx_4);
                    results.insert(pair<string, array<long double, 4>>(key, arr));
                    resultsForOne(arr, results, limit, limitSize);

                    string sdir = "results/" + to_string(sx_1) + "/" + to_string(sx_2) + "/" + to_string(sx_3);
                    string sfilename = sdir + "/" + to_string(sx_4) + ".txt";

                    char* dir = new char[sdir.length() + 1];
                    strcpy_s(dir, sdir.size() + 1, sdir.c_str());

                    char* filename = new char[sfilename.length() + 1];
                    strcpy_s(filename, sfilename.size() + 1, sfilename.c_str());

                    fs::create_directories(dir);

                    ofstream resFile(filename);

                    resFile << "Limit Reached: " << limit << endl;
                    //resFile << "Limit size: " << limitSize << endl;

                    map<string, array<long double, 4>>::iterator itr;

                    for (itr = results.begin(); itr != results.end(); ++itr) {
                        resFile << fixed;
                        resFile << setprecision(15);
                        resFile << itr->second[0] << " | " 
                            << itr->second[1] << " | "
                            << itr->second[2] << " | "
                            << itr->second[3] << endl;
                    }

                    resFile.close();
                }
            }
        }
        if (track) {
            stringstream messg;
            messg << " " << this_id << ": " << 100 - (100 * ((x_1 - end) / (start - end))) << "%" << std::endl;
            cout << messg.str();
        }
        
    }
}

int main()
{
    int step;

    cout << "Enter step size: ";
    cin >> step;
    cout << std::endl;

    cout << "start!" << std::endl;
    bool limit = false;

    int proc = step / 6;
    int last_step_inc = step % 6;

    cout << "Size for process: " << proc << endl;

    cout << "P1 starts: " << ((proc * 6) + last_step_inc) << endl;
    cout << "P2 starts: " << (proc * 5) << endl;
    cout << "P3 starts: " << (proc * 4) << endl;
    cout << "P4 starts: " << (proc * 3) << endl;
    cout << "P5 starts: " << (proc * 2) << endl;
    cout << "P6 starts: " << proc << endl;

    thread thr1(threatF, step, ((proc*6)+last_step_inc), (proc * 5), true);
    thread thr2(threatF, step, (proc*5), (proc*4), false);
    thread thr3(threatF, step, (proc*4), (proc*3), false);
    thread thr4(threatF, step, (proc*3), (proc*2), false);
    thread thr5(threatF, step, (proc*2), proc, false);
    thread thr6(threatF, step, proc, 0, false);

    thr1.join();
    thr2.join();
    thr3.join();
    thr4.join();
    thr5.join();
    thr6.join();

    std::cout << "Hello World!\n";

    cin >> step;
}