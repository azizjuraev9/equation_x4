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
#include <sqlite3.h>

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

bool execSql(sqlite3* db, const char* SQL) {
    char* zErrMsg = 0;
    int rc;
    rc = sqlite3_exec(db, SQL, callback, 0, &zErrMsg);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return false;
    }
    return true;
}

void resultsForOne(array<long double, 4> initValues, vector<array<long double, 4>> &results, bool &limit, int &limitSize) {

    long double
        x_1 = initValues[0], 
        x_2 = initValues[1], 
        x_3 = initValues[2], 
        x_4 = initValues[3], 
    i = 0;


    limit = false;
    limitSize = 0;

    map<string, array<long double, 4>> tmpResults;

    while (i < 1000 && !limit) {
        i++;

        x_1 = getX_1s(x_1, x_2, x_3, x_4);
        x_2 = getX_2s(x_2, x_3, x_4);
        x_3 = getX_3s(x_3, x_4);
        x_4 = getX_4s(x_4);

        array<long double, 4> arr = {x_1, x_2, x_3, x_4 };
        string key = to_string(x_1) + "-" + to_string(x_2) + "-" + to_string(x_3) + "-" + to_string(x_4);

        map<string, array<long double, 4>>::iterator it;
        it = tmpResults.find(key);

        tmpResults.insert(pair<string, array<long double, 4>>(key, arr));
        results.push_back(arr);
        
        if (it != tmpResults.end()) {
            limit = true;
        }
    }

}

void threatF(int step, int start, int end, map<string, vector<array<long double, 4>>>& results, bool track = false) {

    long double x_1, x_2, x_3, x_4;

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

                    vector<array<long double, 4>> resultsFO;
                    string key = to_string(sx_1) + "-" + to_string(sx_2) + "-" + to_string(sx_3) + "-" + to_string(sx_4);
                    resultsFO.push_back(arr);
                    resultsForOne(arr, resultsFO, limit, limitSize);
                    results.insert(pair<string, vector<array<long double, 4>>>(key, resultsFO));

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

    //map<string, vector<array<long double, 4>>> results;
    map<string, vector<array<long double, 4>>> all, res1, res2, res3, res4, res5, res6;

    thread thr1(threatF, step, ((proc*6)+last_step_inc), (proc * 5), ref(res1), true);
    thread thr2(threatF, step, (proc*5), (proc*4), ref(res2), true);
    thread thr3(threatF, step, (proc*4), (proc*3), ref(res3), true);
    thread thr4(threatF, step, (proc*3), (proc*2), ref(res4), true);
    thread thr5(threatF, step, (proc*2), proc, ref(res5), true);
    thread thr6(threatF, step, proc, 0, ref(res6), true);

    thr1.join();
    thr2.join();
    thr3.join();
    thr4.join();
    thr5.join();
    thr6.join();

    all.insert(res1.begin(), res1.end());
    all.insert(res2.begin(), res2.end());
    all.insert(res3.begin(), res3.end());
    all.insert(res4.begin(), res4.end());
    all.insert(res5.begin(), res5.end());
    all.insert(res6.begin(), res6.end());

    res1.clear();
    res2.clear();
    res3.clear();
    res4.clear();
    res5.clear();
    res6.clear();

    sqlite3* db;
    char* zErrMsg = 0;
    int rc;

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
        "group_name      VARCHAR    NOT NULL," \
        "x1             VARCHAR    NOT NULL," \
        "x2             VARCHAR    NOT NULL," \
        "x3             VARCHAR    NOT NULL," \
        "x4             VARCHAR    NOT NULL)");
    execSql(db, "CREATE INDEX group_name_index ON results (group_name)");


    cout << "Preparing SQLs\n";

    vector<string> sql_insert_batches;
    int batch_size = 0;
    string batch_insert = "INSERT INTO results(group_name, x1,x2,x3,x4) VALUES";


    for (auto const& x : all)
    {
        vector<array<long double, 4>> results = x.second;

        int index = 0;
        while (index < results.size()) {
            array<long double, 4> item = results.at(index);

            std::ostringstream x1_str, x2_str, x3_str, x4_str;

            x1_str << fixed;
            x1_str << setprecision(13); 
            x1_str << item[0];

            x2_str << fixed;
            x2_str << setprecision(13);
            x2_str << item[1];

            x3_str << fixed;
            x3_str << setprecision(13);
            x3_str << item[2];

            x4_str << fixed;
            x4_str << setprecision(13);
            x4_str << item[3];

            batch_insert += "(\"" + x.first + "\",";
            batch_insert += "\"" + x1_str.str() + "\",";
            batch_insert += "\"" + x2_str.str() + "\",";
            batch_insert += "\"" + x3_str.str() + "\",";
            batch_insert += "\"" + x4_str.str() + "\")";

            if (batch_size <= 100000 && index != (results.size() - 1))
            {
                batch_insert += ",";
            }
            else
            {

                sql_insert_batches.push_back(batch_insert); //TODO: might be pointer
                batch_insert = "INSERT INTO results(group_name, x1,x2,x3,x4) VALUES";
                batch_size = 0;
            }
            batch_size++;
            index++;
        }

    }

    cout << endl << sql_insert_batches.size() << endl;

    
    all.clear();

    cout << "Executing SQL\n";

    int i = 1;
    for (string SQL : sql_insert_batches) {
        const char* cSQL = SQL.c_str();

        execSql(db, cSQL);
        cout << "Inserted " << i * 10000 << " of " << sql_insert_batches.size() * 10000 << endl;
        i++;
    }


    sqlite3_close(db);


    std::cout << "Hello World!\n";

    cin >> step;
}