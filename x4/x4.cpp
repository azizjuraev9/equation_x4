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

using namespace std;

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

void threatF(int step, int start, int end, vector<array<long double, 8>> &results, bool track = false) {

    long double x_1, x_2, x_3, x_4, old_x_1s = 0, old_x_2s = 0, old_x_3s = 0, old_x_4s = 0;
    bool limit = false;

    thread::id this_id = this_thread::get_id();

    for (x_1 = start; x_1 >= end; x_1 -= 1) {
        if (limit) {
            cout << "Limit! \n";
            break;
        }
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

                    long double x_1s = getX_1s(sx_1, sx_2, sx_3, sx_4);
                    long double x_2s = getX_2s(sx_2, sx_3, sx_4);
                    long double x_3s = getX_3s(sx_3, sx_4);
                    long double x_4s = getX_4s(sx_4);

                    array<long double, 8> arr = { sx_1, sx_2, sx_3, sx_4, x_1s, x_2s, x_3s, x_4s };

                    results.push_back(arr);

                    if (
                        old_x_1s == x_1s
                        && old_x_2s == x_2s
                        && old_x_3s == x_3s
                        && old_x_4s == x_4s
                        ) {
                        limit = true;
                    }
                    old_x_1s = x_1s;
                    old_x_2s = x_2s;
                    old_x_3s = x_3s;
                    old_x_4s = x_4s;

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

    rc = sqlite3_open("results.db", &db);
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

    execSql(db, "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES(1.2,1.2,1.2,1.2,1.2,1.2,1.2,1.2)");

    cout << "start!" << std::endl;
    cout << 0.25 + 0.25 + 0.25 + 0.25 << std::endl;
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

    vector<array<long double, 8>> all, res1, res2, res3, res4, res5, res6;

    thread thr1(threatF, step, ((proc*6)+last_step_inc), (proc * 5), ref(res1), true);
    thread thr2(threatF, step, (proc*5), (proc*4), ref(res2), false);
    thread thr3(threatF, step, (proc*4), (proc*3), ref(res3), false);
    thread thr4(threatF, step, (proc*3), (proc*2), ref(res4), false);
    thread thr5(threatF, step, (proc*2), proc, ref(res5), false);
    thread thr6(threatF, step, proc, 0, ref(res6), true);

    thr1.join();
    thr2.join();
    thr3.join();
    thr4.join();
    thr5.join();
    thr6.join();


    cout << "Results1: " << res1.size() << endl;
    cout << "Results2: " << res2.size() << endl;
    cout << "Results3: " << res3.size() << endl;
    cout << "Results4: " << res4.size() << endl;
    cout << "Results5: " << res5.size() << endl;
    cout << "Results6: " << res6.size() << endl;
    
    all.insert(all.end(), res1.begin(), res1.end());
    all.insert(all.end(), res2.begin(), res2.end());
    all.insert(all.end(), res3.begin(), res3.end());
    all.insert(all.end(), res4.begin(), res4.end());
    all.insert(all.end(), res5.begin(), res5.end());
    all.insert(all.end(), res6.begin(), res6.end());

    res1.clear();
    res2.clear();
    res3.clear();
    res4.clear();
    res5.clear();
    res6.clear();


    cout << "Preparing SQLs\n";

    vector<string> sql_insert_batches;
    int batch_size = 0;
    string batch_insert = "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES";
    int index = 0;
    while (index < all.size()) {
        array<long double, 8> item = all.at(index);

        batch_insert += "(" + to_string(item[0]) + ",";
        batch_insert += to_string(item[1]) + ",";
        batch_insert += to_string(item[2]) + ",";
        batch_insert += to_string(item[3]) + ",";
        batch_insert += to_string(item[4]) + ",";
        batch_insert += to_string(item[5]) + ",";
        batch_insert += to_string(item[6]) + ",";
        batch_insert += to_string(item[7]) + ")";

        //cout << batch_insert << endl;

        if (batch_size <= 10000 && index != (all.size() - 1))
        {
            batch_insert += ",";
        }
        else
        {
            sql_insert_batches.push_back(batch_insert); //TODO: might be pointer
            batch_insert = "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES";
            batch_size = 0;
        }
        batch_size++;
        index++;
    }
    all.clear();

    cout << "Executing SQL\n";

    int i = 1;
    for (string SQL : sql_insert_batches) {
        const char* cSQL = SQL.c_str();

        execSql(db, cSQL);
        cout << "Inserted " << i*10000 << " of " << sql_insert_batches.size()*10000 << endl ;
        i++;
    }


    sqlite3_close(db);
    std::cout << "Hello World!\n";

    cin >> step;
}