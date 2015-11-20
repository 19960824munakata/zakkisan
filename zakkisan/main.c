#include <stdio.h>
#include <stdlib.h>
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

Neuron*    create_node(float *e, int out, int num);
int     is_leaf(Neuron *n);
float   distance(float *e,float *weight);
Neuron*    test(float kyori,float *e,Neuron *winner);
void    update(Neuron *winner);
void    connect_node(Neuron *n,Neuron *winner);
Neuron* get_perhaps_nearest_node(Neuron*, float*);
void show(Neuron *);
void hyouji(Neuron *);
void free_tree(Neuron *);

int main(void){
    int i,j,count0=0,count1=0;
    float data[2] = {0.2,0.5};
    
    float e[24][4][2] = {
        {{0,0},{0,1},{1,0},{1,1}},
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
        {{1,1},{1,0},{0,1},{0,0}}
    };
    
    int out[4] = {0, 1, 1, 0};
    int num;                // ノード番号
    float dis;
    Neuron *root,*winner,*answer, *n;

    for (j=0;j<24;j++) {
        num = 0;
        root = create_node(e[0][0],out[0],num);
        
        for (i=1;i<4;i++) {
            //      e[num]とN[0]の重みの距離を代入
            dis = distance(e[j][i], root->weight);
            //      winnerにN[0](root)のポインタをもたせておく
            winner = root;
            //      winnerのもつ子ノードと比較しe[i]との距離が最も近いものをwinnerにする
            winner = test(dis,e[j][i], winner);
            //      winnerが葉ノードであったとき
            if(is_leaf(winner)) {
                num++;
                n = create_node(winner->weight, winner->result, num);
                connect_node(n, winner);
            }
            num++;
            n = create_node(e[j][i], out[i], num);
            connect_node(n, winner);
            update(winner);
        }
        
        show(root);
        
        answer = get_perhaps_nearest_node(root, data);
        
        printf("最も近いのは%dです。\n",answer->result);
        
        if (answer->result == 0) {
            count0++;
        }else if (answer->result == 1){
            count1++;
        }
        free_tree(root);
    }
    
    printf("[%.1f,%.1f]が1の確率は%.1f[％],0の確率は%.1f[％]です。\n",data[0],data[1],(float)(count0/24)*100,(float)(count1/24)*100);
    printf("end");
    return 0;
}

Neuron* create_node(float *e,int out, int num){
    Neuron* n = (Neuron *)calloc(1, sizeof(Neuron));
    n->num      = num;
    n->weight[0]= e[0];
    n->weight[1]=e[1];
    n->result = out;
    n->leafnum  = 1;
    n->child = n->brother = n->parent = NULL;
    return n;
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
    if (winner == NULL) {
        n->child = n->brother = n->parent = NULL;
        puts("error:connect_nodeの第2引数にNULLが入りました。");
    } else {
        n->parent = winner;
        
        if (winner->child == NULL) {
            winner->child = n;
        } else if (winner->child->brother == NULL) {
            winner->child->brother = n;
        } else if (winner->child->brother->brother == NULL) {
            winner->child->brother->brother = n;
        } else if (winner->child->brother->brother->brother == NULL) {
            winner->child->brother->brother->brother = n;
        }
        
        //    上のif文の塊はこんな感じのwhileを使えば書き直せそうなきがする(書いてあるのは未完成)
        /***
         Neuron *ptr;
         ptr = winner->child;
         while(1) {
         if (ptr == NULL) {
         ptr = n;
         break;
         }
         ptr = ptr->brother;
         }
         ***/
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

// dataとおそらく最も近いノードを返す
Neuron* get_perhaps_nearest_node(Neuron *n, float *data){
    Neuron *nearest = NULL, *ptr;
    float min_distance, dis;

    // 葉ノードまで進んだらそのまま返す
    if (n->child == NULL) {
        return n;
    } else {
        ptr = n->child;
        min_distance = distance(ptr->weight, data);
        nearest = ptr;
        while (ptr->brother != NULL) {
            dis = distance(ptr->brother->weight, data);
            if (min_distance > dis) {
                min_distance = dis;
                nearest = ptr;
            }
            ptr = ptr->brother;
        }
        
        nearest = get_perhaps_nearest_node(nearest, data);
        
        return nearest;
    }
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

// root以下のノードをすべて解放
void free_tree(Neuron *root){
    Neuron *ptr, temp;
    ptr=root->child;
    if(ptr!=NULL){
        while(ptr!=root){
            while(ptr->child!=NULL){
                ptr=ptr->child;      // 子供がいる間、子供のアドレスへ移動
            }
            temp = *ptr;             // ptrをtempに退避させる
            free(ptr);
            ptr = NULL;
            while(temp.brother==NULL){
                ptr=temp.parent;
                temp = *ptr;
                free(ptr);
                if(ptr == root){
                    break;
                }
                ptr = NULL;
            }
            if(temp.num != root->num){
                ptr = temp.brother;
            }
        }
    }
}
