
#ifndef SIMPLE_H
#define SIMPLE_H

#if _MSC_VER > 1000 
#pragma once
#endif

// C++ �������д��
#ifdef MYDLL_EXPORTS
#define MYDLL extern "C" __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif


//�����Զ��庯��
MYDLL int my_add(int a, int b);


#endif 