// CallDllTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <tchar.h>

int main()
{

	HINSTANCE handle = LoadLibrary(_T("SimpleDemo.dll"));//LoadLibrary ddl ,需要引入<tchar.h> 加上_T，不然编译报错
	std::cout << "dll的句柄返回值:"<< handle <<"\n";//打印dll的句柄地址值,不为0就说明调用成功
	if (handle)
	{
		typedef int(*DLL_FUNCTION_ADD) (int, int); //typedef定义一下函数指针，(int, int) 是要调用dll中的函数参数类型
		DLL_FUNCTION_ADD dll_add = (DLL_FUNCTION_ADD)GetProcAddress(handle, "my_add"); //使用GetProcAddress得到dll中的函数，重命名为 dll_add
		std::cout << "dll 函数的句柄返回值:" << handle << "\n";//打印dll的函数句柄地址值,不为0就说明调用成功
		if (dll_add)
		{
			int result = dll_add(10, 11); //终于可以用add了
			std::cout << "dll_add结算结果:" << result << "\n";
			FreeLibrary(handle); //卸载句柄，，
		}
	}
    std::cout << "Hello World!\n";
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
