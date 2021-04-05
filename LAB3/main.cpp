#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>
using namespace std;

typedef enum//词法单元枚举
{
    ENDFILE,ERROR,
    //保留字
    IF,THEN,ELSE,END,REPEAT,UNTIL,READ,WRITE,
    //多字符词法单元
    ID,NUM,
    //特殊符号
    PLUS,MINUS,TIMES,OVER,EQ,LT,LPAREN,RPAREN,SEMI,ASSIGN
}tokenType;

typedef enum//DFA状态
{
    START,INASSIGN,INCOMMENT,INNUM,INID,DONE
}stateType;

struct reserved
{
    string str;
    tokenType token;
};

reserved reservedString[8]={{"if",IF},
                            {"then",THEN},
                            {"else",ELSE},
                            {"end",END},
                            {"repeat",REPEAT},
                            {"until",UNTIL},
                            {"read",READ},
                            {"write",WRITE}};

int lineNum=0;
vector<string> readString;
tokenType reservedCheck(const string &tokenString)//保留字检查
{
    for(int i=0;i<8;i++)
    {
        if (tokenString==reservedString[i].str)
            return reservedString[i].token;
    }
    return ID;
}


void printToken(tokenType token,const string &tokenString)//词法单元输出
{
    switch (token)
    {
        case IF:
        case THEN:
        case ELSE:
        case END:
        case REPEAT:
        case UNTIL:
        case READ:
        case WRITE:
            cout<</*'\t'<<lineNum+1<<*/": reserved word:"<<tokenString<<endl;
            break;
        case PLUS:
            cout<</*'\t'<<lineNum+1<<*/": +"<<endl;
            break;
        case MINUS:
            cout<</*'\t'<<lineNum+1<<*/": -"<<endl;
            break;
        case TIMES:
            cout<</*'\t'<<lineNum+1<<*/": *"<<endl;
            break;
        case OVER:
            cout<</*'\t'<<lineNum+1<<*/": /"<<endl;
            break;
        case EQ:
            cout<</*'\t'<<lineNum+1<<*/": ="<<endl;
            break;
        case LT:
            cout<</*'\t'<<lineNum+1<<*/": <"<<endl;
            break;
        case LPAREN:
            cout<</*'\t'<<lineNum+1<<*/": ("<<endl;
            break;
        case RPAREN:
            cout<</*'\t'<<lineNum+1<<*/": )"<<endl;
            break;
        case SEMI:
            cout<</*'\t'<<lineNum+1<<*/": ;"<<endl;
            break;
        case ASSIGN:
            cout<</*'\t'<<lineNum+1<<*/": :="<<endl;
            break;
        case ENDFILE:
            cout<</*'\t'<<lineNum+1<<*/": EOF"<<endl;
            break;
        case NUM:
            cout<</*'\t'<<lineNum+1<<*/": NUM,val="<<tokenString<<endl;
            break;
        case ID:
            cout<</*'\t'<<lineNum+1<<*/": ID,name="<<tokenString<<endl;
            break;
        case ERROR:
            cout<</*'\t'<<lineNum+1<<*/": ERROR:"<<tokenString<<endl;
            break;
        default:
            cout<</*'\t'<<lineNum+1<<*/": Unknown token:"<<tokenString<<endl;
    }
}

struct Token//词法单元
{
    tokenType tokenT;//词法单元类型
    string tokenString;//词素
    Token(tokenType aToken,string aTokenString)
    {
        tokenT=aToken;
        tokenString=aTokenString;
    }
    Token()
    {
        tokenT=ERROR;
        tokenString="unknown";
    }
};

vector<Token> getToken(string temp)//词法分析
{
    vector<Token> tokenList;
    int tokenStringIndex = 0;//词素字符串的下标
    stateType state = START;//DFA状态
    tokenType currentToken;//保存当前词法单元类型
    string tokenString;//用于保存词法单元字符串
    bool flagSave = true;//用于标志是否要将当前字符加入到保存的词素字符串中

    int tempIndex = 0;
    int maxLenth = temp.length();
    while (tempIndex <=maxLenth)
    {
        while(state != DONE)
        {
            char c;
            if(tempIndex==maxLenth)//已到达EOF
                c=EOF;
            else
                c=temp[tempIndex];
            tempIndex++;
            flagSave = true;
            switch (state)//DFA状态选择与跳转
            {
                case START:
                    if(isdigit(c))
                        state = INNUM;
                    else if(isalpha(c))
                        state = INID;
                    else if(c==':')
                        state = INASSIGN;
                    else if ((c == ' ') || (c == '\t') || (c == '\n'))
                    {
                        flagSave= false;
                        if(c == '\n' && tempIndex!=maxLenth)
                        {
                            lineNum++;
                            cout<<lineNum+1<<":"<<readString[lineNum]<<endl;
                        }

                    }
                    else if(c=='{')
                    {
                        flagSave=false;
                        state = INCOMMENT;
                    }
                    else
                    {
                        state = DONE;
                        switch (c)
                        {
                            case EOF:
                                flagSave = false;
                                currentToken = ENDFILE;
                                break;
                            case '+':
                                currentToken = PLUS;
                                break;
                            case '-':
                                currentToken = MINUS;
                                break;
                            case '*':
                                currentToken = TIMES;
                                break;
                            case '/':
                                currentToken = OVER;
                                break;
                            case '=':
                                currentToken = EQ;
                                break;
                            case '<':
                                currentToken = LT;
                                break;
                            case '(':
                                currentToken = LPAREN;
                                break;
                            case ')':
                                currentToken = RPAREN;
                                break;
                            case ';':
                                currentToken = SEMI;
                                break;
                            default:
                                currentToken = ERROR;
                                break;
                        }
                    }
                    break;
                case INCOMMENT:
                    flagSave = false;
                    if(c==EOF)
                    {
                        state = DONE;
                        currentToken = ENDFILE;
                    }
                    else if(c=='}')
                        state = START;
                    else if(c == '\n' && tempIndex!=maxLenth)
                    {
                        lineNum++;
                        cout<<lineNum+1<<":"<<readString[lineNum]<<endl;
                    }
                    break;
                case INASSIGN:
                    state = DONE;
                    if(c=='=')
                        currentToken = ASSIGN;
                    else
                    {
                        tempIndex--;//读取下标回退1位
                        flagSave = false;
                        currentToken = ERROR;
                    }
                    break;
                case INNUM:
                    if(!isdigit(c))
                    {
                        tempIndex--;
                        flagSave = false;
                        state = DONE;
                        currentToken = NUM;
                    }
                    break;
                case INID:
                    if(!isalpha(c))
                    {
                        tempIndex--;
                        flagSave = false;
                        state = DONE;
                        currentToken = ID;
                    }
                    break;
                case DONE:
                default:
                    cout<<"WRONG STATE"<<endl;
                    break;
            }
            if(flagSave)//当前字符可以保存
                tokenString.push_back(c);
            if(state==DONE)
            {
                if (currentToken==ID)
                    currentToken=reservedCheck(tokenString);
            }
        }
        //printToken(currentToken,tokenString);
        tokenList.emplace_back(currentToken,tokenString);
        state=START;
        tokenString.clear();
    }
    return tokenList;
}

/*-----------------------------------语法分析部分--------------------------------------*/
vector<Token> tokenList;//用于保存所有词法单元的vector数组
int tokenIndex=0;//当前遍历到的词法单元下标
Token token;//当前词法单元
bool wrongFlag;//错误标志

Token getNextToken()
{
    return tokenList[tokenIndex++];
}

typedef enum {StmtK,ExpK}NodeKind;//语句和表达式
typedef enum {IfK,RepeatK,AssignK,ReadK,WriteK} StmtKind;//语句类型
typedef enum {OpK,ConstK,IdK} ExpKind;//表达式类型
typedef enum {Void,Integer,Boolean} ExpType;//变量种类

struct TreeNode//语法树结点
{
    TreeNode *  child[4];//子结点
    TreeNode *  sibling;//兄弟结点
    //int lineno;
    NodeKind nodekind;//结点种类，语句或表达式
    union { StmtKind stmt; ExpKind exp;} kind;//语句种类或是表达式种类
    //union { tokenType op; int val; const string name; } attr;
    string tokenString;//属性
    ExpType type;
};

TreeNode * stmt_sequence();
TreeNode * statement();
TreeNode * if_stmt();
TreeNode * repeat_stmt();
TreeNode * assign_stmt();
TreeNode * read_stmt();
TreeNode * write_stmt();
TreeNode * exp();
TreeNode * simple_exp();
TreeNode * term();
TreeNode * factor();

//比对向前看词法单元和输入的词法单元是否相符
void match(tokenType expected)
{
    if(token.tokenT==expected)
        token=getNextToken();
    else
    {
        cout<<"Unexpected Token";
        printToken(token.tokenT,token.tokenString);
        wrongFlag=true;
    }
}

//建立新的语句结点
TreeNode * newStmtNode(StmtKind kind)
{
    //TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    TreeNode *t=new TreeNode;
    for(int i=0;i<4;i++)
        t->child[i]= nullptr;
    t->sibling= nullptr;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    return t;
}

//建立新的表达式结点
TreeNode * newExpNode(ExpKind kind)
{
    //TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    TreeNode *t=new TreeNode;
    for (int i=0;i<4;i++)
        t->child[i] = nullptr;
    t->sibling = nullptr;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->type = Void;
    return t;
}



//stmt-sequence -> statement {;stmt-sequence}
TreeNode * stmt_sequence()
{
    TreeNode *t=statement();
    TreeNode *p=t;
    while((token.tokenT!=ENDFILE)&&(token.tokenT!=END)&&
    (token.tokenT!=ELSE)&&(token.tokenT!=UNTIL))
    {
        TreeNode *q;
        match(SEMI);
        q=statement();
        if(q!= nullptr)
        {
            if(t== nullptr)
            {
                t=q;
                p=q;
            }
            else
            {
                p->sibling=q;
                p=q;
            }
        }
    }
    return t;
}

//statement -> if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt
TreeNode * statement()
{
    TreeNode *t= nullptr;
    switch (token.tokenT)
    {
        case IF :
            t=if_stmt();
            break;
        case REPEAT:
            t = repeat_stmt();
            break;
        case ID :
            t = assign_stmt();
            break;
        case READ :
            t = read_stmt();
            break;
        case WRITE :
            t = write_stmt();
            break;
        default:
            cout<<"Unexpected Token";
            printToken(token.tokenT,token.tokenString);
            token=getNextToken();
            wrongFlag=true;
            break;
    }
    return t;
}

//if_stmt -> if exp then stmt-sequence [else stmt-sequence] end
TreeNode * if_stmt()
{
    TreeNode *t=newStmtNode(IfK);
    match(IF);
    if(t!= nullptr)
        t->child[0]=exp();
    match(THEN);
    if(t!= nullptr)
        t->child[1]=stmt_sequence();
    if(token.tokenT==ELSE)
    {
        match(ELSE);
        if(t!= nullptr)
            t->child[2]=stmt_sequence();
    }
    match(END);
    return t;
}

//repeat-stmt -> repeat stmt-sequence until exp
TreeNode * repeat_stmt()
{
    TreeNode * t = newStmtNode(RepeatK);
    match(REPEAT);
    if (t!=nullptr)
        t->child[0] = stmt_sequence();
    match(UNTIL);
    if (t!=nullptr)
        t->child[1] = exp();
    return t;
}

//assign-stmt -> identifier := exp
TreeNode * assign_stmt()
{
    TreeNode * t = newStmtNode(AssignK);
    if ((t!=nullptr) && (token.tokenT==ID))
        t->tokenString=token.tokenString;
    match(ID);
    match(ASSIGN);
    if (t!=nullptr)
        t->child[0] = exp();
    return t;
}

//read-stmt -> read identifier
TreeNode * read_stmt()
{
    TreeNode * t = newStmtNode(ReadK);
    match(READ);
    if ((t!=nullptr) && (token.tokenT==ID))
        t->tokenString=token.tokenString;
    match(ID);
    return t;
}

//write-stmt -> write exp
TreeNode * write_stmt()
{
    TreeNode * t = newStmtNode(WriteK);
    match(WRITE);
    if (t!=nullptr)
        t->child[0] = exp();
    return t;
}

//exp -> simple-exp [comparison-op simple-exp]
TreeNode * exp()
{
    TreeNode * t = simple_exp();
    if ((token.tokenT==LT)||(token.tokenT==EQ))
    {
        TreeNode * p = newExpNode(OpK);
        if (p!=nullptr)
        {
            p->child[0] = t;
            p->tokenString=token.tokenString;
            t = p;
        }
        match(token.tokenT);
        if (t!=nullptr)
            t->child[1] = simple_exp();
    }
    return t;
}

//simple-exp -> term {addop term}
TreeNode * simple_exp()
{
    TreeNode * t = term();
    while ((token.tokenT==PLUS)||(token.tokenT==MINUS))
    {
        TreeNode * p = newExpNode(OpK);
        if (p!=nullptr)
        {
            p->child[0] = t;
            p->tokenString = token.tokenString;
            t = p;
            match(token.tokenT);
            t->child[1] = term();
        }
    }
    return t;
}

//term -> factor {mulop factor}
TreeNode * term()
{
    TreeNode * t = factor();
    while ((token.tokenT==TIMES)||(token.tokenT==OVER))
    { TreeNode * p = newExpNode(OpK);
        if (p!=nullptr) {
            p->child[0] = t;
            p->tokenString = token.tokenString;
            t = p;
            match(token.tokenT);
            p->child[1] = factor();
        }
    }
    return t;
}

//factor -> (exp) | number | identifier
TreeNode * factor()
{
    TreeNode * t = nullptr;
    switch (token.tokenT) {
        case NUM :
            t = newExpNode(ConstK);
            if ((t!=nullptr) && (token.tokenT==NUM))
                t->tokenString = token.tokenString;
            match(NUM);
            break;
        case ID :
            t = newExpNode(IdK);
            if ((t!=nullptr) && (token.tokenT==ID))
                t->tokenString = token.tokenString;
            match(ID);
            break;
        case LPAREN :
            match(LPAREN);
            t = exp();
            match(RPAREN);
            break;
        default:
            //printToken(token,tokenString);
            cout<<"Unexpected Token";
            printToken(token.tokenT,token.tokenString);
            token = getNextToken();
            wrongFlag=true;
            break;
    }
    return t;
}

//打印语法树
void printNode(TreeNode *t,int depth)
{
    for(int i=0;i<depth;i++)
        cout<<"  ";
    if(t->nodekind==StmtK)
    {
        if(t->kind.stmt==IfK)
            cout<<"if"<<endl;
        else if(t->kind.stmt==RepeatK)
            cout<<"Repeat"<<endl;
        else if(t->kind.stmt==AssignK)
            cout<<"Assign to:"<<t->tokenString<<endl;
        else if(t->kind.stmt==ReadK)
            cout<<"Read:"<<t->tokenString<<endl;
        else if(t->kind.stmt==WriteK)
            cout<<"Write"<<endl;
        else
            cout<<"UNKNOWN TOKEN"<<endl;
    }
    else if(t->nodekind==ExpK)
    {
        if(t->kind.exp==OpK)
            cout<<"Op:"<<t->tokenString<<endl;
        else if(t->kind.exp==ConstK)
            cout<<"Const:"<<t->tokenString<<endl;
        else if(t->kind.exp==IdK)
            cout<<"Id:"<<t->tokenString<<endl;
        else
            cout<<"UNKNOWN TOKEN"<<endl;
    }
    else
        cout<<"UNKNOWN TOKEN"<<endl;

    if(t->child[0]!= nullptr)
        printNode(t->child[0],depth+1);
    if(t->child[1]!= nullptr)
        printNode(t->child[1],depth+1);
    if(t->child[2]!= nullptr)
        printNode(t->child[2],depth+1);
    if(t->child[3]!= nullptr)
        printNode(t->child[3],depth+1);
    if(t->sibling!= nullptr)
        printNode(t->sibling,depth);

    delete t;//释放动态申请的结点
}

//删除语法树
void deleteTree(TreeNode *t)
{
    if(t->child[0]!= nullptr)
        deleteTree(t->child[0]);
    if(t->child[1]!= nullptr)
        deleteTree(t->child[1]);
    if(t->child[2]!= nullptr)
        deleteTree(t->child[2]);
    if(t->child[3]!= nullptr)
        deleteTree(t->child[3]);
    if(t->sibling!= nullptr)
        deleteTree(t->sibling);
    delete t;
}

//语法分析驱动函数·
void parse()
{
    TreeNode *t;
    token=getNextToken();
    wrongFlag=false;
    t=stmt_sequence();//开始执行第一个产生式
    if(token.tokenT!=ENDFILE||wrongFlag)
    {
        cout<<"Syntax error!"<<endl;
        deleteTree(t);
    }
    else
    {
        cout<<"Syntax tree:"<<endl;
        printNode(t,0);
    }
}


/*
int main()
{
    ifstream in("sample0.tny");
    stringstream buff;
    buff<<in.rdbuf();
    string k=buff.str();
    while(!buff.eof())
    {
        string str;
        getline(buff,str);
        readString.push_back(str);
    }
    cout<<"1:"<<readString[0]<<endl;
    getToken(k);
    return 0;
}
 */
int main()
{
    ifstream in("sample1.tny");
    stringstream buff;
    buff<<in.rdbuf();
    string k=buff.str();
    while(!buff.eof())
    {
        string str;
        getline(buff,str);
        readString.push_back(str);
    }
    cout<<"1:"<<readString[0]<<endl;
    tokenList=getToken(k);
    cout<<endl;
    parse();
    return 0;
}
