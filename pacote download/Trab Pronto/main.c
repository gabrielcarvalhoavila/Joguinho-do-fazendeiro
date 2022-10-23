/* Trabalho final Milipede
Nomes: Gabriel Carvalho D'ávila (341600) e Walter Frank (123721)

Requisitos faltantes:
item 17: Avanço de fase; fase 2 com mais inimigos, cogumelos e milípede maior

*/

#include <raylib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

//ELEMENTOS GRÁFICOS
#define LARGURA 1250
#define ALTURA 800
#define ALTURA_TOTAL 880
#define COR_FUNDO BLACK
#define TAM_TITULO 50
#define TAM_SUBTITULO 30
#define TAM_LETRA 20
#define TAM_NOME_MAX 16

//ELEMENTOS GRÁFICOS DO MENU
#define COR_MENU DARKPURPLE
#define TAM_ITENS_MENU 50
#define ESPACAMENTO 5
#define ALTURA_CABECALHO 60
#define POSX_VIDAS 45
#define POSX_TIROS 225
#define POSX_COG_COLHIDOS 450
#define NUM_JOGADORES 30
#define VEL_ANIMACAO 30

//PARÂMETROS DE RENDERIZAÇÃO
#define FPS 60

//PARÂMETROS DO JOGO
#define NUM_COGUMELOS 60
#define TAM_COGUMELOS 15

#define NUM_ARANHAS 2
#define TAM_ARANHA 20
#define VEL_ARANHA 4

#define TAM_FAZENDEIRO 30
#define VEL_FAZENDEIRO 3
#define VIDAS 3
#define COOLDOWN_DANO 60

#define TIROS 200
#define TAM_TIRO_ALTURA 10
#define TAM_TIRO_LARGURA 5
#define COOLDOWN_TIRO 30
#define VEL_TIRO 4

#define VEL_MILIPEDE 3
#define LARGURA_RABO_MILIPEDE 40
#define LARGURA_SEGMENTO_MILIPEDE 20
#define LARGURA_MILIPEDE 60
#define ALTURA_MILIPEDE 88

typedef struct
{
    int x;
    int y;
    int status; // ainda não foi comido (0) foi comido (1)
} COGUMELO ;

typedef enum
{
    esq,
    dir,
    cima,
    baixo,
    dir_cima,
    dir_baixo,
    esq_cima,
    esq_baixo,
    null
} DIRECAO ;

typedef struct
{
    char nome[60];
    int x;
    int y;
    int vidas;
    int tiros;
    int cogCome;
    int cogColhe;
    int doente;//qtos cogumelos para curar, zero se estiver sao
    DIRECAO dirTiro;
    int status; // Livre, Paralisado, ou Morto. Pode definir enumeracao
    //0 == livre, 1 == paralisado, 2 == morto
    int pontuacao;

} FAZENDEIRO;

//Funções da interface gráfica do jogo

void drawTextoXY(char texto[], int posX, int posY, int tamanho, Color cor);

void drawCabecalho() //desenha as informações do cabecalho
{
    drawTextoXY("[ESC]: SAIR",100,ALTURA_CABECALHO/2,TAM_LETRA,COR_MENU);
    drawTextoXY("[C]: CARREGAR SALVO",300,ALTURA_CABECALHO/2,TAM_LETRA,COR_MENU);
    drawTextoXY("[P]: PAUSA/RETOMA",600,ALTURA_CABECALHO/2,TAM_LETRA,COR_MENU);
    drawTextoXY("[R]: RANKING",800,ALTURA_CABECALHO/2,TAM_LETRA,COR_MENU);
}

void drawCaixaXY(int posX, int posY, int largura, int altura, Color cor) // desenha uma caixa preta com contornos da cor COR_MENU centrada nas coordenadas x e y
{
    DrawRectangle(posX - largura/2, posY - altura/2, largura, altura, cor);
    DrawRectangle(posX - largura/2 + ESPACAMENTO, posY - altura/2 + ESPACAMENTO, largura - 2 * ESPACAMENTO, altura - 2 * ESPACAMENTO, COR_FUNDO);
}

void drawTexturaXY(Texture2D img, int resolucaoX, int resolucaoY, int posX, int posY, int tamanho, Color cor) // versão modifcada de drawtexture(), desenha a textura centrada em x e y
{
    Vector2 origem;
    origem.x = 0;
    origem.y = 0;
    Rectangle fonte;
    Rectangle destino;

    fonte.x = 0;
    fonte.y = 0;
    fonte.width = resolucaoX;
    fonte.height = resolucaoY;

    destino.x = posX - tamanho/2;
    destino.y = posY - tamanho/2;
    destino.height = tamanho;
    destino.width = tamanho;
    DrawTexturePro(img,fonte,destino,origem,0,cor);
}

void drawTextoXY(char texto[], int posX, int posY, int tamanho, Color cor) //versão midificada de drawtext(), escreve o texto com centro em x y
{
    DrawText(texto, posX - MeasureText(texto,tamanho)/2, posY - tamanho/2, tamanho, cor);
}

void telaDeAviso(char mensagem[], Color cor) //abre um menu com uma mensagem no centro, soilica [ESC] para sair
{
    while(!WindowShouldClose())
    {
        BeginDrawing();
        DrawRectangle(0,0,LARGURA,ALTURA_TOTAL,COR_MENU);
        DrawRectangle(ESPACAMENTO,ESPACAMENTO,LARGURA -  2 * ESPACAMENTO, ALTURA_TOTAL - 2 * ESPACAMENTO, COR_FUNDO);
        drawTextoXY(mensagem, LARGURA/2,ALTURA/2,TAM_TITULO,cor);
        drawTextoXY("Aperte [ESC] para voltar:", LARGURA/2,ALTURA,TAM_SUBTITULO,COR_MENU);
        EndDrawing();
    }
}

void drawContornos() //contornos do menu, cabeçalho e cenário
{
    DrawRectangle(0,0,LARGURA,ALTURA_CABECALHO,COR_MENU); //contorno do menu
    DrawRectangle(ESPACAMENTO, ESPACAMENTO, LARGURA - 2 * ESPACAMENTO, ALTURA_CABECALHO - 2 * ESPACAMENTO, COR_FUNDO); //fundo do menu
    DrawRectangle(0, ALTURA, LARGURA, ALTURA_TOTAL - ALTURA, COR_MENU); // contorno do menu
    DrawRectangle(ESPACAMENTO, ALTURA + ESPACAMENTO, LARGURA - 2 * ESPACAMENTO, ALTURA_TOTAL - ALTURA - 2 * ESPACAMENTO, COR_FUNDO); // fundo do menu
    DrawRectangle(0,ALTURA_CABECALHO,ESPACAMENTO,ALTURA,COR_MENU); // borda esquerda
    DrawRectangle(LARGURA - ESPACAMENTO,ALTURA_CABECALHO,ESPACAMENTO,ALTURA,COR_MENU); // borda direita
}

void drawVidasMenu (int vidas, Texture2D imgCoracao) //desenha de 1 até 3 ícones de coração no menu, dependendo da quantidade de vidas
{
    int i;
    for(i = 0; i < vidas; i++) //vidas
    {
        drawTexturaXY(imgCoracao, 256 ,256 , POSX_VIDAS + (TAM_ITENS_MENU + ESPACAMENTO) * i, ALTURA_TOTAL - (ALTURA_TOTAL - ALTURA)/2, TAM_ITENS_MENU, WHITE);
    }
}

void drawTirosMenu (int tiros, Texture2D imgTiro) //desenha o ícone de tiro e o número de tiros restantes
{
    char numTiros[3];
    drawTexturaXY(imgTiro,50,50,POSX_TIROS,ALTURA + (ALTURA_TOTAL - ALTURA)/2,TAM_ITENS_MENU, WHITE);
    DrawText(itoa(tiros, numTiros, 10), POSX_TIROS + TAM_ITENS_MENU + ESPACAMENTO, ALTURA + ESPACAMENTO * 4, TAM_ITENS_MENU, YELLOW);
}

void drawCogumelosMenu(int colhidos, Texture2D imgCogumelo) //desenha o ícone de cogumelo e o número de cogumelos colhidos
{
    char numerocogumelos[3];
    drawTexturaXY(imgCogumelo,50,50,POSX_COG_COLHIDOS,ALTURA + (ALTURA_TOTAL - ALTURA)/2,TAM_ITENS_MENU, WHITE);
    DrawText(itoa(colhidos, numerocogumelos, 10), POSX_COG_COLHIDOS + TAM_ITENS_MENU, ALTURA + ESPACAMENTO * 4, TAM_ITENS_MENU, BROWN);
}

void drawMenu(FAZENDEIRO faz1, Texture2D imgCoracao, Texture2D imgTiro, Texture2D imgCogumelo) //desenha o menu com as informações do fazendeiro
{
    //ITENS DO MENU
    drawVidasMenu(faz1.vidas,imgCoracao);
    drawTirosMenu(faz1.tiros,imgTiro);
    drawCogumelosMenu(faz1.cogColhe, imgCogumelo);
}

//Funções auxiliares de movimentação dos personagens

int colideBorda(int x1, int y1) // verifica se um objeto colidiu com os limites do cenário
{
    if (x1 >= LARGURA - ESPACAMENTO)
    {
        return 1;
    }
    else if (x1 <= 0 + ESPACAMENTO)
    {
        return 1;
    }
    else if (y1 >= ALTURA)
    {
        return 1;
    }
    else if(y1 <= 0)
    {
        return 1;
    }
    else
        return 0;
}

//Funções fazendeiro

void drawFazendeiro(FAZENDEIRO faz1, Texture2D imgFaz) //desenha o fazendeiro
{
    if(faz1.status == 1)
        drawTexturaXY(imgFaz,50,50,faz1.x,faz1.y,TAM_FAZENDEIRO*2,RED);
    else
        drawTexturaXY(imgFaz,50,50,faz1.x,faz1.y,TAM_FAZENDEIRO*2,WHITE);
}

void geraFazendeiro(FAZENDEIRO *faz1) //inicializa o fazendeiro com os parâmetros do jogo
{
    strcpy (faz1->nome, "Anonimo");
    faz1->x = LARGURA/2;
    faz1->y = ALTURA - ALTURA_CABECALHO;
    faz1->tiros = TIROS;
    faz1->cogColhe = 0;
    faz1->cogCome = 0;
    faz1->vidas = VIDAS;
    faz1->status = 0; // (0 = normal; 1 = doente, precisa comer cogumelos; 2 = morto)
    faz1->pontuacao = 0;

}

void moveFazendeiro(FAZENDEIRO *faz1) // lê a tecla pressionada pelo usuário e move o personagem (sem passar dos limites do cenário)
{
    if(faz1->status == 0)
    {
        if (IsKeyDown(KEY_RIGHT))
        {
            if(faz1->x < LARGURA - TAM_FAZENDEIRO - ESPACAMENTO)
                faz1->x += VEL_FAZENDEIRO;
        }
        if (IsKeyDown(KEY_LEFT))
        {
            if(faz1->x > 0 + TAM_FAZENDEIRO + ESPACAMENTO)
                faz1->x -= VEL_FAZENDEIRO;
        }
        if (IsKeyDown(KEY_UP))
        {
            if(faz1->y > (ALTURA) * 0.75 + TAM_FAZENDEIRO)
                faz1->y -= VEL_FAZENDEIRO;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            if(faz1->y < ALTURA - TAM_FAZENDEIRO)
                faz1->y += VEL_FAZENDEIRO;
        }
    }
}

void atualizaFazendeiro(FAZENDEIRO *faz1, int *cooldown) //controla o tempo durante o qual o fazendeiro fica paralisado ao ser atingido
{
    if(faz1->status == 1) //se o fazendneiro estiver ferido
    {
        *cooldown = *cooldown + 1; //começa o cooldown
        if (*cooldown >= COOLDOWN_DANO) //quando o cooldown terminar
        {
            faz1->status = 0; //volta a estar curado
            *cooldown = 0;
        }
    }
}

//Funções aranhas

typedef struct
{
    int x;
    int y;
    int dir;
    int status; // 0 - Oculta, 1 - No cenário, 2 - Morta

} ARANHA;

void reEnquadraAranha(ARANHA *aranha1) //recebe uma estrutura ARANHA e retorna ela para dentro do cenário se estiver fora
{
    if(aranha1->x > LARGURA)
        aranha1->x = LARGURA;
    if(aranha1->y > ALTURA)
        aranha1->y = ALTURA;
    if(aranha1->x < 0)
        aranha1->x = 0;
    if(aranha1->y < TAM_ARANHA)
        aranha1->y = TAM_ARANHA;
}

void moveAranha(ARANHA *aranha, int velocidade) //atualiza as coordenadas de uma estrutura ARANHA com base na direção
{
    if(aranha->status == 2)
        aranha->y -= velocidade;
    else
    switch (aranha->dir)
    {
    case 0:
        aranha->y -= velocidade;
        break;
    case 1:
        aranha->x += velocidade;
        aranha->y -= velocidade;
        break;
    case 2:
        aranha->x += velocidade;
        break;
    case 3:
        aranha->x += velocidade;
        aranha->y += velocidade;
        break;
    case 4:
        aranha->y += velocidade;
        break;
    case 5:
        aranha->x -= velocidade;
        aranha->y += velocidade;
        break;
    case 6:
        aranha->x -= velocidade;
        break;
    case 7:
        aranha->x -= velocidade;
        aranha->y += velocidade;
        break;
    default:
        aranha->y += velocidade;
        break;
    }
}

void inverteAranha(ARANHA *aranha) //inverte a direção da estrutura ARANHA (sempre para baixo ou horizontalmente)
{
    if(aranha->status == 2)
        aranha->dir = 0;
    else
    switch (aranha->dir)
    {
    case 0:
        aranha->dir = GetRandomValue(3,5);
        break;
    case 1:
        aranha->dir = GetRandomValue(4,5);
        break;
    case 2:
        aranha->dir = GetRandomValue(4,5);
        break;
    case 3:
        aranha->dir = GetRandomValue(4,5);
        break;
    case 4:
        aranha->dir = GetRandomValue(5,3);
        break;
    case 5:
        aranha->dir = GetRandomValue(3,4);
        break;
    case 6:
        aranha->dir = GetRandomValue(3,4);
        break;
    case 7:
        aranha->dir = GetRandomValue(4,5);
        break;
    default:
        aranha->dir = 4;
        break;
    }
}

void desenhaAranhas(ARANHA aranhas[], Texture2D imgAranha) //desenha todas as aranhas de um vetor
{
    int i;
    for(i = 0; i < NUM_ARANHAS; i++)
    {
        DrawRectangle(aranhas[i].x, aranhas[i].y - ALTURA, 1, ALTURA, WHITE);
        if(aranhas[i].status == 2)
            drawTexturaXY(imgAranha,50,50,aranhas[i].x,aranhas[i].y,TAM_ARANHA * 2, RED);
        else
            drawTexturaXY(imgAranha,50,50,aranhas[i].x,aranhas[i].y,TAM_ARANHA * 2, WHITE);
    }
}

void geraAranha(ARANHA *aranha1) //inicializa uma aranha com posições e direções aleatórias (sempre com coordenada x no topo do cenário)
{
    aranha1->status = 1;
    aranha1->x = GetRandomValue(0,LARGURA);
    aranha1->y = TAM_ARANHA;
    aranha1->dir = GetRandomValue(0,7);
}

void geraAranhas(ARANHA aranhas[NUM_ARANHAS]) //incializa todas as aranhas de um vetor com posições e direções aleatórias
{
    int i = 0;
    for(i = 0; i < NUM_ARANHAS; i++)
    {
        aranhas[i].x = GetRandomValue(0,LARGURA) + TAM_ARANHA * i;
        aranhas[i].y = ALTURA_CABECALHO + TAM_ARANHA + TAM_ARANHA * i;
        aranhas[i].dir = GetRandomValue(0,7);
    }
}

void moveAranhas(ARANHA aranhas[NUM_ARANHAS], int velocidade) //atualiza as posições de todas as aranhas do vetor
{
    int i;
    for(i = 0; i < NUM_ARANHAS; i++)
    {
        if(colideBorda(aranhas[i].x,aranhas[i].y)) //se a aranha bate na borda, inverte a direção do movimento
        {
            if (aranhas[i].y < ALTURA_CABECALHO) // se a borda com a qual a aranha colidiu foi a borda superior, gera a aranha novamente
                geraAranha(&aranhas[i]);
            aranhas[i].status = 1;
            inverteAranha(&aranhas[i]);
            reEnquadraAranha(&aranhas[i]);
        }
        moveAranha(&aranhas[i], velocidade);
        if(aranhas[i].y >= ALTURA - TAM_ARANHA) //se a aranha chega ao fundo do cenário, re-aparece no topo
            geraAranha(&aranhas[i]);
    }
}

//Funções milipede

typedef struct
{
    int x;
    int y;
    int tamanho; // numero de segmentos
    DIRECAO dir; //usa valores da enumeração ESQ ou DIR ou BAIXO
    int status; // pode estar viva (1) ou morta (0)
} MILIPEDE ;

void GeraMilipede (MILIPEDE *milipede) //inicializa uma milipede no topo do cenário com tamanho entre 4 e 10
{
    milipede->y = ALTURA_CABECALHO; // posição y, parte de cima do cenário
    milipede->x = (rand()%LARGURA - 2*ESPACAMENTO);// posição x
    milipede->dir = (GetRandomValue(esq, dir));
    milipede->tamanho = GetRandomValue(4, 10);
    milipede->status = 1;
}

void DesenhaMilipede(MILIPEDE milipede, Texture2D mili_direita, Texture2D mili_esquerda)
{
    int i;
    Vector2 coord; // coordenadas usadas no desenho da cabeça
    coord.x = milipede.x;
    coord.y = milipede.y;

    Vector2 coord_segmentos; // coordenadas usadas no desenho dos segmentos e do rabo
    coord_segmentos.x = milipede.x;
    coord_segmentos.y = milipede.y;

    Rectangle cabeca_direita; // retangulos criados para recortar a img da milipede (virada p esquerda ou direita) pra poderem ser usados na função DrawTextureRec abaixo
    cabeca_direita.height = ALTURA_MILIPEDE;
    cabeca_direita.width = LARGURA_MILIPEDE;
    cabeca_direita.x = 102;
    cabeca_direita.y = 0;

    Rectangle cabeca_esquerda;
    cabeca_esquerda.height = ALTURA_MILIPEDE;
    cabeca_esquerda.width = LARGURA_MILIPEDE;
    cabeca_esquerda.x = 0;
    cabeca_esquerda.y = 0;

    Rectangle segmentos_direita;
    segmentos_direita.height = ALTURA_MILIPEDE;
    segmentos_direita.width = LARGURA_SEGMENTO_MILIPEDE;
    segmentos_direita.x = LARGURA_RABO_MILIPEDE;
    segmentos_direita.y = 0;

    Rectangle segmentos_esquerda;
    segmentos_esquerda.height = ALTURA_MILIPEDE;
    segmentos_esquerda.width = LARGURA_SEGMENTO_MILIPEDE;
    segmentos_esquerda.x = LARGURA_MILIPEDE;
    segmentos_esquerda.y = 0;

    Rectangle rabo_direita;
    rabo_direita.height = ALTURA_MILIPEDE;
    rabo_direita.width = LARGURA_RABO_MILIPEDE;
    rabo_direita.x = 0;
    rabo_direita.y = 0;

    Rectangle rabo_esquerda;
    rabo_esquerda.height = ALTURA_MILIPEDE;
    rabo_esquerda.width = LARGURA_RABO_MILIPEDE;
    rabo_esquerda.x = 122;
    rabo_esquerda.y = 0;

    if (milipede.dir == dir)
        DrawTextureRec(mili_direita, cabeca_direita, coord,WHITE); // desenha a cabeça

    if (milipede.dir == esq)
        DrawTextureRec(mili_esquerda, cabeca_esquerda, coord,WHITE); // desenha a cabeça


    for (i = 0; i < milipede.tamanho; i++) // laço p desenhar os segmentos
    {


        if (milipede.dir == dir) // Se a milipede estiver indo p direita
            {
                coord_segmentos.x = milipede.x - LARGURA_SEGMENTO_MILIPEDE*i - LARGURA_SEGMENTO_MILIPEDE;
                DrawTextureRec(mili_direita, segmentos_direita, coord_segmentos,WHITE);

            }

        if (milipede.dir == esq) // Se a milipede estiver indo p esquerda
            {
                coord_segmentos.x = milipede.x + LARGURA_SEGMENTO_MILIPEDE*i + LARGURA_MILIPEDE;                                                ;
                DrawTextureRec(mili_esquerda, segmentos_esquerda, coord_segmentos,WHITE);

            }

    }

    if (milipede.dir == dir) // desenha o rabo
        {
        coord_segmentos.x = milipede.x - LARGURA_SEGMENTO_MILIPEDE*i -LARGURA_SEGMENTO_MILIPEDE;
        DrawTextureRec(mili_direita, rabo_direita, coord_segmentos,WHITE);
        }

    if (milipede.dir == esq) // desenha o rabo
        {
        coord_segmentos.x = milipede.x + LARGURA_SEGMENTO_MILIPEDE*i + 2*LARGURA_SEGMENTO_MILIPEDE;
        DrawTextureRec(mili_esquerda, rabo_esquerda, coord_segmentos,WHITE);
        }

}

void Move_Milipede (MILIPEDE *milipede)
{
    int prox_direcao;

    if (milipede->dir == dir)
    {
        milipede->x += VEL_MILIPEDE;
        if (milipede->x >= LARGURA - LARGURA_MILIPEDE)
        {
            milipede->dir = baixo;
            prox_direcao = esq;
        }
    }
    if (milipede->dir == esq)
    {
        milipede->x -= VEL_MILIPEDE;
        if (milipede->x <= ESPACAMENTO)
        {
            milipede->dir = baixo;
            prox_direcao = dir;
        }
    }
    if (milipede->dir == baixo)
    {

        milipede->y += 5*VEL_MILIPEDE;
        milipede->dir = prox_direcao;
    }


}

//Funções cogumelos

void desenhaCogumelo (COGUMELO cogu, Texture2D imgCogumelo)
{
    drawTexturaXY(imgCogumelo,50,50,cogu.x,cogu.y,TAM_COGUMELOS * 2, WHITE);
}

void desenhaCogumelos(COGUMELO cogu[], Texture2D imgCogumelo) //desenha todos os cogumelos em um vetor
{
    int i;
    for(i = 0; i < NUM_COGUMELOS; i++)
    {
        if(cogu[i].status == 0)
            desenhaCogumelo(cogu[i],imgCogumelo);
    }
}

void geraCogumelos(COGUMELO cogu[]) //incializa todos os cogumelos aleatoriamente
{
    srand(time(0));
    int i = 0;
    for(i = 0; i < NUM_COGUMELOS; i++)
    {
        cogu[i].x = GetRandomValue(TAM_COGUMELOS, LARGURA - TAM_COGUMELOS);
        cogu[i].y = GetRandomValue (ALTURA_CABECALHO + ALTURA_MILIPEDE*1.5, ALTURA*0.75);
        cogu[i].status = 0;
    }
}

//Funções tiros

typedef struct
{
    int x;
    int y;
    int status; // não colidiu (0) ou colidiu (1)
    DIRECAO direcao;

} TIRO ;

void desenhaTiro (TIRO tiro) //representação gráfico do tiro
{
    if (tiro.status == 0)
        DrawRectangle(tiro.x, tiro.y, TAM_TIRO_LARGURA, TAM_TIRO_ALTURA, WHITE);
}

void animacaoTiro (TIRO *tiro) //move o tiro pelo cenário
{
    if (tiro->direcao == cima)
        tiro->y -= 5*VEL_TIRO;

    if (tiro->direcao == dir_cima)
    {
        tiro->x += VEL_TIRO;
        tiro->y -= 5*VEL_TIRO;
    }
    if (tiro->direcao == esq_cima)
    {
        tiro->x -= VEL_TIRO;
        tiro->y -= 5*VEL_TIRO;
    }
}

void disparaTiro(FAZENDEIRO *faz1, TIRO *tiro1, int *contador) //inicializa um tiro a partir do fazendeiro
{
    if (IsKeyDown(KEY_SPACE))
    {
        if(*contador >= COOLDOWN_TIRO && faz1->status == 0) // se o cooldown já tiver passado e se não estiver paralisado
        {
            tiro1->x = faz1->x; // define as coordenadas inciais do tiro a partir das coordenadas do fazendeiro
            tiro1->y = faz1->y;
            tiro1->status = 0; //ativa o tivo
            faz1->tiros--; // decrementa os tiros restantes do fazendeiro

            if(IsKeyDown(KEY_LEFT))
                tiro1->direcao = esq_cima;
            else if(IsKeyDown(KEY_RIGHT))
                tiro1->direcao = dir_cima;
            else
                tiro1->direcao = cima;
            *contador = 0; // reinicia o cooldown

        }
    }
}

//Estruturas de coordenadas, guarda o ponto superior esquerdo e o ponto inferior direito

typedef struct
{
    int x1;
    int x2;
    int y1;
    int y2;

} COORD_EXTREMIDADES;

typedef struct
{
    COORD_EXTREMIDADES aranha[NUM_ARANHAS];
    COORD_EXTREMIDADES fazendeiro;
    COORD_EXTREMIDADES cogu[NUM_COGUMELOS];
    COORD_EXTREMIDADES milip;
    COORD_EXTREMIDADES tiro;

} ENVELOPE;

//Funções de colisão

void colideAranhaCogumelo (ARANHA aranhas[], COGUMELO cogumelos[], int *quant_cogumelos) //verifica colisão das aranhas com cogumelos e atualiza os respectivos status
{
    int i = 0;
    int j = 0;
    while(i < NUM_ARANHAS)
    {
        if(aranhas[i].status != 2)
        {
            j = 0;
            while(j < NUM_COGUMELOS) //para toda aranha viva no vetor, procura um cogumelo que esteja em colisão com ela
            {
                if(cogumelos[j].status == 0)
                {
                    if(abs(cogumelos[j].x - aranhas[i].x) < TAM_ARANHA + TAM_COGUMELOS)
                    {
                        if (abs(cogumelos[j].y - aranhas[i].y) < TAM_ARANHA + TAM_COGUMELOS)
                        {
                            inverteAranha(&aranhas[i]);
                            cogumelos[j].status = 1;
                            *quant_cogumelos = *quant_cogumelos - 1;
                            j = NUM_COGUMELOS;
                        }
                    }
                }
                j++;
            }
        }
        i++;
    }
}

void colideTiroAranha(TIRO *tiro, ARANHA aranhas[]) //verifica colisão do tiro com as aranhas e atualiza os estados das aranhas correspondentes
{
    int i = 0;
    if(tiro->status == 0) // se o tiro estiver ativo
    {
        while(i < NUM_ARANHAS)
        {
            if(abs(aranhas[i].x - tiro->x) < TAM_ARANHA) //se a distancia horizontal entre o tiro e a aranha for menor que o tamanho da aranha e do tiro
            {
                if(abs(aranhas[i].y - tiro->y) < TAM_ARANHA) //se a distancia vertical entre o tiro e a aranha for menor que o tamanho da aranha e do tiro
                {
                    aranhas[i].status = 2; // mata a aranha
                    tiro->status = 1; // desativa o tiro
                    i = NUM_ARANHAS; // encerra o laço
                }
            }
            i++;
        }
    }
}

void Gera_envelope_coordenadas(ENVELOPE *envelope, ARANHA ara[], COGUMELO cog[], FAZENDEIRO *faz, MILIPEDE *mili, TIRO *tir)
{
    int i, j;

    for (i = 0; i< NUM_ARANHAS; i++)
    {
        envelope->aranha[i].x1 = ara[i].x - TAM_ARANHA;
        envelope->aranha[i].x2 = ara[i].x + TAM_ARANHA;
        envelope->aranha[i].y1 = ara[i].y - TAM_ARANHA;
        envelope->aranha[i].y2 = ara[i].y + TAM_ARANHA;
    }
    envelope->fazendeiro.x1 = faz->x - TAM_FAZENDEIRO;
    envelope->fazendeiro.x2 = faz->x + TAM_FAZENDEIRO;
    envelope->fazendeiro.y1 = faz->y - TAM_FAZENDEIRO;
    envelope->fazendeiro.y2 = faz->y + TAM_FAZENDEIRO;


    for (j = 0; j< NUM_COGUMELOS; j++)
    {
        envelope->cogu[j].x1 = cog[j].x - TAM_COGUMELOS;
        envelope->cogu[j].x2 = cog[j].x + TAM_COGUMELOS;
        envelope->cogu[j].y1 = cog[j].y - TAM_COGUMELOS;
        envelope->cogu[j].y2 = cog[j].y + TAM_COGUMELOS;
    }

    envelope->milip.x1 = mili->x;
    envelope->milip.x2 = mili->x + LARGURA_MILIPEDE;
    envelope->milip.y1 = mili->y;
    envelope->milip.y2 = mili->y + ALTURA_MILIPEDE;

    envelope->tiro.x1 = tir->x;
    envelope->tiro.x2 = tir->x + TAM_TIRO_LARGURA;
    envelope->tiro.y1 = tir->y;
    envelope->tiro.y2 = tir->y + TAM_TIRO_ALTURA;


}

void ColisaoEConsequencias (ENVELOPE envelope, FAZENDEIRO *faz, MILIPEDE *mili, ARANHA aranha[], TIRO *tiro, COGUMELO cogu[], int *quantidade_cog)
{
    int i, j, k;
    // colisão tiro com milipede
    if ((((((envelope.milip.x1 >= envelope.tiro.x1) && (envelope.milip.x1 <= envelope.tiro.x2)) || ((envelope.milip.x2 >= envelope.tiro.x1) && (envelope.milip.x2 <= envelope.tiro.x2)))
            && (((envelope.milip.y1 >= envelope.tiro.y1) && (envelope.milip.y1 <= envelope.tiro.y2)) || ((envelope.milip.y2 >= envelope.tiro.y1) && (envelope.milip.y2 <= envelope.tiro.y2))))
            || ((envelope.tiro.x1 > envelope.milip.x1 && envelope.tiro.y1 > envelope.milip.y1) && (envelope.tiro.x2 < envelope.milip.x2 && envelope.tiro.y2 < envelope.milip.y2)))
             && (tiro->status == 0)) // tiro colidiu com a cabeça milip
        {
            mili->tamanho--;
            tiro->status = 1;
            faz->pontuacao += 10;
            if (mili->tamanho == 0)
                mili->status = 0;
        }

    // colisão tiro com borda superior do cenário
    if(tiro->y < ALTURA_CABECALHO)
        tiro->status = 1;

    // colisão cogumelos com milipede
    for(i = 0; i<NUM_COGUMELOS; i++)
        if ((((((envelope.milip.x1 >= envelope.cogu[i].x1) && (envelope.milip.x1 <= envelope.cogu[i].x2)) || ((envelope.milip.x2 > envelope.cogu[i].x1) && (envelope.milip.x2 <= envelope.cogu[i].x2)))
                && (((envelope.milip.y1 >= envelope.cogu[i].y1) && (envelope.milip.y1 <= envelope.cogu[i].y2)) || ((envelope.milip.y2 > envelope.cogu[i].y1 && envelope.milip.y2 <= envelope.cogu[i].y2))))
                || ((envelope.cogu[i].x1 > envelope.milip.x1 && envelope.cogu[i].y1 > envelope.milip.y1) && (envelope.cogu[i].x2 < envelope.milip.x2 && envelope.cogu[i].y2 < envelope.milip.y2)))
                && (cogu[i].status == 0))
        {
            cogu[i].status = 1;
            mili->tamanho++;
            mili->dir = baixo;
            *quantidade_cog = *quantidade_cog - 1;
            //printf("%d", *quantidade_cog);
        }

    // colisão cogumelos com tiro
    for(j = 0; j<NUM_COGUMELOS; j++)
        if ((((((envelope.tiro.x1 >= envelope.cogu[j].x1) && (envelope.tiro.x1 <= envelope.cogu[j].x2)) || ((envelope.tiro.x2 > envelope.cogu[j].x1) && (envelope.tiro.x2 <= envelope.cogu[j].x2)))
                && (((envelope.tiro.y1 >= envelope.cogu[j].y1) && (envelope.tiro.y1 <= envelope.cogu[j].y2)) || ((envelope.tiro.y2 > envelope.cogu[j].y1 && envelope.tiro.y2 <= envelope.cogu[j].y2))))
                || ((envelope.cogu[j].x1 > envelope.tiro.x1 && envelope.cogu[j].y1 > envelope.tiro.y1) && (envelope.cogu[j].x2 < envelope.tiro.x2 && envelope.cogu[j].y2 < envelope.tiro.y2)))
                && (cogu[j].status == 0) && (tiro->status == 0)) // cogu[j] colidiu com o tiro
        {
            cogu[j].status = 1;
            faz->cogColhe++;
            tiro->status = 1;
            faz->pontuacao += 20;
            *quantidade_cog = *quantidade_cog - 1;
        }

    // colisão fazendeiro com milipede
    if (((((envelope.milip.x1 >= envelope.fazendeiro.x1) && (envelope.milip.x1 <= envelope.fazendeiro.x2)) || ((envelope.milip.x2 >= envelope.fazendeiro.x1) && (envelope.milip.x2 <= envelope.fazendeiro.x2)))
            && (((envelope.milip.y1 >= envelope.fazendeiro.y1) && (envelope.milip.y1 <= envelope.fazendeiro.y2)) || ((envelope.milip.y2 >= envelope.fazendeiro.y1) && (envelope.milip.y2 <= envelope.fazendeiro.y2))))
            || ((envelope.fazendeiro.x1 > envelope.milip.x1 && envelope.fazendeiro.y1 > envelope.milip.y1) && (envelope.fazendeiro.x2 < envelope.milip.x2 && envelope.fazendeiro.y2 < envelope.milip.y2))) // fazendeiro colidiu com a cabeça milip
        {
            faz->cogCome += mili->tamanho;
            mili->status = 0;
            faz->status = 1; // fica doente
            if (faz->cogColhe - faz->cogCome < 0) // se colidir e não tiver a quantidade de cogumelos pra comer, perde uma vida e zera os cogumelos
                {
                    faz->vidas--;
                    faz->cogColhe = 0;
                    faz->x = LARGURA/2;
                    faz->y = ALTURA - ALTURA_CABECALHO;
                    faz->cogCome = 0;
                    faz->pontuacao -= 25;

                }
            else faz->cogColhe -= faz->cogCome;
            faz->cogCome = 0;
            faz->pontuacao -= 10;
        }

    // colisão aranhas com fazendeiro
    for(k = 0; k<NUM_ARANHAS; k++)
        {
            if ((((((envelope.fazendeiro.x1 >= envelope.aranha[k].x1) && (envelope.fazendeiro.x1 <= envelope.aranha[k].x2)) || ((envelope.fazendeiro.x2 > envelope.aranha[k].x1) && (envelope.fazendeiro.x2 <= envelope.aranha[k].x2)))
                && (((envelope.fazendeiro.y1 >= envelope.aranha[k].y1) && (envelope.fazendeiro.y1 <= envelope.aranha[k].y2)) || ((envelope.fazendeiro.y2 > envelope.aranha[k].y1 && envelope.fazendeiro.y2 <= envelope.aranha[k].y2))))
                || ((envelope.aranha[k].x1 > envelope.fazendeiro.x1 && envelope.aranha[k].y1 > envelope.fazendeiro.y1) && (envelope.aranha[k].x2 < envelope.fazendeiro.x2 && envelope.aranha[k].y2 < envelope.fazendeiro.y2)))
                && (aranha[k].status == 1))
                {
                    aranha[k].status = 2;
                    inverteAranha(&aranha[k]);
                    faz->cogCome += 2;
                    faz->status = 1; // fica doente
                    if (faz->cogColhe - faz->cogCome < 0) // se colidir e não tiver a quantidade de cogumelos pra comer, perde uma vida e zera os cogumelos
                    {
                        faz->vidas--;
                        faz->cogColhe = 0;
                        faz->x = LARGURA/2;
                        faz->y = ALTURA - ALTURA_CABECALHO;
                        faz->cogCome = 0;
                        faz->pontuacao -= 25;

                    }
                    else faz->cogColhe -= faz->cogCome;
                    faz->cogCome = 0;
                    faz->pontuacao -= 10;
                }
        }


}

//Funções do menu de carregamento

void nomeParaArquivo(char nome[]) //converte o nome do jogador para nome de arquivo binário, adicionando ".bin" ao final (vetor *char deve ter 4 posições disponíveis)
{
    int i = 0;
    while(i < TAM_NOME_MAX && nome[i] != '\0')
    {
        i++;
    }
    nome[i] = '.';
    nome[i + 1] = 'b';
    nome[i + 2] = 'i';
    nome[i + 3] = 'n';
    nome[i + 4] = '\0';
}

int carregaArquivoBin(char nomeJog[], FAZENDEIRO *faz1, ARANHA aranhas[], COGUMELO cogumelos[], TIRO *tiro, MILIPEDE *milipede) //procura e lê um arquivo binário com o nome dado e inicializa as variáveis do jogo
{
    FILE *arq;
    nomeParaArquivo(nomeJog);
    arq = fopen(nomeJog,"rb");
    if(arq != NULL)
    {
        fread(faz1,sizeof(FAZENDEIRO),1,arq);
        fread(aranhas,sizeof(ARANHA),NUM_ARANHAS,arq);
        fread(cogumelos,sizeof(COGUMELO),NUM_COGUMELOS,arq);
        fread(tiro,sizeof(TIRO),1,arq);
        fread(milipede,sizeof(MILIPEDE),1,arq);
        fclose(arq);
        return 1;
    }
    fclose(arq);
    return 0; //retorna 0 se houve erro na leitura
}

void menuEntradaNome(char nome[], char carregarOuPause[], int alerta) //desenha o menu onde é solicitado o nome do jogador
{
    DrawRectangle(0,0,LARGURA,ALTURA_TOTAL,COR_MENU);
    DrawRectangle(ESPACAMENTO,ESPACAMENTO,LARGURA -  2 * ESPACAMENTO, ALTURA_TOTAL - 2 * ESPACAMENTO, COR_FUNDO); //contornos da tela
    drawTextoXY(carregarOuPause, LARGURA/2,ALTURA_CABECALHO,TAM_TITULO,COR_MENU);
    drawTextoXY("Digite seu nome de jogador:",LARGURA/2,ALTURA/2 - TAM_ITENS_MENU, TAM_SUBTITULO, COR_MENU);
    if(alerta)
    {
        drawCaixaXY(LARGURA/2, ALTURA/2, 600, TAM_ITENS_MENU, RED); //caixa de texto
        drawTextoXY(nome,LARGURA/2,ALTURA/2,TAM_ITENS_MENU - ESPACAMENTO,RED); //palavra corrente
        drawTextoXY("Limite de caracteres atingido!",LARGURA/2,ALTURA/2 + TAM_ITENS_MENU, TAM_LETRA, RED); //mensagem de alerta
    }
    else
    {
        drawCaixaXY(LARGURA/2, ALTURA/2, 600, TAM_ITENS_MENU, COR_MENU);
        drawTextoXY(nome,LARGURA/2,ALTURA/2,TAM_ITENS_MENU - ESPACAMENTO, COR_MENU);
    }
    drawTextoXY("[ESC] Voltar",LARGURA/2,ALTURA,TAM_SUBTITULO,COR_MENU);
}

void menuCarregamento(FAZENDEIRO *faz1, ARANHA aranhas[], COGUMELO cogumelos[], TIRO *tiro, MILIPEDE *milipede) //abre o menu de carregamento de jogos salvos
{
    char nome[TAM_NOME_MAX + 4] = {}; //tamanho máximo do nome + 4 espaços para ".bin"
    nome[TAM_NOME_MAX - 3] = '\0';
    int carregou = 0;
    int lido;
    int letraAtual = 0;
    while(!WindowShouldClose() && !carregou)
    {
        BeginDrawing();
            switch(lido)
            {
            case KEY_BACKSPACE:
                nome[letraAtual - 1] = '\0';
                if(letraAtual > 0)
                letraAtual--;
                break;

            case KEY_ENTER:
                if(carregaArquivoBin(nome, faz1, aranhas, cogumelos, tiro, milipede))
                {
                    telaDeAviso("Arquivo carregado com sucesso!", GREEN);
                }
                else
                {
                    telaDeAviso("Erro na leitura do arquivo!",RED);
                }
                carregou = 1;
                break;
            default:
                if(letraAtual < TAM_NOME_MAX)
                {

                    if(lido > 32 && lido < 150) //se está na faixa 32 - 150 (é uma letra do alfabeto)
                    {

                        if(IsKeyDown(KEY_LEFT_SHIFT) )
                            nome[letraAtual] = (char) lido; //adiciona a letra ao final da palavra
                        else
                            nome[letraAtual] = tolower(lido);
                        letraAtual++;
                    }
                }
                break;
            }

        if(letraAtual >= TAM_NOME_MAX)
            menuEntradaNome(nome, "Carregar Jogo: ",1);
        else
            menuEntradaNome(nome, "Carregar Jogo: ", 0);
        EndDrawing();
        lido = GetKeyPressed(); //lê a tecla no final do laço para evitar a leitura da tecla 'c' ao abrir o menu
    }
}

//Funções da página inicial

void desenhaInicio(double passo, Texture2D imgTitulo) // desenha o menu de inicio
{
    drawContornos();
    drawTexturaXY(imgTitulo,50,50,LARGURA/2,ALTURA/2 + (int) (cos(passo/30) * VEL_ANIMACAO),725,WHITE);
    drawTextoXY("Aperte [SETA] para iniciar:",LARGURA/2,ALTURA - TAM_SUBTITULO, TAM_SUBTITULO,COR_MENU);
}

int salvaArquivoBin(char nomeJog[], FAZENDEIRO fazendeiro, ARANHA aranhas[], COGUMELO cogumelos[], TIRO tiro, MILIPEDE milipede) //salva o jogo para arquivo binário com nome do jogador
{
    FILE *arq;
    char nomeBinario[TAM_NOME_MAX + 4];
    strcpy(nomeBinario, nomeJog);
    nomeParaArquivo(nomeBinario);
    arq = fopen(nomeBinario,"wb");
    if(arq != NULL)
    {
        if (fwrite (&fazendeiro, sizeof(FAZENDEIRO), 1, arq) != 1)
        {
            printf("Erro nos dados do fazendeiro!");
            fclose(arq);
            return 0;
        }

        if (fwrite (aranhas, sizeof(ARANHA), NUM_ARANHAS, arq) != NUM_ARANHAS)
            {
                printf("Erro nos dados das aranhas");
                fclose(arq);
                return 0;
            }
        if (fwrite (cogumelos, sizeof(COGUMELO), NUM_COGUMELOS, arq) != NUM_COGUMELOS)
            {
                printf("Erro nos dados dos cogumelos!");
                fclose(arq);
                return 0;
            }
        if (fwrite (&tiro, sizeof(TIRO), 1, arq) != 1)
            {
                printf("Erro nos dados do tiro!");
                fclose(arq);
                return 0;
            }
        if (fwrite (&milipede, sizeof(MILIPEDE), 1, arq) != 1)
            {
                printf("Erro nos dados da milipede!");
                fclose(arq);
                return 0;
            }
        fclose(arq);
        return 1;
    }
    fclose(arq);
    return 0;
}

int SalvarArquivoTexto (char nome[], FAZENDEIRO faz) //salva o arquivo de ranking em formato texto
{
    FILE *arq;
    arq = fopen("Ranking.txt", "a+");

    if (!arq)
        {
            printf("Erro na abertura do arquivo!");
            fclose(arq);
            return 0;
        }
    else fprintf(arq, "%s;%d\n", nome, faz.pontuacao);
    fclose(arq);
    return 1;
}

// Funções para determinar o ranking

void LeRankingTexto(FAZENDEIRO lista[]) //abre o arquivo de ranking e preenche o vetor de jogadores com as informações
{
    FILE *arq;
    int i = 0;
    char buffer[60];
    arq = fopen("Ranking.txt", "r");

    if (!arq)
    {
        printf("Erro na abertura do arquivo!");
        fclose(arq);
    }
    else
    {
        fgets(buffer, 60, arq);
        while (!feof(arq))
        {
            //fscanf(arq, "%s", buffer);
            //ponteiro = strtok(buffer, ";");
            strcpy(lista[i].nome, strtok(buffer, ";"));
            //printf("%s\n", lista[i].nome);
            //ponteiro = strtok(NULL, ";");
            //bufferpts = atoi(ponteiro);
            lista[i].pontuacao = atoi(strtok(NULL, ";"));
            //printf("%d\n", lista[i].pontuacao);
            i++;
            fgets(buffer, 60, arq);
        }
        fclose(arq);
    }

}

int InsereJogador (FAZENDEIRO lista[], FAZENDEIRO atual) //insere um fazendeiro na última posição do vetor de fazendeiros
{
    int cont = 0;
    int lidos = 0;

    while (lista[cont].pontuacao != 0 && cont < NUM_JOGADORES)
        cont++;
    if (cont < NUM_JOGADORES)
        lista[cont] = atual;

    for(cont = 0; cont<NUM_JOGADORES; cont++)
        if (lista[cont].pontuacao != 0)
            lidos++;
    return lidos;
    }

void OrdenaJogadores(FAZENDEIRO lista[]) //ordena os jogadores da lista em ordem descrescente de pontuação
{
    int i,j;
    FAZENDEIRO aux;

    for(i = 0; i<NUM_JOGADORES; i++)
    {
        for(j = 0; j< NUM_JOGADORES - 1 - i; j++)
        {
            if(lista[j].pontuacao < lista[j+1].pontuacao)
            {
                aux = lista[j];
                lista[j] = lista[j+1];
                lista[j+1] = aux;
            }
        }
    }
}

void menuSalvar(FAZENDEIRO *fazendeiro, ARANHA aranhas[], COGUMELO cogumelos[], TIRO tiro, MILIPEDE milipede, int indicador) //abre o menu para salvar o progresso
{
    char nome[TAM_NOME_MAX + 4] = {}; //tamanho máximo do nome + 4 espaços para ".bin"
    nome[TAM_NOME_MAX - 3] = '\0';
    int carregou = 0;
    int contador = 0;
    int lido = 0;
    int letraAtual = 0;
    char buffer_fim_jogo[50];
    char bufferpontuacao[50];
    char buffer2[7];
    strcpy(buffer_fim_jogo, "Salve seu progresso! Pontuacao: ");
    strcat(buffer_fim_jogo, itoa(fazendeiro->pontuacao, buffer2, 10));
    strcpy(bufferpontuacao, "Jogo pausado! Pontuacao: ");
    strcat(bufferpontuacao, itoa(fazendeiro->pontuacao, buffer2, 10));
    FAZENDEIRO lista[NUM_JOGADORES];
    lido = GetKeyPressed(); // lê a tecla lida do teclado

    while(!WindowShouldClose() && !carregou)
    {
        BeginDrawing();
        lido = GetKeyPressed(); // lê a tecla lida do teclado
        switch(lido)
        {
            case KEY_BACKSPACE: //anula o último caractere lido e volta 1 posição no vetor
                nome[letraAtual - 1] = '\0';
                if(letraAtual > 0)
                letraAtual--;
                break;

            case KEY_ENTER: //salava o arquivo com o nome escolhido
                SalvarArquivoTexto(nome, *fazendeiro);
                strcpy(fazendeiro->nome, nome);

                if(salvaArquivoBin(nome, *fazendeiro, aranhas, cogumelos, tiro, milipede ))
                {
                    telaDeAviso("Arquivo salvo com sucesso!", GREEN);
                }
                else
                {
                    telaDeAviso("Erro ao salvar o arquivo!",RED);
                }
                carregou = 1;
                break;
            default:
                if(letraAtual < TAM_NOME_MAX)
                {

                    if(lido > 32 && lido < 150) //se está na faixa 32 - 150 (é uma letra do alfabeto)
                    {

                        if(IsKeyDown(KEY_LEFT_SHIFT) )
                            nome[letraAtual] = (char) lido; //adiciona a letra ao final da palavra
                        else
                            nome[letraAtual] = tolower(lido);
                        letraAtual++;
                    }
                }
                break;
            }
        if(letraAtual >= TAM_NOME_MAX)
            {
                if (indicador)
                    menuEntradaNome(nome, bufferpontuacao, 1);
                else
                    menuEntradaNome(nome, buffer_fim_jogo, 1);
            }
        else
            {   if (indicador)
                    menuEntradaNome(nome, bufferpontuacao, 0);
                else
                    menuEntradaNome(nome, buffer_fim_jogo, 0);
            }
        InsereJogador(lista, *fazendeiro);
        OrdenaJogadores(lista);
        contador++;

        EndDrawing();
    }
}

void Ranking(FAZENDEIRO lista[]) //mostra a tela de ranking com os fazendeiros da lista
{

    int i = 1;
    char buffer[30];

    while(!WindowShouldClose())
    {
        BeginDrawing();
        DrawRectangle(0,0,LARGURA,ALTURA_TOTAL,COR_MENU);
        DrawRectangle(ESPACAMENTO,ESPACAMENTO,LARGURA -  2 * ESPACAMENTO, ALTURA_TOTAL - 2 * ESPACAMENTO, COR_FUNDO);
        drawTextoXY("Ranking", LARGURA/2,ALTURA/7,TAM_TITULO,BEIGE);
        for (i = 0; i < 5; i++)
            {
                drawTextoXY(itoa(i+1, buffer, 10), LARGURA/8, 200 + 100*i,TAM_LETRA*2,BEIGE);
                drawTextoXY(".", LARGURA/7, 200 + 100*i,TAM_LETRA*2,BEIGE);
                if (lista[i].pontuacao > 0)
                    {
                    drawTextoXY(lista[i].nome, LARGURA/2, 200 + 100*i,TAM_LETRA*2,BEIGE);
                    drawTextoXY(itoa(lista[i].pontuacao, buffer, 10), LARGURA - 100, 200 + 100*i,TAM_LETRA*2,BEIGE);
                    }
            }
        drawTextoXY("Aperte [ESC] para voltar:", LARGURA/2,ALTURA,TAM_SUBTITULO,COR_MENU);
        EndDrawing();
    }
}

void MensagemFimdeJogo() //tela final
{
    while(!WindowShouldClose())
    {
        BeginDrawing();
        DrawRectangle(0,0,LARGURA,ALTURA_TOTAL,COR_MENU);
        DrawRectangle(ESPACAMENTO,ESPACAMENTO,LARGURA -  2 * ESPACAMENTO, ALTURA_TOTAL - 2 * ESPACAMENTO, COR_FUNDO);
        drawTextoXY("Fim de jogo!", LARGURA/2,ALTURA/2,TAM_TITULO*3,DARKBLUE);
        drawTextoXY("Aperte [ESC] para sair!", LARGURA/2,ALTURA,TAM_SUBTITULO,COR_MENU);
        EndDrawing();
    }
}

void jogo(FAZENDEIRO *faz1, ARANHA aranhas[], COGUMELO *cogumelos, TIRO *tiro, MILIPEDE *milipede, FAZENDEIRO lista[]) //loop principal do jogo
{
    //AUXILIARES
    srand(time(0));
    int cooldown_tiro = 0;
    int cooldown_dano = 0;
    ENVELOPE envelope;
    int quant_cogumelos = NUM_COGUMELOS;
    char bufferpontuacao[50];
    strcpy(bufferpontuacao, "Jogo pausado! Pontuacao: ");

    //INICIALIZAÇÃO DAS TEXTURAS
    Texture2D imgFazendeiro = LoadTexture("resources//farmer.png");
    Texture2D imgCoracao = LoadTexture("resources//hearts.png");
    Texture2D imgCogumelo = LoadTexture("resources//mushroom.png");
    Texture2D imgTiro = LoadTexture("resources//bullet.png");
    Texture2D imgAranha = LoadTexture("resources//spider.png");
    Texture2D imgMilipedeDireita = LoadTexture("resources//milipede_direita.png");
    Texture2D imgMilipedeEsquerda = LoadTexture("resources//milipede_esquerda.png");

    //LOOP PRINCIPAL DO JOGO
    while (!WindowShouldClose() && (quant_cogumelos > 0) && (faz1->vidas > 0))
    {

        BeginDrawing();

        //TECLAS DE MENU
        if(IsKeyPressed(KEY_P))
            menuSalvar(faz1, aranhas, cogumelos, *tiro, *milipede, 1);
        if(IsKeyPressed(KEY_R))
            Ranking(lista);

        //MOVIMENTAÇÃO DOS PERSONAGENS
        moveFazendeiro(faz1);
        atualizaFazendeiro(faz1,&cooldown_dano);
        animacaoTiro(tiro);
        moveAranhas(aranhas, VEL_ARANHA);
        Move_Milipede(milipede);
        if(milipede->y >= ALTURA - LARGURA_MILIPEDE || milipede->status == 0) //se a milipede chega ao fundo do cenário ou morre, re-aparece no topo
            GeraMilipede(milipede);

        //Dispara tiros
        disparaTiro(faz1, tiro, &cooldown_tiro);
        Gera_envelope_coordenadas(&envelope, aranhas, cogumelos, faz1, milipede, tiro);
        cooldown_tiro++;

        //VERIFICAÇÃO DE COLISÕES
        ColisaoEConsequencias(envelope, faz1, milipede, aranhas, tiro, cogumelos, &quant_cogumelos);
        colideAranhaCogumelo(aranhas,cogumelos, &quant_cogumelos);
        colideTiroAranha(tiro,aranhas);

        //RENDERIZAÇÃO DO FUNDO
        ClearBackground(COR_FUNDO);

        //RENDERIZAÇÃO DOS PERSONAGENS
        desenhaAranhas(aranhas,imgAranha);
        drawFazendeiro(*faz1, imgFazendeiro);
        desenhaCogumelos(cogumelos, imgCogumelo);
        DesenhaMilipede(*milipede, imgMilipedeDireita, imgMilipedeEsquerda);
        desenhaTiro(*tiro);

        //RENDERIZAÇÃO DA INTERFACE
        drawContornos();
        drawMenu(*faz1, imgCoracao, imgTiro, imgCogumelo);
        drawCabecalho();

        EndDrawing();
    }
}

void geraJogadores(FAZENDEIRO listaFazendeiros[]) //inicializa o vetor de jogadores
{
    int j;
    for(j = 0; j < NUM_JOGADORES; j++)
    {
        geraFazendeiro(&listaFazendeiros[j]);
    }
}

void inicializar() // inicialização do cenário e personagens antes do loop principal (apresentação do jogo, nome, opção de carregar salvo)
{
    //AUXILIARES
    int iniciar_jogo = 0; //guarda se o usuário jogou uma partida antes de sair

    //INICIALIZAÇÃO DO FAZENDEIRO
    FAZENDEIRO faz1;
    geraFazendeiro(&faz1);
    FAZENDEIRO lista[NUM_JOGADORES];
    geraJogadores(lista);

    //INICIALIZAÇÃO DOS TIROS
    TIRO tiro;
    tiro.status = 1;

    //INICIALIZAÇÃO DAS ARANHAS
    ARANHA aranhas[NUM_ARANHAS];
    geraAranhas(aranhas);

    //INICIALIZAÇÃO DOS COGUMELOS
    COGUMELO cogumelos[NUM_COGUMELOS];
    geraCogumelos(cogumelos);

    //INICIALIZAÇÃO DA MILIPEDE
    MILIPEDE milipede;
    GeraMilipede(&milipede);

    Texture2D imgCogumelo = LoadTexture("resources//mushroom.png");
    Texture2D imgTitulo = LoadTexture("resources//title.png");

    double passo; //controla a animação do título

    while(!iniciar_jogo && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(COR_FUNDO);
        desenhaCogumelos(cogumelos, imgCogumelo);
        desenhaInicio(passo, imgTitulo);
        passo++;
        switch(GetKeyPressed())
        {
        case KEY_C:
            menuCarregamento(&faz1, aranhas, cogumelos, &tiro, &milipede);
            break;
        case KEY_RIGHT:
        case KEY_LEFT:
        case KEY_UP:
        case KEY_DOWN:
            iniciar_jogo = 1;
            break;
        default:
            break;
        }
        drawCabecalho();
        EndDrawing();
    }
    if(iniciar_jogo)
    {
        jogo(&faz1, aranhas, cogumelos, &tiro, &milipede, lista);
        menuSalvar(&faz1, aranhas, cogumelos, tiro, milipede, 0);
        LeRankingTexto(lista);
        OrdenaJogadores(lista);
        Ranking(lista);
    }
    MensagemFimdeJogo();
}

//Funções principais do jogo

int main(void)
{
    InitWindow(LARGURA, ALTURA_TOTAL, "Milipede");
    SetTargetFPS(FPS);
    inicializar();
    CloseWindow();
    return 0;
}
