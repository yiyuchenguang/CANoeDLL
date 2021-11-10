
#ifndef SIMPLE_H
#define SIMPLE_H

#if _MSC_VER > 1000 
#pragma once
#endif

// C++ 常规编译写法
#ifdef MYDLL_EXPORTS
#define MYDLL extern "C" __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif


//声明自定义函数
MYDLL int my_add(int a, int b);


#endif 