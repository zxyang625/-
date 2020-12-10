#include "semantic.h"
extern  double Parameter,Origin_x, Origin_y,Scale_x, Scale_y,Rot_angle;

double GetExprValue(struct ExprNode* root);//��ñ��ʽ��ֵ
void DrawLoop(double Start, double End, double Step, struct ExprNode* HorPtr, struct ExprNode* VerPtr);//ͼ�λ���
void DelExprTree(struct ExprNode* root);//ɾ��һ����
static void CalcCoord(struct ExprNode* Hor_Exp, struct ExprNode* Ver_Exp, double& Hor_x, double& Ver_y);//����������

//���㱻���Ƶ������
static void CalcCoord(struct ExprNode* Hor_Exp,  //��������ʽ�﷨���ĸ��ڵ�
	struct ExprNode* Ver_Exp,  //��������ʽ�﷨���ĸ��ڵ�
	double& Hor_x,  //�������ֵ���𷵻�ֵ������
	double& Ver_y)  //��������ֵ���𷵻�ֵ������
{
	double HorCord, VerCord, Hor_tmp;
	HorCord = GetExprValue(Hor_Exp);
	VerCord = GetExprValue(Ver_Exp);  //���ݱ��ʽ���﷨������ԭʼ����
	HorCord *= Scale_x;
	VerCord *= Scale_y;   //�Ƚ��б����任
	Hor_tmp = HorCord * cos(Rot_angle) + VerCord * sin(Rot_angle);//��Ҫ�����м����Hor_tmp������HorCord��ֵ����ǰ�ı�
	VerCord = VerCord * cos(Rot_angle) - HorCord * sin(Rot_angle);
	HorCord = Hor_tmp;    //�ٽ�����ת�任
	HorCord += Origin_x;
	VerCord += Origin_y;   //������ƽ�Ʊ任
	Hor_x = HorCord;
	Ver_y = VerCord;    //���µ�x��yֵ����Hor_x,Ver_y
}

//ѭ�����Ƶ������
void DrawLoop(double Start,   //��ʼ
	double End,               //��ֹ
	double Step,              //����
	struct ExprNode* HorPtr,  //��������ʽ�﷨���ĸ��ڵ�
	struct ExprNode* VerPtr)  //��������ʽ�﷨���ĸ��ڵ�
{
	double x, y;
	for (Parameter = Start; Parameter <= End; Parameter += Step)//��T�ڷ�Χ�ڵ�ÿһ��ֵ�������
	{
		CalcCoord(HorPtr, VerPtr, x, y);  //����Ҫ���Ƶ��ʵ������
		putpixel((int)x, (int)y, RGB(250, 250, 250));  //���������
	}
}

//������ʽ��ֵ
double  GetExprValue(struct ExprNode* root)  //�����Ǳ��ʽ�ĸ�
{  //���������﷨��  ���ݲ�ͬ�Ľڵ����ͼ��㵱ǰ���ڵ��ֵ
	if (root == NULL)
		return 0.0;
	switch (root->OpCode)
	{
		//��Ԫ�����
	case PLUS:
		return GetExprValue(root->Content.CaseOperater.Left) + GetExprValue(root->Content.CaseOperater.Right);
	case MINUS:
		return GetExprValue(root->Content.CaseOperater.Left) - GetExprValue(root->Content.CaseOperater.Right);
	case MUL:
		return GetExprValue(root->Content.CaseOperater.Left) * GetExprValue(root->Content.CaseOperater.Right);
	case DIV:
		return GetExprValue(root->Content.CaseOperater.Left) / GetExprValue(root->Content.CaseOperater.Right);
	case POWER:
		return pow(GetExprValue(root->Content.CaseOperater.Left), GetExprValue(root->Content.CaseOperater.Right));
	case FUNC: return (*root->Content.CaseFunc.MathFuncPtr)(GetExprValue(root->Content.CaseFunc.Child));//�������ã�����ֻ��һ�����Child
	case CONST_ID: return root->Content.CaseConst;//����
	case T: return *(root->Content.CaseParmPtr);//������ֵ
	default: return 0.0;
	}  //����ֵ�Ǳ��ʽ��ֵ
}

//�ݹ�ɾ��һ���﷨��
void DelExprTree(struct ExprNode* root)
{
	if (root == NULL)//����������������ɾ������
		return;
	switch (root->OpCode)
	{
	case PLUS:       //��Ԫ���������ӵ��ڲ��ڵ�
	case MINUS:
	case MUL:
	case DIV:
	case POWER:
		DelExprTree(root->Content.CaseOperater.Left);
		DelExprTree(root->Content.CaseOperater.Right);
		break;
	case FUNC:       //һԪ����һ�����ӵ��ڲ��ڵ�
		DelExprTree(root->Content.CaseFunc.Child);
		break;
	default:  break;       //Ҷ�ӽڵ�
	}
	delete(root);          //ɾ�����ڵ�
}