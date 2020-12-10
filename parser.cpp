/* ������ݹ飬��ȡ�����ӣ����������Ժ�Ĳ���ʽΪ
Program->Statement SEMICO Program|��

Statement->OriginStatement|ScalseStatement|RotStatement|ForStatement|ColorStatement
OriginStatement->ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET
ScaleStatement->SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET
RotStatement->ROT IS Expression
ForStatement->FOR T FROM Expression TO Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET
ColorStatement->COLOR IS BLUE|GREEN|RED|(Expression,Expression,Expression)
PixelSizeStatement->PIXELSIZE IS CONST_ID

Expression->Term{ (PLUS|MINUS) Term }
Term->Factor{ (MUL|DIV) Factor}
Factor->PLUS Factor|MINUS Factor|Component
Component->Atom POWER Component|Atom
Atom->CONST_ID | T | FUNC L_BRACKET Expression R_BRACKET | L_BRACKET Expression R_BRACKET

*/

#include "parser.h"
#include "semantic.h"

double Parameter = 0, Origin_x = 0, Origin_y = 0, Scale_x = 1, Scale_y = 1, Rot_angle = 0;//Parameter�ǲ���T�Ĵ洢�ռ䣺��¼Tÿ�μ�һ��ı仯
int Color_R = 250, Color_G = 250, Color_B = 250;
static Token token;//�Ǻ�
static void FetchToken();//���ôʷ���������GetToken���ѵõ��ĵ�ǰ��¼��������������õ��ļǺ���errtoken����ָ��һ���﷨����
static void MatchToken(enum Token_Type AToken);//ƥ�䵱ǰ�Ǻ�
static void SyntaxError(int case_of);//�����﷨������ӳ���,��ӡ�����кͶ�Ӧ�ַ�������ֹ�������С�
static void ErrMsg(unsigned LineNo, char* descrip, char* string);  //��ӡ������Ϣ
static void PrintSyntaxTree(struct ExprNode* root, int indent);  //ǰ�������ӡ��

//���ս���ݹ��ӳ������� ��2��
//��1���﷨�������������﷨������������ӳ�������Ϊ����->void���͵ĺ���
static void Program();//�ݹ��½�����
static void Statement();
static void OriginStatement();
static void RotStatement();
static void ScaleStatement();
static void ForStatement();
static void ColorStatement();

//��2���﷨����+�����﷨������˱��ʽ�����Ϊ����ֵΪָ���﷨���ڵ��ָ��ĺ�����
static struct ExprNode* Expression();//����Ӽ�
static struct ExprNode* Term();//����Term���MUL��DIV�����ȼ�������˳�
static struct ExprNode* Factor();//��������
static struct ExprNode* Component();//������
static struct ExprNode* Atom();//����ԭ�ӱ��ʽ

void call_match(char* x)//��ӡ����ƥ����ַ�
{
    printf("matchtoken    "); printf(x); printf("\n");
}

void Tree_trace(ExprNode* x)//��ӡ�﷨��
{
    PrintSyntaxTree(x, 1);
}


//�ⲿ�ӿ����﷨�����캯������
extern void Parser(char* SrcFilePtr);
static struct ExprNode* MakeExprNode(enum Token_Type opcode, ...);//�����﷨����һ���ڵ�

static void FetchToken()//���ôʷ���������GetToken���ѵõ��ĵ�ǰ��¼����������
{
    token = GetToken();
    if (token.type == ERRTOKEN)
        SyntaxError(1); //����õ��ļǺ���errtoken������ʾ��errtoken
}

//ƥ�䵱ǰ�ļǺ�
static void MatchToken(enum Token_Type The_Token)
{
    if (token.type != The_Token)
        SyntaxError(2);//���ʧ�ܣ�����ʾ�﷨����
    FetchToken();//���ɹ������ȡ��һ��
}

//�﷨������
static void SyntaxError(int case_of)
{
    switch (case_of)
    {
    case 1: ErrMsg(LineNo, (char*)"ERRTOKEN:", token.lexeme);//��Ҫ��const char����ת��Ϊchar*����
        break;
    case 2: ErrMsg(LineNo, (char*)"UNEXPECTED TOKEN:", token.lexeme);
        break;
    }
}

//��ӡ������Ϣ
void ErrMsg(unsigned LineNo, char* descrip, char* string)
{
    printf("WARNING: LINE%5d:%s %s !\n", LineNo, descrip, string);
    CloseScanner();
    exit(1);
}

//�ݹ������������ӡ���ʽ���﷨������-->��-->��.���У�CONST_ID��T�Լ�ΪҶ�ӽ�㣬FUNC��һ���ӽ�㣬�������������ӽ��
void PrintSyntaxTree(struct ExprNode* root, int indent)
{
    int temp;
    for (temp = 1; temp <= indent; temp++)//����������ȴ�ӡ�Ʊ��
        printf("\t");  //����
    switch (root->OpCode)  //��ѯ�����Token_Type
    {//��ӡ���ڵ�
    case PLUS:  printf("%s\n", "+"); break;
    case MINUS: printf("%s\n", "-"); break;
    case MUL:   printf("%s\n", "*"); break;
    case DIV:   printf("%s\n", "/"); break;
    case POWER: printf("%s\n", "**"); break;
    case FUNC:  printf("%p\n", root->Content.CaseFunc.MathFuncPtr); break;//��ӡ�����ĺ�����ַ
    case CONST_ID: printf("%f\n", root->Content.CaseConst); break;//��ӡ����ֵ
    case T:     printf("%s\n", "T"); break;//��ӡ����T
    default:    printf("Error Tree Node!\n"); exit(0);
    }
    if (root->OpCode == CONST_ID || root->OpCode == T)  //Ҷ�ӽڵ㷵��
       return;  //�����Ͳ���ֻ��Ҷ�ӽڵ� ��������ֵ����������ֵ��ַ
    if (root->OpCode == FUNC)  //�ݹ��ӡһ�����ӽڵ�
        PrintSyntaxTree(root->Content.CaseFunc.Child, indent + 1);  //�����к��ӽڵ��ָ��������ָ��
    else  //�ݹ��ӡ�������ӽڵ�
    {  //��Ԫ���㣺���Һ��ӵ��ڲ��ڵ�
        PrintSyntaxTree(root->Content.CaseOperater.Left, indent + 1);
        PrintSyntaxTree(root->Content.CaseOperater.Right, indent + 1);
    }
}

//��ͼ���Խ�������ڣ�����������ⲿ�ӿڣ�
void Parser(char* SrcFilePtr)   //�﷨�����������
{
    printf("Enter in Parser\n");
    if (!InitScanner(SrcFilePtr))  //��ʼ���ʷ�������
    {
        printf("Failed to Open File!\n");
        return;
    }
    FetchToken();  //��ȡ��һ���Ǻ�
    Program();  //�ݹ��½�����
    CloseScanner();  //�رմʷ�������
    printf("Exit from Parser\n");
    return;
}

//Program�ĵݹ��ӳ���
//Program->Statement SEMICO Program|��
static void Program()
{
    printf("Enter in Program\n");
    while (token.type != NONTOKEN)  //�Ǻ����Ͳ��ǿ�,���ı�û�н���
    {//ѭ�������ͼ���ͷֺ�
        Statement();  //ת��ÿһ���ķ�
        MatchToken(SEMICO);//ƥ�䵽�ָ���
    }
    printf("Exit from Program\n");
}

//Statement�ĵݹ��ӳ���ʶ��ת��ÿһ���ķ�
static void Statement()
{
    printf("Enter in Statement\n");
    switch (token.type)
    {
    case ORIGIN:   OriginStatement(); break;
    case SCALE:   ScaleStatement(); break;
    case ROT:   RotStatement(); break;
    case FOR:   ForStatement(); break;
    case COLOR:  ColorStatement(); break;
    default:   SyntaxError(2);//�����ϵľ���ʾ������Ϣ
    }
    printf("Exit from Statement");
}


//OriginStatement�ĵݹ��ӳ���
//OriginStatement->ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET
//eg:origin is (20, 200);
static void OriginStatement(void)
{
    struct ExprNode* tmp;  //�﷨���ڵ������
    printf("Enter in Original Statement\n");
    MatchToken(ORIGIN);
    MatchToken(IS);
    MatchToken(L_BRACKET);  
    tmp = Expression();
    Origin_x = GetExprValue(tmp);    //��ȡ�������ƽ�ƾ���
    DelExprTree(tmp);    //�ͷ�tmp��ռ�ռ�
    MatchToken(COMMA);   
    tmp = Expression();
    Origin_y = GetExprValue(tmp);   //��ȡ�������ƽ�ƾ���
    DelExprTree(tmp);   //�ͷ�tmp��ռ�ռ�
    MatchToken(R_BRACKET);    
    printf("Exit from Origin Statement");
}


//ScaleStatement�ĵݹ��ӳ���
//ScaleStatement->SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET
//eg:scale is (40, 40);
static void ScaleStatement(void)
{
    struct ExprNode* tmp;
    printf("Enter in Scale Statement\n");
    MatchToken(SCALE);
    MatchToken(IS);
    MatchToken(L_BRACKET);     
    tmp = Expression();
    Scale_x = GetExprValue(tmp);   //��ȡ������ı�������
    DelExprTree(tmp);//�ͷŲ�����ռ�ռ�
    MatchToken(COMMA);   
    tmp = Expression();
    Scale_y = GetExprValue(tmp);//��ȡ������ı�������
    DelExprTree(tmp);//�ͷŲ�����ռ�ռ�
    MatchToken(R_BRACKET);    
    printf("Exit from Scale Statement\n");
}

//RotStatement�ĵݹ��ӳ���
//RotStatement->ROT IS Expression
//eg:rot is 0;
static void RotStatement(void)
{
    struct ExprNode* tmp;
    printf("Enter in Rot Statement\n");
    MatchToken(ROT);
    MatchToken(IS);     
    tmp = Expression();
    Rot_angle = GetExprValue(tmp);     //��ȡ��ת�Ƕ�
    DelExprTree(tmp);//�ͷŲ�����ת�Ƕ��﷨����ռ�ռ�
    printf("Exit from Rot Statement\n");
}

//ForStatement�ĵݹ��ӳ���,���Ҳ��ķ����ŵ�չ��->�����ս����ֱ��ƥ�䣬�������ս���͵�����Ӧ�ӳ���
//ForStatement->FOR T FROM Expression TO Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET)
static void ForStatement()
{
    //eg:for T from 0 to 2*pi step pi/50 draw (t, -sin(t));
    double Start, End, Step;  //��ͼ��㡢�յ㡢����
    struct ExprNode* start_ptr, * end_ptr, * step_ptr, * x_ptr, * y_ptr;  //ָ������ʽ�﷨�����ڵ�
    printf("Enter in For Statement\n");
    //�������ս���͵�����Ӧ�ӳ���
    MatchToken(FOR); call_match((char*)"FOR");
    MatchToken(T); call_match((char*)"T");
    MatchToken(FROM); call_match((char*)"FROM");  
    start_ptr = Expression();   //��ò��������ʽ���﷨��
    Start = GetExprValue(start_ptr);   //������������ʽ��ֵ
    DelExprTree(start_ptr);   //�ͷŲ�������﷨����ռ�ռ�  
    MatchToken(TO); call_match((char*)"TO"); 
    end_ptr = Expression();  //��������յ���ʽ�﷨��
    End = GetExprValue(end_ptr);  //��������յ���ʽ��ֵ 
    DelExprTree(end_ptr);   //�ͷŲ����յ��﷨����ռ�ռ�
    MatchToken(STEP); call_match((char*)"STEP");   
    step_ptr = Expression();   //��������������ʽ�﷨��
    Step = GetExprValue(step_ptr);   //��������������ʽ��ֵ  
    DelExprTree(step_ptr);   //�ͷŲ��������﷨����ռ�ռ�
    MatchToken(DRAW);
    call_match((char*)"DRAW");
    MatchToken(L_BRACKET);
    call_match((char*)"("); 
    x_ptr = Expression();     
    MatchToken(COMMA);
    call_match((char*)",");   
    y_ptr = Expression();   
    MatchToken(R_BRACKET);
    call_match((char*)")");
    DrawLoop(Start, End, Step, x_ptr, y_ptr);       //����ͼ��
    DelExprTree(x_ptr);                             //�ͷź������﷨����ռ�ռ�
    DelExprTree(y_ptr);                             //�ͷ��������﷨����ռ�ռ�
    printf("Exit from For Statement\n");
}

//COLOR���ӳ�������BLUE,GREEN,RED�޸�RGB(r,g,b)��ֵ
//ColorStatement->COLOR IS BLUE|GREEN|RED|(Expression,Expression,Expression)
static void ColorStatement()
{
    Token_Type token_tmp;
    struct ExprNode* value_ptr;
    MatchToken(COLOR); call_match((char*)"COLOR");
    MatchToken(IS); call_match((char*)"IS");
    switch (token.type)
    {//��ɫ��Ϊ����
     //��һ��ֱ��ָ����ɫ���죬�̣�����
    case red:Color_R = 255; Color_B = 0; Color_G = 0; MatchToken(token.type);call_match((char*)"RED");break;
    case green:Color_R = 0; Color_B = 0; Color_G = 255;MatchToken(token.type);call_match((char*)"GREEN");break;
    case blue:Color_R = 0; Color_B = 255; Color_G = 0; MatchToken(token.type);call_match((char*)"BLUE");break;
    //�ڶ���ͨ��RGB��r,g,b��������ɫ
    case L_BRACKET:
        MatchToken(L_BRACKET);
        value_ptr = Expression();
        Color_R = GetExprValue(value_ptr);
        MatchToken(COMMA);
        value_ptr = Expression();
        Color_G = GetExprValue(value_ptr);
        MatchToken(COMMA);
        value_ptr = Expression();
        Color_B = GetExprValue(value_ptr);
        MatchToken(R_BRACKET);
        printf(" (%d,%d,%d)\n",Color_R,Color_G,Color_B);
        break;
    default: break;
    }
    printf("Exit from Color Statement\n");
}

//Expression�ĵݹ��ӳ���,���ʽӦ�����������Ż��޷��ſ�ͷ�������ɸ����ԼӼ������Ӷ��ɡ�
//�Ѻ������Ϊ�﷨���ڵ��ָ�룬�ں���������2���﷨���ڵ��ָ��������ֱ���ΪExpression���Ҳ�������Term�����﷨���ڵ�ָ��
//Expression->Term{ (PLUS|MINUS) Term }
static struct ExprNode* Expression()    //չ���Ҳ������ҹ����﷨��
{
    struct ExprNode* left, * right;      //���������ڵ��ָ��
    Token_Type token_tmp;     //��ǰ�Ǻ�����
    printf("Enter in Expression\n");
    left = Term();     //������������ҵõ����﷨��
    while (token.type == PLUS || token.type == MINUS)
    {
        token_tmp = token.type;
        MatchToken(token_tmp);
        right = Term();      //�����Ҳ������ҵõ����﷨��
        left = MakeExprNode(token_tmp, left, right);    //����������﷨�������Ϊ������
        Tree_trace(left);   //��ӡ���ʽ���﷨��
    }
    printf("Exit from Expression\n");
    return left;       //�������ձ��ʽ���﷨��
}

//Term�ĵݹ��ӳ������������ɸ������Գ˳������Ӷ���
//Term->Factor{ (MUL|DIV) Factor }
static struct ExprNode* Term()
{
    struct ExprNode* left, * right;
    Token_Type token_tmp;
    left = Factor();
    while(token.type == MUL || token.type == DIV)
    {
        token_tmp = token.type;
        MatchToken(token_tmp);
        right = Factor();
        left = MakeExprNode(token_tmp, left, right);
        //Tree_trace(left);������������Ӧ��ֻȱ�ٸ����
    }
    return left;//����������
}

//Factor�ĵݹ��ӳ��������������һ����ʶ����һ�����֣�����һ�����������������ӱ��ʽ
//Factor->PLUS Factor|MINUS Factor|Component
static struct ExprNode* Factor()
{
    struct ExprNode* left, * right;
    if (token.type == PLUS)           //ƥ��һԪ������
    {
        MatchToken(PLUS);
        right = Factor();             //���ʽ�˻�Ϊ�����Ҳ������ı��ʽ
    }
    else if (token.type == MINUS)
    {
        MatchToken(MINUS);//������ͷ����Ϊ0.0-����
        right = Factor();
        left = new ExprNode;
        left->OpCode = CONST_ID;
        left->Content.CaseConst = 0.0;
        right = MakeExprNode(MINUS, left, right);//��Factor����ʽ֪Factor��right��Ϊ�գ�����ͨ��right���ɽ��
    }
    else
        right = Component();         //ƥ����ս��Component
    return right;
}


//Component->Atom POWER Component|Atom
//Component�ĵݹ��ӳ���
static struct ExprNode* Component()//�ҽ��
{
    struct ExprNode* left, * right;
    left = Atom();
    if (token.type == POWER)
    {
        MatchToken(POWER);
        right = Component();         //�ݹ����Component��ʵ��POWER���ҽ��
        left = MakeExprNode(POWER, left, right);//����ֻ��ͨ��left����Component��㣬��Ϊ������right����tmp���п���Ϊ��
    }
    return left;
}


//Atom->CONST_ID | T | FUNC L_BRACKET Expression R_BRACKET | L_BRACKET Expression R_BRACKET
//Atom�ĵݹ��ӳ�������ָ��� ���� ���� ����
static struct ExprNode* Atom()
{
    struct Token t = token;
    struct ExprNode* address = nullptr, * tmp;
    switch (token.type)
    {
    case CONST_ID:
        MatchToken(CONST_ID);
        address = MakeExprNode(CONST_ID, t.value);//����������Ӧ��������ֵ
        break;
    case T:
        MatchToken(T);
        address = MakeExprNode(T);//����������Ӧ��ȡParameter�ĵ�ַ��Ϊ����T�ĵ�ַ
        break;
    case FUNC:
        MatchToken(FUNC);
        MatchToken(L_BRACKET);//sin��cos��tan��ln��exp��sqrt����������������
        tmp = Expression();//���������ʽ
        address = MakeExprNode(FUNC, t.FuncPtr, tmp);//���û�ȡ��tmpΪ�ú������ɽ��
        MatchToken(R_BRACKET);//�����������
        break;
    case L_BRACKET:
        MatchToken(L_BRACKET);
        address = Expression();//�����������ʽ
        MatchToken(R_BRACKET);
        break;
    default:
        SyntaxError(2);
    }
    return address;
}

//�����﷨����һ���ڵ�
static struct ExprNode* MakeExprNode(enum Token_Type opcode, ...)
{
    struct ExprNode* ExprPtr = new(struct ExprNode);//����һ���µĽ��
    ExprPtr->OpCode = opcode;       //���ռǺŵ����
    va_list ArgPtr;    //����һ��ת�������ı���
    va_start(ArgPtr, opcode);   //��ʼ��������ArgPtrָ�����Ĳ���opcode
    switch (opcode)     //���ݼǺŵ�����첻ͬ�Ľڵ�
    {
    case CONST_ID:     //����
        ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);  //va_arg������ֵ
        break;
    case T:   //����
        ExprPtr->Content.CaseParmPtr = &Parameter;  //va_arg���ز�����ַ
        break;
    case FUNC:  //��������
        ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);  //���ض�Ӧ��������ָ�� MathFuncPtr
        ExprPtr->Content.CaseFunc.Child = (struct ExprNode*)va_arg(ArgPtr, struct ExprNode*);  //���ӵ��ڲ��ڵ�
        break;
    default:  //��Ԫ����
        ExprPtr->Content.CaseOperater.Left = (struct ExprNode*)va_arg(ArgPtr, struct ExprNode*);
        ExprPtr->Content.CaseOperater.Right = (struct ExprNode*)va_arg(ArgPtr, struct ExprNode*);
        break;
    }
    va_end(ArgPtr);  //����������ArgPtr,ArgPtr=NULL
    return ExprPtr;
}
