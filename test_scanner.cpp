
/*
#include"scanner.h"//���Դʷ�����������ȷ��
using namespace std;
void main()
{
    Token token;
    char file[] = "draw2.txt";
    if (!InitScanner(file))                                    //��ʼ���ʷ�������
    {
        printf("Open Sorce File Error !\n");
        return;
    }
    printf("�Ǻ����         �ַ���        ����ֵ           ����ָ��\n");
    printf("--------------------------------------------\n");
    while (true)
    {
        token = GetToken();//���һ���Ǻ�
        if (token.type != NONTOKEN)//�Ǻŵ�����Ǵ��󣬾ʹ�ӡ����������
            printf("%12s,%12s,%12f,%12x\n", GetEnum(token.type), token.lexeme, token.value, token.FuncPtr);
        else
            break;
    }
    printf("-------------------------------------------\n");
    CloseScanner();//�رմʷ�������
    system("pause");
}
*/