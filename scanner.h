#define _CRT_SECURE_NO_WARNINGS    //���԰�ȫ����
#pragma once
#ifndef MSCANNER_H
#define MSCANNER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
enum Token_Type  //ö�ټǺŵ����
{
	ORIGIN, SCALE, ROT, IS, TO, STEP, DRAW, FOR, FROM,   //������
	T,  //����
	SEMICO, L_BRACKET, R_BRACKET, COMMA,  //�ָ���
	PLUS, MINUS, MUL, DIV, POWER,         //�����
	FUNC,      //����
	CONST_ID,  //����
	NONTOKEN,  //�ռǺ�
	ERRTOKEN, //����Ǻ�
	COLOR, blue, green, red,
	size
};

typedef double(*MathFuncPtr) (double);

struct Token  //�Ǻŵ����ݽṹ,�Ǻ��������������
{
	Token_Type type;         //�Ǻŵ����
	char* lexeme;            //���ԣ�ԭʼ������ַ�������Ҫָ��TokenBuffer��ָ��
	double value;            //���ԣ�Ϊ������ֵ
	double(*FuncPtr)(double);//���ԣ�������ָ��
};

static Token TokenTab[] =//���ű��ֵ䣩������Ԫ�ص������ڼǺŵ�������ͬ
{//��ʶ���һ��IDʱ��ͨ���˱���ȷ�Ͼ������ĸ��Ǻ�
{ CONST_ID,    (char*)"PI",         3.1415926,  NULL },
{ CONST_ID,    (char*)"E",          2.71828,    NULL },
{ T,           (char*)"T",          0.0,        NULL },
{ FUNC,        (char*)"SIN",        0.0,        sin  },
{ FUNC,        (char*)"COS",        0.0,        cos  },
{ FUNC,        (char*)"TAN",        0.0,        tan  },
{ FUNC,        (char*)"LN",        0.0,         log  },
{ FUNC,        (char*)"EXP",        0.0,        exp  },
{ FUNC,        (char*)"SQRT",      0.0,        sqrt },
{ ORIGIN,      (char*)"ORIGIN",    0.0,        NULL },
{ SCALE,       (char*)"SCALE",     0.0,        NULL },
{ ROT,         (char*)"ROT",       0.0,        NULL },
{ IS,          (char*)"IS",         0.0,        NULL },
{ FOR,         (char*)"FOR",        0.0,        NULL },
{ FROM,        (char*)"FROM",      0.0,        NULL },
{ TO,          (char*)"TO",        0.0,        NULL },
{ STEP,        (char*)"STEP",       0.0,        NULL },
{ DRAW,        (char*)"DRAW",      0.0,        NULL },
{ COLOR ,      (char*)"COLOR",     0.0,         NULL},
{ blue,        (char*)"BLUE",       0.0,       NULL },
{ green,       (char*)"GREEN",     0.0,         NULL },
{ red,         (char*)"RED",        0.0,        NULL },
{ size,        (char*)"PIXELSIZE",  0.0,       NULL }
};

extern char* GetEnum(int i);
extern unsigned int LineNo;              //���ټǺ�����Դ�ļ��к�
extern int InitScanner(const char*);     //��ʼ���ʷ�������
extern Token GetToken(void);             //��ȡ�Ǻź���
extern void CloseScanner(void);
#endif