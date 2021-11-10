// CallDllTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <tchar.h>

int main()
{

	HINSTANCE handle = LoadLibrary(_T("SimpleDemo.dll"));//LoadLibrary ddl ,��Ҫ����<tchar.h> ����_T����Ȼ���뱨��
	std::cout << "dll�ľ������ֵ:"<< handle <<"\n";//��ӡdll�ľ����ֵַ,��Ϊ0��˵�����óɹ�
	if (handle)
	{
		typedef int(*DLL_FUNCTION_ADD) (int, int); //typedef����һ�º���ָ�룬(int, int) ��Ҫ����dll�еĺ�����������
		DLL_FUNCTION_ADD dll_add = (DLL_FUNCTION_ADD)GetProcAddress(handle, "my_add"); //ʹ��GetProcAddress�õ�dll�еĺ�����������Ϊ dll_add
		std::cout << "dll �����ľ������ֵ:" << handle << "\n";//��ӡdll�ĺ��������ֵַ,��Ϊ0��˵�����óɹ�
		if (dll_add)
		{
			int result = dll_add(10, 11); //���ڿ�����add��
			std::cout << "dll_add������:" << result << "\n";
			FreeLibrary(handle); //ж�ؾ������
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
