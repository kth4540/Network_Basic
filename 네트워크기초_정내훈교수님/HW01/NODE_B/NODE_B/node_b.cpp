#include <iostream>

#include "conn.h"

using namespace std;

void do_node_b()
{
	int a;
	bool end_node = false;

	cout << "Hello World, I am node B.\n";
	do {
		cout << "\nSelect One => 0.Quit,    1:Set CONN,     2:Reset CONN,     3:Read CONN : ";
		cin >> a;
		cout << "You entered " << a << endl;
		switch (a) {
		case 0: end_node = true;  break;
		case 1: g_conn.set(true); break;
		case 2: g_conn.set(false); break;
		case 3: if (true == g_conn.get()) cout << "True\n";
			  else cout << "False\n";
		}
	} while (false == end_node);
	cout << "Bye.\n";
}