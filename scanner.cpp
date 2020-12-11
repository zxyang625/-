#include"scanner.h"
#define TOKEN_LEN 100//�ַ�����������С��Ϊ100�����������Ǻŵ��ַ���

unsigned int LineNo;//��¼�ַ������е��к�
static FILE* InFile;//ָ���Ż�ͼ���Գ����ָ��
static char TokenBuffer[TOKEN_LEN];//����һ���ַ������������������Ǻŵ��ַ���������Ҫ�Ǻŵ��ַ���ʱ��char* lexemeָ���ָ��TokenBuffer

char* GetEnum(int i)//�ж�Enum��ֵ�����ض�Ӧ����
{
	char name[10];
	switch (i)
	{
	case 0:strcpy(name, "ORIGIN"); break;
	case 1:strcpy(name, "SCALE"); break;
	case 2:strcpy(name, "ROT"); break;
	case 3:strcpy(name, "IS"); break;
	case 4:strcpy(name, "TO"); break;
	case 5:strcpy(name, "STEP"); break;
	case 6:strcpy(name, "DRAW"); break;
	case 7:strcpy(name, "FOR"); break;
	case 8:strcpy(name, "FROM"); break;
	case 9:strcpy(name, "T"); break;
	case 10:strcpy(name, "SEMICO"); break;
	case 11:strcpy(name, "L_BRACKET"); break;
	case 12:strcpy(name, "R_BRACKET"); break;
	case 13:strcpy(name, "COMMA"); break;
	case 14:strcpy(name, "PLUS"); break;
	case 15:strcpy(name, "MINUS"); break;
	case 16:strcpy(name, "MUL"); break;
	case 17:strcpy(name, "DIV"); break;
	case 18:strcpy(name, "POWER"); break;
	case 19:strcpy(name, "FUNC"); break;
	case 20:strcpy(name, "CONST_ID"); break;
	case 21:strcpy(name, "NONTOKEN"); break;
	case 22:strcpy(name, "ERRTOKEN"); break;
	case 23:strcpy(name, "COLOR"); break;
	case 24:strcpy(name, "BLUE"); break;
	case 25:strcpy(name, "GREEN"); break;
	case 26:strcpy(name, "RED"); break;
	case 27:strcpy(name, "PIXELSIZE"); break;
	}
	return name;
}


//��ʼ���ʷ�������
extern int InitScanner(const char* FileName) //����Ҫ������Դ�����ļ���
{
	LineNo = 1;
	InFile = fopen(FileName, "r");
	if (InFile != NULL)
		return 1;     //������ڣ����ļ�������ʼ��lineNO��ֵΪ1������true
	else
		return 0;    //�����ڷ���0
}

//�رմʷ�������
extern void CloseScanner(void)
{
	if (InFile != NULL)
		fclose(InFile);
}

//������Դ�����ж���һ���ַ�
static char GetChar(void)
{
	int Char = getc(InFile);
	return toupper(Char);//���Դ�����һ���ַ�
}

//��Ԥ�����ַ��˻ص�����Դ�����У������Ĺ�������ҪԤ��1��2�������ַ���Ԥ�����ַ�������ˣ��Դ˱�֤�´ζ�ʱ���ᶪ���ַ�
static void BackChar(char Char)   //���룺����һ���ַ���û�����
{
	if (Char != EOF)
		ungetc(Char, InFile); 
}

//���Ѿ�ʶ����ַ��ӵ�TokenBuffer
static void AddCharTokenString(char Char)//����Դ�����һ���ַ���û�����
{
	int TokenLength = strlen(TokenBuffer);//�趨�ó���
	if (TokenLength + 1 >= sizeof(TokenBuffer))
		return;//��ʱ�ַ����ĳ��ȳ������ֵ�����ش���
	TokenBuffer[TokenLength] = Char;//���һ���ַ�
	TokenBuffer[TokenLength + 1] = '\0';
}

//��ռǺŻ�����
static void EmptyTokenString()
{
	memset(TokenBuffer, 0, TOKEN_LEN);
}

//����ʶ����ַ����ڷ��ű��в�����Ӧ�ļǺ�
static Token JudgeKeyToken(const char* IDString)//���룺ʶ������ַ�����������Ǻ�
{
	int loop;
	for (loop = 0; loop < sizeof(TokenTab) / sizeof(TokenTab[0]); loop++)
		if (strcmp(TokenTab[loop].lexeme, IDString) == 0)
			return TokenTab[loop];//���ҳɹ������ظüǺ�
	Token errortoken;
	memset(&errortoken, 0, sizeof(Token));  //��ʼ��errortoken
	errortoken.type = ERRTOKEN;
	return errortoken;//����ʧ�ܣ����ش���Ǻ�
}

//��ȡһ���Ǻ�
extern Token GetToken(void)  
//�˺�����DFAת���������˺������һ���Ǻš�ÿ���øú���һ�Σ��������һ���Ǻš�
//��ˣ�Ҫ���Դ��������мǺţ���Ҫ�ظ�����������������������ĺ��������˺������ù���
//���һ���Ǻţ�û������
{
	Token token;
	int Char;
	memset(&token, 0, sizeof(Token));
	EmptyTokenString();//��ջ�����
	token.lexeme = TokenBuffer;//�Ǻŵ��ַ�ָ��ָ���ַ�������
	for (;;)
	{
		Char = GetChar();//��Դ�����ж���һ���ַ�
		if (Char == EOF)
		{
			token.type = NONTOKEN;
			return token;
		}
		if (Char == '\n') // \n��ת����һ��
			LineNo++;
		if (!isspace(Char))//�����ո����ôʽ���
			break;
	}//end of for
	AddCharTokenString(Char);
	//�����ǿո�TAB���س����ļ��������ȣ����ȼ��뵽�Ǻŵ��ַ���������
	if (isalpha(Char))  //��char��A-Za-z����һ���Ǻ������ؼ��֡�PI��E��
	{
		for (;;)
		{
			Char = GetChar();
			if (isalnum(Char))//�ж������ֻ�����ĸ
				AddCharTokenString(Char);//�Ǿͼ��뵽������
			else
				break;//���ǵĻ��ʹ����Ǵ�����ַ�����ͣ
		}
		BackChar(Char);//����'\0'
		token = JudgeKeyToken(TokenBuffer);//����tokenbuffer�еĺϷ����ַ�token
		token.lexeme = TokenBuffer;//���������ݸ���lexme����
		return token;
	}
	else if (isdigit(Char))  //����һ�����֣���һ���ǳ���
	{
		for (;;)
		{
			Char = GetChar();
			if (isdigit(Char))
				AddCharTokenString(Char);//��λ����λ���ϵ���
			else
				break;//���Ϸ�����ͣ
		}
		if (Char == '.')//С���Ĵ���
		{
			AddCharTokenString(Char);
			for (;;)
			{
				Char = GetChar();//С���������ִ���
				if (isdigit(Char))
					AddCharTokenString(Char);
				else
					break;
			}
		}
		BackChar(Char);
		token.type = CONST_ID;//����token�͸���CONST_ID
		token.value = atof(TokenBuffer);//�ַ���ת������
		return token;
	}
	else    //������ĸ�����֣���һ������������߷ָ���
	{
		switch (Char)
		{
		case ';':token.type = SEMICO; break;
		case '(':token.type = L_BRACKET; break;
		case ')':token.type = R_BRACKET; break;
		case ',':token.type = COMMA; break;
		case '+':token.type = PLUS; break;
		case '-':
			Char = GetChar();
			if (Char == '-')//��һ���ж��Ƿ���ע��
			{
				while (Char != '\n' && Char != EOF)//��ע�ͣ���һֱ��������ע��λ��
					Char = GetChar();
				BackChar(Char);
				return GetToken();//ע�ͽ��������¿�ʼGetToken
			}
			else//���ǵĻ������ǵ����ļ���
			{
				BackChar(Char);
				token.type = MINUS;
				break;
			}
		case '/':
			Char = GetChar();//��ոյ�-��һ��Ҫ�ж��Ƿ���ע��
			if (Char == '/')
			{
				while (Char != '\n' && Char != EOF)
					Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			else//���ǵĻ������ǵ����ǳ���
			{
				BackChar(Char);
				token.type = DIV;
				break;
			}
		case '*':
			Char = GetChar();
			if (Char == '*')//��һ���ж��Ƿ��ǳ˷�
			{
				token.type = POWER;
				break;
			}
			else//���ǵĻ��͵������ǳ˺�
			{
				BackChar(Char);
				token.type = MUL;
				break;
			}
		default:token.type = ERRTOKEN; break;//��������������㣬����ERRTOKEN
		} 
	}  
	return token;
}