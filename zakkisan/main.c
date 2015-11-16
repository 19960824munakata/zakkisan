#include <stdio.h>
#include <math.h>

// issue:rootノードが存在しない
// issue:子ノードを作るときの処理が間違っている

typedef struct neuron{
    int     num;                // ノード番号
    int     leafnum;            // 自身の子ノードの葉ノードの数
    int     result;             // 0 or 1
    float   weight[2];
    struct  neuron *child;
    struct  neuron *brother;
    struct  neuron *parent;
}Neuron;

void    copy(Neuron *n, float *e, int out, int num);
int     is_leaf(Neuron *n);
float   distance(float *e,float *weight);
Neuron*    test(float kyori,float *e,Neuron *winner);
void    update(Neuron *winner);
void    connect_node(Neuron *n,Neuron *winner);
Neuron* get_nearest_node(Neuron*, float*);
void show(Neuron *);
void hyouji(Neuron *);

int main(void){
    int i,j,count0=0,count1=0;
    float probability0,probability1;
    float data[2] = {0.5,0.5};
    
    float e[24][4][2] = {{{0,0},{0,1},{1,0},{1,1}},
        {{0,0},{0,1},{1,1},{1,0}},
        {{0,0},{1,0},{0,1},{1,1}},
        {{0,0},{1,0},{1,1},{0,1}},
        {{0,0},{1,1},{0,1},{1,0}},
        {{0,0},{1,1},{1,0},{0,1}},
        {{0,1},{0,0},{1,0},{1,1}},
        {{0,1},{0,0},{1,1},{1,0}},
        {{0,1},{1,0},{0,0},{1,1}},
        {{0,1},{1,0},{1,1},{0,0}},
        {{0,1},{1,1},{0,0},{1,0}},
        {{0,1},{1,1},{1,0},{0,0}},
        {{1,0},{0,0},{0,1},{1,1}},
        {{1,0},{0,0},{1,1},{0,1}},
        {{1,0},{0,1},{0,0},{1,1}},
        {{1,0},{0,1},{1,1},{0,0}},
        {{1,0},{1,1},{0,0},{0,1}},
        {{1,0},{1,1},{0,1},{0,0}},
        {{1,1},{0,0},{0,1},{1,0}},
        {{1,1},{0,0},{1,0},{0,1}},
        {{1,1},{0,1},{0,0},{1,0}},
        {{1,1},{0,1},{1,0},{0,0}},
        {{1,1},{1,0},{0,0},{0,1}},
        {{1,1},{1,0},{0,1},{0,0}}};
    
    int out[4] = {0, 1, 1, 0};
    int num = 0;                // ノード番号
    float dis;
    
    Neuron N[100],*winner,*answer;
    //    N[0]にノード番号num,葉ノードの数1,重みe[0]をコピー
    copy(&N[0],e[0][0],out[0],num);
    N[0].child   = NULL;
    N[0].brother = NULL;
    N[0].parent  = NULL;
    
    for (j=0;j<24;j++) {
        for (i=0;i<4;i++) {
            //      e[num]とN[0]の重みの距離を代入
            dis = distance(e[j][i], N[0].weight);
            //      winnerにN[0](root)のポインタをもたせておく
            winner = &N[0];
            //      winnerのもつ子ノードと比較しe[i]との距離が最も近いものをwinnerにする
            winner = test(dis,e[j][i], winner);
            //      winnerが葉ノードであったとき
            if(is_leaf(winner)) {
                num++;
                copy(&N[num], winner->weight, winner->result, num);
                connect_node(&N[num], winner);
            }
            num++;
            copy(&N[num], e[j][i], out[i], num);
            connect_node(&N[num], winner);
            update(winner);
        }
        
        show(&N[0]);
        
        answer = get_nearest_node(N, data);
        
        printf("最も近いのは%dです。\n",answer->result);
        
        if (answer->result == 0) {
            count0++;
        }else if (answer->result == 1){
            count1++;
        }
        
    }
    
    probability0 = (count0/24)*100;
    probability1 = (count1/24)*100;
    
    printf("[%.1f,%.1f]が1の確率は%.1f[％],0の確率は%.1f[％]です。\n",data[0],data[1],probability1,probability0);
    printf("end");
    return 0;
}

void copy(Neuron *n,float *e,int out, int num){
    n->num      = num;
    n->weight[0]= e[0];
    n->weight[1]=e[1];
    n->result = out;
    n->leafnum  = 1;
    n->child = n->brother = n->parent = NULL;
    return;
}

int is_leaf(Neuron *ptr){
    if(ptr->child == NULL) {
        return 1;
    } else {
        return 0;
    }
}

float distance(float *e,float *weight){
    float dis;
    double x,y;
    x=(double)e[0]-(double)weight[0];
    y=(double)e[1]-(double)weight[1];
    dis=(float)sqrt(x*x+y*y);
    return fabsf(dis);
}


// winnerを最も距離の短いノードに更新する
Neuron* test(float kyori, float *e, Neuron *winner){
    Neuron *ptr,*n;
    n = winner;
    float dis;
    ptr = winner->child;
    if(ptr != NULL){
        do{
            dis = distance(e, ptr->weight);
            if(dis < kyori) {
                kyori = dis;
                winner = ptr;
            }
            ptr = ptr->brother;
        }while(ptr != NULL);
        if(winner != n){
            winner = test(kyori, e, winner);
        }
    }
    return winner;
}

void update(Neuron *winner){
    Neuron *ptr;
    int leafnum;
    float omomi[2];
    
    do {
        leafnum = omomi[0]=omomi[1] = 0;       // 初期化
        
        // leafnum,weightを更新
        
        ptr = winner->child;
        do {
            leafnum += ptr->leafnum;
            omomi[0] += (ptr->weight[0] * ptr->leafnum);
            omomi[1] += (ptr->weight[1] * ptr->leafnum);
            ptr = ptr->brother;
        }while(ptr != NULL);
        
        winner->leafnum = leafnum;
        winner->weight[0] = omomi[0]/leafnum;
        winner->weight[1] = omomi[1]/leafnum;
        
        //      winnerの親ノードで同じ処理を行う
        
        winner = winner->parent;
    } while(winner != NULL);
    
    return;
}

void connect_node(Neuron *n,Neuron *winner){
    Neuron *ptr;
    n->parent = winner;
    
    if (winner->child == NULL) {
        winner->child = n;
    } else {
        ptr = winner->child;
        while(ptr->brother != NULL) {
            ptr = ptr->brother;
        }
        ptr->brother = n;
    }
    
    return;
}

void show(Neuron *n){
    Neuron *ptr;
    ptr=n->child;
    if(ptr!=NULL){
        while(ptr!=n){
            while(ptr->child!=NULL){
                ptr=ptr->child;      //子供がいる間、子供のアドレスへ移動
            }
            hyouji(ptr);
            while(ptr->brother==NULL){
                ptr=ptr->parent;
                hyouji(ptr);
                if(ptr == n){
                    break;
                }
            }
            if(ptr != n){
                ptr = ptr->brother;
            }
        }
    }
}

// detaと最も近い葉ノードを返す
Neuron* get_nearest_node(Neuron *n, float *data){
    Neuron *ptr, *nearest;
    float dis;
    float min_distance = 100;
    ptr = n->child;
    if(ptr != NULL){
        while(ptr != n){
            while(ptr->child != NULL){
                ptr = ptr->child;      //子供がいる間、子供のアドレスへ移動
            }
            
            //            ここの式の重複を直したい
            dis = distance(data, ptr->weight);
            if (min_distance > dis) {
                min_distance = dis;
                nearest = ptr;
            }
            
            while(ptr->brother == NULL){
                ptr=ptr->parent;
                if(ptr == n){
                    break;
                }
                
            }
            if(ptr!=n){
                ptr=ptr->brother;
            }
        }
    }
    return nearest;
}

void hyouji(Neuron *n){
    printf("num=%d,leafnum=%d,weight(%.2f,%.2f)\n",n->num,n->leafnum,n->weight[0],n->weight[1]);
    if(n->parent!=NULL){
        printf("parent=%d\n",n->parent->num);
    }
    if(n->brother!=NULL){
        printf("brother=%d\n",n->brother->num);
    }
    if(n->child!=NULL){
        printf("child=%d\n",n->child->num);
    }
}