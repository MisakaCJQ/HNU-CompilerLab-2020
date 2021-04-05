//
// Created by cai on 2020/11/12.
//

#include "FA.h"

FA::FA()
{
    FAType = 0;
    chNum = 0;
    stateNum = 0;
    startStateID = 0;
}

void FA::read(ifstream &fin)
{
    fin>>FAType;//读入自动机类型
    fin>>chNum;//读入字符集个数
    for(int i=0;i<chNum;i++)//读入字符集
    {
        char c;
        fin>>c;
        alphabet.push_back(c);
    }
    fin>>stateNum;//读入状态数
    for(int i=0;i<stateNum;i++)
    {
        int ID,numEdge,stateType;//输入该状态的ID,出边数,类型
        fin>>ID>>numEdge>>stateType;
        State tempState(ID,stateType,numEdge);
        if(stateType==0)//为开始状态
            startStateID=ID;
        else if(stateType==2)//为接受状态
            finishStateID.push_back(ID);
        else if(stateType==3)//同时是开始状态和接收状态
        {
            startStateID=ID;
            finishStateID.push_back(ID);
        }
        for(int j=0;j<numEdge;j++)//接下来的numEdge行输入每一条出边的输入字符和目标状态ID
        {
            char c;
            int targetID;
            fin>>c>>targetID;
            Edge tempEdge(c,targetID);
            tempState.trans.push_back(tempEdge);
        }
        stateSet[ID]=tempState;
    }
}

void FA::print()
{
    cout<<"-----------------------------------"<<endl;
    cout<<"FAType:";
    if(FAType==0) cout<<"NFA"<<endl;
    else if(FAType==1) cout<<"DFA"<<endl;
    else if(FAType==2) cout<<"MinDFA"<<endl;
    else cout<<"unknown";

    cout<<"chNum:"<<chNum<<endl;
    cout<<"alphebet:";
    for(int i=0;i<chNum;i++)
    {
        cout<<alphabet[i]<<' ';
    }
    cout<<endl;
    cout<<"stateNum:"<<stateNum<<endl<<endl;
    for(int i=0;i<stateNum;i++)
    {
        cout<<"StateID:"<<stateSet[i].ID<<endl;
        cout<<"StateType:";
        if(stateSet[i].stateType==0)
            cout<<"Start"<<endl;
        else if(stateSet[i].stateType==1)
            cout<<"Normal"<<endl;
        else if(stateSet[i].stateType==2)
            cout<<"Final"<<endl;
        else if(stateSet[i].stateType==3)
            cout<<"Start&Final"<<endl;
        else
            cout<<"Unknown"<<endl;
        cout<<"EdgeNum:"<<stateSet[i].numEdge<<endl;
        cout<<"Input\tNextState"<<endl;
        for(int j=0;j<stateSet[i].numEdge;j++)
        {
            cout<<stateSet[i].trans[j].c<<"\t"<<stateSet[i].trans[j].targetID<<endl;
        }
        cout<<endl;
    }
    cout<<"-----------------------------------"<<endl;
}

//得到T的ε-closure
set<int> FA::getClosure(const set<int> &T)
{
    stack<int> stk;
    for(int it : T)
        stk.push(it);

    set<int> ansT(T);//使用T来初始化
    while(!stk.empty())
    {
        int temp =stk.top();
        stk.pop();
        int len=stateSet[temp].trans.size();
        for(int i=0;i<len;i++)
        {
            if(stateSet[temp].trans[i].c=='-')//找到一个可通过输入ε转换得到的状态
            {
                int u=stateSet[temp].trans[i].targetID;
                if(ansT.find(u)==ansT.end())//如果这个状态不在结果集合中
                {
                    ansT.insert(u);
                    stk.push(u);
                }
            }

        }
    }
    return ansT;
}

//判断Dstates中是否还有未标记的状态,有就返回其下标，否则返回-1
int FA::unflagInDstates(const vector<dState> &Dstates) const
{
    int len=Dstates.size();
    for(int i=0;i<len;i++)
    {
        if(!Dstates[i].flag)
            return i;
    }
    return -1;
}

//判断某个子集是否已经存在,存在则返回其下标，否则返回-1
int FA::isInDstates(const vector<dState> &dStates,const set<int> &U) const
{
    int len=dStates.size();
    for(int i=0;i<len;i++)
    {
        if(dStates[i].NFAset==U)
            return i;
    }
    return -1;
}

//状态集T输入字符c后得到的目标状态集合
set<int> FA::getMove(const set<int> &T,char c)
{
    set<int> ansT;
    for(int sid : T)
    {
        for(int i=0;i<stateSet[sid].numEdge;i++)
        {
            if(stateSet[sid].trans[i].c==c)
                ansT.insert(stateSet[sid].trans[i].targetID);
        }
    }
    return ansT;
}

//获得子集的状态种类
int FA::getStatesType(const set<int> &T) const
{
    bool flagSta= false;
    bool flagFin= false;
    if(T.find(startStateID)!=T.end())
        flagSta=true;

    int len=finishStateID.size();
    for(int i=0;i<len;i++)
    {
        if(T.find(finishStateID[i])!=T.end())
        {
            flagFin=true;
            break;
        }
    }

    if(flagSta && !flagFin)//start
        return 0;
    else if(!flagSta && flagFin)//finish
        return 2;
    else if(flagSta && flagFin)//start&finish
        return 3;
    else//normal;
        return 1;
}

//检查集合中是否有接受状态
bool FA::isFinishStateIDinSet(const set<int> &T) const
{
    int len=finishStateID.size();
    for(int i=0;i<len;i++)
    {
        if(T.find(finishStateID[i])!=T.end())
            return true;
    }
    return false;
}

//返回DFA某个状态输入一个字符后的下一个状态
int FA::DFAnextState(int stateID,char c)
{
    for(int i=0;i<chNum;i++)
    {
        if(stateSet[stateID].trans[i].c==c)
            return stateSet[stateID].trans[i].targetID;
    }
    return -1;//WRONG
}

//返回某状态在分划中的组号
int FA::stateInGroupNum(const set<set<int> > &Division,int stateID)
{
    int len=Division.size();
    int i=0;
    for(auto it=Division.begin();it!=Division.end();it++,i++)
    {
        if(it->find(stateID)!=it->end())
            return i;
    }
    return -1;
}

//NFA确定化
void FA::NFAtoDFA()
{
    if (FAType!=0)//不是NFA的自动机
        return;
    map<int,State> newStateSet;
    vector<dState> dStateSet;
    set<int> s0;
    s0.insert(startStateID);//构造s0

    set<int> T=getClosure(s0);//获得初始状态的ε-closureQ
    dState dSt(0,T,false);
    dStateSet.push_back(dSt);//将这个closure放入dState集合

    int stateType=getStatesType(T);
    State St(0,stateType,0);
    newStateSet[0]=St;//将新DFA初始状态放入新StateSet集合

    int temp;
    for(;;)
    {
        //寻找一个未标记的集合T
        temp=unflagInDstates(dStateSet);
        if (temp==-1) break;
        dStateSet[temp].flag= true;//给T加上标记
        for(int i=0;i<chNum;i++)
        {
            set<int> para0=getMove(dStateSet[temp].NFAset,alphabet[i]);
            set<int> U=getClosure(para0);
            int temp1=isInDstates(dStateSet,U);//判断这一子集是否已存在于Dstates中
            if(temp1==-1)
            {//不存在则加入到Dstates中
                int index=dStateSet.size();
                //if(index==4) cout<<4<<endl;
                dState dSt0(index,U,false);
                dStateSet.push_back(dSt0);
                //同时加入到新StateSet中

                int stateType0=getStatesType(U);


                State St0(index,stateType0,0);
                newStateSet[index]=St0;
                temp1=index;
            }
            newStateSet[temp].numEdge++;
            newStateSet[temp].trans.emplace_back(alphabet[i],temp1);//右值引用
        }
    }
    //更新当前对象中的数据
    FAType=1;
    stateSet=newStateSet;
    stateNum=newStateSet.size();
    finishStateID.clear();
    int len=stateSet.size();
    for(int i=0;i<len;i++)
    {
        int staType=newStateSet[i].stateType;
        if(staType==2 || staType==3)
            finishStateID.push_back(i);
    }
}

//DFA最小化
void FA::DFAminimize()
{
    if(FAType!=1)
        return;
    set<int> group1,group2;
    for(int i=0;i<stateNum;i++)
    {
        if (stateSet[i].stateType==2||stateSet[i].stateType==3)//终态
            group2.insert(stateSet[i].ID);
        else//非终态
            group1.insert(stateSet[i].ID);
    }

    //构造初始分划，并将终态和非终态两个集合放入
    set<set<int> > Division;
    //set<set<int> > DivisionFinal;
    Division.insert(group1);
    Division.insert(group2);

    for(;;)//使用队列BFS进行子集划分
    {
        set<set<int> > DivisionNew=Division;
        for(set<int> group:Division)
        {
            queue<qnode> q;
            q.push(qnode(0,group));
            for(;;)
            {
                qnode temp =q.front();
                if(temp.depth>=chNum) break;//到达根结点则终止BFS
                q.pop();
                map<int,set<int> > stateToSet;//输入当前字符后到达同一组的所有状态set
                for(int tempState:temp.DFAset)
                {
                    int nextState=DFAnextState(tempState,alphabet[temp.depth]);//找到其下一状态
                    int nextGroupNum=stateInGroupNum(Division,nextState);//找到下一状态所在的组
                    if(stateToSet.find(nextGroupNum)!=stateToSet.end())//到达此状态的集合已存在
                        stateToSet[nextGroupNum].insert(tempState);
                    else//此集合不存在，则新建一个
                    {
                        set<int> newSet;
                        newSet.insert(tempState);
                        stateToSet[nextGroupNum]=newSet;
                    }
                }

                for(auto & it : stateToSet)//遍历map，将新的小集合加入队列
                    q.push(qnode(temp.depth+1,it.second));
            }
            DivisionNew.erase(group);//删除原有的组，加入所有新的小组
            while(!q.empty())
            {
                qnode temp=q.front();
                q.pop();
                DivisionNew.insert(temp.DFAset);
            }
        }
        //if(DivisionNew.size()==6)
            //cout<<"6"<<endl;
        if (DivisionNew==Division)
            break;
        else
            Division=DivisionNew;
    }
    //定义三个新值
    map<int,State> newStateSet;
    vector<int> newFinishStateID;
    int newStateStartID=0;
    //将划分读入到集合数组中
    vector<set<int> > minDFAset;
    for(const set<int>& temp:Division)
        minDFAset.push_back(temp);
    //进行新数据构造工作
    int len=minDFAset.size();
    for(int i=0;i<len;i++)
    {
        //检测集合所属的状态类型
        int stateType=1;
        if(minDFAset[i].find(startStateID)!=minDFAset[i].end())
        {
            if(isFinishStateIDinSet(minDFAset[i]))//不仅有开始状态，同时还有结束状态
                stateType=3;
            else
                stateType=0;
            newStateStartID=i;
        }
        else if(isFinishStateIDinSet(minDFAset[i]))
        {
            stateType=2;
            newFinishStateID.push_back(i);
        }
        else
            stateType=1;

        State St0(i,stateType,chNum);

        int temp=*(minDFAset[i].begin());//选取其中第一个状态temp作为代表
        for(int j=0;j<chNum;j++)
        {//遍历代表状态的出边，找到其新的目标状态
            char c=stateSet[temp].trans[j].c;
            int nextState=stateSet[temp].trans[j].targetID;
            for(int k=0;k<len;k++)
                if(minDFAset[k].find(nextState)!=minDFAset[k].end())
                {
                    nextState=k;
                    break;
                }
            St0.trans.emplace_back(c,nextState);
        }
        newStateSet[i]=St0;
    }
    //更新对象数据
    FAType=2;
    stateSet=newStateSet;
    stateNum=newStateSet.size();
    startStateID=newStateStartID;
    finishStateID=newFinishStateID;
}