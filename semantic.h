#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <graphics.h>
#include <conio.h>
#include "parser.h"

//----------�ⲿ��������
extern double GetExprValue(struct ExprNode* root);//���ݽ���ñ��ʽ��ֵ
extern void DrawLoop(double Start, double End, double Step, struct ExprNode* HorPtr, struct ExprNode* VerPtr);//ͼ�λ���
extern void DelExprTree(struct ExprNode* root);//ɾ��һ����

#endif // !SEMANTIC_H