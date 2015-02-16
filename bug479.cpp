/**
 * @file bug479.cpp regression case for bug 479 ( Undefined filter reference in MaxScale.cnf causes a crash)
 *
 * - Maxscale.cnf with "filters=non existing filter | не существуюший фильтер", cheks error log for warnings and
 * - check if Maxscale is alive
 */

#include <my_config.h>
#include <iostream>
#include <unistd.h>
#include "testconnections.h"

using namespace std;

int main(int argc, char *argv[])
{
    TestConnections * Test = new TestConnections(argv[0]);
    int global_result = CheckLogErr((char *) "Unable to find filter 'non existing filter", TRUE);
    global_result = CheckLogErr((char *) "не существуюший фильтер", TRUE);
    global_result += CheckMaxscaleAlive();
    Test->Copy_all_logs(); return(global_result);
}

