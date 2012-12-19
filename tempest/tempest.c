/*--------------------------------------------*/
/* 3�l�p���o�[�V�Q�[���u�e���y�X�g-�P�v       */
/*--------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

/* �ϐ���` */
#define Debug 0                 /* �f�o�O�p�t���O */
#define Edge  9                 /* �ӂ̃}�X��(2..26) */
#define Depth 5                 /* AI�̐�ǂ݂̐[��(1..) */
#define Mark0 "�E"              /* �����Ȃ��}�X */
#define Mark1 "��"              /* P1�̐� */
#define Mark2 "��"              /* P2�̐� */
#define Mark3 "��"              /* P3�̐� */
#define Mark4 "�{"              /* �ݒu�\�}�X */

/* �֐��錾 */
void Print_Title(int*);                 /* �^�C�g���̕\�� */
int  Input_Pos(int,int,int);            /* �ݒu�ʒu�̓��� */
void Reset_Table(int);                  /* �Ֆʂ̏����� */
void Print_Table(int,int);              /* �Ֆʂ̕\�� */
void Put_Stone(int,int,int);            /* �΂̐ݒu�Ɣ��] */
int  Check_Table(int,int);              /* �Ֆʂ̔��]���� */
int  Check_Lines(int,int,int,int,int*); /* �S���C���̔��]���� */
int  Exchange_Pos(int,int);             /* �Ֆʂ̍��W�ʒu�Ɣz��Y���̕ϊ� */
void Print_Debug(int,int,int);          /* �f�o�O�p �Ֆʂ̕\�� */
void Sort_Num(void);                    /* ���ʂ̌v�Z */
void Honor_Platform(void);              /* ���ʂ̕\�� */
int  Calculate_EvaluatedValue(int,int); /* �]���l�̌v�Z */
int  AI_AlphaBeta(int,int,int,int,int); /* AI�A���S���Y�� */
void Count_Stone(int);                  /* ���̌v�Z */
void Free_ListMemory (int);             /* �X�^�b�N���X�g�̃������J�� */

/* �O���[�o���ϐ��錾 */
struct PlayerStatus {           /* �v���[���[�ƃ}�X�̏�� */
  int St;                       /* ��� 0 CPU, 1 Man */
  char Mark[10];                /* �� */
  int Num;                      /* ���� */
  int Rank;                     /* ���� */
} Player[5] = {{0,Mark0,0,0},{0,Mark1,0,0},{0,Mark2,0,0},{0,Mark3,0,0},{0,Mark4,0,0}};

struct StackList {              /* ���]�ʒu�ۑ��X�^�b�N�̃��X�g */
  int id;                       /* �΂̐ݒu�ʒu */
  int Stack[(Edge*2)-2];        /* ���]�ʒu�ۑ��X�^�b�N(-1 �I�[�L��) */
  struct StackList *next;       /* �|�C���^ */
} List[Depth+1];

int Table[Depth+1][(Edge*Edge+Edge)/2];  /* �Ղ̏��[(0 ����, 1..Depth AI�p)][(-1 1P, -2 2P, -3 3P, 0.. �����Ȃ�(0 �΂ɗא�, 1.. ���]��, nMass �΂ɔ�א�))]  */

int nMass = (Edge*Edge+Edge)/2;                       /* �}�X�ڂ̐� */
int maxVal = ((Edge*Edge+Edge)/2)+(3*9)+((Edge-2)*2); /* �]���l�̍ő�l */


/*--------------------------------------------*/
/* ���C���֐� */
/*--------------------------------------------*/
int main (void) {
  int flg=0;                    /* ���]�\�t���O(0 �s��, 1 ��) */
  int nT = 0;                   /* �^�[���� */
  int Order[3];                 /* ���ԕۑ��z�� */
  int i;

  /* �e���X�g�̃|�C���^�̏����� */
  for (i=0; i<Depth+1; i++) { List[i].next=NULL; }

  Reset_Table(0);               /* �Ղ̏����� */
  Print_Title(Order);           /* �^�C�g���̕\�� */
  printf("[ Game Start ]\n\n");

  /* ���C�����[�v */
  while(1) {
 
    /* ���ʂ̌v�Z */
    Count_Stone(0);
    Sort_Num();                 

    /* �I������ */
    if (Player[0].Num==0) { break; } /* �S�}�X�����܂����Ƃ� */

    /* �^�[���Ə��ʂ̕\�� */
    printf("[ Turn %02d ] P%d %s ", nT+1,Order[nT%3],Player[Order[nT%3]].Mark);
    if (Player[Order[nT%3]].Rank==1) { printf("1st\n\n"); }
    if (Player[Order[nT%3]].Rank==2) { printf("2nd\n\n"); }
    if (Player[Order[nT%3]].Rank==3) { printf("3rd\n\n"); }

    /* ���]����ƔՖʂ̕\�� */
    flg = Check_Table(0,Order[nT%3]);
    Print_Table(0,flg);
    if (Debug) { Print_Debug(0,Order[nT%3],flg); }

    /* �e�v���[���[�̌��̕\�� */
    printf("\n%s:%02d %s:%02d ", Player[0].Mark, Player[0].Num,Player[Order[0]].Mark, Player[Order[0]].Num);
    printf("%s:%02d %s:%02d\n\n",  Player[Order[1]].Mark, Player[Order[1]].Num,Player[Order[2]].Mark, Player[Order[2]].Num);

    /* �΂̐ݒu */
    if (Player[Order[nT%3]].St==0) { /* CPU */
      Put_Stone(0,Order[nT%3],AI_AlphaBeta(0,Order[nT%3],0,-maxVal,maxVal));
      printf("EnterKey : ");
      while(getchar() != '\n');
      printf("\n");
      nT++;        /* ���^�[���� */
    } else if (Player[Order[nT%3]].St==1) { /* Man */
      Put_Stone(0,Order[nT%3],Input_Pos(0,Order[nT%3],flg));
      getchar();
      printf("\n");
      nT++;        /* ���^�[���� */
    }

    /* �S���X�g�̃������J�� */
    for (i=0; i<Depth+1; i++) { Free_ListMemory(i); }
  }

  /* ���ʂ̕\�� */
  printf("[ Game Over ]\n\n");
  Print_Table(0,0);
  printf("\n");
  Honor_Platform();

	return 0;
}


/*--------------------------------------------*/
/* �Ֆʂ̏����� (�ӂ̃}�X���ɉ����ď����z�u�ύX) (in �e�[�u���Y��) */
/*--------------------------------------------*/
void Reset_Table (int iT) {
  int i,j;

  /* ������ */
  for (i=0; i<nMass; i++) { Table[iT][i] = nMass; }

  j=(int)Edge/3;
  /* �ӂ�3�̔{���̂Ƃ� �O�p�`�z�u */
  if (Edge%3==0) {
    Table[iT][Exchange_Pos(j-1,j)] = -1;
    Table[iT][Exchange_Pos(j,j-1)] = -2;
    Table[iT][Exchange_Pos(j,j)]   = -3;
  }
  /* �ӂ�3�̔{��+1�̂Ƃ� �Z�p�`�z�u */
  if (Edge%3==1) {
    Table[iT][Exchange_Pos(j-1,j)]   = -1;
    Table[iT][Exchange_Pos(j-1,j+1)] = -2;
    Table[iT][Exchange_Pos(j,j-1)]   = -3;
    Table[iT][Exchange_Pos(j,j+1)]   = -3;
    Table[iT][Exchange_Pos(j+1,j-1)] = -2;
    Table[iT][Exchange_Pos(j+1,j)]   = -1;
  }
  /* �ӂ�3�̔{��+2�̂Ƃ� �t�O�p�`�z�u */
  if (Edge%3==2) {
    Table[iT][Exchange_Pos(j,j)]   = -1;
    Table[iT][Exchange_Pos(j,j+1)] = -2;
    Table[iT][Exchange_Pos(j+1,j)] = -3;
  }
}


/*--------------------------------------------*/
/* �^�C�g�����j���[�̕\���ƃ��[�h���͏��� (in ���ԗp�z��) */
/*--------------------------------------------*/
void Print_Title (int o[]) {
  int i=0, nM=0;
  int rnd=0;                    /* ���ԗp�̗��� */

  srand( time(NULL) );
  
  /* �^�C�g�����j���[�̕\�� */
  printf(" ---[ Tempest-1 ]---\n\n");
  printf("      P1%s P2%s P3%s\n", Player[1].Mark,Player[2].Mark,Player[3].Mark);
	printf("  1 : Man  Man  Man\n");
	printf("  2 : Man  Man  CPU\n");
	printf("  3 : Man  CPU  CPU\n");
	printf("  4 : CPU  CPU  CPU\n");
	printf("  0 : Help\n");

  /* ���j���[�I������ */
  while(1) {
    printf("\nMode(0_4) : ");
    scanf("%d", &nM);
    getchar();
    printf("\n");

    /* �v���[���[�̐ݒ� */
    if ((nM>=1)&&(nM<=4)) {
      /* �v���[���[�̑��� (Man or CPU) */
      for (i=1; i<=3; i++) { Player[i].St = (i+nM <= 4); }

      /* ���Ԃ̌��� */
      rnd = rand()%6;
      if (rnd==0) { o[0]=1; o[1]=2; o[2]=3; }
      if (rnd==1) { o[0]=1; o[1]=3; o[2]=2; }
      if (rnd==2) { o[0]=2; o[1]=1; o[2]=3; }
      if (rnd==3) { o[0]=2; o[1]=3; o[2]=1; }
      if (rnd==4) { o[0]=3; o[1]=1; o[2]=2; }
      if (rnd==5) { o[0]=3; o[1]=2; o[2]=1; }

      /* �v���[���̑���Ə��Ԃ̕\�� */
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
    /* �w���v�̕\�� */
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
/* ���v�Z (in �e�[�u���Y��)*/
/*--------------------------------------------*/
void Count_Stone (int iT) {
  int i;

  for (i=0; i<=3; i++) { Player[i].Num = 0; } /* ���̏����� */

  /* �Ղ̑��� */
  for (i=0; i<=nMass; i++) {      
    if (Table[iT][i]>=0) {      /* �����Ȃ��}�X */
      Table[iT][i]=nMass;
      Player[0].Num++;          /* ���̃C���N�������g */
    } else {                    /* �ΐݒu�}�X */
      Player[-Table[iT][i]].Num++; /* ���̃C���N�������g */
    }
  }
}


/*--------------------------------------------*/
/* �Ֆʂ̕\�� (in �e�[�u���Y��,���]�t���O)*/
/*--------------------------------------------*/
void Print_Table (int iT, int flg) {
  int i,row=0,col=0;

  /* ��̐����̕\�� */
  if (Edge>=10) { printf("   "); } else { printf("  "); } /* ��̐����̕����� */
  for (i=0; i<Edge; i++) { printf("%c ", 'a'+i); }

  for (row=0; row<Edge; row++) {
    /* �s�̐����̕\�� */
    if (Edge>=10) { printf("\n%02d ", row+1); } else { printf("\n%d ", row+1); }
    for (i=0; i<row; i++) { printf(" "); } /* �����p�̋󔒂̑}�� */

    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);

      /* �}�X�̕\�� */
      if (Table[iT][i]>=0) {    /* �����Ȃ��}�X�̏��� */
        if (Table[iT][i]==0) {  /* �אڃ}�X */
          if (flg==0) { printf("%s",Player[4].Mark); } /* ���]�s�̂Ƃ� */
          if (flg==1) { printf("%s",Player[0].Mark); } /* ���]�\�̂Ƃ� */
        } else if (Table[iT][i]==nMass) { /* ��אڃ}�X */
          printf("%s",Player[0].Mark);
        } else {                /* ���]�\�}�X */
          printf("%s",Player[4].Mark);
        }
      } else {                  /* �ΐݒu�}�X�̏��� */
        printf("%s",Player[-Table[iT][i]].Mark);
      }
      
    }
  }
  printf("\n");
}


/*--------------------------------------------*/
/* �z�u�ʒu�̓��� (in �e�[�u���Y��,�v���[���[,���]�t���O out ���͈ʒu) */
/*--------------------------------------------*/
int Input_Pos (int iT, int nP, int flg) {
  int row=0, col=0;
  int i;
  char tmp;

  while(1) {
    /* �ʒu�̓��� */
    printf("Pos(1_%d a_%c) : ", Edge,'a'+Edge-1);
    scanf("%d %c", &row,&tmp);
    row--; col = tmp-'a';

    /* -1�ŋ����I�� */
    if (row==-1) { exit(1); }

    i=Exchange_Pos(row,col);
    /* ���]�\�̂Ƃ��͔��]�\�}�X�� ���]�s�̂Ƃ��͗אڃ}�X�� */
    if ((i!=-1)&&(Table[iT][i]!=nMass)&&(Table[iT][i]>=flg)) { return i; }
    else { printf("Error : Wrong Position!\n"); }
    printf("\n");
  }
  return -1;
}


/*--------------------------------------------*/
/* �΂̐ݒu (in �e�[�u���Y��,�v���[���[,�ݒu�ʒu) */
/*--------------------------------------------*/
void Put_Stone (int iT, int nP, int id) {
  int i,j;
  struct StackList *pL;         /* ���X�g�̃|�C���^ */

  /* �X�^�b�N���X�g�𑖍� */
  pL = List[iT].next;
  while(pL != NULL){
    /* �X�^�b�N�̈ʒu���ݒu�ʒu�̂Ƃ� */
    if (pL->id==id) {
      if (Debug) { printf("Put : (%d) : ",id); }
      /* �X�^�b�N�𑖍� */
      for (j=0; j<(Edge*2)-2; j++) {
        if (pL->Stack[j]==-1) { break; } /* �Ō�܂Ō�����I�� */
        if (Debug) { printf("%d ",pL->Stack[j]); }
        Table[iT][pL->Stack[j]]=-nP; /* �ݒu���]���� */
      }
      if (Debug) { printf("\n"); }
      break;
    }

    pL = pL->next;              /* ���� */
  }  
}


/*--------------------------------------------*/
/* �Ղ̔��]����  (in �e�[�u���Y��,�v���[���[, out ���]�t���O) */
/*--------------------------------------------*/
int Check_Table (int iT, int nP) {
  int i,j,row=0,col=0;          /* �s,�� */
  int flg=0;                    /* ���]�\�t���O(0 �s��, 1 ��) */
  struct StackList *pLl = NULL; /* ���X�g�̖��[�v�f�p�|�C���^ */
  struct StackList *pLa = NULL; /* ���X�g�̒ǉ��v�f�p�|�C���^ */
  int s[(Edge*2)-2];            /* �ꎞ�ۑ��p�̃X�^�b�N(-1 �I�[�L��) */

  pLl = &List[iT];              /* ���X�g�̃A�h���X���� */
  
  /* �S�}�X�̒��� */
  for (row=0; row<Edge; row++) {
    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);

      /* �΂�u���ĂȂ��}�X�̂Ƃ� */
      if (Table[iT][i]>=0) {
        for (j=0; j<(Edge*2)-2; j++) { s[j]=-1; } /* �X�^�b�N�̏����� */

        /* ���]����Ɣ��]���̑�� */
        Table[iT][i] = Check_Lines(iT,nP,row,col,s);

        /* �t���O�̕ύX (�t���O��0�Ŕ��]�̂Ƃ�) */
        if (flg==0) { flg = ((Table[iT][i]!=0)&&(Table[iT][i]!=nMass)); }

        /* �f�o�O�p */
        if (0) {
          printf("(%d,%d) %d s[] : ",row,col,Table[iT][i]);
          for (j=0; j<(Edge*2)-2; j++) { printf("%d ", s[j]); }
          printf("\n");
        }
        
        /* �אڂ����]�\�̂Ƃ����X�g�ɒǉ� */
        if (Table[iT][i]!=nMass) {
          pLa = (struct StackList *)malloc(sizeof(struct StackList)); /* �������̊m�� */

          pLa->id = i;
          for (j=0; j<(Edge*2)-2; j++) { pLa->Stack[j] = s[j]; } /* ���]�ʒu���R�s�[ */
          pLa->next = NULL;
          pLl->next = pLa;

          pLl = pLa;
        }

      }
    }
  }

  /* �f�o�O�p */
  if (Debug) {
    pLa = &List[iT];
    while(pLa != NULL){
      printf("%02d : (%d) s[] : ", iT,pLa->id);
      for (j=0; j<(Edge*2)-2; j++) { printf("%d ", pLa->Stack[j]); }
      printf("\n");
      pLa = pLa->next;
    }
  }
  
  return flg;                   /* �t���O��Ԃ� */
}


/*--------------------------------------------*/
/* �S���C���̔��]���� (in �e�[�u���Y��,�v���[���[,�s,��,�X�^�b�N out ���]��(0 �א�, nMass ��א�)) */
/*--------------------------------------------*/
int Check_Lines (int iT, int nP, int row, int col, int s[]) {
  int r=row, c=col;             /* �s,�� */
  int cnt=nMass, cntt=0;        /* ���]��(���]�Ȃ��̂Ƃ���nMass�̂܂�) */
  int iS=0, iSt=0;              /* �X�^�b�N�̓Y�� */
  int i=0,j=0;

  s[0]=(Exchange_Pos(row,col)); iS=1; /* �ݒu�ʒu��ۑ� */
  
  /* �S�U���C���̑��� */
  for (i=-1; i<=1; i++) {
    for (j=-1; j<=1; j++) {
      if (i==j) { continue; }   /* �U�����ȊO�͏��O */
      r=row; c=col;             /* �s�Ɨ�̏����l */
      cntt=cnt;                 /* ���̏����l */
      iSt=iS;                   /* �Y���̏����l */

      /* �e���C���̑��� */
      while (1) {
        r += i; c += j;         /* �e�����Ɉړ� */

        /* �Ղ̒[�܂��͉����Ȃ��Ƃ� */
        if ((Exchange_Pos(r,c)==-1) || (Table[iT][Exchange_Pos(r,c)]>=0)) {
          cnt=cntt; iS=iSt;     /* ���]���Ȃ����߁A���ƓY���̏����� */
          s[iS] = -1;           /* �X�^�b�N�̏����ʒu�ɖ��[�L�����v�b�V�� */
          break;                /* �����I�� */
        /* �����̐΂̂Ƃ� */
        } else if (Table[iT][Exchange_Pos(r,c)]==-nP) {
          s[iS] = -1;           /* �X�^�b�N�ɖ��[�L�����v�b�V�� */
          cnt%=nMass;           /* �אڂ��Ă���̂�cnt��nMass�łȂ��悤�ɂ��� */
          break;                /* �����I�� */
        /* ���l�̐΂̂Ƃ� */
        } else {
          s[iS] = Exchange_Pos(r,c); /* �X�^�b�N�ɔ��]�ʒu���v�b�V�� */
          cntt%=nMass;          /* �אڂ��Ă��蔽�]���Ȃ��\��������̂�cntt��nMass�łȂ��悤�ɂ��� */
          cnt=(cnt%nMass)+1; iS++; /* ���]���ƃX�^�b�N�Y���̃C���N�������g */
        }
      }
    }
  }

  return cnt;
}


/*--------------------------------------------*/
/* ���W�ʒu�Ɣz��Y���̕ϊ� (in �s,��, out �Y��) */
/*--------------------------------------------*/
int Exchange_Pos (int row,int col) {
  if ((row>=0 && row<=Edge) && (col>=0 && col<=Edge) && (col<Edge-row)) {
    return (Edge*row-(row*row-row)/2)+col;
  } else { return -1; }         /* �G���[ */
}


/*--------------------------------------------*/
/* �f�o�O�p �Ֆʂ̕\��(�Ֆʂ̒��g��\��) (in �e�[�u���Y��,�v���[���[,���]�t���O) */
/*--------------------------------------------*/
void Print_Debug (int iT, int nP, int flg) {
  int i,row=0,col=0;

  /* ��̐����̕\�� */
  if (Edge>=10) { printf("   "); } else { printf("  "); } /* �s�̐����̕����� */
  for (i=0; i<Edge; i++) { printf("%c ", 'a'+i); }

  for (row=0; row<Edge; row++) {
    /* �s�̐����̕\�� */
    if (Edge>=10) { printf("\n%02d ", row+1); } else { printf("\n%d ", row+1); }
    for (i=0; i<row; i++) { printf(" "); } /* �����p�̋󔒂̑}�� */

    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);
      /* �}�X�̕\�� */
      if (Table[iT][i]>=0) {    /* �����Ȃ��}�X�̏��� */
        printf("%02d",Table[iT][i]);
      } else {                  /* �ΐݒu�}�X */
        printf("%d",Table[iT][i]);
      }
    }
  }
  printf("\n");
  
  /* ���]�t���O�ƕ]���l�̕\�� */
  for (i=0; i<1+Edge/2; i++) { printf(" "); } /* �����p�̋󔒂̑}�� */
  printf("flg:%d val:%d\n", flg, Calculate_EvaluatedValue(0,nP));
}


/*--------------------------------------------*/
/* ���ʂ̌v�Z */
/*--------------------------------------------*/
void Sort_Num (void) {
  int rank[3] = {1,2,3};        /* �\�[�g�p�z�� */
  int i,j,tmp;

  /* �\�[�g */
  for (i=0; i<2; i++) {
    for (j=0; j<2-i; j++) {
      if (Player[rank[j]].Num < Player[rank[j+1]].Num) {
        tmp = rank[j];
        rank[j] = rank[j+1];
        rank[j+1] = tmp;
      }
    }
  }

  /* ���ʂ̑�� (�����ʂ̔���) */
  Player[rank[0]].Rank = 1;
  if (Player[rank[1]].Num==Player[rank[0]].Num) { Player[rank[1]].Rank = 1; }
  else { Player[rank[1]].Rank = 2; }
  if (Player[rank[2]].Num==Player[rank[0]].Num) { Player[rank[2]].Rank = 1; }
  else if (Player[rank[2]].Num==Player[rank[1]].Num) { Player[rank[2]].Rank = 2; }
  else { Player[rank[2]].Rank = 3; }
}


/*--------------------------------------------*/
/* ���ʂ̕\�� */
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

  /* 1st��Man�̂Ƃ� */
  for (i=1; i<=3; i++) {
    if ((Player[i].St==1)&&(Player[i].Rank==1)) {
      printf("\nCongratulations!!\n ...and Thank you for Playing.\n");
    }
  }
}


/*--------------------------------------------*/
/* AI�A���S���Y��(�����J�b�g�@) */
/*  (in �e�[�u���Y��,�v���[���[,�[�x,���l,���l, out �]���l(�[�x��0�̂Ƃ��͔z�u�ʒu) ) */
/*--------------------------------------------*/
int AI_AlphaBeta(int iT, int nP, int d, int a, int b) {
  int flg=0,val=0;              /* ���]�t���O,�]���l */
  int i,j,row=0,col=0;
  int p;                        /* �z�u�ʒu */
  struct StackList *pL;         /* ���X�g�̃|�C���^ */

  if (Debug) { printf("\n[%d] a:%d b:%d\n", d,a,b); }

  /* �I�� */
  Count_Stone(iT);
  if (Player[0].Num==0) { return Calculate_EvaluatedValue(iT,nP); }  
  /* �Ő[�[�x */
  if (d==Depth) { return Calculate_EvaluatedValue(iT,nP); }
  /* ���̑� (�I�ǎ������l�̏���) */
  if (d<Depth) {
    flg = Check_Table(iT,1+(nP+d-1)%3); /* ���]���� */

    /* �X�^�b�N���X�g�̑��� */
    pL = List[iT].next;
    while(pL != NULL){

      if (Debug) { printf("(%d) ", pL->id); }

      /* �ݒu�\�ȂƂ� */
      if (Table[iT][pL->id]>=flg) {

        /* ���̔Ֆʂ��쐬 */
        for (i=0; i<nMass; i++) { Table[iT+1][i]=Table[iT][i]; }; /* �Ֆʂ̃R�s�[ */
        Free_ListMemory(iT+1);           /* ���X�g�̏����� */
        List[iT+1].next = List[iT].next; /* �X�^�b�N���X�g�̈ꎞ�Q�ƃR�s�[ */
        Put_Stone(iT+1,1+(nP+d-1)%3,pL->id);  /* �Ֆʍ쐬 */
        List[iT+1].next = NULL;          /* �X�^�b�N�̈ꎞ�Q�ƃR�s�[�̉��� */

        /* ���̔Ֆʂ̒��� */
        val = AI_AlphaBeta(iT+1, nP, d+1, a, b);

        if (Debug) { Print_Debug(iT+1,nP,flg);}
        if (Debug) { printf("val:%d a:%d b:%d\n", val,a,b); }

        /* ���^�[�� */
        if ((d%3==0)&&(val>a)) {
          a = val; p=pL->id;    /* �z�u�ʒu�̋L�� */
          /* ���J�b�g */
          if (a > b) { if (Debug) { printf("a:%d b:%d [Acut]\n", a,b); } break; }
        }

        /* �G�^�[�� */
        if ((d%3>=1)&&(val<b)) {
          b = val;
          /* ���J�b�g */
          if (a > b) { if (Debug) { printf("a:%d b:%d [Bcut]\n", a,b); } break; }
        }
      }
      pL = pL->next;            /* ���� */
    }
  }

  if (d==0) { return p; }       /* �[�x0 */
  if (d%3==0) { return a; }     /* ���^�[�� */
  if (d%3>=1) { return b; }     /* �G�^�[�� */
}


/*--------------------------------------------*/
/* �X�^�b�N���X�g�̃������J�� (in ���X�g�Y��) */   
/*--------------------------------------------*/
void Free_ListMemory (int n) {
  struct StackList *pL1, *pL2;  /* ���X�g�̃|�C���^ */

  pL1 = List[n].next;
  while(pL1 != NULL){
    pL2 = pL1;
    pL1 = pL1->next;
    free(pL2);
  }

  List[n].next=NULL;            /* �擪�A�h���X�̏����� */
}


/*--------------------------------------------*/
/* �]���l(�����̐΂̌�)�̌v�Z (in �e�[�u���Y��,�v���[���[ out �]���l) */   
/*--------------------------------------------*/
int Calculate_EvaluatedValue(int iT, int nP) {
  int i,row=0,col=0;
  int val=0;                    /* �]���l(�[�}�X 3, ���}�X 30, ���̑� 1) */

  /* �S�}�X�̑��� */
  for (row=0; row<Edge; row++) {
    for (col=0; col<Edge-row; col++) {
      i=Exchange_Pos(row,col);
      
      if (Table[iT][i]==-nP) {
        val++;
        /* �[�}�X�̂Ƃ� */
        if ((col==0)||(row==0)||(col+row==Edge-1)) { val += 2; }
        /* ���}�X�̂Ƃ� */
        if ((col==0)&&(row==0)) { val += 27; }
        if ((col==0)&&(row==Edge-1)) { val += 27; }
        if ((col==Edge-1)&&(row==0)) { val += 27; }
        /* �������� */
        if (rand()%100<25) { val++; };
      }
    }
  }
  return val;
}

/*--------------------------------------------*/
