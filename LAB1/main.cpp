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
            cout<<'\t'<<lineNum+1<<": reserved word:"<<tokenString<<endl;
            break;
        case PLUS:
            cout<<'\t'<<lineNum+1<<": +"<<endl;
            break;
        case MINUS:
            cout<<'\t'<<lineNum+1<<": -"<<endl;
            break;
        case TIMES:
            cout<<'\t'<<lineNum+1<<": *"<<endl;
            break;
        case OVER:
            cout<<'\t'<<lineNum+1<<": /"<<endl;
            break;
        case EQ:
            cout<<'\t'<<lineNum+1<<": ="<<endl;
            break;
        case LT:
            cout<<'\t'<<lineNum+1<<": <"<<endl;
            break;
        case LPAREN:
            cout<<'\t'<<lineNum+1<<": ("<<endl;
            break;
        case RPAREN:
            cout<<'\t'<<lineNum+1<<": )"<<endl;
            break;
        case SEMI:
            cout<<'\t'<<lineNum+1<<": ;"<<endl;
            break;
        case ASSIGN:
            cout<<'\t'<<lineNum+1<<": :="<<endl;
            break;
        case ENDFILE:
            cout<<'\t'<<lineNum+1<<": EOF"<<endl;
            break;
        case NUM:
            cout<<'\t'<<lineNum+1<<": NUM,val="<<tokenString<<endl;
            break;
        case ID:
            cout<<'\t'<<lineNum+1<<": ID,name="<<tokenString<<endl;
            break;
        case ERROR:
            cout<<'\t'<<lineNum+1<<": ERROR:"<<tokenString<<endl;
            break;
        default:
            cout<<'\t'<<lineNum+1<<": Unknown token:"<<tokenString<<endl;
    }
}

void getToken(string temp)//词法分析
{
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
            printToken(currentToken,tokenString);
            state=START;
            tokenString.clear();
    }
}

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
