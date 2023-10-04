#include "raylib.h"
#include <math.h>
#include <string.h>
#include "pila.h"

///HOLA , GIT

//DEFINICIONES
#define TILESIZE 32  //lucreciadenisebazan@gmail.com
#define MAXFRUTITAS 400
#define MAXFRUTASBONUS 50
#define COLIZQUIERDA 17
#define COLDERECHA 16
#define COLUMNAS 33
#define FILAS 21
#define MAXTILES 693
#define FPS 60
#define DIM 30
#define DIMNOMBRE 18
#define MAXSCORES 5

//ESTRUCTURAS
typedef struct {
    Sound waka1;
    Sound waka2;
    int waka;
    Sound death;
} Wakawaka;

typedef struct {
    int i;
    int j;
} Tile;

typedef struct {
    Rectangle hitbox;
    int existe;
} Objeto;

typedef struct {
    Tile tile[MAXTILES];
    int indexTextura[MAXTILES];
    Texture2D texturas[DIM*2];
    int frameAnimacion;
    Objeto paredes[MAXTILES];
    Objeto frutitas[MAXTILES];
    Objeto frutasBonus[MAXTILES];
    Objeto teletransportadores[2];
    Objeto puertas[2];
    Objeto itemBonus;
    int tipoItemBonus;
    int enfriamientoItemBonus;
} Mapa;

typedef struct {
    char nombre[DIMNOMBRE];
    int score;
} Score;

typedef struct {
    int estado; //0: Menú, 1: Juego, 2: GameOver, 3: Scores, 4:SaveScreen, 5: Créditos
    int frames;
    Vector2 posicionMouse;
    Score bufferScores[DIM];
    Score scoreYNombre;
    int posicionLetra;
    int scoreGuardado;
    int contadorMarcadorPosicion;
    int switchMarcadorPosicion;
    Objeto botonPlay;
    Objeto botonBackScores;
    Objeto botonScore;
    Objeto botonVidas;
    Objeto botonCreditos;
    Objeto botonVolverAJugar;
    Objeto botonBackCreditos;
    Objeto scrollCreditos;
    int validosFrutitas;
    int validosFrutasBonus;
    int validosArchivo;
} Menu;

typedef struct {
    Rectangle E;//0
    int contactoE;
    Rectangle N;//1
    int contactoN;
    Rectangle W;//2
    int contactoW;
    Rectangle S;//3
    int contactoS;
} Bumpers;

typedef struct {
    Vector2 posicion;
    Tile tile;
    Texture2D texturas[DIM];
    int frameAnimacion;
    Rectangle hitbox;
    Bumpers bumpers; //Paragolpes
    float velocidad;
    int direccion; //0 = ESTE , 1 = NORTE , 2 = OESTE , 3 = SUR
    int reloco; //0 = NORMAL, 1 = RELOCO
    int enfriamientoReloco;
    Pila vidas;
    Pila descarteVidas;
    int muerto;
    int enfriamientoMuerto;
} Pacman;

typedef struct {
    int salidasRestantes;
    int cantidadBumpersEn0;
    int salida[4];
    int framesIA;
} InteligenciaArtificial;

typedef struct {
    Vector2 posicion;
    Texture2D texturas[DIM];
    int frameAnimacion;
    Rectangle hitbox;
    Bumpers bumpers;
    int velocidad;
    int direccion;
    int enfriamientoMiedo;
    int muerto;
    int enfriamientoMuerto;
    InteligenciaArtificial IA;
} Fantasma;

//HERRAMIENTA DEBUG
typedef struct {
    Color veryDarkTeal;
    Color darkTeal;
    Color teal;
    Color lightTeal;
    Color veryLightTeal;
} ColoresDebug;

typedef struct {
    Rectangle ventana;
    int variableActivada[12];
    float valorVariable[10];
    Rectangle botonVariable[12];
    ColoresDebug color; //Teal
    int mostrarHerramienta;
} Debug;

//PROTOTIPADO
Tile posicionATile(Vector2 );

//CONSTANTES
const int anchoVentana = TILESIZE * COLUMNAS;
const int altoVentana = TILESIZE * FILAS;
const char nombreArchivoScores[DIM] = "pacmanScores.bin" ;

//VARIABLES GLOBALES
int i, j, k, m;

int main() {
    //INICIALIZACIONES
    //-------------------------------------------------------------------------------------
    srand(time(NULL));
    Wakawaka wakawaka;
    Mapa mapa;
    Menu menu;
    Pacman pacman;
    Fantasma fantasmas[4];
    Debug herramientaDebug;
    inicializaciones(&wakawaka, &menu, &mapa, &pacman, &fantasmas, &herramientaDebug);
    //-------------------------------------------------------------------------------------
    //BUCLE PRINCIPAL
    //-------------------------------------------------------------------------------------
    while( !WindowShouldClose() ) {
        actualizar(&wakawaka, &menu, &mapa, &pacman, &fantasmas, &herramientaDebug);
        dibujar(menu, mapa, pacman, fantasmas, herramientaDebug);
    }
    //-------------------------------------------------------------------------------------
    //DE-INICIALIZACION
    //-------------------------------------------------------------------------------------
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
//INICIALIZACIONES
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void inicializaciones(Wakawaka *wakawaka, Menu *menu, Mapa *mapa, Pacman *pacman, Fantasma fantasmas[], Debug *herramientaDebug) {
    InitWindow(anchoVentana, altoVentana, "Pac-man");
    SetTargetFPS(FPS);
    InitAudioDevice();
    inicializarSonidos(wakawaka);
    inicializarMenu(menu);
    inicializarMapa(menu, mapa);
    inicializarPacman(pacman);
    inicializarFantasmas(fantasmas);
    if( !elArchivoExiste() ) inicializarScoresDefault();
    inicializarDebug(herramientaDebug);
}
void inicializarSonidos(Wakawaka *wakawaka) {
    wakawaka->waka1 = LoadSound("pacman-waka1.wav");
    wakawaka->waka2 = LoadSound("pacman-waka2.wav");
    wakawaka->death = LoadSound("pacman-death.wav");
    wakawaka->waka = 0;
}
void inicializarMenu(Menu *menu) {
    inicializarEstadoJuego(menu);
    inicializarScores(menu);
    inicializarBotonesMenu(menu);
    menu->posicionMouse = GetMousePosition();
}
void inicializarEstadoJuego(Menu *menu) {
    menu->estado = 0; //0: menu, 1: juego, 2: GameOver, 3:Score, 4: WinScreen/SaveScreen
    menu->frames = 0;
}
void inicializarScores(Menu *menu) {
    menu->scoreYNombre.score = 0;
    menu->scoreYNombre.nombre[0] = '\0';
    menu->posicionLetra = 0;
    menu->scoreGuardado = 0;
    menu->switchMarcadorPosicion = 0;
    menu->contadorMarcadorPosicion = 0;
    menu->validosArchivo = 0;
}
void inicializarBotonesMenu(Menu *menu) {
    inicializarBotonPlay(menu);
    inicializarbotonBackScores(menu);
    inicializarBotonScore(menu);
    inicializarBotonVidas(menu);
    inicializarBotonCreditos(menu);
    inicializarBotonVolverAJugar(menu);
    inicializarBotonBackCreditos(menu);
    inicializarScrollCreditos(menu);
}
void inicializarBotonPlay(Menu *menu) {
    menu->botonPlay.existe = 1;
    menu->botonPlay.hitbox.x = TILESIZE * 10.25;
    menu->botonPlay.hitbox.y = TILESIZE * 4.18;
    menu->botonPlay.hitbox.width = TILESIZE * 11.86;
    menu->botonPlay.hitbox.height = TILESIZE * 2;
}
void inicializarbotonBackScores(Menu *menu) {
    menu->botonBackScores.existe = 1;
    menu->botonBackScores.hitbox.x = TILESIZE * 13.5;
    menu->botonBackScores.hitbox.y = TILESIZE * 17.25;
    menu->botonBackScores.hitbox.width = TILESIZE * 5.7;
    menu->botonBackScores.hitbox.height = TILESIZE * 1.8;
}
void inicializarBotonScore(Menu *menu) {
    menu->botonScore.existe = 1;
    menu->botonScore.hitbox.x = TILESIZE * 6.59;
    menu->botonScore.hitbox.y = TILESIZE * 10.18;
    menu->botonScore.hitbox.width = TILESIZE * 19.86;
    menu->botonScore.hitbox.height = TILESIZE * 2;
}
void inicializarBotonVidas(Menu *menu) {
    menu->botonVidas.existe = 1;
    menu->botonVidas.hitbox.x = TILESIZE * 6.59;
    menu->botonVidas.hitbox.y = TILESIZE * 14.18;
    menu->botonVidas.hitbox.width = TILESIZE * 19.86;
    menu->botonVidas.hitbox.height = TILESIZE * 2;
}
void inicializarBotonCreditos(Menu *menu) {
    menu->botonCreditos.existe = 1;
    menu->botonCreditos.hitbox.x = TILESIZE * 6.59;
    menu->botonCreditos.hitbox.y = TILESIZE * 17.18;
    menu->botonCreditos.hitbox.width = TILESIZE * 19.86;
    menu->botonCreditos.hitbox.height = TILESIZE * 2;
}
void inicializarBotonVolverAJugar(Menu *menu) {
    menu->botonVolverAJugar.existe = 0;
    menu->botonVolverAJugar.hitbox.x = TILESIZE * 10.283;
    menu->botonVolverAJugar.hitbox.y = TILESIZE * 8.93;
    menu->botonVolverAJugar.hitbox.width = TILESIZE * 11.4;
    menu->botonVolverAJugar.hitbox.height = TILESIZE * 1.68;
}
void inicializarBotonBackCreditos(Menu *menu) {
    menu->botonBackCreditos.existe = 1;
    menu->botonBackCreditos.hitbox.x = TILESIZE * 13.5;
    menu->botonBackCreditos.hitbox.y = TILESIZE * 17.25;
    menu->botonBackCreditos.hitbox.width = TILESIZE * 5.7;
    menu->botonBackCreditos.hitbox.height = TILESIZE * 1.8;
}
void inicializarScrollCreditos(Menu *menu) {
    menu->scrollCreditos.existe = 1;
    menu->scrollCreditos.hitbox.x = TILESIZE * 7;
    menu->scrollCreditos.hitbox.y = TILESIZE * 16;
    menu->scrollCreditos.hitbox.width = TILESIZE * 0.1;
    menu->scrollCreditos.hitbox.height = TILESIZE * 0.1;
}
void inicializarMapa(Menu *menu, Mapa *mapa) {
    inicializarTexturasMapa(mapa);
    inicializarMatrizMapa(mapa);
    inicializarParedes(mapa);
    menu->validosFrutitas = inicializarFrutitas(mapa);
    menu->validosFrutasBonus = inicializarFrutasBonus(mapa);
    inicializarTeletransportadores(mapa);
    inicializarPuertas(mapa);
    inicializarItemBonus(mapa);
}
void inicializarMatrizMapa(Mapa *mapa) {
    int bufferMapa1Izquierda[21][17] = { //j es el eje x, i es el eje y
        // 0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16
        {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //0
        {  0,  12,   4,   4,   4,   4,   4,   4,   4,  18,   4,  18,   4,  18,   4,   4,   4}, //1
        {  0,   8,   3,   2,   2,   2,   2,   2,   2,  10,   2,  10,   3,  10,   2,   2,   2}, //2
        {  0,  15,   5,   2,  12,   5,   2,   9,   2,   2,   2,   2,   2,   2,   2,   9,   2}, //3
        {  0,   8,   2,   2,   8,   3,   2,  15,   4,   4,   5,   2,  12,  11,   2,   8,   3}, //4
        {  0,  15,   5,   2,  13,   5,   2,  10,   2,   2,   2,   2,  13,  14,   2,  10,   2}, //5
        {  0,   8,   3,   2,   2,   2,   2,   2,   2,  12,  11,   3,   2,   2,   2,   2,   2}, //6
        {  0,  13,   4,   4,   4,  11,   2,   9,   2,   8,   8,   2,   9,   2,  12,  21,   7}, //7
        {  0,   0,   0,   0,   0,   8,   2,   8,   2,   8,   8,   2,   8,   2,   8,   1,   1}, //8
        {  0,   6,   4,   4,   4,  14,   2,  10,   2,  13,  14,   2,  10,   2,  13,   4,   4}, //9
        {  0,   0,  23,   2,   2,   2,   2,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2}, //10
        {  0,   6,   4,   4,   4,  11,   2,   9,   2,   6,   4,   4,   4,   4,   4,  11,   2}, //11
        {  0,   0,   0,   0,   0,   8,   2,   8,   2,   2,   2,   2,   2,   2,   3,  10,   2}, //12
        {  0,  12,   4,   4,   4,  14,   2,  10,   2,   6,   4,  18,   4,   5,   2,   2,   2}, //13
        {  0,   8,   2,   2,   2,   2,   2,   3,   2,   2,   2,  10,   2,   2,   2,   9,   2}, //14
        {  0,  15,   5,   2,   6,   5,   2,   9,   2,  20,   2,   2,   2,   6,   4,  14,   2}, //15
        {  0,   8,   2,   2,   2,   2,   2,   8,   2,   2,   2,   9,   2,   2,   2,   2,   3}, //16
        {  0,   8,   2,   6,   4,   4,   4,  16,   4,   5,   2,  10,   2,   6,   4,   5,   2}, //17
        {  0,   8,   3,   2,   2,   2,   2,   2,   2,   2,   2,   3,   2,   2,   2,   2,   2}, //18
        {  0,  13,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4}, //19
        {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}  //20
    };
    int bufferMapa1Derecha[21][16] = {
        //17   18   19   20    21  22   23   24   25   26   27    28   29   30   31   32
        {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //0
        {  4,   4,  18,   4,  18,   4,  18,   4,   4,   4,   4,   4,   4,   4,  11,   0}, //1
        {  2,   2,  10,   3,  10,   2,  10,   2,   2,   2,   2,   2,   2,   3,   8,   0}, //2
        {  9,   2,   2,   2,   2,   2,   2,   2,   9,   2,   6,  11,   2,   6,  17,   0}, //3
        {  8,   2,  12,  11,   2,   6,   4,   4,  17,   2,   3,   8,   2,   2,   8,   0}, //4
        {  8,   2,  13,  14,   2,   2,   2,   2,  10,   2,   6,  14,   2,   6,  17,   0}, //5
        {  2,   2,   2,   2,   3,  12,  11,   2,   2,   2,   2,   2,   2,   3,   8,   0}, //6
        { 21,  11,   2,   9,   2,   8,   8,   2,   9,   2,  12,   4,   4,   4,  14,   0}, //7
        {  1,   8,   2,   8,   2,   8,   8,   2,   8,   2,   8,   0,   0,   0,   0,   0}, //8
        {  4,  14,   2,  10,   2,  13,  14,   2,  10,   2,  13,   4,   4,   4,   5,   0}, //9
        {  2,   2,   2,   2,   2,   2,   2,   2,   3,   2,   2,   2,   2,  22,   0,   0}, //10
        { 12,   4,   4,   4,   4,   4,   5,   2,   9,   2,  12,   4,   4,   4,   5,   0}, //11
        { 10,   3,   2,   2,   2,   2,   2,   2,   8,   2,   8,   0,   0,   0,   0,   0}, //12
        {  2,   2,   4,   4,  18,   4,   5,   2,  10,   2,  13,   4,   4,   4,  11,   0}, //13
        {  9,   2,   2,   2,  10,   2,   2,   2,   3,   2,   2,   2,   2,   2,   8,   0}, //14
        { 13,   4,   4,   2,   2,   2,  20,   2,   9,   2,   6,   5,   2,   6,  17,   0}, //15
        {  2,   2,   2,   2,   9,   2,   2,   2,   8,   2,   2,   2,   2,   2,   8,   0}, //16
        {  6,   4,   4,   2,  10,   2,   6,   4,  16,   4,   4,   4,   5,   2,   8,   0}, //17
        {  2,   2,   2,   2,   3,   2,   2,   2,   2,   2,   2,   2,   2,   3,   8,   0}, //18
        {  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  14,   0}, //19
        {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}  //20
    }; //Carga del mapa
    for( i = 0 ; i < 21 ; i++ ) {
        for( j = 0 ; j < 17 ; j++ ) {
            mapa->tile[k].i = i;
            mapa->tile[k].j = j;
            mapa->indexTextura[k] = bufferMapa1Izquierda[i][j];
            k++;
        }
        for( j = 0 ; j < 16 ; j++ ) {
            mapa->tile[k].i = i;
            mapa->tile[k].j = j + 17;
            mapa->indexTextura[k] = bufferMapa1Derecha[i][j];
            k++;
        }
    }
}
void inicializarTexturasMapa(Mapa *mapa) {
    mapa->frameAnimacion = 0;
    mapa->texturas[0] =  LoadTexture("PNG/MAPA/Default.png");
    mapa->texturas[1] =  LoadTexture("PNG/MAPA/White_floor.png");
    mapa->texturas[2] =  LoadTexture("PNG/MAPA/White_floor_fruit.png");
    mapa->texturas[3] =  LoadTexture("PNG/MAPA/White_floor_big_fruit.png");
    mapa->texturas[4] =  LoadTexture("PNG/MAPA/Vertical_wall.png");
    mapa->texturas[5] =  LoadTexture("PNG/MAPA/Horizontal_wall_right.png");
    mapa->texturas[6] =  LoadTexture("PNG/MAPA/Horizontal_wall_left.png");
    mapa->texturas[7] =  LoadTexture("PNG/MAPA/Full_horizontal_wall.png");
    mapa->texturas[8] =  LoadTexture("PNG/MAPA/Horizontal_wall.png");
    mapa->texturas[9] =  LoadTexture("PNG/MAPA/Vertical_wall_up.png");
    mapa->texturas[10] = LoadTexture("PNG/MAPA/Vertical_wall_down.png");
    mapa->texturas[11] = LoadTexture("PNG/MAPA/Left_upper_-corner.png");
    mapa->texturas[12] = LoadTexture("PNG/MAPA/Right_upper_corner.png");
    mapa->texturas[13] = LoadTexture("PNG/MAPA/Lower_right_corner.png");
    mapa->texturas[14] = LoadTexture("PNG/MAPA/Lower_left_corner.png");
    mapa->texturas[15] = LoadTexture("PNG/MAPA/T_right.png");
    mapa->texturas[16] = LoadTexture("PNG/MAPA/T_upwards.png");
    mapa->texturas[17] = LoadTexture("PNG/MAPA/T_left.png");
    mapa->texturas[18] = LoadTexture("PNG/MAPA/T_down.png");
    mapa->texturas[19] = LoadTexture("PNG/MAPA/Cross.png");
    mapa->texturas[20] = LoadTexture("PNG/MAPA/Block.png");
    mapa->texturas[21] = LoadTexture("PNG/MAPA/Door.png");
    mapa->texturas[22] = LoadTexture("PNG/MAPA/Right_arrow.png");
    mapa->texturas[23] = LoadTexture("PNG/MAPA/Left_arrow.png");

    mapa->texturas[24] = LoadTexture("PNG/CEREZA/Cereza1.png");
    mapa->texturas[25] = LoadTexture("PNG/CEREZA/Cereza2.png");
    mapa->texturas[26] = LoadTexture("PNG/CEREZA/Cereza3.png");
    mapa->texturas[27] = LoadTexture("PNG/CEREZA/Cereza4.png");
    mapa->texturas[28] = LoadTexture("PNG/CEREZA/Cereza5.png");
    mapa->texturas[29] = LoadTexture("PNG/CEREZA/Cereza6.png");

    mapa->texturas[30] = LoadTexture("PNG/FRUTILLITA/Frutillita1.png");
    mapa->texturas[31] = LoadTexture("PNG/FRUTILLITA/Frutillita2.png");
    mapa->texturas[32] = LoadTexture("PNG/FRUTILLITA/Frutillita3.png");
    mapa->texturas[33] = LoadTexture("PNG/FRUTILLITA/Frutillita4.png");
    mapa->texturas[34] = LoadTexture("PNG/FRUTILLITA/Frutillita5.png");
    mapa->texturas[35] = LoadTexture("PNG/FRUTILLITA/Frutillita6.png");

    mapa->texturas[36] = LoadTexture("PNG/MATECITO/Matecito1.png");
    mapa->texturas[37] = LoadTexture("PNG/MATECITO/Matecito2.png");
    mapa->texturas[38] = LoadTexture("PNG/MATECITO/Matecito3.png");
    mapa->texturas[39] = LoadTexture("PNG/MATECITO/Matecito4.png");
    mapa->texturas[40] = LoadTexture("PNG/MATECITO/Matecito5.png");
    mapa->texturas[41] = LoadTexture("PNG/MATECITO/Matecito6.png");
}
void inicializarParedes(Mapa *mapa) {
    for( k = 0 ; k < MAXTILES ; k++ ) {
        if( mapa->indexTextura[k] >= 4 && mapa->indexTextura[k] <= 20 ) {
            mapa->paredes[k].hitbox.x = mapa->tile[k].j * TILESIZE;
            mapa->paredes[k].hitbox.y = mapa->tile[k].i * TILESIZE;
            mapa->paredes[k].hitbox.width = TILESIZE;
            mapa->paredes[k].hitbox.height = TILESIZE;
            mapa->paredes[k].existe = 1;
        } else {
            mapa->paredes[k].hitbox.x = 0;
            mapa->paredes[k].hitbox.y = 0;
            mapa->paredes[k].hitbox.width = 0;
            mapa->paredes[k].hitbox.height = 0;
            mapa->paredes[k].existe = 0;
        }
    }
}
int inicializarFrutitas(Mapa *mapa) {
    int validosFrutitas = 0;
    for( k = 0 ; k < MAXTILES ; k++ ) {
        if( mapa->indexTextura[k] == 2 ) {
            mapa->frutitas[k].hitbox.x = mapa->tile[k].j * TILESIZE + TILESIZE / 2;
            mapa->frutitas[k].hitbox.y = mapa->tile[k].i * TILESIZE + TILESIZE / 2;
            mapa->frutitas[k].hitbox.width = 2;
            mapa->frutitas[k].hitbox.height = 2;
            mapa->frutitas[k].existe = 1;
            validosFrutitas++;
        } else {
            mapa->frutitas[k].hitbox.x = 0;
            mapa->frutitas[k].hitbox.y = 0;
            mapa->frutitas[k].hitbox.width = 0;
            mapa->frutitas[k].hitbox.height = 0;
            mapa->frutitas[k].existe = 0;
        }
    }
    return validosFrutitas;
}
int inicializarFrutasBonus(Mapa *mapa) {
    int validosFrutasBonus = 0;
    for( k = 0 ; k < MAXTILES ; k++ ) {
        if( mapa->indexTextura[k] == 3 ) {
            mapa->frutasBonus[k].hitbox.x = mapa->tile[k].j * TILESIZE + TILESIZE / 2;
            mapa->frutasBonus[k].hitbox.y = mapa->tile[k].i * TILESIZE + TILESIZE / 2;
            mapa->frutasBonus[k].hitbox.width = 3;
            mapa->frutasBonus[k].hitbox.height = 3;
            mapa->frutasBonus[k].existe = 1;
            validosFrutasBonus++;
        } else {
            mapa->frutasBonus[k].hitbox.x = 0;
            mapa->frutasBonus[k].hitbox.y = 0;
            mapa->frutasBonus[k].hitbox.width = 0;
            mapa->frutasBonus[k].hitbox.height = 0;
            mapa->frutasBonus[k].existe = 0;
        }
    }
    return validosFrutasBonus;
}
void inicializarTeletransportadores(Mapa *mapa) {
    for( k = 0 ; k < MAXTILES ; k++ ) {
        if( mapa->indexTextura[k] == 22 ) {
            mapa->teletransportadores[0].hitbox.x = mapa->tile[k].j * TILESIZE;
            mapa->teletransportadores[0].hitbox.y = mapa->tile[k].i * TILESIZE;
            mapa->teletransportadores[0].hitbox.width = TILESIZE;
            mapa->teletransportadores[0].hitbox.height = TILESIZE;
            mapa->teletransportadores[0].existe = 1;
        }
        if( mapa->indexTextura[k] == 23 ) {
            mapa->teletransportadores[1].hitbox.x = mapa->tile[k].j * TILESIZE;
            mapa->teletransportadores[1].hitbox.y = mapa->tile[k].i * TILESIZE;
            mapa->teletransportadores[1].hitbox.width = TILESIZE;
            mapa->teletransportadores[1].hitbox.height = TILESIZE;
            mapa->teletransportadores[1].existe = 1;
        }
    }
}
void inicializarPuertas(Mapa *mapa) {
    i = 0;
    for( k = 0 ; k < MAXTILES ; k++ )
        if( mapa->indexTextura[k] == 21 && i < 2 ) {
            mapa->puertas[i].hitbox.x = mapa->tile[k].j * TILESIZE;
            mapa->puertas[i].hitbox.y = mapa->tile[k].i * TILESIZE;
            mapa->puertas[i].hitbox.width = TILESIZE;
            mapa->puertas[i].hitbox.height = TILESIZE;
            mapa->puertas[i].existe = 1;
            i++;
        }
}
void inicializarItemBonus(Mapa *mapa) {
    mapa->tipoItemBonus = rand()%3;
    mapa->enfriamientoItemBonus = 900;
    buscarPosicionInicialItemBonus(mapa);
    mapa->itemBonus.hitbox.width = TILESIZE;
    mapa->itemBonus.hitbox.height = TILESIZE;
    mapa->itemBonus.existe = 0;
}
void buscarPosicionInicialItemBonus(Mapa *mapa) {
    int hastaEncontrarPiso = 1;
    int posicionFrutitaRandom = rand()%MAXTILES;
    while( hastaEncontrarPiso == 1 ) {
        posicionFrutitaRandom = rand()%MAXTILES;
        if( mapa->frutitas[posicionFrutitaRandom].existe == 1 ) {
            mapa->itemBonus.hitbox.x = mapa->frutitas[posicionFrutitaRandom].hitbox.x;
            mapa->itemBonus.hitbox.y = mapa->frutitas[posicionFrutitaRandom].hitbox.y;
            hastaEncontrarPiso = 0;
        }
    }
}
void inicializarPacman(Pacman *pacman) {
    inicializarPosicionPacman(pacman);
    inicializarTexturasPacman(pacman);
    inicializarHitboxPacman(pacman);
    inicializarBumpersPacman(pacman);
    inicializarEstadoPacman(pacman);
    inicializarVidas(pacman);
}
void inicializarPosicionPacman(Pacman *pacman) {
    Tile bufferTile;
    pacman->posicion.x = 16 * TILESIZE + TILESIZE / 2;
    pacman->posicion.y = 10 * TILESIZE + TILESIZE / 2;
    bufferTile = posicionATile(pacman->posicion);
    pacman->tile.i = bufferTile.i;
    pacman->tile.j = bufferTile.j;
    pacman->velocidad = 0;
    pacman->direccion = 0;
}
void inicializarTexturasPacman(Pacman *pacman) {
    pacman->frameAnimacion = 0;
    pacman->texturas[0] =  LoadTexture("PNG/PACMAN/Pacman1.png");
    pacman->texturas[1] =  LoadTexture("PNG/PACMAN/Pacman2.png");
    pacman->texturas[2] =  LoadTexture("PNG/PACMAN/Pacman3.png");
    pacman->texturas[3] =  LoadTexture("PNG/PACMAN/Pacman4.png");
    pacman->texturas[4] =  LoadTexture("PNG/PACMAN/Pacman5.png");
    pacman->texturas[5] =  LoadTexture("PNG/PACMAN/Pacman6.png");
    pacman->texturas[6] =  LoadTexture("PNG/PACMAN/PacmanFlip1.png");
    pacman->texturas[7] =  LoadTexture("PNG/PACMAN/PacmanFlip2.png");
    pacman->texturas[8] =  LoadTexture("PNG/PACMAN/PacmanFlip3.png");
    pacman->texturas[9] =  LoadTexture("PNG/PACMAN/PacmanFlip4.png");
    pacman->texturas[10] = LoadTexture("PNG/PACMAN/PacmanFlip5.png");
    pacman->texturas[11] = LoadTexture("PNG/PACMAN/PacmanFlip6.png");
}
void inicializarHitboxPacman(Pacman *pacman) {
    pacman->hitbox.x = pacman->posicion.x - ( TILESIZE / 2 * 0.9 );
    pacman->hitbox.y = pacman->posicion.y - ( TILESIZE / 2 * 0.9 );
    pacman->hitbox.height = TILESIZE * 0.95;
    pacman->hitbox.width = TILESIZE * 0.95;
}
void inicializarBumpersPacman(Pacman *pacman) {
    inicializarBumperEPacman(pacman);
    inicializarBumperNPacman(pacman);
    inicializarBumperWPacman(pacman);
    inicializarBumperSPacman(pacman);
}
void inicializarBumperEPacman(Pacman *pacman) {
    pacman->bumpers.E.x = pacman->posicion.x + ( TILESIZE / 2 ) * 0.9;
    pacman->bumpers.E.y = pacman->posicion.y - ( TILESIZE / 2 ) * 0.6;
    pacman->bumpers.E.height = TILESIZE * 0.65;
    pacman->bumpers.E.width = TILESIZE / 4.5;
}
void inicializarBumperNPacman(Pacman *pacman) {
    pacman->bumpers.N.x = pacman->posicion.x - ( TILESIZE / 2 ) * 0.6;
    pacman->bumpers.N.y = pacman->posicion.y - ( TILESIZE / 2 ) * 1.3;
    pacman->bumpers.N.height = TILESIZE / 4.5;
    pacman->bumpers.N.width = TILESIZE * 0.65;
}
void inicializarBumperWPacman(Pacman *pacman) {
    pacman->bumpers.W.x = pacman->posicion.x - ( TILESIZE / 2 ) * 1.3;
    pacman->bumpers.W.y = pacman->posicion.y - ( TILESIZE / 2 ) * 0.6;
    pacman->bumpers.W.height = TILESIZE * 0.65;
    pacman->bumpers.W.width = TILESIZE / 4.5;
}
void inicializarBumperSPacman(Pacman *pacman) {
    pacman->bumpers.S.x = pacman->posicion.x - ( TILESIZE / 2 ) * 0.6;
    pacman->bumpers.S.y = pacman->posicion.y + ( TILESIZE / 2 ) * 0.9;
    pacman->bumpers.S.height = TILESIZE / 4.5;
    pacman->bumpers.S.width = TILESIZE * 0.65;
}
void inicializarVidas(Pacman *pacman) {
    inicpila(&pacman->vidas);
    inicpila(&pacman->descarteVidas);
    apilar(&pacman->vidas, 1);
    apilar(&pacman->vidas, 2);
    apilar(&pacman->vidas, 3);
}
void inicializarEstadoPacman(Pacman *pacman) {
    pacman->reloco = 0;
    pacman->enfriamientoReloco = 0;
    pacman->muerto = 0;
    pacman->enfriamientoMuerto = 0;
}
void inicializarFantasmas(Fantasma fantasmas[]) {
    inicializarTexturasFantasmas(fantasmas);
    for( i = 0 ; i < 4 ; i++ ) {
        inicializarPosicionFantasmas(fantasmas, i);
        inicializarHitboxFantasmas(fantasmas, i);
        inicializarBumpersFantasmas(fantasmas, i);
        inicializarEstadoFantasmas(fantasmas, i);
        fantasmas[i].IA.framesIA = 0;
    }
}
void inicializarTexturasFantasmas(Fantasma fantasmas[]) {
    inicializarTexturasFantasmaAmarillo(fantasmas);
    inicializarTexturasFantasmaRojo(fantasmas);
    inicializarTexturasFantasmaVerde(fantasmas);
    inicializarTexturasFantasmaVioleta(fantasmas);
}
void inicializarTexturasFantasmaAmarillo(Fantasma fantasmas[]) {
    fantasmas[0].frameAnimacion = 0;
    fantasmas[0].texturas[0] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloE1.png");
    fantasmas[0].texturas[1] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloE2.png");
    fantasmas[0].texturas[2] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloE3.png");
    fantasmas[0].texturas[3] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloN1.png");
    fantasmas[0].texturas[4] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloN2.png");
    fantasmas[0].texturas[5] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloN3.png");
    fantasmas[0].texturas[6] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloW1.png");
    fantasmas[0].texturas[7] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloW2.png");
    fantasmas[0].texturas[8] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloW3.png");
    fantasmas[0].texturas[9] =  LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloS1.png");
    fantasmas[0].texturas[10] = LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloS2.png");
    fantasmas[0].texturas[11] = LoadTexture("PNG/FANTASMITAS/FantasmitaAmarilloS3.png");
    fantasmas[0].texturas[12] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo1.png");
    fantasmas[0].texturas[13] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo2.png");
    fantasmas[0].texturas[14] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo3.png");
}
void inicializarTexturasFantasmaRojo(Fantasma fantasmas[]) {
    fantasmas[1].frameAnimacion = 0;
    fantasmas[1].texturas[0] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoE1.png");
    fantasmas[1].texturas[1] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoE2.png");
    fantasmas[1].texturas[2] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoE3.png");
    fantasmas[1].texturas[3] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoN1.png");
    fantasmas[1].texturas[4] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoN2.png");
    fantasmas[1].texturas[5] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoN3.png");
    fantasmas[1].texturas[6] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoW1.png");
    fantasmas[1].texturas[7] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoW2.png");
    fantasmas[1].texturas[8] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoW3.png");
    fantasmas[1].texturas[9] =  LoadTexture("PNG/FANTASMITAS/FantasmitaRojoS1.png");
    fantasmas[1].texturas[10] = LoadTexture("PNG/FANTASMITAS/FantasmitaRojoS2.png");
    fantasmas[1].texturas[11] = LoadTexture("PNG/FANTASMITAS/FantasmitaRojoS3.png");
    fantasmas[1].texturas[12] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo1.png");
    fantasmas[1].texturas[13] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo2.png");
    fantasmas[1].texturas[14] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo3.png");
}
void inicializarTexturasFantasmaVerde(Fantasma fantasmas[]) {
    fantasmas[2].frameAnimacion = 0;
    fantasmas[2].texturas[0] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeE1.png");
    fantasmas[2].texturas[1] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeE2.png");
    fantasmas[2].texturas[2] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeE3.png");
    fantasmas[2].texturas[3] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeN1.png");
    fantasmas[2].texturas[4] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeN2.png");
    fantasmas[2].texturas[5] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeN3.png");
    fantasmas[2].texturas[6] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeW1.png");
    fantasmas[2].texturas[7] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeW2.png");
    fantasmas[2].texturas[8] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeW3.png");
    fantasmas[2].texturas[9] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeS1.png");
    fantasmas[2].texturas[10] = LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeS2.png");
    fantasmas[2].texturas[11] = LoadTexture("PNG/FANTASMITAS/FantasmitaVerdeS3.png");
    fantasmas[2].texturas[12] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo1.png");
    fantasmas[2].texturas[13] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo2.png");
    fantasmas[2].texturas[14] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo3.png");
}
void     inicializarTexturasFantasmaVioleta(Fantasma fantasmas[]) {
    fantasmas[3].frameAnimacion = 0;
    fantasmas[3].texturas[0] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaE1.png");
    fantasmas[3].texturas[1] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaE2.png");
    fantasmas[3].texturas[2] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaE3.png");
    fantasmas[3].texturas[3] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaN1.png");
    fantasmas[3].texturas[4] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaN2.png");
    fantasmas[3].texturas[5] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaN3.png");
    fantasmas[3].texturas[6] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaW1.png");
    fantasmas[3].texturas[7] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaW2.png");
    fantasmas[3].texturas[8] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaW3.png");
    fantasmas[3].texturas[9] =  LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaS1.png");
    fantasmas[3].texturas[10] = LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaS2.png");
    fantasmas[3].texturas[11] = LoadTexture("PNG/FANTASMITAS/FantasmitaVioletaS3.png");
    fantasmas[3].texturas[12] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo1.png");
    fantasmas[3].texturas[13] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo2.png");
    fantasmas[3].texturas[14] = LoadTexture("PNG/FANTASMITAS/FantasmitaConMiedo3.png");
}
void inicializarPosicionFantasmas(Fantasma fantasmas[], int i) {
    fantasmas[i].posicion.x = 15 * TILESIZE + TILESIZE / 2;
    fantasmas[i].posicion.y = 8 * TILESIZE + TILESIZE / 2;
    fantasmas[i].velocidad = 0.0781 * TILESIZE;
    fantasmas[i].direccion = 0;
}
void inicializarHitboxFantasmas(Fantasma fantasmas[], int i) {
    fantasmas[i].hitbox.x = fantasmas[i].posicion.x - ( TILESIZE / 2 );
    fantasmas[i].hitbox.y = fantasmas[i].posicion.y - ( TILESIZE / 2 );
    fantasmas[i].hitbox.height = TILESIZE;
    fantasmas[i].hitbox.width = TILESIZE;
}
void inicializarBumpersFantasmas(Fantasma fantasmas[], int i) {
    inicializarBumperEFantasmas(fantasmas, i);
    inicializarBumperNFantasmas(fantasmas, i);
    inicializarBumperWFantasmas(fantasmas, i);
    inicializarBumperSFantasmas(fantasmas, i);
}
void inicializarBumperEFantasmas(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.E.x = fantasmas[i].posicion.x + ( TILESIZE / 2 ) * 0.95;
    fantasmas[i].bumpers.E.y = fantasmas[i].posicion.y - ( TILESIZE / 2 ) * 0.85;
    fantasmas[i].bumpers.E.height = TILESIZE * 0.9;
    fantasmas[i].bumpers.E.width = TILESIZE * 0.1;
}
void inicializarBumperNFantasmas(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.N.x = fantasmas[i].posicion.x - ( TILESIZE / 2 ) * 0.85;
    fantasmas[i].bumpers.N.y = fantasmas[i].posicion.y - ( TILESIZE / 2 ) * 1.12;
    fantasmas[i].bumpers.N.height = TILESIZE * 0.1;
    fantasmas[i].bumpers.N.width = TILESIZE * 0.9;
}
void inicializarBumperWFantasmas(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.W.x = fantasmas[i].posicion.x - ( TILESIZE / 2 ) * 1.12;
    fantasmas[i].bumpers.W.y = fantasmas[i].posicion.y - ( TILESIZE / 2 ) * 0.85;
    fantasmas[i].bumpers.W.height = TILESIZE * 0.9;
    fantasmas[i].bumpers.W.width = TILESIZE * 0.1;
}
void inicializarBumperSFantasmas(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.S.x = fantasmas[i].posicion.x - ( TILESIZE / 2 ) * 0.85;
    fantasmas[i].bumpers.S.y = fantasmas[i].posicion.y + ( TILESIZE / 2 ) * 0.95;
    fantasmas[i].bumpers.S.height = TILESIZE * 0.1;
    fantasmas[i].bumpers.S.width = TILESIZE * 0.9;
}
void inicializarEstadoFantasmas(Fantasma fantasmas[], int i){
    fantasmas[i].enfriamientoMiedo = 0;
    fantasmas[i].muerto = 1;
    fantasmas[i].enfriamientoMuerto = i * 4;
}
int elArchivoExiste() {
    int existe = 0;
    FILE *bufferArchivo = fopen(nombreArchivoScores,"rb");
    if(bufferArchivo != NULL) existe = 1;
    return existe;
}
void inicializarScoresDefault() {
    Score scores[10];
    inicializarNombresScoresDefault(scores);
    inicializarValoresScoresDefault(scores);
    sobreescribirArchivoScoresDefault(scores);
}
void inicializarNombresScoresDefault(Score scores[]) {
    char nombreScore1[DIM] = "tripleCampeon";
    strcpy(scores[0].nombre, nombreScore1);
    char nombreScore2[DIM] = "pacmanGOD";
    strcpy(scores[1].nombre, nombreScore2);
    char nombreScore3[DIM] = "elDeArribaUsaHacks";
    strcpy(scores[2].nombre, nombreScore3);
    char nombreScore4[DIM] = "Juancito";
    strcpy(scores[3].nombre, nombreScore4);
    char nombreScore5[DIM] = "asdasd";
    strcpy(scores[4].nombre, nombreScore5);
}
void inicializarValoresScoresDefault(Score scores[]) {
    scores[0].score = 122500;
    scores[1].score = 122000;
    scores[2].score = 95500;
    scores[3].score = 82500;
    scores[4].score = 500;
}
void sobreescribirArchivoScoresDefault(Score scores[]) {
    int validosScore = MAXSCORES;
    FILE *archivoScores = fopen(nombreArchivoScores, "wb");
    if( archivoScores == NULL ) printf("\nError. El archivo no existe.");
    if( archivoScores != NULL ) {
        for( i = 0 ; i < validosScore ; i++ ) fwrite(&scores[i], sizeof(Score), 1, archivoScores);
        fclose(archivoScores);
    }
}
//BUCLE PRINCIPAL
//-------------------------------------------------------------------------------------
//ACTUALIZAR
//-------------------------------------------------------------------------------------
void actualizar(Wakawaka *wakawaka, Menu *menu, Mapa *mapa, Pacman *pacman, Fantasma fantasmas[], Debug *herramientaDebug) {
    actualizarInput(menu, mapa, pacman, fantasmas);
    actualizarColisiones(wakawaka, menu, mapa, pacman, fantasmas);
    actualizarPosicion(menu, pacman, fantasmas);
    actualizarDebug(herramientaDebug, menu);
}
void actualizarInput(Menu *menu, Mapa *mapa, Pacman *pacman, Fantasma fantasmas[]) {
    moversePorStateMachine(menu); //DEBUG, ADMIN ONLY
    //0: Menu, 1: Juego, 2: GameOver, 3: Scores, 4: SaveScreen
    if     ( menu->estado == 0 ) actualizarInputMenu(menu);
    else if( menu->estado == 1 ) actualizarInputJuego(menu, mapa, pacman, fantasmas);
    else if( menu->estado == 2 ) actualizarInputGameOver(menu);
    else if( menu->estado == 4 ) actualizarInputSaveScreen(menu);
}
void moversePorStateMachine(Menu *menu) {
    if(IsKeyReleased(KEY_TWO))   menu->estado = 0;
    if(IsKeyReleased(KEY_ONE))   menu->estado = 1;
    if(IsKeyReleased(KEY_THREE)) menu->estado = 2;
    if(IsKeyReleased(KEY_FOUR))  menu->estado = 3;
    if(IsKeyReleased(KEY_FIVE))  menu->estado = 4;
    if(IsKeyReleased(KEY_SIX))   menu->estado = 5;
}
void actualizarInputMenu(Menu *menu) {
    if( IsKeyReleased(KEY_ENTER) ) menu->estado = 1;
}
void actualizarInputJuego(Menu *menu, Mapa *mapa, Pacman *pacman, Fantasma fantasmas[]) {
    menu->frames++;
    actualizarEnfriamientoItemBonus(mapa);
    if( menu->frames%6 == 0 ) actualizarFramesAnimacion(mapa, pacman, fantasmas);
    inputMovimientoPacman(menu, pacman);
    inputPausaJuego(menu);
}
void actualizarEnfriamientoItemBonus(Mapa *mapa) {
    if( mapa->enfriamientoItemBonus > 0 ) mapa->enfriamientoItemBonus--;
    if( mapa->enfriamientoItemBonus == 0 ) {
        mapa->enfriamientoItemBonus = -1;
        mapa->itemBonus.existe = 1;
    }
}
void actualizarFramesAnimacion(Mapa *mapa, Pacman *pacman, Fantasma fantasmas[]) {
    mapa->frameAnimacion++;
    if( mapa->frameAnimacion == 6 ) mapa->frameAnimacion = 0;
    pacman->frameAnimacion++;
    if( pacman->frameAnimacion == 6 ) pacman->frameAnimacion = 0;
    for( i = 0 ; i < 4 ; i++ ) {
        fantasmas[i].frameAnimacion++;
        if( fantasmas[i].frameAnimacion == 3 ) fantasmas[i].frameAnimacion = 0;
    }
}
void inputMovimientoPacman(Menu *menu, Pacman *pacman) {
    inputMovimientoDerechaPacman(menu, pacman);
    inputMovimientoArribaPacman(menu, pacman);
    inputMovimientoIzquierdaPacman(menu, pacman);
    inputMovimientoAbajoPacman(menu, pacman);
}
void inputMovimientoDerechaPacman(Menu *menu, Pacman *pacman) {
    if(( IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT) ) && pacman->bumpers.contactoE == 0 ) {
        if( pacman->reloco == 0 ) pacman->velocidad = 0.0781 * TILESIZE;
        if( pacman->reloco == 1 ) pacman->velocidad = 0.085 * TILESIZE;
        pacman->direccion = 0;
    }
}
void inputMovimientoArribaPacman(Menu *menu, Pacman *pacman) {
    if(( IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) ) && pacman->bumpers.contactoN == 0 ) {
        if( pacman->reloco == 0 ) pacman->velocidad = 0.0781 * TILESIZE;
        if( pacman->reloco == 1 ) pacman->velocidad = 0.085 * TILESIZE;
        pacman->direccion = 1;
    }
}
void inputMovimientoIzquierdaPacman(Menu *menu, Pacman *pacman) {
    if(( IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) ) && pacman->bumpers.contactoW == 0 ) {
        if( pacman->reloco == 0 ) pacman->velocidad = 0.0781 * TILESIZE;
        if( pacman->reloco == 1 ) pacman->velocidad = 0.085 * TILESIZE;
        pacman->direccion = 2;
    }
}
void inputMovimientoAbajoPacman(Menu *menu, Pacman *pacman) {
    if(( IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN) ) && pacman->bumpers.contactoS == 0 ) {
        if( pacman->reloco == 0 ) pacman->velocidad = 0.0781 * TILESIZE;
        if( pacman->reloco == 1 ) pacman->velocidad = 0.085 * TILESIZE;
        pacman->direccion = 3;
    }
}
void inputPausaJuego(Menu *menu) {
    if(IsKeyReleased(KEY_ENTER))  menu->estado = 0;
}
void actualizarInputGameOver(Menu *menu) {
    if(IsKeyReleased(KEY_ENTER)) menu->estado = 4;
}
void actualizarInputSaveScreen(Menu *menu) {
    menu->contadorMarcadorPosicion++;
    if( menu->contadorMarcadorPosicion%45 == 0 )
        menu->switchMarcadorPosicion = ( menu->switchMarcadorPosicion == 0 ) ? 1 : 0;
    cargarNombreDinamico(menu);
    if( IsKeyPressed(KEY_ENTER) && menu->scoreGuardado == 0 ) guardarArchivoScore(menu);
}
void cargarNombreDinamico(Menu *menu) {
    int letra = GetCharPressed();
    if( menu->scoreGuardado == 0 ) {
        while( letra > 0 ) {
            cargarLetraDinamica(menu, letra);
            letra = GetCharPressed();
        }
        if( IsKeyPressed(KEY_BACKSPACE) ) borrarLetraDinamica(menu);
    }
}
void cargarLetraDinamica(Menu *menu, int letra) {
    if(  letra <= 125 && letra >= 32 && menu->posicionLetra < DIMNOMBRE && letra != 53 ) {
        menu->scoreYNombre.nombre[menu->posicionLetra] = (char)letra;
        menu->scoreYNombre.nombre[menu->posicionLetra+1] = '\0';
        menu->posicionLetra++;
    }
}
void borrarLetraDinamica(Menu *menu) {
    menu->posicionLetra--;
    if (menu->posicionLetra < 0) menu->posicionLetra = 0;
    menu->scoreYNombre.nombre[menu->posicionLetra] = '\0';
}
void guardarArchivoScore(Menu *menu) {
    menu->botonVolverAJugar.existe = 1;
    menu->scoreGuardado = 1;
    agregarScoreAArchivo(menu);
    calcularValidosArchivoScores(menu);
    guardarScoreEnArray(menu);
    ordenarArrayScores(menu);
    mostrarArrayScores(menu->bufferScores, menu->validosArchivo);
    sobreescribirArchivoScores(menu);
}
void agregarScoreAArchivo(Menu *menu) {
    Score bufferScore;
    FILE *archivoScore = fopen(nombreArchivoScores, "ab");
    if( archivoScore ==  NULL ) printf("\nError. El archivo de Score no existe.");
    if( archivoScore !=  NULL ) {
        copiarScore(&bufferScore, menu->scoreYNombre);
        fwrite(&bufferScore, sizeof(Score), 1, archivoScore);
        fclose(archivoScore);
    }
}
void calcularValidosArchivoScores(Menu *menu) {
    int validos = 0;
    FILE *archivoScore = fopen(nombreArchivoScores, "ab");
    if( archivoScore ==  NULL ) printf("\nError. El archivo de Score no existe.");
    if( archivoScore !=  NULL ) {
        fseek(archivoScore, 0, SEEK_END);
        validos = ftell(archivoScore) / sizeof(Score);
        fclose(archivoScore);
    }
    menu->validosArchivo = validos;
}
void guardarScoreEnArray(Menu *menu) {
    i = 0;
    Score bufferScore;
    FILE *archivoScore = fopen(nombreArchivoScores, "rb");
    if( archivoScore == NULL ) printf("\nError. El archivo de Score no existe.");
    if( archivoScore != NULL ) {
        while( !feof(archivoScore) ) {
            fread(&bufferScore, sizeof(Score), 1, archivoScore);
            if( !feof(archivoScore) ) {
                copiarScoreAMenu(menu, i, &bufferScore);
                i++;
            }
        }
        fclose(archivoScore);
    }
}
void ordenarArrayScores(Menu *menu) {
    i = 1;
    while( i < menu->validosArchivo ) {
        j = i;
        while( j > 0 && menu->bufferScores[j - 1].score < menu->bufferScores[j].score ) {
            intercambiarScores(&menu->bufferScores[j - 1], &menu->bufferScores[j]);
            j--;
        }
        i++;
    }
}
void sobreescribirArchivoScores(Menu *menu) {
    FILE *archivoScore = fopen(nombreArchivoScores, "ab");
    fopen(nombreArchivoScores, "wb");
    if( archivoScore == NULL ) printf("\nError. El archivo de Score no existe.");
    if( archivoScore != NULL ) {
        for( i = 0 ; i < MAXSCORES ; i++ )
            fwrite(&menu->bufferScores[i], sizeof(Score), 1, archivoScore);
        fclose(archivoScore);
    }
}
void copiarScore(Score *scoreDestino, Score *scoreOrigen) {
    scoreDestino->score = scoreOrigen->score;
    strcpy(scoreDestino->nombre, scoreOrigen->nombre);
}
void copiarScoreAMenu(Menu *menu, int i, Score *scoreOrigen) {
    menu->bufferScores[i].score = scoreOrigen->score;
    strcpy(menu->bufferScores[i].nombre, scoreOrigen->nombre);
}
void mostrarScore(Score score) {
    printf("\n------------------------");
    printf("\nNombre: %s", score.nombre);
    printf("\nScore: %i", score.score);
    printf("\n------------------------");
}
void mostrarArrayScores(Score scoreArray[], int validos) {
    for( i = 0 ; i < validos ; i++ ) mostrarScore(scoreArray[i]);
}
void intercambiarScores(Score *score1, Score *score2) {
    Score scoreAux;
    strcpy(scoreAux.nombre, score1->nombre);
    scoreAux.score = score1->score;
    strcpy(score1->nombre, score2->nombre);
    score1->score = score2->score;
    strcpy(score2->nombre, scoreAux.nombre);
    score2->score = scoreAux.score;
}
void actualizarColisiones(Wakawaka *wakawaka, Menu *menu, Mapa *mapa, Pacman *pacman, Fantasma fantasmas[]) {
    if( menu->estado == 0 )      actualizarColisionesMenu(menu);
    else if( menu->estado == 1 ) actualizarColisionesJuego(wakawaka, menu, mapa, pacman, fantasmas);
    else if( menu->estado == 3 ) actualizarColisionesScores(menu);
    else if( menu->estado == 5 ) actualizarColisionesCreditos(menu);
    if( menu->estado == 1 ) {
        //EN PROGRESO
        //---------------------------------------------------------------------------------------------------------
        //IA COLISIONES + DIRECCION
        int azar;
        fantasmas[0].IA.framesIA++;
        for( i = 0 ; i < 4 ; i++ )
        {
            fantasmas[i].IA.salidasRestantes = 4;
            fantasmas[i].IA.cantidadBumpersEn0 = 4;
            for( j = 0 ; j < 4 ; j++ ) fantasmas[i].IA.salida[j] = 1;
            fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoE;
            fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoN;
            fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoW;
            fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoS;
            if( fantasmas[i].IA.cantidadBumpersEn0 == 1 )
            {
                if( fantasmas[i].bumpers.contactoE == 1 ) fantasmas[i].IA.salida[0] = 0;
                if( fantasmas[i].bumpers.contactoN == 1 ) fantasmas[i].IA.salida[1] = 0;
                if( fantasmas[i].bumpers.contactoW == 1 ) fantasmas[i].IA.salida[2] = 0;
                if( fantasmas[i].bumpers.contactoS == 1 ) fantasmas[i].IA.salida[3] = 0;
            }
            else if( fantasmas[i].IA.cantidadBumpersEn0 == 2 )
            {
                if( fantasmas[i].bumpers.contactoE == 1 ) fantasmas[i].IA.salida[0] = 0;
                if( fantasmas[i].bumpers.contactoN == 1 ) fantasmas[i].IA.salida[1] = 0;
                if( fantasmas[i].bumpers.contactoW == 1 ) fantasmas[i].IA.salida[2] = 0;
                if( fantasmas[i].bumpers.contactoS == 1 ) fantasmas[i].IA.salida[3] = 0;
                if( fantasmas[i].direccion == 0 ) fantasmas[i].IA.salida[2] = 0;
                if( fantasmas[i].direccion == 1 ) fantasmas[i].IA.salida[3] = 0;
                if( fantasmas[i].direccion == 2 ) fantasmas[i].IA.salida[0] = 0;
                if( fantasmas[i].direccion == 3 ) fantasmas[i].IA.salida[1] = 0;
            }
            else if( fantasmas[i].IA.cantidadBumpersEn0 == 3 )
            {
                if( fantasmas[i].bumpers.contactoE == 1 ) fantasmas[i].IA.salida[0] = 0;
                if( fantasmas[i].bumpers.contactoN == 1 ) fantasmas[i].IA.salida[1] = 0;
                if( fantasmas[i].bumpers.contactoW == 1 ) fantasmas[i].IA.salida[2] = 0;
                if( fantasmas[i].bumpers.contactoS == 1 ) fantasmas[i].IA.salida[3] = 0;
                if( fantasmas[i].direccion == 0 ) fantasmas[i].IA.salida[2] = 0;
                if( fantasmas[i].direccion == 1 ) fantasmas[i].IA.salida[3] = 0;
                if( fantasmas[i].direccion == 2 ) fantasmas[i].IA.salida[0] = 0;
                if( fantasmas[i].direccion == 3 ) fantasmas[i].IA.salida[1] = 0;
                fantasmas[i].IA.salidasRestantes = 1; //elimino una de las salidas restantes al azar
                for( j = 0 ; j < 200 && fantasmas[i].IA.salidasRestantes != 0 ; j++ )
                {
                    azar = rand()%4;
                    if( fantasmas[i].IA.salida[azar] == 1 )
                    {
                        fantasmas[i].IA.salida[azar] = 0;
                        fantasmas[i].IA.salidasRestantes--;
                    }
                }
            }
            else if( fantasmas[i].IA.cantidadBumpersEn0 == 4 )
            {
                if( fantasmas[i].direccion == 0 ) fantasmas[i].IA.salida[2] = 0;
                if( fantasmas[i].direccion == 1 ) fantasmas[i].IA.salida[3] = 0;
                if( fantasmas[i].direccion == 2 ) fantasmas[i].IA.salida[0] = 0;
                if( fantasmas[i].direccion == 3 ) fantasmas[i].IA.salida[1] = 0;
                fantasmas[i].IA.salidasRestantes = 2; //elimino dos de las salidas restantes al azar
                for( j = 0 ; j < 200 && fantasmas[i].IA.salidasRestantes != 0 ; j++ )
                {
                    azar = rand()%4;
                    if( fantasmas[i].IA.salida[azar] == 1 )
                    {
                        fantasmas[i].IA.salida[azar] = 0;
                        fantasmas[i].IA.salidasRestantes--;
                    }
                }
            }
            for( k = 0 ; k < 4 ; k++ )
            {
                if( fantasmas[i].IA.salida[k] == 1 && fantasmas[0].IA.framesIA%4 == 0 )
                {
                    fantasmas[i].direccion = k;
                    fantasmas[i].velocidad = 0.0781 * TILESIZE;
                }
            }
        }
        //---------------------------------------------------------------------------------------------------------
        //EN PROGRESO
    }
}
void actualizarColisionesMenu(Menu *menu) {
    if( IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ) {
        menu->posicionMouse = GetMousePosition();
        if( CheckCollisionPointRec(menu->posicionMouse, menu->botonPlay.hitbox) )  menu->estado = 1;
        if( CheckCollisionPointRec(menu->posicionMouse, menu->botonScore.hitbox) ) menu->estado = 3;
        if( CheckCollisionPointRec(menu->posicionMouse, menu->botonCreditos.hitbox) ) menu->estado = 5;
    }
}
void actualizarColisionesJuego(Wakawaka *wakawaka, Menu *menu, Mapa *mapa, Pacman *pacman, Fantasma fantasmas[]) {
    actualizarColisionPacmanFrutitas(wakawaka, menu, mapa, pacman);
    actualizarColisionPacmanFrutasBonus(wakawaka, menu, mapa, pacman);
    actualizarColisionPacmanItemBonus(menu, mapa, pacman);
    actualizarColisionPacmanTeletransportadores(mapa, pacman);
    actualizarColisionPacmanFantasmas(wakawaka, menu, pacman, fantasmas);
    actualizarColisionBumpersPacman(mapa, pacman);
    actualizarColisionPacmanParedes(mapa, pacman);
    actualizarColisionBumpersFantasmas(mapa, fantasmas);
    actualizarColisionFantasmasTeletransportadores(mapa, fantasmas);
    //actualizarIA(mapa, fantasmas); //EN PROGRESO
}
void actualizarColisionPacmanFrutitas(Wakawaka *wakawaka, Menu *menu, Mapa *mapa, Pacman *pacman) {
    if(  menu->validosFrutitas == 0 && menu->validosFrutasBonus == 0 ) menu->estado = 4;
    for( k = 0 ; k < MAXTILES ; k++ )
        if( CheckCollisionRecs( pacman->hitbox, mapa->frutitas[k].hitbox ) == 1 && mapa->frutitas[k].existe == 1 )
            comerFrutita(wakawaka, menu, mapa);
}
void actualizarColisionPacmanFrutasBonus(Wakawaka *wakawaka, Menu *menu, Mapa *mapa, Pacman *pacman) {
    if(  menu->validosFrutitas == 0 && menu->validosFrutasBonus == 0 ) menu->estado = 4;
    for( k = 0 ; k < MAXTILES ; k++ )
        if( CheckCollisionRecs( pacman->hitbox, mapa->frutasBonus[k].hitbox ) == 1 && mapa->frutasBonus[k].existe == 1 )
            comerFrutaBonus(wakawaka, menu, mapa, pacman);
}
void actualizarColisionPacmanItemBonus(Menu *menu, Mapa *mapa, Pacman *pacman) {
    if( CheckCollisionRecs(pacman->hitbox, mapa->itemBonus.hitbox) && mapa->itemBonus.existe == 1 )
        comerItemBonus(menu, mapa);
}
void comerItemBonus(Menu *menu, Mapa *mapa) {
    mapa->itemBonus.existe = 0;
    menu->scoreYNombre.score += 25000;
}
void comerFrutita(Wakawaka *wakawaka, Menu *menu, Mapa *mapa) {
    mapa->frutitas[k].existe = 0;
    menu->validosFrutitas--;
    menu->scoreYNombre.score = menu->scoreYNombre.score + 500;
    sonidoComerFrutita(wakawaka);
}
void comerFrutaBonus(Wakawaka *wakawaka, Menu *menu, Mapa *mapa, Pacman *pacman) {
    mapa->frutasBonus[k].existe = 0;
    menu->validosFrutasBonus--;
    menu->scoreYNombre.score = menu->scoreYNombre.score + 2500;
    pacman->reloco = 1;
    pacman->enfriamientoReloco = 160;
    sonidoComerFrutita(wakawaka);
}
void sonidoComerFrutita(Wakawaka *wakawaka) {
    if( wakawaka->waka == 0) PlaySound(wakawaka->waka1);
    if( wakawaka->waka == 1) PlaySound(wakawaka->waka2);
    wakawaka->waka = ( wakawaka->waka == 1 ) ? 0 : 1 ;
}
void actualizarColisionPacmanTeletransportadores(Mapa *mapa, Pacman *pacman) {
    if( CheckCollisionRecs(pacman->hitbox, mapa->teletransportadores[0].hitbox) == 1 )
        teletransportarPacmanAIzquierda(pacman);
    if( CheckCollisionRecs(pacman->hitbox, mapa->teletransportadores[1].hitbox) == 1 )
        teletransportarPacmanADerecha(pacman);
}
void teletransportarPacmanAIzquierda(Pacman *pacman) {
    pacman->posicion.x = 3 * TILESIZE + TILESIZE / 2;
    pacman->posicion.y = 10 * TILESIZE + TILESIZE / 2;
}
void teletransportarPacmanADerecha(Pacman *pacman) {
    pacman->posicion.x = 29 * TILESIZE + TILESIZE / 2;
    pacman->posicion.y = 10 * TILESIZE + TILESIZE / 2;
}
void actualizarColisionPacmanFantasmas(Wakawaka *wakawaka, Menu *menu, Pacman *pacman, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ )
        if( CheckCollisionRecs(pacman->hitbox, fantasmas[i].hitbox) == 1 && !pilavacia(&pacman->vidas) && pacman->enfriamientoMuerto == 0 ) {
            if( pacman->reloco == 0 ) pacmanPierdeUnaVida(wakawaka, menu, pacman);
            if( pacman->reloco == 1 ) pacmanComeFantasma(menu, fantasmas, i);
        }
}
void pacmanPierdeUnaVida(Wakawaka *wakawaka, Menu *menu, Pacman *pacman) {
    pacman->muerto = 1;
    pacman->enfriamientoMuerto = 120;
    apilar(&pacman->descarteVidas, desapilar(&pacman->vidas));
    PlaySound(wakawaka->death);
    if( pilavacia( &pacman->vidas ) ) menu->estado = 2;
}
void pacmanComeFantasma(Menu *menu, Fantasma fantasmas[], int i) {
    if( fantasmas[i].muerto == 0 ) menu->scoreYNombre.score += 5000;
    fantasmas[i].muerto = 1;
    fantasmas[i].enfriamientoMuerto = 120;
}
void actualizarColisionBumpersPacman(Mapa *mapa, Pacman *pacman) {
    pacman->bumpers.contactoE = 0;
    pacman->bumpers.contactoN = 0;
    pacman->bumpers.contactoW = 0;
    pacman->bumpers.contactoS = 0;
    for( k = 0 ; k < MAXTILES ; k++ ) {
        if( CheckCollisionRecs(pacman->bumpers.E, mapa->paredes[k].hitbox) ) pacman->bumpers.contactoE = 1;
        if( CheckCollisionRecs(pacman->bumpers.N, mapa->paredes[k].hitbox) ) pacman->bumpers.contactoN = 1;
        if( CheckCollisionRecs(pacman->bumpers.W, mapa->paredes[k].hitbox) ) pacman->bumpers.contactoW = 1;
        if( CheckCollisionRecs(pacman->bumpers.S, mapa->paredes[k].hitbox) ) pacman->bumpers.contactoS = 1;
    }
    for( j = 0 ; j < 2 ; j++ ) {
        if( CheckCollisionRecs(pacman->bumpers.E, mapa->puertas[j].hitbox) ) pacman->bumpers.contactoE = 1;
        if( CheckCollisionRecs(pacman->bumpers.N, mapa->puertas[j].hitbox) ) pacman->bumpers.contactoN = 1;
        if( CheckCollisionRecs(pacman->bumpers.W, mapa->puertas[j].hitbox) ) pacman->bumpers.contactoW = 1;
        if( CheckCollisionRecs(pacman->bumpers.S, mapa->puertas[j].hitbox) ) pacman->bumpers.contactoS = 1;
    }
}
void actualizarColisionPacmanParedes(Mapa *mapa, Pacman *pacman) {
    actualizarTilePacman(pacman);
    for( k = 0 ; k < MAXTILES ; k++ )
        if( CheckCollisionRecs(pacman->hitbox, mapa->paredes[k].hitbox) ) {
            corregirPosicionPacman(pacman);
            if( pacmanAtropellaParedE(mapa, pacman, k) ) pacman->velocidad = 0;
            if( pacmanAtropellaParedN(mapa, pacman, k) ) pacman->velocidad = 0;
            if( pacmanAtropellaParedW(mapa, pacman, k) ) pacman->velocidad = 0;
            if( pacmanAtropellaParedS(mapa, pacman, k) ) pacman->velocidad = 0;
        }
}
void actualizarTilePacman(Pacman *pacman) {
    Tile tileActual;
    tileActual = posicionATile(pacman->posicion);
    pacman->tile.i = tileActual.i;
    pacman->tile.j = tileActual.j;
}
void corregirPosicionPacman(Pacman *pacman) {
    pacman->posicion.x = ( pacman->tile.i * TILESIZE ) + ( TILESIZE / 2 );
    pacman->posicion.y = ( pacman->tile.j * TILESIZE ) + ( TILESIZE / 2 );
}
int pacmanAtropellaParedE(Mapa *mapa, Pacman *pacman, int k) {
    int paredAtropellada = 0;
    if( CheckCollisionRecs(pacman->bumpers.E, mapa->paredes[k].hitbox) && pacman->direccion == 0 && pacman->velocidad != 0 )
        paredAtropellada = 1;
    return paredAtropellada;
}
int pacmanAtropellaParedN(Mapa *mapa, Pacman *pacman, int k) {
    int paredAtropellada = 0;
    if( CheckCollisionRecs(pacman->bumpers.N, mapa->paredes[k].hitbox) && pacman->direccion == 1 && pacman->velocidad != 0 )
        paredAtropellada = 1;
    return paredAtropellada;
}
int pacmanAtropellaParedW(Mapa *mapa, Pacman *pacman, int k) {
    int paredAtropellada = 0;
    if( CheckCollisionRecs(pacman->bumpers.W, mapa->paredes[k].hitbox) && pacman->direccion == 2 && pacman->velocidad != 0 )
        paredAtropellada = 1;
    return paredAtropellada;
}
int pacmanAtropellaParedS(Mapa *mapa, Pacman *pacman, int k) {
    int paredAtropellada = 0;
    if( CheckCollisionRecs(pacman->bumpers.S, mapa->paredes[k].hitbox) && pacman->direccion == 3 && pacman->velocidad != 0 )
        paredAtropellada = 1;
    return paredAtropellada;
}
void actualizarColisionBumpersFantasmas(Mapa *mapa, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        fantasmas[i].bumpers.contactoE = 0;
        fantasmas[i].bumpers.contactoN = 0;
        fantasmas[i].bumpers.contactoW = 0;
        fantasmas[i].bumpers.contactoS = 0;
        for( k = 0 ; k < MAXTILES ; k++ ) {
            if( CheckCollisionRecs(fantasmas[i].bumpers.E, mapa->paredes[k].hitbox) ) fantasmas[i].bumpers.contactoE = 1;
            if( CheckCollisionRecs(fantasmas[i].bumpers.N, mapa->paredes[k].hitbox) ) fantasmas[i].bumpers.contactoN = 1;
            if( CheckCollisionRecs(fantasmas[i].bumpers.W, mapa->paredes[k].hitbox) ) fantasmas[i].bumpers.contactoW = 1;
            if( CheckCollisionRecs(fantasmas[i].bumpers.S, mapa->paredes[k].hitbox) ) fantasmas[i].bumpers.contactoS = 1;
        }
    }
}
void actualizarColisionFantasmasTeletransportadores(Mapa *mapa, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( CheckCollisionRecs(fantasmas[i].hitbox, mapa->teletransportadores[0].hitbox) == 1 )
            teletransportarFantasmasAIzquierda(fantasmas, i);
        if( CheckCollisionRecs(fantasmas[i].hitbox, mapa->teletransportadores[1].hitbox) == 1 )
            teletransportarFantasmasADerecha(fantasmas, i);
    }
}
void teletransportarFantasmasAIzquierda(Fantasma fantasmas[], int i) {
    fantasmas[i].posicion.x = 4 * TILESIZE + TILESIZE / 2;
    fantasmas[i].posicion.y = 10 * TILESIZE + TILESIZE / 2;
}
void teletransportarFantasmasADerecha(Fantasma fantasmas[], int i) {
    fantasmas[i].posicion.x = 28 * TILESIZE + TILESIZE / 2;
    fantasmas[i].posicion.y = 10 * TILESIZE + TILESIZE / 2;
}
//EN PROGRESO
//---------------------------------------------------------------------------------------------------------
/* REFACTORIZADO DE IA
void actualizarIA(Fantasma fantasmas[]) //GENERAR UNA SUBESTRUCTURA DE Fantasma Q SEA IA
{
    //IA COLISIONES + DIRECCION
    for( i = 0 ; i < 4 ; i++ )
    {
        printf("\ntest1");
        fantasmas[i].IA.framesIA++;
        inicializarIAFantasma(fantasmas, i);
        printf("\ntest2");
        contarBumpersDesactivados(fantasmas, i);
        printf("\ntest3");
        if( fantasmas[i].IA.cantidadBumpersEn0 == 1 )
            verificarSalidasXBumpers(fantasmas, i);
        printf("\ntest4");
        if( fantasmas[i].IA.cantidadBumpersEn0 == 2 )
        {
            verificarSalidasXBumpers(fantasmas, i);
            verificarSalidasXDireccion(fantasmas, i);
        }
        printf("\ntest5");
        if( fantasmas[i].IA.cantidadBumpersEn0 == 3 )
        {
            verificarSalidasXBumpers(fantasmas, i);
            verificarSalidasXDireccion(fantasmas, i);
            fantasmas[i].IA.salidasRestantes = 1;
            eliminarUnaSalidaAlAzar(fantasmas, i);
        }
        printf("\ntest6");
        if( fantasmas[i].IA.cantidadBumpersEn0 == 4 )
        {
            verificarSalidasXDireccion(fantasmas, i);
            fantasmas[i].IA.salidasRestantes = 2;
        }
        printf("\ntest7");
        while( fantasmas[i].IA.salidasRestantes > 0 ) eliminarUnaSalidaAlAzar(fantasmas, i);
        seleccionarSalida(fantasmas, i);
        printf("\ntest8");
    }
}

void contarBumpersDesactivados(Fantasma fantasmas[], int i)
{
    fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoE;
    fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoN;
    fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoW;
    fantasmas[i].IA.cantidadBumpersEn0 -= fantasmas[i].bumpers.contactoS;
}

void inicializarIAFantasma(Fantasma fantasmas[], int i)
{   //INICIALIZACION, TODAS LAS SALIDAS DISPONIBLES, TODOS LOS BUMPERS DESACTIVADOS
    fantasmas[i].IA.salidasRestantes = 4;
    fantasmas[i].IA.cantidadBumpersEn0 = 4;
    for( j = 0 ; j < 4 ; j++ ) fantasmas[i].IA.salida[j] = 1;
}

void verificarSalidasXBumpers(Fantasma fantasmas[], int i)
{
    if( fantasmas[i].bumpers.contactoE == 1 ) fantasmas[i].IA.salida[0] = 0;
    if( fantasmas[i].bumpers.contactoN == 1 ) fantasmas[i].IA.salida[1] = 0;
    if( fantasmas[i].bumpers.contactoW == 1 ) fantasmas[i].IA.salida[2] = 0;
    if( fantasmas[i].bumpers.contactoS == 1 ) fantasmas[i].IA.salida[3] = 0;
}

void verificarSalidasXDireccion(Fantasma fantasmas[], int i)
{
    if( fantasmas[i].direccion == 0 ) fantasmas[i].IA.salida[2] = 0;
    if( fantasmas[i].direccion == 1 ) fantasmas[i].IA.salida[3] = 0;
    if( fantasmas[i].direccion == 2 ) fantasmas[i].IA.salida[0] = 0;
    if( fantasmas[i].direccion == 3 ) fantasmas[i].IA.salida[1] = 0;
}

void eliminarUnaSalidaAlAzar(Fantasma fantasmas[], int i)
{   //prueba un maximo arbitrario de 200 veces hasta q elimina una salida
    int azar;
    for( j = 0 ; j < 200 && fantasmas[i].IA.salidasRestantes != 0 ; j++ )
    {
        azar = rand()%4;
        if( fantasmas[i].IA.salida[azar] == 1 )
        {
            fantasmas[i].IA.salida[azar] = 0;
            fantasmas[i].IA.salidasRestantes--;
        }
    }
}

void seleccionarSalida(Fantasma fantasmas[], int i)
{
    for( k = 0 ; k < 4 ; k++ )
    {    //cada 4 frames para q no den giros de 180º
        if( fantasmas[i].IA.salida[k] == 1 && fantasmas[i].IA.framesIA%4 == 0 ) fantasmas[i].direccion = k;
    }
}
*/
//---------------------------------------------------------------------------------------------------------
//EN PROGRESO
void actualizarColisionesScores(Menu *menu) {
    if( IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ) cambiarEstadoAMenu(menu);
}
void actualizarColisionesCreditos(Menu *menu) {
    menu->posicionMouse = GetMousePosition();
    if( IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(menu->posicionMouse, menu->botonBackCreditos.hitbox) )
        menu->estado = 0;
}
void cambiarEstadoAMenu(Menu *menu) {
    menu->posicionMouse = GetMousePosition();
    if( CheckCollisionPointRec(menu->posicionMouse, menu->botonBackScores.hitbox) )
        menu->estado = 0;
}
void actualizarPosicion(Menu *menu, Pacman *pacman, Fantasma fantasmas[]) {
    if( menu->estado == 1 ) actualizarPosicionJuego(pacman, fantasmas);
    if( menu->estado == 5 ) actualizarPosicionCreditos(menu);
}
void actualizarPosicionJuego(Pacman *pacman, Fantasma fantasmas[]) {
    actualizarEnfriamientoPacmanReloco(pacman, fantasmas);
    actualizarEnfriamientoPacmanMuerto(pacman);
    actualizarEnfriamientoFantasmasMuertos(fantasmas);
    if( pacman->muerto == 1 ) {
        reiniciarPosicionPacman(pacman);
        reiniciarPosicionFantasmas(fantasmas);
    }
    if( pacman->muerto == 0 ) {
        actualizarPosicionPacman(pacman);
        actualizarPosicionFantasmas(pacman, fantasmas);
    }
}
void actualizarEnfriamientoPacmanReloco(Pacman *pacman, Fantasma fantasmas[]) {
    if( pacman->enfriamientoReloco > 0 ) pacman->enfriamientoReloco--;
    if( pacman->enfriamientoReloco == 0 ) pacman->reloco = 0;
    for( i = 0 ; i < 4 ; i++ ) fantasmas[i].enfriamientoMiedo = pacman->enfriamientoReloco;
}
void actualizarEnfriamientoPacmanMuerto(Pacman *pacman) {
    if( pacman->enfriamientoMuerto > 0 ) pacman->enfriamientoMuerto--;
    if( pacman->enfriamientoMuerto == 0 ) pacman->muerto = 0;
}
void actualizarEnfriamientoFantasmasMuertos(Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( fantasmas[i].enfriamientoMuerto > 0 ) fantasmas[i].enfriamientoMuerto--;
        if( fantasmas[i].enfriamientoMuerto == 0 ) fantasmas[i].muerto = 0;
    }
}
void actualizarPosicionPacman(Pacman *pacman) {
    actualizarPosicionPacmanXVelocidad(pacman);
    actualizarPosicionHitboxPacman(pacman);
    actualizarPosicionBumpersPacman(pacman);
}
void reiniciarPosicionPacman(Pacman *pacman) {
    pacman->posicion.x = 16 * TILESIZE + TILESIZE / 2;
    pacman->posicion.y = 10 * TILESIZE + TILESIZE / 2;
}
void actualizarPosicionPacmanXVelocidad(Pacman *pacman) {
    if( pacman->velocidad != 0 && pacman->direccion == 0 ) pacman->posicion.x += pacman->velocidad; //derecha
    if( pacman->velocidad != 0 && pacman->direccion == 1 ) pacman->posicion.y -= pacman->velocidad; //arriba
    if( pacman->velocidad != 0 && pacman->direccion == 2 ) pacman->posicion.x -= pacman->velocidad; //izquierda
    if( pacman->velocidad != 0 && pacman->direccion == 3 ) pacman->posicion.y += pacman->velocidad; //abajo
}
void actualizarPosicionHitboxPacman(Pacman *pacman) {
    pacman->hitbox.x = pacman->posicion.x - ( TILESIZE / 2 * 0.9 );
    pacman->hitbox.y = pacman->posicion.y - ( TILESIZE / 2 * 0.9 );
}
void actualizarPosicionBumpersPacman(Pacman *pacman) {
    actualizarPosicionBumperPacmanE(pacman);
    actualizarPosicionBumperPacmanN(pacman);
    actualizarPosicionBumperPacmanW(pacman);
    actualizarPosicionBumperPacmanS(pacman);
}
void actualizarPosicionBumperPacmanE(Pacman *pacman) {
    pacman->bumpers.E.x = pacman->posicion.x + ( TILESIZE / 2 ) * 0.9;
    pacman->bumpers.E.y = pacman->posicion.y - ( TILESIZE / 2 ) * 0.6;
}
void actualizarPosicionBumperPacmanN(Pacman *pacman) {
    pacman->bumpers.N.x = pacman->posicion.x - ( TILESIZE / 2 ) * 0.6;
    pacman->bumpers.N.y = pacman->posicion.y - ( TILESIZE / 2 ) * 1.3;
}
void actualizarPosicionBumperPacmanW(Pacman *pacman) {
    pacman->bumpers.W.x = pacman->posicion.x - ( TILESIZE / 2 ) * 1.3;
    pacman->bumpers.W.y = pacman->posicion.y - ( TILESIZE / 2 ) * 0.6;
}
void actualizarPosicionBumperPacmanS(Pacman *pacman) {
    pacman->bumpers.S.x = pacman->posicion.x - ( TILESIZE / 2 ) * 0.6;
    pacman->bumpers.S.y = pacman->posicion.y + ( TILESIZE / 2 ) * 0.9;
}
void actualizarPosicionFantasmas(Pacman *pacman, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( fantasmas[i].muerto == 1 ) reiniciarPosicionFantasma(fantasmas,  i);
        if( fantasmas[i].muerto == 0 ) actualizarPosicionFantasmaXVelocidad(fantasmas,  i);
        if( pacman->muerto == 1 )      reiniciarPosicionFantasma(fantasmas,  i);
    }
    actualizarPosicionHitboxFantasmas(fantasmas);
    actualizarPosicionBumpersFantasmas(fantasmas);
}
void reiniciarPosicionFantasmas(Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) reiniciarPosicionFantasma(fantasmas, i);
}
void reiniciarPosicionFantasma(Fantasma fantasmas[], int i) {
    fantasmas[i].posicion.x = 15 * TILESIZE + TILESIZE / 2;
    fantasmas[i].posicion.y = 8 * TILESIZE + TILESIZE / 2;
}
void actualizarPosicionFantasmaXVelocidad(Fantasma fantasmas[], int i) {
    if( fantasmas[i].direccion == 0 ) fantasmas[i].posicion.x += fantasmas[i].velocidad; //derecha
    if( fantasmas[i].direccion == 1 ) fantasmas[i].posicion.y -= fantasmas[i].velocidad; //arriba
    if( fantasmas[i].direccion == 2 ) fantasmas[i].posicion.x -= fantasmas[i].velocidad; //izquierda
    if( fantasmas[i].direccion == 3 ) fantasmas[i].posicion.y += fantasmas[i].velocidad; //abajo
}
void actualizarPosicionHitboxFantasmas(Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        fantasmas[i].hitbox.x = fantasmas[i].posicion.x - ( TILESIZE / 2 );
        fantasmas[i].hitbox.y = fantasmas[i].posicion.y - ( TILESIZE / 2 );
    }
}
void actualizarPosicionBumpersFantasmas(Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        actualizarPosicionBumperFantasmaE(fantasmas, i);
        actualizarPosicionBumperFantasmaN(fantasmas, i);
        actualizarPosicionBumperFantasmaW(fantasmas, i);
        actualizarPosicionBumperFantasmaS(fantasmas, i);
    }
}
void actualizarPosicionBumperFantasmaE(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.E.x = fantasmas[i].posicion.x + ( TILESIZE / 2 ) * 0.95;
    fantasmas[i].bumpers.E.y = fantasmas[i].posicion.y - ( TILESIZE / 2 ) * 0.85;
}
void actualizarPosicionBumperFantasmaN(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.N.x = fantasmas[i].posicion.x - ( TILESIZE / 2 ) * 0.85;
    fantasmas[i].bumpers.N.y = fantasmas[i].posicion.y - ( TILESIZE / 2 ) * 1.12;
}
void actualizarPosicionBumperFantasmaW(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.W.x = fantasmas[i].posicion.x - ( TILESIZE / 2 ) * 1.12;
    fantasmas[i].bumpers.W.y = fantasmas[i].posicion.y - ( TILESIZE / 2 ) * 0.85;
}
void actualizarPosicionBumperFantasmaS(Fantasma fantasmas[], int i) {
    fantasmas[i].bumpers.S.x = fantasmas[i].posicion.x - ( TILESIZE / 2 ) * 0.85;
    fantasmas[i].bumpers.S.y = fantasmas[i].posicion.y + ( TILESIZE / 2 ) * 0.95;
}
void actualizarPosicionCreditos(Menu *menu) {
    menu->scrollCreditos.hitbox.y -= 0.6;
    if( menu->scrollCreditos.hitbox.y < -18 * TILESIZE ) menu->scrollCreditos.hitbox.y = TILESIZE * 22;
}
Tile posicionATile(Vector2 punto) {
    Tile tile;
    tile.i = 0;
    tile.j = 0;
    for( i = 0 ; i < COLUMNAS ; i++ )
        if( i * TILESIZE <= punto.x && punto.x < TILESIZE * (i + 1) )
            tile.i = i;
    for( j = 0 ; j < FILAS ; j++ )
        if( j * TILESIZE <= punto.y && punto.y < TILESIZE * (j + 1) )
            tile.j = j;
    return tile;
}
//DIBUJAR
//-------------------------------------------------------------------------------------
void dibujar(Menu menu, Mapa mapa, Pacman pacman, Fantasma fantasmas[], Debug herramientaDebug) {
    BeginDrawing();
    ClearBackground(BLACK);
    //0: menu, 1: juego, 2: GameOver, 3: Scores, 4:SaveScreen
    if( menu.estado == 0 ) dibujarMenu(menu, pacman);
    if( menu.estado == 1 ) dibujarJuego(menu, mapa, pacman, fantasmas);
    if( menu.estado == 2 ) dibujarGameOver(menu);
    if( menu.estado == 3 ) dibujarScore(menu);
    if( menu.estado == 4 ) dibujarWinScreen(menu);
    if( menu.estado == 5 ) dibujarCreditos(menu);
    if( herramientaDebug.mostrarHerramienta == 1 ) {
        dibujarVentanaDebug(herramientaDebug, menu);
        if( menu.estado == 1 ) dibujarHitboxDebug(herramientaDebug, mapa, pacman, fantasmas);
    }
    EndDrawing();
}
void dibujarMenu(Menu menu, Pacman pacman) {
    dibujarFondoMenu();
    dibujarBotones(menu, pacman);
}
void dibujarFondoMenu() {
    DrawRectangle(0 * TILESIZE, 0 * TILESIZE, COLUMNAS * TILESIZE, FILAS * TILESIZE, YELLOW);
}
void dibujarBotones(Menu menu, Pacman pacman) {
    dibujarBotonPlay(menu);
    dibujarBotonScore(menu);
    dibujarBotonVidas(menu, pacman);
    dibujarBotonCreditos(menu);
}
void dibujarBotonPlay(Menu menu) {
    DrawRectangle(menu.botonPlay.hitbox.x, menu.botonPlay.hitbox.y, menu.botonPlay.hitbox.width, menu.botonPlay.hitbox.height, BLACK);
    DrawRectangleLines(menu.botonPlay.hitbox.x, menu.botonPlay.hitbox.y, menu.botonPlay.hitbox.width, menu.botonPlay.hitbox.height, LIGHTGRAY);
    DrawText(TextFormat("   PLAY"), menu.botonPlay.hitbox.x, menu.botonPlay.hitbox.y, TILESIZE * 2.36, RAYWHITE);
}
void dibujarBotonScore(Menu menu) {
    DrawRectangle(menu.botonScore.hitbox.x, menu.botonScore.hitbox.y, menu.botonScore.hitbox.width, menu.botonScore.hitbox.height, BLACK);
    DrawRectangleLines(menu.botonScore.hitbox.x, menu.botonScore.hitbox.y, menu.botonScore.hitbox.width, menu.botonScore.hitbox.height, LIGHTGRAY);
    DrawText(TextFormat("   SCORE: %10i", menu.scoreYNombre.score), menu.botonScore.hitbox.x, menu.botonScore.hitbox.y, 2 * TILESIZE, RAYWHITE);
}
void dibujarBotonVidas(Menu menu, Pacman pacman) {
    DrawRectangle(menu.botonVidas.hitbox.x, menu.botonVidas.hitbox.y, menu.botonVidas.hitbox.width, menu.botonVidas.hitbox.height, BLACK);
    DrawRectangleLines(menu.botonVidas.hitbox.x, menu.botonVidas.hitbox.y, menu.botonVidas.hitbox.width, menu.botonVidas.hitbox.height, LIGHTGRAY);
    DrawText(TextFormat("   VIDAS: %i", tope(&pacman.vidas)), menu.botonVidas.hitbox.x, menu.botonVidas.hitbox.y, 2 * TILESIZE, RAYWHITE);
}
void dibujarBotonCreditos(Menu menu) {
    DrawRectangle(menu.botonCreditos.hitbox.x, menu.botonCreditos.hitbox.y, menu.botonCreditos.hitbox.width, menu.botonCreditos.hitbox.height, BLACK);
    DrawRectangleLines(menu.botonCreditos.hitbox.x, menu.botonCreditos.hitbox.y, menu.botonCreditos.hitbox.width, menu.botonCreditos.hitbox.height, LIGHTGRAY);
    DrawText(TextFormat("      CREDITOS"), menu.botonCreditos.hitbox.x, menu.botonCreditos.hitbox.y, 2 * TILESIZE, RAYWHITE);
}
void dibujarJuego(Menu menu, Mapa mapa, Pacman pacman, Fantasma fantasmas[]) {
    dibujarTexturasMapa(mapa);
    dibujarInterfazJuego(menu, pacman);
    dibujarFrutitas(mapa);
    dibujarFrutasBonus(mapa);
    if( mapa.itemBonus.existe == 1 ) dibujarItemBonus(mapa);
    dibujarPacman(pacman);
    dibujarFantasmas(fantasmas);
}
void dibujarTexturasMapa(Mapa mapa) {
    for( k = 0; k < MAXTILES ; k++)
        DrawTexture(mapa.texturas[mapa.indexTextura[k]], mapa.tile[k].j * TILESIZE, mapa.tile[k].i * TILESIZE, WHITE);
}
void dibujarInterfazJuego(Menu menu, Pacman pacman) {
    DrawText(TextFormat("Score: %10i", menu.scoreYNombre.score), 5 * TILESIZE, -0.037 * TILESIZE, 1.158 * TILESIZE, WHITE);
    DrawText(TextFormat("Vidas: %i", tope(&pacman.vidas) ), 23 * TILESIZE, 0.006 * TILESIZE, 1.158 * TILESIZE, WHITE);
}
void dibujarFrutitas(Mapa mapa) {
    for( k = 0 ; k < MAXTILES ; k++ )
        if( mapa.frutitas[k].existe == 1 ) DrawCircle(mapa.frutitas[k].hitbox.x, mapa.frutitas[k].hitbox.y, TILESIZE/6, GREEN);
}
void dibujarFrutasBonus(Mapa mapa) {
    for( k = 0 ; k < MAXTILES ; k++ )
        if( mapa.frutasBonus[k].existe == 1 )
            DrawCircle(mapa.frutasBonus[k].hitbox.x, mapa.frutasBonus[k].hitbox.y, TILESIZE/4, RED);
}
void dibujarItemBonus(Mapa mapa) {
    if( mapa.tipoItemBonus == 0 )
        DrawTexture(mapa.texturas[mapa.frameAnimacion + 24], mapa.itemBonus.hitbox.x - TILESIZE / 2, mapa.itemBonus.hitbox.y - TILESIZE / 2, WHITE);
    if( mapa.tipoItemBonus == 1 )
        DrawTexture(mapa.texturas[mapa.frameAnimacion + 30], mapa.itemBonus.hitbox.x - TILESIZE / 2, mapa.itemBonus.hitbox.y - TILESIZE / 2, WHITE);
    if( mapa.tipoItemBonus == 2 )
        DrawTexture(mapa.texturas[mapa.frameAnimacion + 36], mapa.itemBonus.hitbox.x - TILESIZE / 2, mapa.itemBonus.hitbox.y - TILESIZE / 2, WHITE);
}
void dibujarPacman(Pacman pacman) {
    Vector2 bufferPosicionPacman;
    bufferPosicionPacman.x = pacman.posicion.x - TILESIZE / 2;
    bufferPosicionPacman.y = pacman.posicion.y - TILESIZE / 2;
    for( i = 0 ; i < 4 ; i++ )
        if( pacman.direccion == i ) {
            if( pacman.direccion == 1 || pacman.direccion == 2 ) bufferPosicionPacman.y = bufferPosicionPacman.y + TILESIZE;
            if( pacman.direccion == 3 || pacman.direccion == 2 ) bufferPosicionPacman.x = bufferPosicionPacman.x + TILESIZE;
            if( pacman.reloco == 0 && pacman.direccion != 2 )
                DrawTextureEx(pacman.texturas[pacman.frameAnimacion], bufferPosicionPacman, 90 * -i, 1, WHITE);
            if( pacman.reloco == 1 && pacman.direccion != 2 )
                DrawTextureEx(pacman.texturas[pacman.frameAnimacion], bufferPosicionPacman, 90 * -i, 1, RED);
            if( pacman.reloco == 0 && pacman.direccion == 2 ) {
                bufferPosicionPacman.x = bufferPosicionPacman.x - TILESIZE;
                bufferPosicionPacman.y = bufferPosicionPacman.y - TILESIZE;
                DrawTextureEx(pacman.texturas[pacman.frameAnimacion + 6], bufferPosicionPacman, 0, 1, WHITE);
            }
            if( pacman.reloco == 1 && pacman.direccion == 2 ) {
                bufferPosicionPacman.x = bufferPosicionPacman.x - TILESIZE;
                bufferPosicionPacman.y = bufferPosicionPacman.y - TILESIZE;
                DrawTextureEx(pacman.texturas[pacman.frameAnimacion + 6], bufferPosicionPacman, 0, 1, RED);
            }
        }
}
void dibujarFantasmas(Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( fantasmas[i].direccion == 0 ) dibujarFantasmaDerecha(fantasmas, i);
        if( fantasmas[i].direccion == 1 ) dibujarFantasmasArriba(fantasmas, i);
        if( fantasmas[i].direccion == 2 ) dibujarFantasmasIzquierda(fantasmas, i);
        if( fantasmas[i].direccion == 3 ) dibujarFantasmasAbajo(fantasmas, i);
    }
}
void dibujarFantasmaDerecha(Fantasma fantasmas[], int i) {
    Vector2 bufferPosicionFantasma;
    bufferPosicionFantasma.x = fantasmas[i].posicion.x - TILESIZE / 2;
    bufferPosicionFantasma.y = fantasmas[i].posicion.y - TILESIZE / 2;
    if( fantasmas[i].enfriamientoMiedo == 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
    if( fantasmas[i].enfriamientoMiedo > 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion + 12], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
}
void dibujarFantasmasArriba(Fantasma fantasmas[], int i) {
    Vector2 bufferPosicionFantasma;
    bufferPosicionFantasma.x = fantasmas[i].posicion.x - TILESIZE / 2;
    bufferPosicionFantasma.y = fantasmas[i].posicion.y - TILESIZE / 2;
    if( fantasmas[i].enfriamientoMiedo == 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion + 3], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
    if( fantasmas[i].enfriamientoMiedo > 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion + 12], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
}
void dibujarFantasmasIzquierda(Fantasma fantasmas[], int i) {
    Vector2 bufferPosicionFantasma;
    bufferPosicionFantasma.x = fantasmas[i].posicion.x - TILESIZE / 2;
    bufferPosicionFantasma.y = fantasmas[i].posicion.y - TILESIZE / 2;
    if( fantasmas[i].enfriamientoMiedo == 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion + 6], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
    if( fantasmas[i].enfriamientoMiedo > 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion + 12], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
}
void dibujarFantasmasAbajo(Fantasma fantasmas[], int i) {
    Vector2 bufferPosicionFantasma;
    bufferPosicionFantasma.x = fantasmas[i].posicion.x - TILESIZE / 2;
    bufferPosicionFantasma.y = fantasmas[i].posicion.y - TILESIZE / 2;
    if( fantasmas[i].enfriamientoMiedo == 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion + 9], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
    if( fantasmas[i].enfriamientoMiedo > 0 )
        DrawTexture(fantasmas[i].texturas[fantasmas[i].frameAnimacion + 12], bufferPosicionFantasma.x, bufferPosicionFantasma.y, WHITE);
}
void dibujarGameOver(Menu menu) {
    DrawText(TextFormat("PAC - MAN"), 5.5 * TILESIZE, 4 * TILESIZE, 4 * TILESIZE, YELLOW);
    DrawText(TextFormat("GAME OVER"), 9.5 * TILESIZE,  9 * TILESIZE, 2 * TILESIZE, RED);
    DrawText(TextFormat("SCORE %8i", menu.scoreYNombre.score), 8.5 * TILESIZE, 12 * TILESIZE, 2 * TILESIZE, RAYWHITE);
    DrawText(TextFormat("presione ENTER para guardar su Score"), 5.5 * TILESIZE, 15 * TILESIZE, TILESIZE, RAYWHITE);
}
void dibujarScore(Menu menu) {
    guardarScoreEnArray(&menu);
    DrawText(TextFormat("Scores:"), 6 * TILESIZE, TILESIZE * 3, 45, RED);
    DrawRectangle(menu.botonBackScores.hitbox.x, menu.botonBackScores.hitbox.y, menu.botonBackScores.hitbox.width, menu.botonBackScores.hitbox.height, YELLOW);
    DrawText("BACK",menu.botonBackScores.hitbox.x * 1.07, menu.botonBackScores.hitbox.y * 1.012, TILESIZE * 1.5, BLACK);
    for( i = 0 ; i < 5  ; i++  )
        DrawText(TextFormat("%s: %i", menu.bufferScores[i].nombre, menu.bufferScores[i].score), TILESIZE * 6, TILESIZE * ( 6 + ( 2 * i ) ), 45, RAYWHITE);
}
void dibujarWinScreen(Menu menu) {
    dibujarGanasteYScore(menu);
    dibujarIngreseNombreParaGuardar();
    dibujarNombreDinamico(menu);
    if( menu.switchMarcadorPosicion == 0 && menu.scoreGuardado == 0 ) dibujarMarcadorPosicionMenuGuardado(menu);
    if( menu.scoreGuardado == 1 ) dibujarScoreGuardadoCorrectamente();
    if( menu.botonVolverAJugar.existe == 1 ) dibujarPresioneAqui(menu);
}
void dibujarGanasteYScore(Menu menu) {
    DrawText(TextFormat("GANASTE"), 11.15 * TILESIZE, 2 * TILESIZE, 2 * TILESIZE, RED);
    DrawText(TextFormat("SCORE %8i", menu.scoreYNombre.score), 8.4 * TILESIZE, 5 * TILESIZE, 2 * TILESIZE, RAYWHITE);
}
void dibujarIngreseNombreParaGuardar() {
    DrawRectangle(7 * TILESIZE, 12 * TILESIZE, 18 * TILESIZE, 2 * TILESIZE, DARKGRAY);
    DrawText(TextFormat("INGRESE UN NOMBRE PARA"), 8.1 * TILESIZE, 12 * TILESIZE, 36, RAYWHITE);
    DrawText(TextFormat("GUARDAR SU PUNTAJE"), 9.25 * TILESIZE, 13 * TILESIZE, 36, RAYWHITE);
}
void dibujarNombreDinamico(Menu menu) {
    DrawRectangle(7 * TILESIZE, 16 * TILESIZE, 18 * TILESIZE, 2 * TILESIZE, YELLOW);
    DrawText(TextFormat("%s", menu.scoreYNombre.nombre), 7 * TILESIZE, 15.8 * TILESIZE, 2.5 * TILESIZE, RED);
}
void dibujarMarcadorPosicionMenuGuardado(Menu menu) {
    DrawText("_", 7 * TILESIZE + 8 + MeasureText(menu.scoreYNombre.nombre, 2.5 * TILESIZE), 15.8 * TILESIZE, 2.5 * TILESIZE, RED);
}
void dibujarScoreGuardadoCorrectamente() {
    DrawRectangle(7 * TILESIZE, 19 * TILESIZE, 18 * TILESIZE, 2 * TILESIZE, DARKGRAY);
    DrawText(TextFormat("SCORE GUARDADO"), 11.5 * TILESIZE, 19 * TILESIZE, TILESIZE, GREEN);
    DrawText(TextFormat("CORRECTAMENTE"), 11.5 * TILESIZE, 20 * TILESIZE, TILESIZE, GREEN);
}
void dibujarPresioneAqui(Menu menu) {
    DrawRectangle(menu.botonVolverAJugar.hitbox.x, menu.botonVolverAJugar.hitbox.y, menu.botonVolverAJugar.hitbox.width, menu.botonVolverAJugar.hitbox.height, WHITE);
    DrawText("Presione AQUI", menu.botonVolverAJugar.hitbox.x * 1.195, menu.botonVolverAJugar.hitbox.y * 0.996, TILESIZE, RED);
    DrawText("para volver a jugar", menu.botonVolverAJugar.hitbox.x * 1.07, menu.botonVolverAJugar.hitbox.y * 1.07, TILESIZE, RED);
}
void dibujarCreditos(Menu menu) {
    dibujarScrollCreditos(menu);
    dibujarBotonBackCreditos(menu);
}
void dibujarBotonBackCreditos(Menu menu) {
    DrawRectangle(menu.botonBackScores.hitbox.x, menu.botonBackScores.hitbox.y, menu.botonBackScores.hitbox.width, menu.botonBackScores.hitbox.height, YELLOW);
    DrawText("BACK", menu.botonBackScores.hitbox.x * 1.07, menu.botonBackScores.hitbox.y * 1.012, TILESIZE * 1.5, BLACK);
}
void dibujarScrollCreditos(Menu menu) {
    DrawText("Desarrolladores:", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y, TILESIZE * 2, RED);
    DrawText("Ortiz Rocio", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 2 * TILESIZE, TILESIZE * 2, WHITE);
    DrawText("Ortiz Brisa", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 4 * TILESIZE, TILESIZE * 2, WHITE);
    DrawText("Olmos Fernando", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 6 * TILESIZE, TILESIZE * 2, WHITE);
    DrawText("Gonzalez Matias", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 8 * TILESIZE, TILESIZE * 2, WHITE);
    DrawText("Docentes y ayudantes:", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 10 * TILESIZE, TILESIZE * 2, RED);
    DrawText("Lucrecia Bazan", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 12 * TILESIZE, TILESIZE * 2, WHITE);
    DrawText("Hilario Monti", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 14 * TILESIZE, TILESIZE * 2, WHITE);
    DrawText("Nicanor Dondero", menu.scrollCreditos.hitbox.x, menu.scrollCreditos.hitbox.y + 16 * TILESIZE, TILESIZE * 2, WHITE);
}
//-------------------------------------------------------------------------------------
//BUCLE PRINCIPAL
//AUX
void mostrarMatrizInt(int matrizInt[][COLUMNAS], int m, int n) {
    printf("\n");
    for( i = 0 ; i < m ; i++ ){
        printf("|");
        for( j = 0; j < n ; j++ )
            printf(" %i", matrizInt[i][j]);
        printf(" |\n");
    }
}
void dibujarFondoMenuAlternativo() {
    Color color;
    char colorFondo[4] = { 120, 30, 120, 255 };
    for( i = 0 ; i < FILAS ; i++ )
        for( j = 0 ; j < COLUMNAS ; j++ ) {
            color.r = colorFondo[0] + ( i * 2 );
            color.g = colorFondo[1] + ( j * 4 );
            color.b = colorFondo[2];
            color.a = colorFondo[3];
            DrawRectangle(j*TILESIZE, i*TILESIZE, TILESIZE, TILESIZE, color);
        }
}
//DEBUG NUMPAD
//-------------------------------------------------------------------------------------
void inicializarDebug(Debug *herramientaDebug) {
    herramientaDebug->mostrarHerramienta = 0;
    inicializarVentanaDebug(herramientaDebug);
    inicializarBotonesDebug(herramientaDebug);
    inicializarValoresVariablesDebug(herramientaDebug);
    inicializarColoresDebug(herramientaDebug);
}
void inicializarVentanaDebug(Debug *herramientaDebug) {
    herramientaDebug->ventana.x = 0;
    herramientaDebug->ventana.y = 0;
    herramientaDebug->ventana.width = 96;
    herramientaDebug->ventana.height = 124;
}
void inicializarBotonesDebug(Debug *herramientaDebug) {
    inicializarBotonesNumericosDebug(herramientaDebug);
    inicializarBotonMenosDebug(herramientaDebug);
    inicializarBotonMasDebug(herramientaDebug);
}
void inicializarBotonesNumericosDebug(Debug *herramientaDebug){
    inicializarCeroDebug(herramientaDebug);
    int numBoton = 1;
    for( i = 0 ; i < 3 ; i++ ) //inicializa 1 al 9
        for( j = 0 ; j < 3 ; j++ ) {
            herramientaDebug->botonVariable[numBoton].x = 8 + ( 28 * j ) ;
            herramientaDebug->botonVariable[numBoton].y = 64 - ( 28 * i );
            herramientaDebug->botonVariable[numBoton].width = 24;
            herramientaDebug->botonVariable[numBoton].height = 24;
            numBoton++;
        }
}
void inicializarCeroDebug(Debug *herramientaDebug) {
    herramientaDebug->botonVariable[0].x = 8;
    herramientaDebug->botonVariable[0].y = 92;
    herramientaDebug->botonVariable[0].width = 24;
    herramientaDebug->botonVariable[0].height = 24;
}
void inicializarBotonMenosDebug(Debug *herramientaDebug) {
    herramientaDebug->botonVariable[10].x = 36 + ( 10 - 10 );
    herramientaDebug->botonVariable[10].y = 92;
    herramientaDebug->botonVariable[10].width = 24;
    herramientaDebug->botonVariable[10].height = 24;
}
void inicializarBotonMasDebug(Debug *herramientaDebug) {
    herramientaDebug->botonVariable[11].x = 64;
    herramientaDebug->botonVariable[11].y = 92;
    herramientaDebug->botonVariable[11].width = 24;
    herramientaDebug->botonVariable[11].height = 24;
}
void inicializarValoresVariablesDebug(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++) herramientaDebug->valorVariable[i] = 1;
}
void inicializarColoresDebug(Debug *herramientaDebug) {
    char coloresRGBA[5][4] = {
        { 212, 242, 239, 210 },   //0: VERYLIGHTTEAL
        { 180, 233, 231, 210 },   //1: LIGHTTEAL
        { 127, 214, 213, 210 },   //1: TEAL
        {  32, 186, 181, 210 },   //2: DARKTEAL
        {   1, 140, 133, 210 } }; //3: VERYDARKTEAL
    inicializarColorVLTeal(herramientaDebug, coloresRGBA);
    inicializarColorLTeal(herramientaDebug, coloresRGBA);
    inicializarColorTeal(herramientaDebug, coloresRGBA);
    inicializarColorDTeal(herramientaDebug, coloresRGBA);
    inicializarColorVDTeal(herramientaDebug, coloresRGBA);
}
void inicializarColorVLTeal(Debug *herramientaDebug, char coloresRGBA[][4]) {
    herramientaDebug->color.veryLightTeal.r = coloresRGBA[0][0];
    herramientaDebug->color.veryLightTeal.g = coloresRGBA[0][1];
    herramientaDebug->color.veryLightTeal.b = coloresRGBA[0][2];
    herramientaDebug->color.veryLightTeal.a = coloresRGBA[0][3];
}
void inicializarColorLTeal(Debug *herramientaDebug, char coloresRGBA[][4]) {
    herramientaDebug->color.lightTeal.r = coloresRGBA[1][0];
    herramientaDebug->color.lightTeal.g = coloresRGBA[1][1];
    herramientaDebug->color.lightTeal.b = coloresRGBA[1][2];
    herramientaDebug->color.lightTeal.a = coloresRGBA[1][3];
}
void inicializarColorTeal(Debug *herramientaDebug, char coloresRGBA[][4]) {
    herramientaDebug->color.teal.r = coloresRGBA[2][0];
    herramientaDebug->color.teal.g = coloresRGBA[2][1];
    herramientaDebug->color.teal.b = coloresRGBA[2][2];
    herramientaDebug->color.teal.a = coloresRGBA[2][3];
}
void inicializarColorDTeal(Debug *herramientaDebug, char coloresRGBA[][4]) {
    herramientaDebug->color.darkTeal.r = coloresRGBA[3][0];
    herramientaDebug->color.darkTeal.g = coloresRGBA[3][1];
    herramientaDebug->color.darkTeal.b = coloresRGBA[3][2];
    herramientaDebug->color.darkTeal.a = coloresRGBA[3][3];
}
void inicializarColorVDTeal(Debug *herramientaDebug, char coloresRGBA[][4]) {
    herramientaDebug->color.veryDarkTeal.r = coloresRGBA[4][0];
    herramientaDebug->color.veryDarkTeal.g = coloresRGBA[4][1];
    herramientaDebug->color.veryDarkTeal.b = coloresRGBA[4][2];
    herramientaDebug->color.veryDarkTeal.a = coloresRGBA[4][3];
}
void actualizarDebug(Debug *herramientaDebug, Menu *menu) {
    actualizarInputDebug(herramientaDebug);
    actualizarTestDebug(herramientaDebug, menu);
}
void actualizarInputDebug(Debug *herramientaDebug) {
    if( IsKeyReleased(KEY_H) )
        herramientaDebug->mostrarHerramienta = herramientaDebug->mostrarHerramienta == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_DECIMAL) ) mostrarValoresVariables(herramientaDebug);
    actualizarValoresVariables(herramientaDebug);
    actualizarInterruptoresVariables(herramientaDebug);
}
void mostrarValoresVariables(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ ) printf("\nVariable %i: %.3f", i, herramientaDebug->valorVariable[i]);
}
void actualizarInterruptoresVariables(Debug *herramientaDebug) {
    if( IsKeyReleased(KEY_KP_0) )
        herramientaDebug->variableActivada[0] = herramientaDebug->variableActivada[0] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_1) )
        herramientaDebug->variableActivada[1] = herramientaDebug->variableActivada[1] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_2) )
        herramientaDebug->variableActivada[2] = herramientaDebug->variableActivada[2] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_3) )
        herramientaDebug->variableActivada[3] = herramientaDebug->variableActivada[3] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_4) )
        herramientaDebug->variableActivada[4] = herramientaDebug->variableActivada[4] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_5) )
        herramientaDebug->variableActivada[5] = herramientaDebug->variableActivada[5] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_6) )
        herramientaDebug->variableActivada[6] = herramientaDebug->variableActivada[6] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_7) )
        herramientaDebug->variableActivada[7] = herramientaDebug->variableActivada[7] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_8) )
        herramientaDebug->variableActivada[8] = herramientaDebug->variableActivada[8] == 1 ? 0 : 1 ;
    if( IsKeyReleased(KEY_KP_9) )
        herramientaDebug->variableActivada[9] = herramientaDebug->variableActivada[9] == 1 ? 0 : 1 ;
}
void actualizarValoresVariables(Debug *herramientaDebug) {
    herramientaDebug->variableActivada[10] = 0;
    herramientaDebug->variableActivada[11] = 0;
    if( IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_KP_ADD) )
        incrementarMuchoValorVariable(herramientaDebug);
    if( IsKeyDown(KEY_KP_ADD) && !IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_LEFT_ALT) )
        incrementarValorVariable(herramientaDebug);
    if( IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_KP_ADD) )
        incrementarPocoValorVariable(herramientaDebug);
    if( IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_KP_SUBTRACT) )
        decrementarPocoValorVariable(herramientaDebug);
    if( IsKeyDown(KEY_KP_SUBTRACT) && !IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_LEFT_ALT) )
        decrementarValorVariable(herramientaDebug);
    if( IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_KP_SUBTRACT) )
        decrementarMuchoValorVariable(herramientaDebug);
}
void incrementarMuchoValorVariable(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ )
        if( herramientaDebug->variableActivada[i] == 1 ) {
            herramientaDebug->valorVariable[i] = herramientaDebug->valorVariable[i] + 0.1;
            herramientaDebug->variableActivada[11] = 1;
        }
}
void incrementarPocoValorVariable(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ )
        if( herramientaDebug->variableActivada[i] == 1 ) {
            herramientaDebug->valorVariable[i] = herramientaDebug->valorVariable[i] + 0.001;
            herramientaDebug->variableActivada[11] = 1;
        }
}
void incrementarValorVariable(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ )
        if( herramientaDebug->variableActivada[i] == 1 ) {
            herramientaDebug->valorVariable[i] = herramientaDebug->valorVariable[i] + 0.01;
            herramientaDebug->variableActivada[11] = 1;
        }
}
void decrementarMuchoValorVariable(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ )
        if( herramientaDebug->variableActivada[i] == 1 ) {
            herramientaDebug->valorVariable[i] = herramientaDebug->valorVariable[i] - 0.1;
            herramientaDebug->variableActivada[10] = 1;
        }
}
void decrementarPocoValorVariable(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ )
        if( herramientaDebug->variableActivada[i] == 1 ) {
            herramientaDebug->valorVariable[i] = herramientaDebug->valorVariable[i] - 0.001;
            herramientaDebug->variableActivada[10] = 1;
        }
}
void decrementarValorVariable(Debug *herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ )
        if( herramientaDebug->variableActivada[i] == 1 ) {
            herramientaDebug->valorVariable[i] = herramientaDebug->valorVariable[i] - 0.01;
            herramientaDebug->variableActivada[10] = 1;
        }
}
void dibujarVentanaDebug(Debug herramientaDebug, Menu menu) {
    DrawRectangle(herramientaDebug.ventana.x,herramientaDebug.ventana.y,herramientaDebug.ventana.width,herramientaDebug.ventana.height,herramientaDebug.color.veryDarkTeal);
    dibujarBotonesNumericosDebug(herramientaDebug);
    dibujarBotonesSumaYResta(herramientaDebug);
    dibujarTestDebug(herramientaDebug, menu);
}
void dibujarBotonesNumericosDebug(Debug herramientaDebug) {
    for( i = 0 ; i < 10 ; i++ ) {
        if(herramientaDebug.variableActivada[i] == 1 )
            DrawRectangle(herramientaDebug.botonVariable[i].x,herramientaDebug.botonVariable[i].y,herramientaDebug.botonVariable[i].width,herramientaDebug.botonVariable[i].height,herramientaDebug.color.veryLightTeal);
        if(herramientaDebug.variableActivada[i] == 0 )
            DrawRectangle(herramientaDebug.botonVariable[i].x,herramientaDebug.botonVariable[i].y,herramientaDebug.botonVariable[i].width,herramientaDebug.botonVariable[i].height,herramientaDebug.color.teal);
        DrawText(TextFormat("%i", i),herramientaDebug.botonVariable[i].x + 4,herramientaDebug.botonVariable[i].y, 28,herramientaDebug.color.veryDarkTeal);
    }
}
void dibujarBotonesSumaYResta(Debug herramientaDebug) {
    if(herramientaDebug.variableActivada[10] == 1 )
        DrawRectangle(herramientaDebug.botonVariable[10].x,herramientaDebug.botonVariable[10].y,herramientaDebug.botonVariable[10].width,herramientaDebug.botonVariable[10].height,herramientaDebug.color.veryLightTeal);
    if(herramientaDebug.variableActivada[10] == 0 )
        DrawRectangle(herramientaDebug.botonVariable[10].x,herramientaDebug.botonVariable[10].y,herramientaDebug.botonVariable[10].width,herramientaDebug.botonVariable[10].height,herramientaDebug.color.teal);
    if(herramientaDebug.variableActivada[11] == 1 )
        DrawRectangle(herramientaDebug.botonVariable[11].x,herramientaDebug.botonVariable[11].y,herramientaDebug.botonVariable[11].width,herramientaDebug.botonVariable[11].height,herramientaDebug.color.veryLightTeal);
    if(herramientaDebug.variableActivada[11] == 0 )
        DrawRectangle(herramientaDebug.botonVariable[11].x,herramientaDebug.botonVariable[11].y,herramientaDebug.botonVariable[11].width,herramientaDebug.botonVariable[11].height,herramientaDebug.color.teal);
    DrawText(TextFormat("-"),herramientaDebug.botonVariable[10].x + 6,herramientaDebug.botonVariable[10].y, 28,herramientaDebug.color.veryDarkTeal);
    DrawText(TextFormat("+"),herramientaDebug.botonVariable[11].x + 5,herramientaDebug.botonVariable[11].y, 28,herramientaDebug.color.veryDarkTeal);
}
void dibujarHitboxDebug(Debug herramientaDebug, Mapa mapa, Pacman pacman, Fantasma fantasmas[]) {
    dibujarHitboxParedes(herramientaDebug, mapa);
    dibujarHitboxPacman(herramientaDebug, pacman);
    dibujarHitboxFantasmas(herramientaDebug, fantasmas);
}
void dibujarHitboxParedes(Debug herramientaDebug, Mapa mapa) {
    for( k = 0 ; k < MAXTILES ; k++ )
        if( mapa.paredes[k].existe == 1 )
            DrawRectangleLines( mapa.paredes[k].hitbox.x, mapa.paredes[k].hitbox.y, mapa.paredes[k].hitbox.width, mapa.paredes[k].hitbox.height, WHITE);
}
void dibujarHitboxPacman(Debug herramientaDebug, Pacman pacman) {
    DrawRectangleLines(pacman.hitbox.x, pacman.hitbox.y, pacman.hitbox.width, pacman.hitbox.height, WHITE);
    dibujarBumpersPacman(herramientaDebug, pacman);
}
void dibujarBumpersPacman(Debug herramientaDebug, Pacman pacman) {
    dibujarBumperPacmanE(herramientaDebug, pacman);
    dibujarBumperPacmanN(herramientaDebug, pacman);
    dibujarBumperPacmanW(herramientaDebug, pacman);
    dibujarBumperPacmanS(herramientaDebug, pacman);
}
void dibujarBumperPacmanE(Debug herramientaDebug, Pacman pacman) {
    if( pacman.bumpers.contactoE == 0 )
        DrawRectangle(pacman.bumpers.E.x, pacman.bumpers.E.y, pacman.bumpers.E.width, pacman.bumpers.E.height, BLACK);
    if( pacman.bumpers.contactoE == 1 )
        DrawRectangle(pacman.bumpers.E.x, pacman.bumpers.E.y, pacman.bumpers.E.width, pacman.bumpers.E.height, RED);
}
void dibujarBumperPacmanN(Debug herramientaDebug, Pacman pacman) {
    if( pacman.bumpers.contactoN == 0 )
        DrawRectangle(pacman.bumpers.N.x, pacman.bumpers.N.y, pacman.bumpers.N.width, pacman.bumpers.N.height, BLACK);
    if( pacman.bumpers.contactoN == 1 )
        DrawRectangle(pacman.bumpers.N.x, pacman.bumpers.N.y, pacman.bumpers.N.width, pacman.bumpers.N.height, RED);
}
void dibujarBumperPacmanW(Debug herramientaDebug, Pacman pacman) {
    if( pacman.bumpers.contactoW == 0 )
        DrawRectangle(pacman.bumpers.W.x, pacman.bumpers.W.y, pacman.bumpers.W.width, pacman.bumpers.W.height, BLACK);
    if( pacman.bumpers.contactoW == 1 )
        DrawRectangle(pacman.bumpers.W.x, pacman.bumpers.W.y, pacman.bumpers.W.width, pacman.bumpers.W.height, RED);
}
void dibujarBumperPacmanS(Debug herramientaDebug, Pacman pacman) {
    if( pacman.bumpers.contactoS == 0 )
        DrawRectangle(pacman.bumpers.S.x, pacman.bumpers.S.y, pacman.bumpers.S.width, pacman.bumpers.S.height, BLACK);
    if( pacman.bumpers.contactoS == 1 )
        DrawRectangle(pacman.bumpers.S.x, pacman.bumpers.S.y, pacman.bumpers.S.width, pacman.bumpers.S.height, RED);
}
void dibujarHitboxFantasmas(Debug herramientaDebug, Fantasma fantasmas[]) {
    dibujarHitboxLinesFantasmas(herramientaDebug, fantasmas);
    dibujarBumpersFantasmas(herramientaDebug, fantasmas);
}
void dibujarHitboxLinesFantasmas(Debug herramientaDebug, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ )
        DrawRectangleLines(fantasmas[i].hitbox.x, fantasmas[i].hitbox.y, fantasmas[i].hitbox.width, fantasmas[i].hitbox.height, WHITE);
}
void dibujarBumpersFantasmas(Debug herramientaDebug, Fantasma fantasmas[]) {
    dibujarBumperFantasmaE(herramientaDebug, fantasmas);
    dibujarBumperFantasmaN(herramientaDebug, fantasmas);
    dibujarBumperFantasmaW(herramientaDebug, fantasmas);
    dibujarBumperFantasmaS(herramientaDebug, fantasmas);
}
void dibujarBumperFantasmaE(Debug herramientaDebug, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( fantasmas[i].bumpers.contactoE == 0 )
            DrawRectangle(fantasmas[i].bumpers.E.x, fantasmas[i].bumpers.E.y, fantasmas[i].bumpers.E.width, fantasmas[i].bumpers.E.height, BLACK);
        if( fantasmas[i].bumpers.contactoE == 1 )
            DrawRectangle(fantasmas[i].bumpers.E.x, fantasmas[i].bumpers.E.y, fantasmas[i].bumpers.E.width, fantasmas[i].bumpers.E.height, RED);
    }
}
void dibujarBumperFantasmaN(Debug herramientaDebug, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( fantasmas[i].bumpers.contactoN == 0 )
            DrawRectangle(fantasmas[i].bumpers.N.x, fantasmas[i].bumpers.N.y, fantasmas[i].bumpers.N.width, fantasmas[i].bumpers.N.height, BLACK);
        if( fantasmas[i].bumpers.contactoN == 1 )
            DrawRectangle(fantasmas[i].bumpers.N.x, fantasmas[i].bumpers.N.y, fantasmas[i].bumpers.N.width, fantasmas[i].bumpers.N.height, RED);
    }
}
void dibujarBumperFantasmaW(Debug herramientaDebug, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( fantasmas[i].bumpers.contactoW == 0 )
            DrawRectangle(fantasmas[i].bumpers.W.x, fantasmas[i].bumpers.W.y, fantasmas[i].bumpers.W.width, fantasmas[i].bumpers.W.height, BLACK);
        if( fantasmas[i].bumpers.contactoW == 1 )
            DrawRectangle(fantasmas[i].bumpers.W.x, fantasmas[i].bumpers.W.y, fantasmas[i].bumpers.W.width, fantasmas[i].bumpers.W.height, RED);
    }
}
void dibujarBumperFantasmaS(Debug herramientaDebug, Fantasma fantasmas[]) {
    for( i = 0 ; i < 4 ; i++ ) {
        if( fantasmas[i].bumpers.contactoS == 0 )
            DrawRectangle(fantasmas[i].bumpers.S.x, fantasmas[i].bumpers.S.y, fantasmas[i].bumpers.S.width, fantasmas[i].bumpers.S.height, BLACK);
        if( fantasmas[i].bumpers.contactoS == 1 )
            DrawRectangle(fantasmas[i].bumpers.S.x, fantasmas[i].bumpers.S.y, fantasmas[i].bumpers.S.width, fantasmas[i].bumpers.S.height, RED);
    }
}
void actualizarTestDebug(Debug *herramientaDebug, Menu *menu) {
    //ZONA DE TESTEO DE ACTUALIZACION--------------------------------------

    //----------------------------------------------------------------
}
void dibujarTestDebug(Debug herramientaDebug, Menu menu) {
    //ZONA DE TESTEO DE DIBUJO----------------------------------------
    //DrawRectangle(herramientaDebug->valorVariable[4] * TILESIZE, herramientaDebug->valorVariable[5] * TILESIZE, herramientaDebug->valorVariable[6] * TILESIZE, herramientaDebug->valorVariable[7] * TILESIZE, DARKGRAY);
    //DrawText("Presione AQUI", herramientaDebug->valorVariable[1] * TILESIZE, herramientaDebug->valorVariable[2] * TILESIZE, TILESIZE * herramientaDebug->valorVariable[3], RED);

    //----------------------------------------------------------------
}
//-------------------------------------------------------------------------------------
//DEBUG NUMPAD

//Ortiz Rocio, Ortiz Brisa, Olmos Fernando, Gonzalez Matias
//TP Programacion I, UTN Mar Del Plata 2023
