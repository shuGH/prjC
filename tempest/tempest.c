/*--------------------------------------------*/
/* 3人用リバーシゲーム「テンペスト-１」       */
/*--------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

/* 変数定義 */
#define Debug 0                 /* デバグ用フラグ */
#define Edge  9                 /* 辺のマス数(2..26) */
#define Depth 5                 /* AIの先読みの深さ(1..) */
#define Mark0 "・"              /* 何もないマス */
#define Mark1 "●"              /* P1の石 */
#define Mark2 "▲"              /* P2の石 */
#define Mark3 "◆"              /* P3の石 */
#define Mark4 "＋"              /* 設置可能マス */

/* 関数宣言 */
void Print_Title(int*);                 /* タイトルの表示 */
int  Input_Pos(int,int,int);            /* 設置位置の入力 */
void Reset_Table(int);                  /* 盤面の初期化 */
void Print_Table(int,int);              /* 盤面の表示 */
void Put_Stone(int,int,int);            /* 石の設置と反転 */
int  Check_Table(int,int);              /* 盤面の反転判定 */
int  Check_Lines(int,int,int,int,int*); /* 全ラインの反転判定 */
int  Exchange_Pos(int,int);             /* 盤面の座標位置と配列添字の変換 */
void Print_Debug(int,int,int);          /* デバグ用 盤面の表示 */
void Sort_Num(void);                    /* 順位の計算 */
void Honor_Platform(void);              /* 結果の表示 */
int  Calculate_EvaluatedValue(int,int); /* 評価値の計算 */
int  AI_AlphaBeta(int,int,int,int,int); /* AIアルゴリズム */
void Count_Stone(int);                  /* 個数の計算 */
void Free_ListMemory (int);             /* スタックリストのメモリ開放 */

/* グローバル変数宣言 */
struct PlayerStatus {           /* プレーヤーとマスの状態 */
  int St;                       /* 状態 0 CPU, 1 Man */
  char Mark[10];                /* 石 */
  int Num;                      /* 枚数 */
  int Rank;                     /* 順位 */
} Player[5] = {{0,Mark0,0,0},{0,Mark1,0,0},{0,Mark2,0,0},{0,Mark3,0,0},{0,Mark4,0,0}};

struct StackList {              /* 反転位置保存スタックのリスト */
  int id;                       /* 石の設置位置 */
  int Stack[(Edge*2)-2];        /* 反転位置保存スタック(-1 終端記号) */
  struct StackList *next;       /* ポインタ */
} List[Depth+1];

int Table[Depth+1][(Edge*Edge+Edge)/2];  /* 盤の状態[(0 現状, 1..Depth AI用)][(-1 1P, -2 2P, -3 3P, 0.. 何もなし(0 石に隣接, 1.. 反転個数, nMass 石に非隣接))]  */

int nMass = (Edge*Edge+Edge)/2;                       /* マス目の数 */
int maxVal = ((Edge*Edge+Edge)/2)+(3*9)+((Edge-2)*2); /* 評価値の最大値 */


/*--------------------------------------------*/
/* メイン関数 */
/*--------------------------------------------*/
int main (void) {
  int flg=0;                    /* 反転可能フラグ(0 不可, 1 可) */
  int nT = 0;                   /* ターン数 */
  int Order[3];                 /* 順番保存配列 */
  int i;

  /* 各リストのポインタの初期化 */
  for (i=0; i<Depth+1; i++) { List[i].next=NULL; }

  Reset_Table(0);               /* 盤の初期化 */
  Print_Title(Order);           /* タイトルの表示 */
  printf("[ Game Start ]\n\n");

  /* メインループ */
  while(1) {
 
    /* 順位の計算 */
    Count_Stone(0);
    Sort_Num();                 

    /* 終了判定 */
    if (Player[0].Num==0) { break; } /* 全マスが埋まったとき */

    /* ターンと順位の表示 */
    printf("[ Turn %02d ] P%d %s ", nT+1,Order[nT%3],Player[Order[nT%3]].Mark);
    if (Player[Order[nT%3]].Rank==1) { printf("1st\n\n"); }
    if (Player[Order[nT%3]].Rank==2) { printf("2nd\n\n"); }
    if (Player[Order[nT%3]].Rank==3) { printf("3rd\n\n"); }

    /* 反転判定と盤面の表示 */
    flg = Check_Table(0,Order[nT%3]);
    Print_Table(0,flg);
    if (Debug) { Print_Debug(0,Order[nT%3],flg); }

    /* 各プレーヤーの個数の表示 */
    printf("\n%s:%02d %s:%02d ", Player[0].Mark, Player[0].Num,Player[Order[0]].Mark, Player[Order[0]].Num);
    printf("%s:%02d %s:%02d\n\n",  Player[Order[1]].Mark, Player[Order[1]].Num,Player[Order[2]].Mark, Player[Order[2]].Num);

    /* 石の設置 */
    if (Player[Order[nT%3]].St==0) { /* CPU */
      Put_Stone(0,Order[nT%3],AI_AlphaBeta(0,Order[nT%3],0,-maxVal,maxVal));
      printf("EnterKey : ");
      while(getchar() != '\n');
      printf("\n");
      nT++;        /* 次ターンへ */
    } else if (Player[Order[nT%3]].St==1) { /* Man */
      Put_Stone(0,Order[nT%3],Input_Pos(0,Order[nT%3],flg));
      getchar();
      printf("\n");
      nT++;        /* 次ターンへ */
    }

    /* 全リストのメモリ開放 */
    for (i=0; i<Depth+1; i++) { Free_ListMemory(i); }
  }

  /* 結果の表示 */
  printf("[ Game Over ]\n\n");
  Print_Table(0,0);
  printf("\n");
  Honor_Platform();

	return 0;
}


/*--------------------------------------------*/
/* 盤面の初期化 (辺のマス数に応じて初期配置変更) (in テーブル添字) */
/*--------------------------------------------*/
void Reset_Table (int iT) {
  int i,j;

  /* 初期化 */
  for (i=0; i<nMass; i++) { Table[iT][i] = nMass; }

  j=(int)Edge/3;
  /* 辺が3の倍数のとき 三角形配置 */
  if (Edge%3==0) {
    Table[iT][Exchange_Pos(j-1,j)] = -1;
    Table[iT][Exchange_Pos(j,j-1)] = -2;
    Table[iT][Exchange_Pos(j,j)]   = -3;
  }
  /* 辺が3の倍数+1のとき 六角形配置 */
  if (Edge%3==1) {
    Table[iT][Exchange_Pos(j-1,j)]   = -1;
    Table[iT][Exchange_Pos(j-1,j+1)] = -2;
    Table[iT][Exchange_Pos(j,j-1)]   = -3;
    Table[iT][Exchange_Pos(j,j+1)]   = -3;
    Table[iT][Exchange_Pos(j+1,j-1)] = -2;
    Table[iT][Exchange_Pos(j+1,j)]   = -1;
  }
  /* 辺が3の倍数+2のとき 逆三角形配置 */
  if (Edge%3==2) {
    Table[iT][Exchange_Pos(j,j)]   = -1;
    Table[iT][Exchange_Pos(j,j+1)] = -2;
    Table[iT][Exchange_Pos(j+1,j)] = -3;
  }
}


/*--------------------------------------------*/
/* タイトルメニューの表示とモード入力処理 (in 順番用配列) */
/*--------------------------------------------*/
void Print_Title (int o[]) {
  int i=0, nM=0;
  int rnd=0;                    /* 順番用の乱数 */

  srand( time(NULL) );
  
  /* タイトルメニューの表示 */
  printf(" ---[ Tempest-1 ]---\n\n");
  printf("      P1%s P2%s P3%s\n", Player[1].Mark,Player[2].Mark,Player[3].Mark);
	printf("  1 : Man  Man  Man\n");
	printf("  2 : Man  Man  CPU\n");
	printf("  3 : Man  CPU  CPU\n");
	printf("  4 : CPU  CPU  CPU\n");
	printf("  0 : Help\n");

  /* メニュー選択処理 */
  while(1) {
    printf("\nMode(0_4) : ");
    scanf("%d", &nM);
    getchar();
    printf("\n");

    /* プレーヤーの設定 */
    if ((nM>=1)&&(nM<=4)) {
      /* プレーヤーの操作 (Man or CPU) */
      for (i=1; i<=3; i++) { Player[i].St = (i+nM <= 4); }

      /* 順番の決定 */
      rnd = rand()%6;
      if (rnd==0) { o[0]=1; o[1]=2; o[2]=3; }
      if (rnd==1) { o[0]=1; o[1]=3; o[2]=2; }
      if (rnd==2) { o[0]=2; o[1]=1; o[2]=3; }
      if (rnd==3) { o[0]=2; o[1]=3; o[2]=1; }
      if (rnd==4) { o[0]=3; o[1]=1; o[2]=2; }
      if (rnd==5) { o[0]=3; o[1]=2; o[2]=1; }

      /* プレーヤの操作と順番の表示 */
      printf("[ Order ]\n\n");
      for (i=0; i<3; i++) {
        printf("  %d : P%d %s ", i+1, o[i], Player[o[i]].Mark);
        if (Player[o[i]].St==0) { printf("CPU"); }
        if (Player[o[i]].St==1) { printf("Man"); }
        printf("\n");
      }
      printf("\n");
      printf("EnterKey : ");
      while(getchar() != '\n');
      printf("\n");

      break;
    }
    /* ヘルプの表示 */
    else if (nM==0) {
      printf("This game is a kind of Reversi.\n");
      printf(" %s : P1's Stone.\n", Player[1].Mark);
      printf(" %s : P2's Stone.\n", Player[2].Mark);
      printf(" %s : P3's Stone.\n", Player[3].Mark);
      printf(" %s : No Stone.\n", Player[0].Mark);
      printf(" %s : No Stone (Can put the Stone).\n", Player[4].Mark);
      printf("If you can't reverse enemy's stone,\n");
      printf(" you can put your stone on side of all stones.\n");
    } else { printf("\nError\n"); }
  }
}


/*--------------------------------------------*/
/* 個数計算 (in テーブル添字)*/
/*--------------------------------------------*/
void Count_Stone (int iT) {
  int i;

  for (i=0; i<=3; i++) { Player[i].Num = 0; } /* 個数の初期化 */

  /* 盤の走査 */
  for (i=0; i<=nMass; i++) {      
    if (Table[iT][i]>=0) {      /* 何もなしマス */
      Table[iT][i]=nMass;
      Player[0].Num++;          /* 個数のインクリメント */
    } else {                    /* 石設置マス */
      Player[-Table[iT][i]].Num++; /* 個数のインクリメント */
    }
  }
}


/*--------------------------------------------*/
/* 盤面の表示 (in テーブル添字,反転フラグ)*/
/*--------------------------------------------*/
void Print_Table (int iT, int flg) {
  int i,row=0,col=0;

  /* 列の数字の表示 */
  if (Edge>=10) { printf("   "); } else { printf("  "); } /* 列の数字の幅調整 */
  for (i=0; i<Edge; i++) { printf("%c ", 'a'+i); }

  for (row=0; row<Edge; row++) {
    /* 行の数字の表示 */
    if (Edge>=10) { printf("\n%02d ", row+1); } else { printf("\n%d ", row+1); }
    for (i=0; i<row; i++) { printf(" "); } /* 調整用の空白の挿入 */

    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);

      /* マスの表示 */
      if (Table[iT][i]>=0) {    /* 何もないマスの処理 */
        if (Table[iT][i]==0) {  /* 隣接マス */
          if (flg==0) { printf("%s",Player[4].Mark); } /* 反転不可のとき */
          if (flg==1) { printf("%s",Player[0].Mark); } /* 反転可能のとき */
        } else if (Table[iT][i]==nMass) { /* 非隣接マス */
          printf("%s",Player[0].Mark);
        } else {                /* 反転可能マス */
          printf("%s",Player[4].Mark);
        }
      } else {                  /* 石設置マスの処理 */
        printf("%s",Player[-Table[iT][i]].Mark);
      }
      
    }
  }
  printf("\n");
}


/*--------------------------------------------*/
/* 配置位置の入力 (in テーブル添字,プレーヤー,反転フラグ out 入力位置) */
/*--------------------------------------------*/
int Input_Pos (int iT, int nP, int flg) {
  int row=0, col=0;
  int i;
  char tmp;

  while(1) {
    /* 位置の入力 */
    printf("Pos(1_%d a_%c) : ", Edge,'a'+Edge-1);
    scanf("%d %c", &row,&tmp);
    row--; col = tmp-'a';

    /* -1で強制終了 */
    if (row==-1) { exit(1); }

    i=Exchange_Pos(row,col);
    /* 反転可能のときは反転可能マスに 反転不可のときは隣接マスに */
    if ((i!=-1)&&(Table[iT][i]!=nMass)&&(Table[iT][i]>=flg)) { return i; }
    else { printf("Error : Wrong Position!\n"); }
    printf("\n");
  }
  return -1;
}


/*--------------------------------------------*/
/* 石の設置 (in テーブル添字,プレーヤー,設置位置) */
/*--------------------------------------------*/
void Put_Stone (int iT, int nP, int id) {
  int i,j;
  struct StackList *pL;         /* リストのポインタ */

  /* スタックリストを走査 */
  pL = List[iT].next;
  while(pL != NULL){
    /* スタックの位置が設置位置のとき */
    if (pL->id==id) {
      if (Debug) { printf("Put : (%d) : ",id); }
      /* スタックを走査 */
      for (j=0; j<(Edge*2)-2; j++) {
        if (pL->Stack[j]==-1) { break; } /* 最後まで見たら終了 */
        if (Debug) { printf("%d ",pL->Stack[j]); }
        Table[iT][pL->Stack[j]]=-nP; /* 設置反転処理 */
      }
      if (Debug) { printf("\n"); }
      break;
    }

    pL = pL->next;              /* 次へ */
  }  
}


/*--------------------------------------------*/
/* 盤の反転判定  (in テーブル添字,プレーヤー, out 反転フラグ) */
/*--------------------------------------------*/
int Check_Table (int iT, int nP) {
  int i,j,row=0,col=0;          /* 行,列 */
  int flg=0;                    /* 反転可能フラグ(0 不可, 1 可) */
  struct StackList *pLl = NULL; /* リストの末端要素用ポインタ */
  struct StackList *pLa = NULL; /* リストの追加要素用ポインタ */
  int s[(Edge*2)-2];            /* 一時保存用のスタック(-1 終端記号) */

  pLl = &List[iT];              /* リストのアドレスを代入 */
  
  /* 全マスの調査 */
  for (row=0; row<Edge; row++) {
    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);

      /* 石を置いてないマスのとき */
      if (Table[iT][i]>=0) {
        for (j=0; j<(Edge*2)-2; j++) { s[j]=-1; } /* スタックの初期化 */

        /* 反転判定と反転個数の代入 */
        Table[iT][i] = Check_Lines(iT,nP,row,col,s);

        /* フラグの変更 (フラグが0で反転可のとき) */
        if (flg==0) { flg = ((Table[iT][i]!=0)&&(Table[iT][i]!=nMass)); }

        /* デバグ用 */
        if (0) {
          printf("(%d,%d) %d s[] : ",row,col,Table[iT][i]);
          for (j=0; j<(Edge*2)-2; j++) { printf("%d ", s[j]); }
          printf("\n");
        }
        
        /* 隣接か反転可能のときリストに追加 */
        if (Table[iT][i]!=nMass) {
          pLa = (struct StackList *)malloc(sizeof(struct StackList)); /* メモリの確保 */

          pLa->id = i;
          for (j=0; j<(Edge*2)-2; j++) { pLa->Stack[j] = s[j]; } /* 反転位置をコピー */
          pLa->next = NULL;
          pLl->next = pLa;

          pLl = pLa;
        }

      }
    }
  }

  /* デバグ用 */
  if (Debug) {
    pLa = &List[iT];
    while(pLa != NULL){
      printf("%02d : (%d) s[] : ", iT,pLa->id);
      for (j=0; j<(Edge*2)-2; j++) { printf("%d ", pLa->Stack[j]); }
      printf("\n");
      pLa = pLa->next;
    }
  }
  
  return flg;                   /* フラグを返す */
}


/*--------------------------------------------*/
/* 全ラインの反転判定 (in テーブル添字,プレーヤー,行,列,スタック out 反転個数(0 隣接, nMass 非隣接)) */
/*--------------------------------------------*/
int Check_Lines (int iT, int nP, int row, int col, int s[]) {
  int r=row, c=col;             /* 行,列 */
  int cnt=nMass, cntt=0;        /* 反転個数(反転なしのときはnMassのまま) */
  int iS=0, iSt=0;              /* スタックの添字 */
  int i=0,j=0;

  s[0]=(Exchange_Pos(row,col)); iS=1; /* 設置位置を保存 */
  
  /* 全６ラインの走査 */
  for (i=-1; i<=1; i++) {
    for (j=-1; j<=1; j++) {
      if (i==j) { continue; }   /* ６方向以外は除外 */
      r=row; c=col;             /* 行と列の初期値 */
      cntt=cnt;                 /* 個数の初期値 */
      iSt=iS;                   /* 添字の初期値 */

      /* 各ラインの走査 */
      while (1) {
        r += i; c += j;         /* 各方向に移動 */

        /* 盤の端または何もないとき */
        if ((Exchange_Pos(r,c)==-1) || (Table[iT][Exchange_Pos(r,c)]>=0)) {
          cnt=cntt; iS=iSt;     /* 反転しないため、個数と添字の初期化 */
          s[iS] = -1;           /* スタックの初期位置に末端記号をプッシュ */
          break;                /* 走査終了 */
        /* 自分の石のとき */
        } else if (Table[iT][Exchange_Pos(r,c)]==-nP) {
          s[iS] = -1;           /* スタックに末端記号をプッシュ */
          cnt%=nMass;           /* 隣接しているのでcntをnMassでないようにする */
          break;                /* 走査終了 */
        /* 他人の石のとき */
        } else {
          s[iS] = Exchange_Pos(r,c); /* スタックに反転位置をプッシュ */
          cntt%=nMass;          /* 隣接しており反転しない可能性があるのでcnttをnMassでないようにする */
          cnt=(cnt%nMass)+1; iS++; /* 反転個数とスタック添字のインクリメント */
        }
      }
    }
  }

  return cnt;
}


/*--------------------------------------------*/
/* 座標位置と配列添字の変換 (in 行,列, out 添字) */
/*--------------------------------------------*/
int Exchange_Pos (int row,int col) {
  if ((row>=0 && row<=Edge) && (col>=0 && col<=Edge) && (col<Edge-row)) {
    return (Edge*row-(row*row-row)/2)+col;
  } else { return -1; }         /* エラー */
}


/*--------------------------------------------*/
/* デバグ用 盤面の表示(盤面の中身を表示) (in テーブル添字,プレーヤー,反転フラグ) */
/*--------------------------------------------*/
void Print_Debug (int iT, int nP, int flg) {
  int i,row=0,col=0;

  /* 列の数字の表示 */
  if (Edge>=10) { printf("   "); } else { printf("  "); } /* 行の数字の幅調整 */
  for (i=0; i<Edge; i++) { printf("%c ", 'a'+i); }

  for (row=0; row<Edge; row++) {
    /* 行の数字の表示 */
    if (Edge>=10) { printf("\n%02d ", row+1); } else { printf("\n%d ", row+1); }
    for (i=0; i<row; i++) { printf(" "); } /* 調整用の空白の挿入 */

    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);
      /* マスの表示 */
      if (Table[iT][i]>=0) {    /* 何もないマスの処理 */
        printf("%02d",Table[iT][i]);
      } else {                  /* 石設置マス */
        printf("%d",Table[iT][i]);
      }
    }
  }
  printf("\n");
  
  /* 反転フラグと評価値の表示 */
  for (i=0; i<1+Edge/2; i++) { printf(" "); } /* 調整用の空白の挿入 */
  printf("flg:%d val:%d\n", flg, Calculate_EvaluatedValue(0,nP));
}


/*--------------------------------------------*/
/* 順位の計算 */
/*--------------------------------------------*/
void Sort_Num (void) {
  int rank[3] = {1,2,3};        /* ソート用配列 */
  int i,j,tmp;

  /* ソート */
  for (i=0; i<2; i++) {
    for (j=0; j<2-i; j++) {
      if (Player[rank[j]].Num < Player[rank[j+1]].Num) {
        tmp = rank[j];
        rank[j] = rank[j+1];
        rank[j+1] = tmp;
      }
    }
  }

  /* 順位の代入 (同順位の判定) */
  Player[rank[0]].Rank = 1;
  if (Player[rank[1]].Num==Player[rank[0]].Num) { Player[rank[1]].Rank = 1; }
  else { Player[rank[1]].Rank = 2; }
  if (Player[rank[2]].Num==Player[rank[0]].Num) { Player[rank[2]].Rank = 1; }
  else if (Player[rank[2]].Num==Player[rank[1]].Num) { Player[rank[2]].Rank = 2; }
  else { Player[rank[2]].Rank = 3; }
}


/*--------------------------------------------*/
/* 結果の表示 */
/*--------------------------------------------*/
void Honor_Platform(void) {
  int i;

  printf("[ Result ]\n\n");

  for (i=1; i<=3; i++) {
    if (Player[i].Rank==1) { printf(" 1st : P%d %s %02d\n", i,Player[i].Mark,Player[i].Num); }
  }
  for (i=1; i<=3; i++) {
    if (Player[i].Rank==2) { printf(" 2nd : P%d %s %02d\n", i,Player[i].Mark,Player[i].Num); }
  }
  for (i=1; i<=3; i++) {
    if (Player[i].Rank==3) { printf(" 3rd : P%d %s %02d\n", i,Player[i].Mark,Player[i].Num); }
  }

  /* 1stがManのとき */
  for (i=1; i<=3; i++) {
    if ((Player[i].St==1)&&(Player[i].Rank==1)) {
      printf("\nCongratulations!!\n ...and Thank you for Playing.\n");
    }
  }
}


/*--------------------------------------------*/
/* AIアルゴリズム(αβカット法) */
/*  (in テーブル添字,プレーヤー,深度,α値,β値, out 評価値(深度が0のときは配置位置) ) */
/*--------------------------------------------*/
int AI_AlphaBeta(int iT, int nP, int d, int a, int b) {
  int flg=0,val=0;              /* 反転フラグ,評価値 */
  int i,j,row=0,col=0;
  int p;                        /* 配置位置 */
  struct StackList *pL;         /* リストのポインタ */

  if (Debug) { printf("\n[%d] a:%d b:%d\n", d,a,b); }

  /* 終局 */
  Count_Stone(iT);
  if (Player[0].Num==0) { return Calculate_EvaluatedValue(iT,nP); }  
  /* 最深深度 */
  if (d==Depth) { return Calculate_EvaluatedValue(iT,nP); }
  /* その他 (終局時も同様の処理) */
  if (d<Depth) {
    flg = Check_Table(iT,1+(nP+d-1)%3); /* 反転判定 */

    /* スタックリストの走査 */
    pL = List[iT].next;
    while(pL != NULL){

      if (Debug) { printf("(%d) ", pL->id); }

      /* 設置可能なとき */
      if (Table[iT][pL->id]>=flg) {

        /* 次の盤面を作成 */
        for (i=0; i<nMass; i++) { Table[iT+1][i]=Table[iT][i]; }; /* 盤面のコピー */
        Free_ListMemory(iT+1);           /* リストの初期化 */
        List[iT+1].next = List[iT].next; /* スタックリストの一時参照コピー */
        Put_Stone(iT+1,1+(nP+d-1)%3,pL->id);  /* 盤面作成 */
        List[iT+1].next = NULL;          /* スタックの一時参照コピーの解除 */

        /* 次の盤面の調査 */
        val = AI_AlphaBeta(iT+1, nP, d+1, a, b);

        if (Debug) { Print_Debug(iT+1,nP,flg);}
        if (Debug) { printf("val:%d a:%d b:%d\n", val,a,b); }

        /* 自ターン */
        if ((d%3==0)&&(val>a)) {
          a = val; p=pL->id;    /* 配置位置の記憶 */
          /* αカット */
          if (a > b) { if (Debug) { printf("a:%d b:%d [Acut]\n", a,b); } break; }
        }

        /* 敵ターン */
        if ((d%3>=1)&&(val<b)) {
          b = val;
          /* βカット */
          if (a > b) { if (Debug) { printf("a:%d b:%d [Bcut]\n", a,b); } break; }
        }
      }
      pL = pL->next;            /* 次へ */
    }
  }

  if (d==0) { return p; }       /* 深度0 */
  if (d%3==0) { return a; }     /* 自ターン */
  if (d%3>=1) { return b; }     /* 敵ターン */
}


/*--------------------------------------------*/
/* スタックリストのメモリ開放 (in リスト添字) */   
/*--------------------------------------------*/
void Free_ListMemory (int n) {
  struct StackList *pL1, *pL2;  /* リストのポインタ */

  pL1 = List[n].next;
  while(pL1 != NULL){
    pL2 = pL1;
    pL1 = pL1->next;
    free(pL2);
  }

  List[n].next=NULL;            /* 先頭アドレスの初期化 */
}


/*--------------------------------------------*/
/* 評価値(自分の石の個数)の計算 (in テーブル添字,プレーヤー out 評価値) */   
/*--------------------------------------------*/
int Calculate_EvaluatedValue(int iT, int nP) {
  int i,row=0,col=0;
  int val=0;                    /* 評価値(端マス 3, 隅マス 30, その他 1) */

  /* 全マスの走査 */
  for (row=0; row<Edge; row++) {
    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);
      
      if (Table[iT][i]==-nP) {
        val++;
        /* 端マスのとき */
        if ((col==0)||(row==0)||(col+row==Edge-1)) { val += 2; }
        /* 隅マスのとき */
        if ((col==0)&&(row==0)) { val += 27; }
        if ((col==0)&&(row==Edge-1)) { val += 27; }
        if ((col==Edge-1)&&(row==0)) { val += 27; }
        /* 乱数増加 */
        if (rand()%100<25) { val++; };
      }
    }
  }
  return val;
}

/*--------------------------------------------*/
