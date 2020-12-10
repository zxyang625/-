#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"

typedef double(*FuncPtr)(double);
struct ExprNode        //�﷨���ڵ�����
{
	enum Token_Type OpCode;  //scanner�еĲ���
	union//�����壬ÿһ���ֻ�������������֮һ
	{
		struct { 
			ExprNode* Left, * Right;
		}CaseOperater;//��Ԫ���㣺�����Һ��ӵ��ڲ��ڵ�
		struct { 
			ExprNode* Child; 
			FuncPtr MathFuncPtr;
		}CaseFunc;//�������ã�ֻ��һ�����ӵ��ڲ��ڵ㣬����һ��ָ���Ӧ��������ָ�� MathFuncPtr

		double CaseConst;  //������Ҷ�ӽڵ�  ��ֵ
		double* CaseParmPtr;  //����T   ��ֵ�����Tֵ�ĵ�ַ
	}Content;
};
extern void Parser(char* FILE);   //�����﷨������

#endif // !PARSER_H