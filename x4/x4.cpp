// x4.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include<set>
#include<iterator>
#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <array>
#include <vector>

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

int main()
{

    long double x_1, x_2, x_3, x_4, old_x_1s = 0, old_x_2s = 0, old_x_3s = 0, old_x_4s = 0;
    long double one = 1;
    double step;

    sqlite3* db;
    char* zErrMsg = 0;
    int rc;

    cout << "Enter step size: ";
    cin >> step;
    cout << endl;

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

    cout << "Start!" << endl;
    cout << 0.25 + 0.25 + 0.25 + 0.25 << endl;
    bool limit = false;

    vector<array<long double, 8>> results;

    for (x_1 = step; x_1 >= 0; x_1 -= 1) {
        if (limit) {
            cout << "Limit!" << endl;
            break;
        }
        for (x_2 = step; x_2 >= 0; x_2 -= 1) {
            for (x_3 = step; x_3 >= 0; x_3 -= 1) {
                for (x_4 = step; x_4 >= 0; x_4 -= 1) {

                    if( (x_1 + x_2 + x_3 + x_4) != step) {
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

                    results.push_back({ sx_1, sx_2, sx_3, sx_4, x_1s, x_2s, x_3s, x_4s });

                    if (
                        old_x_1s == x_1s
                        && old_x_2s == x_2s
                        && old_x_3s == x_3s
                        && old_x_4s == x_4s
                        ) {
                        results.push_back({ sx_1, sx_2, sx_3, sx_4, x_1s, x_2s, x_3s, x_4s });
                        limit = true;
                    }
                    old_x_1s = x_1s;
                    old_x_2s = x_2s;
                    old_x_3s = x_3s;
                    old_x_4s = x_4s;

                }
            }
        }
        std::cout << 100 - (100 * (x_1 / step)) << "%" << std::endl;
    }

    vector<string> sql_insert_batches;


    cout << "Found : " << results.size() << " results\n";
    cout << "Preparing SQLs\n";

    int batch_size = 0;
    string batch_insert = "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES";
    int index = 0;
    while (index < results.size()) {
        array<long double, 8> item = results.at(index);

        batch_insert += "(" + to_string(item[0]) + ",";
        batch_insert += to_string(item[1]) + ",";
        batch_insert += to_string(item[2]) + ",";
        batch_insert += to_string(item[3]) + ",";
        batch_insert += to_string(item[4]) + ",";
        batch_insert += to_string(item[5]) + ",";
        batch_insert += to_string(item[6]) + ",";
        batch_insert += to_string(item[7]) + ")";

        //cout << batch_insert << endl;

        if (batch_size <= 10000 && index != (results.size() - 1))
        {
            batch_insert += ",";
        }
        else
        {
            sql_insert_batches.push_back(batch_insert); //TODO: might be pointer
            batch_insert = "INSERT INTO results(x1,x2,x3,x4,x1s,x2s,x3s,x4s) VALUES";
            batch_size = 0;
        }
        index++;
    }

    cout << "Executing SQLs\n";

    for (string SQL : sql_insert_batches) {
        const char* cSQL = SQL.c_str();

        execSql(db, cSQL);
        cout << "Inserted 1000 of " << sql_insert_batches.size() * 1000 << endl;
    }

    sqlite3_close(db);
    std::cout << "Hello World!\n";
    cin >> step;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
