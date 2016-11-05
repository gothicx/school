//
// 3D Engine v1.0 em C com duas funções em Assembly! ;-)
//
// Composicao de quatro piramides pentagonais de base centrada na origem com
// a altura oito vezes superior a medida da aresta da base.
//
// bY Marco Rodrigues in 2k+3 <Marco@Tondela.org>
//
// Nota: Por causa do Assembly a Instruction Set do compilador tem de ser 80186.
//
// RED CODE: Usem isto para aprender, e não para ripar, e dizer que foram vocês que fizeram.. isso é lame! :-D

// Librarias necessarias.
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <math.h>

// Caminho para os ficheiros .bgi (graphics)
#define BGI_PATH "\\prog\\tc\\bgi"

// Numero de pontos da figura.
#define N_PONTOS 6

// Todas as Teclas.
#define DIREITA 77
#define ESQUERDA 75
#define CIMA 72
#define BAIXO 80
#define ESC 27
// Teclado numerico (pequenino do lado direito).
#define SETA_DIREITA 54
#define SETA_ESQUERDA 52
#define SETA_BAIXO 50
#define TECLA_MAIS 43
#define TECLA_MENOS 45
// Teclas dos Botoes (Menu).
#define TECLA_ESC 27
#define TECLA_A 97
#define TECLA_D 100
#define TECLA_P 112
#define TECLA_X 120
#define TECLA_Y 121
#define TECLA_Z 122
#define TECLA_R 114
#define TECLA_M 109
#define TECLA_N 110
#define TECLA_E 101
#define TECLA_S 115
#define TECLA_U 117

// Cores v lidas para pintar a figura.
enum cores {
	blue=1,
	green=2,
	cyan=3,
	red=4,
	magenta=5,
	brown=6,
	lightgray=7,
	darkgray=8,
	lightblue=9,
	lightgreen=10,
	lightcyan=11,
	lightred=12,
	lightmagenta=13,
	yello=14,
	white=15
};

// Estrutura dos interruptores (registos) do processador.
struct REGPACK reg;

// Estruturas dos pontos de uma figura.
typedef struct {
	float x, y, z;
} pontos[N_PONTOS+1];

pontos po_um, po_dois, po_tres, po_quatro;

// Horas.
struct time t;
struct date d;

int nx0=320, ny0=220, sent=0, eixo=0, i=0, showeixos=0;
float PI=3.1415926535897932384626433832795, delta=0;
unsigned char fundo[768];

// Assembler.
char DACTABLE1[768];
char DACTABLE2[768];

// Vai buscar a palete de cores.
void getpalette(unsigned char far *paletesdelas)
{
	asm {
		les     di, paletesdelas
		mov     cx, 768
		mov     dx, 03c7h
		xor     al, al
		out     dx, al
		inc     dx
		inc     dx
		rep     insb
	}
}

// Faz um fadeout a totalidade do ecra com determinada velocidade.
void fadeout(unsigned char *array, char veloc)
{
	int f;

	for (f=0;f<768;f++)
		DACTABLE2[f] = array[f];

	asm push ds
	asm mov bl, 32

FOLoop:
	for (f=0;f<768;f++)
	{
		if (DACTABLE2[f] > 1) DACTABLE2[f] -= 2;
		else if (DACTABLE2[f] == 1) DACTABLE2[f]--;
	}

	asm {
		mov si, offset DACTABLE2
		mov cx, 768
	}

	for (f=0;f<veloc;f++)
	{
		asm     mov     dx, 3dah
FOP1:
		asm {
			in      al, dx
			and     al, 8
			jnz     FOP1
		}
FOP2:
		asm {
			in      al, dx
			and     al, 8
			jz      FOP2
		}
	 }

	asm {
		mov dx, 03c8h
		xor al, al
		out dx, al
		inc dx
		rep outsb
		dec bl
		jnz FOLoop
		pop ds
	}
}

// Faz um fadein a totalidade do ecra com determinada velocidade.
void fadein(unsigned char *array, char veloc)
{
	int f;

	for (f=0;f<768;f++)
		DACTABLE1[f] = array[f];

	asm push ds
	asm mov bl, 45

FILoop:

	for (f=0;f<768;f++)
	{
		if (DACTABLE2[f]==(DACTABLE1[f]-1)) DACTABLE2[f] += 1;
		else if (DACTABLE2[f]<DACTABLE1[f]) DACTABLE2[f] += 2;
				else if (DACTABLE2[f]>DACTABLE1[f]) DACTABLE2[f] -= 1;
	}

	asm {
		mov si, offset DACTABLE2
		mov cx, 768
	}

	for (f=0;f<veloc;f++)
	{
		asm     mov     dx, 3dah
FIP1:
		asm {
			in      al, dx
			and     al, 8
			jnz     FIP1
		}
FIP2:
		asm {
			in      al, dx
			and     al, 8
			jz      FIP2
		}
	}

	asm {
		mov dx, 03c8h
		xor al, al
		out dx, al
		inc dx
		rep outsb
		dec bl
		jnz FILoop
		pop ds
	}
}

void ecran(float x,float y,float *ex,float *ey)
{
    *ex=nx0+x;
    *ey=ny0-y;

    return;
}

// 3D para 2D.
void conv3d(float x,float y,float z,float *ex,float *ey)
{
    *ex=y;
    *ey=z;
    ecran(*ex, *ey, ex, ey);

    return;
}

void rect(float ro,float teta,float *x,float *y)
{
    *x=ro*cos(teta);
    *y=ro*sin(teta);

    return;
}

void pol(float x,float y,float *ro,float *teta)
{
    *ro=sqrt(pow(x,2)+pow(y,2));

    if (x==0)
      if (y>0)
	*teta=PI/2;
      else
	*teta=3*PI/2;
    else
      {
	*teta=atan(y/x);
	if (x<0)
	  *teta=*teta+PI;
      }

    return;
}

void roda(float x,float y,float *nx,float *ny)
{
    float r,t,zx,zy;

    pol(x,y,&r,&t);
    if (sent==1)
       t=t+delta;
    else
      if (sent==-1)
       t=t-delta;
    rect(r,t,&zx,&zy);
    *nx=zx;
    *ny=zy;

    return;
}

// Ve se o rato esta instalado e retorna TRUE ou FALSE.
int inicia_rato()
{
	reg.r_ax=00;
	intr(0x33,&reg);

	if (reg.r_ax!=0) return 0;
	else return 1;
}

// Mostra o rato no ecra.
void mostra_rato()
{
	reg.r_ax=01;
	intr(0x33,&reg);
}

// Esconde o rato do ecra.
void esconde_rato()
{
	reg.r_ax=02;
	intr(0x33,&reg);
}

// Devolve as Coordenada de X e Y do rato.
void rato_xy(float &x,float &y)
{
	reg.r_ax=03;
	intr(0x33,&reg);
	x=floor(reg.r_cx/8);
	y=floor(reg.r_dx/8);
}

// Retorna um valor TRUE ou FALSE se detectar o rato em determinadas coordenadas.
int rato_detecta(int x1, int y1, int x2, int y2)
{
	float x=0, y=0;

	reg.r_ax=03;
	intr(0x33,&reg);
	x=floor(reg.r_cx/8);
	y=floor(reg.r_dx/8);

	x=x*getmaxx()/80;
	y=y*getmaxy()/25;

	if ((x1 <= x) && (x2 >= x) && (y1 <= y) && (y2 >= y))
		return 0;
	else return 1;
}

// Verifica qual a tecla do rato foi premida.
// 0 = Nenhuma
// 1 = Tecla Esquerda do Rato
// 2 = Tecla Direita do Rato
int rato_tecla()
{
	reg.r_ax=03;
	intr(0x33,&reg);

	return reg.r_bx;
}

// Verifica se foi premida uma tecla do rato.
int rato_press(int tecla)
{
	if (rato_tecla()==tecla) return 0;
	else return 1;
}

// Relogio analogico c/ horas do sistema (maquina).
void relogio_anal()
{
	int x1=320, y1=240, sec=0, min=0, hor=0;
	double ang_sec=0, ang_min=0, ang_hor=0;

	esconde_rato();

	setcolor(WHITE);
	circle(x1,y1,150);

	settextstyle(4,0,2);
	setcolor(WHITE);
	outtextxy(320,70,"12");
	outtextxy(323,400,"6");
	outtextxy(482,221,"3");
	outtextxy(155,229,"9");
	settextstyle(0,0,0);
	outtextxy(320,35,"Pressione uma tecla para sair do relogio...");

	do {
		// Horas do sistema (maquina).
		gettime(&t);
		hor=t.ti_hour;
		sec=t.ti_sec;
		min=t.ti_min;

		// Segundos.
		setcolor(BLACK);
		line(x1,y1,x1+cos(ang_sec)*148,y1+sin(ang_sec)*148);
		ang_sec=4.71+sec*(PI/2/3/5);
		setcolor(GREEN);
		line(x1,y1,x1+cos(ang_sec)*148,y1+sin(ang_sec)*148);

		// Minutos.
		setcolor(BLACK);
		line(x1,y1,x1+cos(ang_min)*95,y1+sin(ang_min)*95);
		ang_min=4.71+min*(PI/2/3/5);
		setcolor(RED);
		line(x1,y1,x1+cos(ang_min)*95,y1+sin(ang_min)*95);

		// Horas.
		setcolor(BLACK);
		line(x1,y1,x1+cos(ang_hor)*40,y1+sin(ang_hor)*40);
		ang_hor=4.71+hor*(PI/2/3);
		setcolor(BLUE);
		line(x1,y1,x1+cos(ang_hor)*40,y1+sin(ang_hor)*40);

		// Dois circulos da parte de dentro do relogio.
		setcolor(WHITE);
		circle(x1,y1,98);
		setcolor(WHITE);
		circle(x1,y1,40);
		delay(120);
	} while (!kbhit());

	// Apaga o Relogio e volta para a figura.
	setcolor(BLACK);
	circle(x1,y1,150);

	setcolor(BLACK);
	circle(x1,y1,98);

	setcolor(BLACK);
	circle(x1,y1,40);

	settextstyle(4,0,2);
	setcolor(BLACK);
	outtextxy(320,70,"12");
	outtextxy(323,400,"6");
	outtextxy(482,221,"3");
	outtextxy(155,229,"9");
	settextstyle(0,0,0);
	outtextxy(320,35,"Pressione uma tecla para sair do relogio...");

	setcolor(BLACK);
	line(x1,y1,x1+cos(ang_sec)*148,y1+sin(ang_sec)*148);
	setcolor(BLACK);
	line(x1,y1,x1+cos(ang_min)*95,y1+sin(ang_min)*95);
	setcolor(BLACK);
	line(x1,y1,x1+cos(ang_hor)*40,y1+sin(ang_hor)*40);
}

// Desenha uma barra de titulo ou de tarefas (your choise).
// y = Posicao da barra em Y (deslocacao).
void barra(int y, char titulo[80])
{
	int midx=0, maxx=0;

	midx=getmaxx();
	maxx=midx / 2;

	setfillstyle(SOLID_FILL, LIGHTGRAY);
	bar(1,19+y,midx,2+y);
	setcolor(BLACK);
	// Centra o texto na barra.
	settextjustify(CENTER_TEXT, CENTER_TEXT);
	// Escreve o texto na barra.
	outtextxy(maxx,11+y,titulo);
	setcolor(BLACK);
	setlinestyle(SOLID_LINE, 1, 2);
	// Ao alto do lado esquerdo.
	line(2,3+y,2,17+y);
	line(3,3+y,3,18+y);
	// Ao alto do lado direito.
	line(midx-2,3+y,midx-2,18+y);
	line(midx-3,4+y,midx-3,18+y);
	// Linha de cima.
	line(2,3+y,2+midx-5,3+y);
	line(2,4+y,3+midx-5,4+y);
	// Linha de baixo.
	line(midx-4,18+y,2,18+y);
	line(midx-4,17+y,3,17+y);
}

// Desenha um Botao (normal).
void botao(int x, int y, int x1, int y1, char titulo[15])
{
	char titulo2[15];
	float localx;
	int light=0;

	setfillstyle(SOLID_FILL, LIGHTGRAY);
	setcolor(LIGHTGRAY);
	bar(x,y,x1,y1);

	// Escreve o texto no botao.
	// Caso detecte um & entao faz highlight a vermelho (Tecla de atalho)

	localx=floor(x+((x1-x)/2))-floor(strlen(titulo)*2.5);

	for (int i=0,k=0;i<strlen(titulo);i++)
	{
		if (titulo[i]=='&')
		{
			setcolor(RED);
			light=1;
		} else setcolor(BLACK);

		if (light==1)
		{
			sprintf(titulo2,"%c",titulo[i+1]);
			outtextxy(localx+i*7,y+11,titulo2);
			light=0;
			k++;
		} else {
			sprintf(titulo2,"%c",titulo[i+k]);
			outtextxy(localx+i*7-i+2+k,y+11,titulo2);
		}
	}

	setcolor(BLACK);
	// Borda superior.
	line(x+1,y+1,x1-1,y+1);
	// Borda inferior.
	line(x+1,y1-1,x1-1,y1-1);
	// Borda lateral esquerda.
	line(x+1,y+1,x+1,y1-1);
	// Borda lateral direita.
	line(x1-1,y+1,x1-1,y1-2);
}

// Desenha um Botao (clique).
void botao_clk(int x, int y, int x1, int y1, char titulo[15])
{
	char titulo2[15];
	float localx;
	int light=0;

	setfillstyle(SOLID_FILL, LIGHTGRAY);
	setcolor(LIGHTGRAY);
	bar(x,y,x1,y1);

	// Escreve o texto no botao.
	// Caso detecte um & entao faz highlight a vermelho (Tecla de atalho)
	localx=floor(x+((x1-x)/2))-floor(strlen(titulo)*3)+5;

	for (int i=0,k=0;i<strlen(titulo);i++)
	{
		if (titulo[i]=='&')
		{
			setcolor(RED);
			light=1;
		} else setcolor(BLACK);

		if (light==1)
		{
			sprintf(titulo2,"%c",titulo[i+1]);
			outtextxy(localx+i*7,y+12,titulo2);
			light=0;
			k++;
		} else {
			sprintf(titulo2,"%c",titulo[i+k]);
			outtextxy(localx+i*7-i+2+k,y+12,titulo2);
		}
	}

	setcolor(BLUE);
	// Borda superior.
	line(x+1,y+1,x1-1,y+1);
	// Borda inferior.
	line(x+1,y1-1,x1-1,y1-1);
	// Borda lateral esquerda.
	line(x+1,y+1,x+1,y1-1);
	// Borda lateral direita.
	line(x1-1,y+1,x1-1,y1-2);
	delay(200);
}

// Desenha os eixos X, Y e Z.
/* void eixos()
{
	setcolor(6);
	line(nx0,ny0,nx0+250,ny0);
	line(nx0,ny0,nx0,ny0-200);
	line(nx0,ny0,nx0-100,ny0+150);
} */

void desenha_um(int cor, int coorxyz)
{
	float xe1,ye1,xe2,ye2,xe3,ye3,xe4,ye4,xe5,ye5,xe6,ye6;

	setcolor(cor);

	conv3d(po_um[1].x,po_um[1].y,po_um[1].z,&xe1,&ye1);
	conv3d(po_um[2].x,po_um[2].y,po_um[2].z,&xe2,&ye2);
	conv3d(po_um[3].x,po_um[3].y,po_um[3].z,&xe3,&ye3);
	conv3d(po_um[4].x,po_um[4].y,po_um[4].z,&xe4,&ye4);
	conv3d(po_um[5].x,po_um[5].y,po_um[5].z,&xe5,&ye5);
	conv3d(po_um[6].x,po_um[6].y,po_um[6].z,&xe6,&ye6);

	// Desenha a figura.
	line(xe1,ye1,xe2,ye2);
	line(xe3,ye3,xe2,ye2);
	line(xe3,ye3,xe4,ye4);
	line(xe4,ye4,xe4,ye4);
	line(xe1,ye1,xe5,ye5);
	line(xe4,ye4,xe5,ye5);

	if (coorxyz==1)
	{
		outtextxy(xe1,ye1,"1");
		outtextxy(xe2,ye2,"2");
		outtextxy(xe3,ye3,"3");
		outtextxy(xe4,ye4,"4");
		outtextxy(xe5,ye5,"5");
		outtextxy(xe6,ye6,"6");
	}

	// Linhas que ligam do ponto 6 para a base.
	line(xe1,ye1,xe6,ye6);
	line(xe2,ye2,xe6,ye6);
	line(xe3,ye3,xe6,ye6);
	line(xe4,ye4,xe6,ye6);
	line(xe5,ye5,xe6,ye6);
}

void desenha_dois(int cor, int coorxyz)
{
	float xe1,ye1,xe2,ye2,xe3,ye3,xe4,ye4,xe5,ye5,xe6,ye6;

	setcolor(cor);

	conv3d(po_dois[1].x,po_dois[1].y,po_dois[1].z,&xe1,&ye1);
	conv3d(po_dois[2].x,po_dois[2].y,po_dois[2].z,&xe2,&ye2);
	conv3d(po_dois[3].x,po_dois[3].y,po_dois[3].z,&xe3,&ye3);
	conv3d(po_dois[4].x,po_dois[4].y,po_dois[4].z,&xe4,&ye4);
	conv3d(po_dois[5].x,po_dois[5].y,po_dois[5].z,&xe5,&ye5);
	conv3d(po_dois[6].x,po_dois[6].y,po_dois[6].z,&xe6,&ye6);

	// Desenha a figura.
	line(xe1,ye1,xe2,ye2);
	line(xe3,ye3,xe2,ye2);
	line(xe3,ye3,xe4,ye4);
	line(xe4,ye4,xe4,ye4);
	line(xe1,ye1,xe5,ye5);
	line(xe4,ye4,xe5,ye5);

	if (coorxyz==1)
	{
		outtextxy(xe1,ye1,"7");
		outtextxy(xe2,ye2,"8");
		outtextxy(xe3,ye3,"9");
		outtextxy(xe4,ye4,"10");
		outtextxy(xe5,ye5,"11");
		outtextxy(xe6,ye6,"12");
	}

	// Linhas que ligam do ponto 6 para a base.
	line(xe1,ye1,xe6,ye6);
	line(xe2,ye2,xe6,ye6);
	line(xe3,ye3,xe6,ye6);
	line(xe4,ye4,xe6,ye6);
	line(xe5,ye5,xe6,ye6);
}

void desenha_tres(int cor, int coorxyz)
{
	float xe1,ye1,xe2,ye2,xe3,ye3,xe4,ye4,xe5,ye5,xe6,ye6;

	setcolor(cor);

	conv3d(po_tres[1].x,po_tres[1].y,po_tres[1].z,&xe1,&ye1);
	conv3d(po_tres[2].x,po_tres[2].y,po_tres[2].z,&xe2,&ye2);
	conv3d(po_tres[3].x,po_tres[3].y,po_tres[3].z,&xe3,&ye3);
	conv3d(po_tres[4].x,po_tres[4].y,po_tres[4].z,&xe4,&ye4);
	conv3d(po_tres[5].x,po_tres[5].y,po_tres[5].z,&xe5,&ye5);
	conv3d(po_tres[6].x,po_tres[6].y,po_tres[6].z,&xe6,&ye6);

	// Desenha a figura.
	line(xe1,ye1,xe2,ye2);
	line(xe3,ye3,xe2,ye2);
	line(xe3,ye3,xe4,ye4);
	line(xe4,ye4,xe4,ye4);
	line(xe1,ye1,xe5,ye5);
	line(xe4,ye4,xe5,ye5);

	if (coorxyz==1)
	{
		outtextxy(xe1,ye1,"13");
		outtextxy(xe2,ye2,"14");
		outtextxy(xe3,ye3,"15");
		outtextxy(xe4,ye4,"16");
		outtextxy(xe5,ye5,"17");
		outtextxy(xe6,ye6,"18");
	}

	// Linhas que ligam do ponto 6 para a base.
	line(xe1,ye1,xe6,ye6);
	line(xe2,ye2,xe6,ye6);
	line(xe3,ye3,xe6,ye6);
	line(xe4,ye4,xe6,ye6);
	line(xe5,ye5,xe6,ye6);
}

void desenha_quatro(int cor, int coorxyz)
{
	float xe1,ye1,xe2,ye2,xe3,ye3,xe4,ye4,xe5,ye5,xe6,ye6;

	setcolor(cor);

	conv3d(po_quatro[1].x,po_quatro[1].y,po_quatro[1].z,&xe1,&ye1);
	conv3d(po_quatro[2].x,po_quatro[2].y,po_quatro[2].z,&xe2,&ye2);
	conv3d(po_quatro[3].x,po_quatro[3].y,po_quatro[3].z,&xe3,&ye3);
	conv3d(po_quatro[4].x,po_quatro[4].y,po_quatro[4].z,&xe4,&ye4);
	conv3d(po_quatro[5].x,po_quatro[5].y,po_quatro[5].z,&xe5,&ye5);
	conv3d(po_quatro[6].x,po_quatro[6].y,po_quatro[6].z,&xe6,&ye6);

	if (coorxyz==1)
	{
		outtextxy(xe1,ye1,"19");
		outtextxy(xe2,ye2,"20");
		outtextxy(xe3,ye3,"21");
		outtextxy(xe4,ye4,"22");
		outtextxy(xe5,ye5,"23");
		outtextxy(xe6,ye6,"24");
	}

	// Desenha a figura.
	line(xe1,ye1,xe2,ye2);
	line(xe3,ye3,xe2,ye2);
	line(xe3,ye3,xe4,ye4);
	line(xe4,ye4,xe4,ye4);
	line(xe1,ye1,xe5,ye5);
	line(xe4,ye4,xe5,ye5);

	line(xe1,ye1,xe6,ye6);
	line(xe2,ye2,xe6,ye6);
	line(xe3,ye3,xe6,ye6);
	line(xe4,ye4,xe6,ye6);
	line(xe5,ye5,xe6,ye6);
}

// Funcao zoom (mais e menos).
//  1 = Aumenta a figura.
// -1 = Diminui a figura.
void zoom(int valor)
{
	float rot3d=0, xeta=0, ro1=0, rois=0, treta=0;

	for (i=0;i<=N_PONTOS;i++)
	{
		pol(po_um[i].x,po_um[i].y,&rois,&treta);
		rot3d=sqrt(pow(po_um[i].x,2)+pow(po_um[i].z,2)+pow(po_um[i].y,2));
		pol(rois,po_um[i].z,&ro1,&xeta);

		if (valor==1)
			rot3d*=1.1;
		else rot3d/=1.1;

		rois=rot3d*cos(xeta);
		po_um[i].z=rot3d*sin(xeta);
		po_um[i].y=rois*sin(treta);
		po_um[i].x=rois*cos(treta);

		pol(po_dois[i].x,po_dois[i].y,&rois,&treta);
		rot3d=sqrt(pow(po_dois[i].x,2)+pow(po_dois[i].z,2)+pow(po_dois[i].y,2));
		pol(rois,po_dois[i].z,&ro1,&xeta);

		if (valor==1)
			rot3d*=1.1;
		else rot3d/=1.1;

		rois=rot3d*cos(xeta);
		po_dois[i].z=rot3d*sin(xeta);
		po_dois[i].y=rois*sin(treta);
		po_dois[i].x=rois*cos(treta);

		pol(po_tres[i].x,po_tres[i].y,&rois,&treta);
		rot3d=sqrt(pow(po_tres[i].x,2)+pow(po_tres[i].z,2)+pow(po_tres[i].y,2));
		pol(rois,po_tres[i].z,&ro1,&xeta);

		if (valor==1)
			rot3d*=1.1;
		else rot3d/=1.1;

		rois=rot3d*cos(xeta);
		po_tres[i].z=rot3d*sin(xeta);
		po_tres[i].y=rois*sin(treta);
		po_tres[i].x=rois*cos(treta);

		pol(po_quatro[i].x,po_quatro[i].y,&rois,&treta);
		rot3d=sqrt(pow(po_quatro[i].x,2)+pow(po_quatro[i].z,2)+pow(po_quatro[i].y,2));
		pol(rois,po_quatro[i].z,&ro1,&xeta);

		if (valor==1)
			rot3d*=1.1;
		else rot3d/=1.1;

		rois=rot3d*cos(xeta);
		po_quatro[i].z=rot3d*sin(xeta);
		po_quatro[i].y=rois*sin(treta);
		po_quatro[i].x=rois*cos(treta);
	}
}

// Enquanto carrega no botao do rato escolhido nÆo efectua a operacao,
// mesmo que saia do campo do rato, tem mesmo de clicar e largar (ex: um botao).
void enquanto(int x1, int y1, int x2, int y2, int botao)
{
	// Ciclo while infinito, enquanto nao receber a instrucao "break;"
	while(1)
	{
		// Nao efectua a operacao enquanto nao largar o botao do rato,
		// enquanto nao tiver no "campo" (x1,y1,x2,y2).
		if (rato_press(0)==0 && rato_detecta(x1,y1,x2,y2)==0)
		{
			// Caso nao tenha nenhuma tecla premida e passa pelo campo,
			// nao ira efectura nenhuma operacao.
			if (rato_press(botao)==0)
				break;
		}
	}
}

void main()
{
	int gdriver=VGA, gmode=2, ch=0, i=0, maxx=0, velrot=16, knt=0, cor=WHITE, coorxyz=0;
	float rois=40, treta=0, x=0, y=0;
	// Variavel para a data do sistema.
	char *data;

	if (inicia_rato()==1)
	{
		clrscr();
		printf("Erro: Por favor instale o Rato na COM devida...\n\n");
		printf("Prima uma tecla para continuar...\n");
		getch();
		exit(1);
	}

	initgraph(&gdriver, &gmode, BGI_PATH);
	getpalette(fundo);

	barra(1,"3D Engine v1.0");
	barra(453,"I'm a lost soul in this lost world - bY Kmos in 2k+3");

	// Escreve a data na barra superior.
	getdate(&d);
	setcolor(BLACK);
	sprintf(data,"%d/%d/%d",d.da_day,d.da_mon,d.da_year);
	outtextxy(getmaxx()-65,13,data);

	fadein(fundo,1);

	// Velocidade de Rotacao.
	delta=PI/velrot;

	// Desenha as linhas laterais.
	setcolor(LIGHTGRAY);
	setlinestyle(SOLID_LINE, 1, 2);
	line(1,21,1,454);
	maxx=getmaxx();
	line(maxx,21,maxx,454);

	// Desenha o botÆo X para fechar o programa.
	setcolor(BLACK);
	rectangle(maxx-6,7,maxx-18,16);
	line(maxx-6,7,maxx-18,16);
	line(maxx-6,16,maxx-17,8);

	for (i=0;i<=N_PONTOS;i++)
	{
		rect(rois,treta,&po_um[i].x,&po_um[i].y);
		treta+=2*PI/5;
	}

	po_um[6].x=0;
	po_um[6].y=0;
	po_um[6].z=130;

	// ---

	for (i=0;i<=N_PONTOS;i++)
	{
		rect(rois,treta,&po_dois[i].x,&po_dois[i].y);
		treta+=2*PI/5;
	}

	po_dois[6].x=0;
	po_dois[6].y=0;
	po_dois[6].z=-130;

	// ---

	for (i=0;i<=N_PONTOS;i++)
	{
		rect(rois,treta,&po_tres[i].x,&po_tres[i].z);
		treta+=2*PI/5;
	}

	po_tres[6].x=0;
	po_tres[6].y=130;
	po_tres[6].z=0;

	// ---

	for (i=0;i<=N_PONTOS;i++)
	{
		rect(rois,treta,&po_quatro[i].x,&po_quatro[i].z);
		treta+=2*PI/5;
	}

	po_quatro[6].x=0;
	po_quatro[6].y=-130;
	po_quatro[6].z=0;

	setviewport(2,23,getmaxx()-2,getmaxy()-25,1);

	// Variaveis iniciais da rotacao... Direita ; Eixo dos ZZ.
	sent=1;
	eixo=3;

	do
	{
		// Botao X para Fechar o Programa.
		if (rato_detecta(616,2,626,30)==0 && rato_press(1)==0)
		{
			enquanto(616,2,626,30,1);
			fadeout(fundo,2);
			exit(1);
		}

		// Botao + Zoom (Aumenta a Figura).
		if (rato_detecta(5,633,64,690)==0 && rato_press(1)==0)
		{
			botao_clk(5,250,67,270,"&+ Zoom");
			mostra_rato();
			botao(5,250,67,270,"&+ Zoom");

			// Limite no zoom.
			if (knt < 5)
			{
				knt++;
				zoom(1);
			}
		}

		// Botao - Zoom (Diminui a Figura).
		if (rato_detecta(71,633,135,690)==0 && rato_press(1)==0)
		{
			botao_clk(70,250,132,270,"&- Zoom");
			mostra_rato();
			botao(70,250,132,270,"&- Zoom");

			// Limite no zoom.
			if (knt != -10)
			{
				knt--;
				zoom(-1);
			}
		}

		// Botao Direita (Roda a Figura para a Direita).
		if (rato_detecta(7,691,64,750)==0 && rato_press(1)==0)
		{
			botao_clk(5,275,67,295,"Dire&ita");
			mostra_rato();
			botao(5,275,67,295,"Dire&ita");
			sent=1;
		}

		// Botao Esquerda (Roda a Figura para a Esquerda).
		if (rato_detecta(71,691,135,750)==0 && rato_press(1)==0)
		{
			botao_clk(70,275,132,295,"Esqu&erda");
			mostra_rato();
			botao(70,275,132,295,"Esqu&erda");
			sent=-1;
		}

		// Botao Todos os Eixos.
		if (rato_detecta(5,766,64,815)==0 && rato_press(1)==0)
		{
			botao_clk(5,300,67,320,"T&ds Eixos");
			mostra_rato();
			botao(5,300,67,320,"T&ds Eixos");
			eixo=0;
		}

		// Botao Roda Eixo XX.
		if (rato_detecta(70,766,130,815)==0 && rato_press(1)==0)
		{
			botao_clk(70,300,132,320,"Roda &X");
			mostra_rato();
			botao(70,300,132,320,"Roda &X");
			eixo=1;
		}

		// Botao Roda Eixo YY.
		if (rato_detecta(7,823,65,881)==0 && rato_press(1)==0)
		{
			botao_clk(10,325,65,345,"Roda &Y");
			mostra_rato();
			botao(10,325,65,345,"Roda &Y");
			eixo=2;
		}

		// Botao Roda Eixo ZZ.
		if (rato_detecta(70,823,129,881)==0 && rato_press(1)==0)
		{
			botao_clk(70,325,130,345,"Roda &Z");
			mostra_rato();
			botao(70,325,130,345,"Roda &Z");
			eixo=3;
		}

		// Botao Mais Rapido (Aumenta a Velocidade de Rotacao).
		if (rato_detecta(7,884,70,935)==0 && rato_press(1)==0)
		{
			botao_clk(5,350,67,370,"+ Ra&pido");
			mostra_rato();
			botao(5,350,67,370,"+ Ra&pido");
			velrot--;
			if (velrot < 10)
				velrot=10;
		}

		// Botao Menos Rapido (Diminui a Velocidade de Rotacao).
		if (rato_detecta(70,881,129,938)==0 && rato_press(1)==0)
		{
			botao_clk(70,350,135,370,"- R&apido");
			mostra_rato();
			botao(70,350,135,370,"- R&apido");
			velrot++;
			if (velrot > 50)
				velrot=50;
		}

		// Botao Rel¢gio (Aparece um rel¢gio anal¢gico).
		if (rato_detecta(7,940,63,996)==0 && rato_press(1)==0)
		{
			botao_clk(10,375,65,395,"&Relogio");
			mostra_rato();
			botao(10,375,65,395,"&Relogio");
			relogio_anal();
		}

		// Botao Muda Cor (Muda a Cor da Figura).
		if (rato_detecta(70,940,127,996)==0 && rato_press(1)==0)
		{
			botao_clk(70,375,130,395,"&Muda Cor");
			mostra_rato();
			botao(70,375,130,395,"&Muda Cor");
			cor=cores(rand()%14);
		}

		// Botao Parar (Para a Rotacao da Figura).
		if (rato_detecta(7,1010,63,1054)==0 && rato_press(1)==0)
		{
			botao_clk(7,400,65,420,"Pa&usar");
			mostra_rato();
			botao(7,400,65,420,"Pa&usar");
			sent=0;
		}

		// Botao Sair (Sair do Programa).
		if (rato_detecta(69,1010,128,1054)==0 && rato_press(1)==0)
		{
			botao_clk(70,400,130,420,"&Sair");
			mostra_rato();
			botao(70,400,130,420,"&Sair");
			fadeout(fundo,2);
			exit(1);
		}

		// Velocidade de Rotacao.
		delta=PI/velrot;
		mostra_rato();

		// Botoes.
		botao(5,250,67,270,"&+ Zoom");
		botao(70,250,132,270,"&- Zoom");
		botao(5,275,67,295,"Dire&ita");
		botao(70,275,132,295,"Esqu&erda");
		botao(5,300,67,320,"T&ds Eixos");
		botao(70,300,132,320,"Roda &X");
		botao(10,325,65,345,"Roda &Y");
		botao(70,325,130,345,"Roda &Z");
		botao(5,350,67,370,"+ Ra&pido");
		botao(70,350,135,370,"- R&apido");
		botao(10,375,65,395,"&Relogio");
		botao(70,375,130,395,"&Muda Cor");
		botao(7,400,65,420,"Pa&usar");
		botao(70,400,130,420,"&Sair");

		while (kbhit())
		{
			// Busca a tecla.
			ch=getch();

			// Seta para a Direita.
			if (ch==DIREITA)
			{
				if (nx0 >= getmaxx()-2)
					nx0=getmaxx()-2;
				else nx0+=3;
			}

			// Seta para a Esquerda.
			if (ch==ESQUERDA)
			{
				if (nx0 <= 2)
					nx0=2;
				else nx0-=3;
			}

			// Seta para Cima.
			if (ch==CIMA)
			{
				if (ny0 <= 2)
					ny0=2;
				else ny0-=3;
			}

			// Seta para Baixo.
			if (ch==BAIXO)
			{
				if (ny0 >= getmaxy()-50)
					ny0=getmaxy()-50;
				else ny0+=3;
			}

			// Rotacao para a Direita.
			if (ch==SETA_DIREITA)
			{
				botao_clk(5,275,67,295,"Dire&ita");
				mostra_rato();
				botao(5,275,67,295,"Dire&ita");
				sent=1;
			}

			// Rotacao para a Esquerda.
			if (ch==SETA_ESQUERDA)
			{
				botao_clk(70,275,132,295,"Esqu&erda");
				mostra_rato();
				botao(70,275,132,295,"Esqu&erda");
				sent=-1;
			}

			// Baixo para parar a Rotacao.
			if (ch==SETA_BAIXO)
			{
				botao_clk(7,400,65,420,"Pa&usar");
				mostra_rato();
				botao(7,400,65,420,"Pa&usar");
				sent=0;
			}

			// Zoom (mais).
			if (ch==TECLA_MAIS)
			{
				botao_clk(5,250,67,270,"&+ Zoom");
				mostra_rato();
				botao(5,250,67,270,"&+ Zoom");

				// Limite no zoom.
				if (knt < 5)
				{
					knt++;
					zoom(1);
				}
			}

			// Zoom (menos).
			if (ch==TECLA_MENOS)
			{
				botao_clk(70,250,132,270,"&- Zoom");
				mostra_rato();
				botao(70,250,132,270,"&- Zoom");

				// Limite no zoom.
				if (knt != -10)
				{
					knt--;
					zoom(-1);
				}
			}

			// Aumenta a velocidade de rotacao.
			if (ch==TECLA_P)
			{
				botao_clk(5,350,67,370,"+ Ra&pido");
				mostra_rato();
				botao(5,350,67,370,"+ Ra&pido");
				velrot--;
				if (velrot < 10)
					velrot=10;
			}

			// Diminui a velocidade de rotacao.
			if (ch==TECLA_A)
			{
				botao_clk(70,350,135,370,"- R&apido");
				mostra_rato();
				botao(70,350,135,370,"- R&apido");
				velrot++;
				if (velrot > 50)
					velrot=50;
			}

			// Roda em Todos os eixos;
			if (ch==TECLA_D)
			{
				botao_clk(5,300,67,320,"T&ds Eixos");
				mostra_rato();
				botao(5,300,67,320,"T&ds Eixos");
				eixo=0;
			}

			// Roda no eixo dos XX.
			if (ch==TECLA_X)
			{
				botao_clk(70,300,132,320,"Roda &X");
				mostra_rato();
				botao(70,300,132,320,"Roda &X");
				eixo=1;
			}

			// Roda no eixo dos YY.
			if (ch==TECLA_Y)
			{
				botao_clk(10,325,65,345,"Roda &Y");
				mostra_rato();
				botao(10,325,65,345,"Roda &Y");
				eixo=2;
			}

			// Roda no eixo dos ZZ.
			if (ch==TECLA_Z)
			{
				botao_clk(70,325,130,345,"Roda &Z");
				mostra_rato();
				botao(70,325,130,345,"Roda &Z");
				eixo=3;
			}

			// Relogio Analogico.
			if (ch==TECLA_R)
			{
				botao_clk(10,375,65,395,"&Relogio");
				mostra_rato();
				botao(10,375,65,395,"&Relogio");
				relogio_anal();
			}

			// Mudar a Cor da Figura.
			if (ch==TECLA_M)
			{
				botao_clk(70,375,130,395,"&Muda Cor");
				mostra_rato();
				botao(70,375,130,395,"&Muda Cor");
				cor=cores(rand()%14);
			}

			// Mostra os Numeros dos Pontos da Figura.
			if (ch==TECLA_N)
			{
				if (coorxyz==1)
					coorxyz=0;
				else coorxyz=1;
			}

			if (ch==TECLA_S)
			{
				botao_clk(70,400,130,420,"&Sair");
				mostra_rato();
				botao(70,400,130,420,"&Sair");
				fadeout(fundo,2);
				exit(1);
			}
		}

		// Roda no Eixo dos XX.
		if (eixo==1 || eixo==0)
		{
			for (i=0;i<=N_PONTOS;i++)
			{
				roda(po_um[i].z,po_um[i].y,&po_um[i].z,&po_um[i].y);
				roda(po_dois[i].z,po_dois[i].y,&po_dois[i].z,&po_dois[i].y);
				roda(po_tres[i].z,po_tres[i].y,&po_tres[i].z,&po_tres[i].y);
				roda(po_quatro[i].z,po_quatro[i].y,&po_quatro[i].z,&po_quatro[i].y);
			}
		}

		// Roda no Eixo dos YY.
		if (eixo==2 || eixo==0)
		{
			for (i=0;i<=N_PONTOS;i++)
			{
				roda(po_um[i].z,po_um[i].x,&po_um[i].z,&po_um[i].x);
				roda(po_dois[i].z,po_dois[i].x,&po_dois[i].z,&po_dois[i].x);
				roda(po_tres[i].z,po_tres[i].x,&po_tres[i].z,&po_tres[i].x);
				roda(po_quatro[i].z,po_quatro[i].x,&po_quatro[i].z,&po_quatro[i].x);
			}
		}

		// Roda no Eixo dos ZZ.
		if (eixo==3 || eixo==0)
		{
			for (i=0;i<=N_PONTOS;i++)
			{
				roda(po_um[i].x,po_um[i].y,&po_um[i].x,&po_um[i].y);
				roda(po_dois[i].x,po_dois[i].y,&po_dois[i].x,&po_dois[i].y);
				roda(po_tres[i].x,po_tres[i].y,&po_tres[i].x,&po_tres[i].y);
				roda(po_quatro[i].x,po_quatro[i].y,&po_quatro[i].x,&po_quatro[i].y);
			}
		}

		// Coordenadas do Rato (X e Y).
		rato_xy(x,y);
		gotoxy(70,24);
		printf("Rato X: %.0f",x);
		gotoxy(70,25);
		printf("Rato Y: %.0f",y);

		// Desenha e Apaga as Figuras.
		desenha_um(cor,coorxyz);
		desenha_dois(cor,coorxyz);
		desenha_tres(cor,coorxyz);
		desenha_quatro(cor,coorxyz);
		delay(20);
		esconde_rato();
		desenha_um(0,coorxyz);
		desenha_dois(0,coorxyz);
		desenha_tres(0,coorxyz);
		desenha_quatro(0,coorxyz);
		mostra_rato();
	} while (ch!=TECLA_ESC);

	botao_clk(70,400,130,420,"&Sair");
	mostra_rato();
	botao(70,400,130,420,"&Sair");

	fadeout(fundo,2);
	clearviewport();
	closegraph();
}
