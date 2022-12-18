// x4.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include<sstream>
#include<set>
#include<iterator>
#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <map>
#include <filesystem> // C++17 standard header file name
//#include <experimental/filesystem> // Header file for pre-standard implementation
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

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

bool execSql(sqlite3* db, const char *SQL) {
    char* zErrMsg = 0;
    int rc;
    rc = sqlite3_exec(db, SQL, callback, 0, &zErrMsg);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return false;
    }
    return true;
}

void resultsForOne(array<long double, 4> initValues, map<string, array<long double, 4>> &results, bool &limit, int &limitSize) {

    long double
        x_1 = initValues[0], 
        x_2 = initValues[1], 
        x_3 = initValues[2], 
        x_4 = initValues[3], 
    i = 0;

    while (i < 1000 && !limit) {
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
        
        if (it == results.end()) {
            limit = true;
            limitSize = distance(results.begin(), it);
        }
    }

}

void threatF(int step, int start, int end, bool track = false) {

    long double x_1, x_2, x_3, x_4;
    bool limit = false;
    int limitSize = 0;
    map<string, array<long double, 4>> results;

    thread::id this_id = this_thread::get_id();

    for (x_1 = start; x_1 >= end; x_1 -= 1) {
        for (x_2 = step; x_2 >= 0; x_2 -= 1) {
            for (x_3 = step; x_3 >= 0; x_3 -= 1) {
                for (x_4 = step; x_4 >= 0; x_4 -= 1) {

                    if ((x_1 + x_2 + x_3 + x_4) != step) {
                        continue;
                    }

                    long double sx_1 = x_1 / step;
                    long double sx_2 = x_2 / step;
                    long double sx_3 = x_3 / step;
                    long double sx_4 = x_4 / step;

                    array<long double, 4> arr = { sx_1, sx_2, sx_3, sx_4};

                    string key = to_string(sx_1) + "-" + to_string(sx_2) + "-" + to_string(sx_3) + "-" + to_string(sx_4);
                    results.insert(pair<string, array<long double, 4>>(key, arr));
                    resultsForOne(arr, results, limit, limitSize);

                    string sdir = to_string(sx_1) + "/" + to_string(sx_2) + "/" + to_string(sx_3);
                    string sfilename = sdir + "/" + to_string(sx_4) + ".txt";

                    char* dir = new char[sdir.length() + 1];
                    strcpy_s(dir, sdir.size() + 1, sdir.c_str());

                    char* filename = new char[sfilename.length() + 1];
                    strcpy_s(filename, sfilename.size() + 1, sfilename.c_str());

                    fs::create_directories(dir);

                    ofstream resFile(filename);

                    resFile << "Limit Reached: " << limit << endl;
                    resFile << "Limit size: " << limitSize << endl;

                    map<string, array<long double, 4>>::iterator itr;

                    for (itr = results.begin(); itr != results.end(); ++itr) {
                        resFile << itr->second[0] << "-" 
                            << itr->second[1] << "-"
                            << itr->second[2] << "-"
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

    long double x_1, x_2, x_3, x_4, old_x_1s = 0, old_x_2s = 0, old_x_3s = 0, old_x_4s = 0;
    long double one = 1;
    int step;

    sqlite3* db;
    char* zErrMsg = 0;
    int rc;

    cout << "Enter step size: ";
    cin >> step;
    cout << std::endl;

    /*rc = sqlite3_open("results.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    }
    else {
        fprintf(stderr, "Opened database successfully\n");
    }

    execSql(db, "DROP TABLE IF EXISTS results;");
    execSql(db, "CREATE TABLE results("  \
        "id INTEGER PRIMARY KEY AUTOINCREMENT    NOT NULL," \
        "x1             VARCHAR    NOT NULL," \
        "x2             VARCHAR    NOT NULL," \
        "x3             VARCHAR    NOT NULL," \
        "x4             VARCHAR    NOT NULL," \
        "x1s            VARCHAR    NOT NULL," \
        "x2s            VARCHAR    NOT NULL," \
        "x3s            VARCHAR    NOT NULL," \
        "x4s            VARCHAR    NOT NULL)");

    execSql(db, "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES(1.2,1.2,1.2,1.2,1.2,1.2,1.2,1.2)");*/

    cout << "start!" << std::endl;
    //cout << 0.25 + 0.25 + 0.25 + 0.25 << std::endl;
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

    //vector<array<long double, 8>> all, res1, res2, res3, res4, res5, res6;

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


    //cout << "Results1: " << res1.size() << endl;
    //cout << "Results2: " << res2.size() << endl;
    //cout << "Results3: " << res3.size() << endl;
    //cout << "Results4: " << res4.size() << endl;
    //cout << "Results5: " << res5.size() << endl;
    //cout << "Results6: " << res6.size() << endl;
    //
    //all.insert(all.end(), res1.begin(), res1.end());
    //all.insert(all.end(), res2.begin(), res2.end());
    //all.insert(all.end(), res3.begin(), res3.end());
    //all.insert(all.end(), res4.begin(), res4.end());
    //all.insert(all.end(), res5.begin(), res5.end());
    //all.insert(all.end(), res6.begin(), res6.end());

    //res1.clear();
    //res2.clear();
    //res3.clear();
    //res4.clear();
    //res5.clear();
    //res6.clear();


    //cout << "Preparing SQLs\n";

    //vector<string> sql_insert_batches;
    //int batch_size = 0;
    //string batch_insert = "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES";
    //int index = 0;
    //while (index < all.size()) {
    //    array<long double, 8> item = all.at(index);

    //    batch_insert += "(" + to_string(item[0]) + ",";
    //    batch_insert += to_string(item[1]) + ",";
    //    batch_insert += to_string(item[2]) + ",";
    //    batch_insert += to_string(item[3]) + ",";
    //    batch_insert += to_string(item[4]) + ",";
    //    batch_insert += to_string(item[5]) + ",";
    //    batch_insert += to_string(item[6]) + ",";
    //    batch_insert += to_string(item[7]) + ")";

    //    //cout << batch_insert << endl;

    //    if (batch_size <= 10000 && index != (all.size() - 1))
    //    {
    //        batch_insert += ",";
    //    }
    //    else
    //    {
    //        sql_insert_batches.push_back(batch_insert); //TODO: might be pointer
    //        batch_insert = "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES";
    //        batch_size = 0;
    //    }
    //    batch_size++;
    //    index++;
    //}
    //all.clear();

    //cout << "Executing SQL\n";

    //int i = 1;
    //for (string SQL : sql_insert_batches) {
    //    const char* cSQL = SQL.c_str();

    //    execSql(db, cSQL);
    //    cout << "Inserted " << i*10000 << " of " << sql_insert_batches.size()*10000 << endl ;
    //    i++;
    //}


    //sqlite3_close(db);
    std::cout << "Hello World!\n";

    cin >> step;
}