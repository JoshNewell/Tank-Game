//Includes
#include <GL/glew.h>
#include <GL/glut.h>
#include <Shader.h>
#include <Vector.h>
#include <Matrix.h>
#include <Mesh.h>
#include <Texture.h>
#include <SphericalCameraManipulator.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

//define
#define INITPAN 0.0
#define INITTILT -PI/2
#define INITRAD 0.0
#define PI 3.14159265

#define FRONT 1
#define BACK -1
#define FORWARDS 1
#define BACKWARDS -1

#define FPS 60
#define FADJ 5

#define ISCREENW 720
#define ISCREENH 720

#define MAPL 8
#define MAPW 10
#define MAPH 3
#define MAPSIZE 5
#define GRAVITY -0.01

#define SHOTSPEED 2.5
#define MAXBULLETS 20
#define MAXSPEED 0.2
#define JUMPHEIGHT 0.45
#define ACCEL 0.1

typedef struct Mouse{
   float x = 0;
   float y = 0;
   int state = 1;
   int button = -1; 
};

typedef struct Shot{
   int applied = 0;
   Vector3f position;
   float timeInit = -1.0;
   float persistance = 0.0; 
   Vector3f direction;
   Vector3f velocity;
};

typedef struct Player{
   Vector3f tankPos = Vector3f(0, 0, 0);
   Vector3f speed = Vector3f(0,0,0);
   float rotationBody = 0.0; 
   float rotationTurret = 0.0;
   float movingX = 0.0;
   float movingZ = 0.0;
   int shooting = 0;
   float shotCooldown = 0.0;
   int score = 0;
   Vector3f directionBody;
   int health = 3;
   int cameraMode = 0;
   Vector3f directionTurret;
   Vector3f initalPos = Vector3f(0,0,0);
   int damagable = 1;
   int camTrigger = 0;
   int inAir = 0;
   int jump = 0;
};

typedef struct Create{
   int applied = 0;
   Vector3f position = Vector3f(-20,0,20);;
};

typedef struct TimeStamp{
   int updatable = 1;
   float time = 0;
};

//Function Prototypes
bool initGL(int argc, char** argv);
void display(void);
void keyboard(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void handleKeys();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void Timer(int value);

//Function Prototypes(self)
int drawMesh(float x, float y, float z, Mesh mesh, GLuint texture, float rotationP, float rotationT, float scale);
int drawCreate(float x, float y, float z, float rotationP, float rotationT, float scale);
int drawMaze(float scale, int coinScale);
int drawTank(Player& player, float offX, float offY, float offZ, float scale);
int setCamera();
int updatePlayerPositions(Player& player, int floorScale);
int loadMap(char* mapFile);
int setOriantation(float Pan, float Tilt);
int handleCameraMouse(Player& player);
float determinGravPlayer(Player player, int floorScale);
int isFinished();
int drawCoin(float x, float y, float z, float rotationP, float rotationT, float scale);
int drawShot(Player& player, float x, float y, float z, float rotationP, float rotationT, float scale);
int nullBullets();
int nullBullet(int i);
int mapX(float x, int floorScale);
int mapZ(float z, int floorScale);
int mapY(float y, int floorScale);
int playerCollisionX(float x, Player player, float padO, float padX);
int playerCollisionY(float y, Player player, float padO, float padY);
int playerCollisionZ(float z, Player player, float padO, float padZ);
float absoluteFloat(float in);
int getCreate(float x, float y, float z);
int drawHealthBar(Player player, int screenSide);
int playerCreateCollisionX(Player& player, int position, int movment, float hitBox);
int playerCreateCollisionZ(Player& player, int position, int movment, float hitBox);
int drawHUD(float x, float y, float rotation, float scaleX,  float scaleY, float red, float green, float blue);
char* nextLevel();
int drawMenu();
int handleMenuMouse();
void initGeometry();
void drawGeometry();
int resetPlayer(Player& player);
int isTwoFinished();
int updateShot(float rotationP, float rotationT, float scale, float velocityMultiplyer);

void initShader();					   //Function to init Shader
void initTexture(std::string filename, GLuint & textureID);
void render2dText(std::string text, float r, float g, float b, float x, float y);


float cinitX(10);
float cinitY(6);
float cinitZ(10);

//Global Variables(self)
GLuint shaderProgramID;			                    // Shader Program ID
GLuint vertexPositionAttribute;		      // Vertex Position Attribute Location
GLuint vertexNormalAttribute;		
GLuint vertexTexcoordAttribute;

//Material Prop (self)
GLuint LightPositionUniformLocation;                // Light Position Uniform   
GLuint AmbientUniformLocation;
GLuint SpecularUniformLocation;
GLuint SpecularPowerUniformLocation;

Vector3f lightPosition= Vector3f(100.0,100.0,100.0);   // Light Position 
Vector3f ambient    = Vector3f(0.1,0.1,0.1);
Vector3f specular   = Vector3f(1.0,1.0,1.0);
float specularPower(10.0);

GLuint TextureMapUniformLocation;               	// Texture Map Location
GLuint textures[4];

//Viewing (self)
SphericalCameraManipulator cameraManip;
Matrix4x4 ModelViewMatrix;		                    // ModelView Matrix
GLuint MVMatrixUniformLocation;		                // ModelView Matrix Uniform
Matrix4x4 ProjectionMatrix;		                    // Projection Matrix
GLuint ProjectionUniformLocation;	                // Projection Matrix Uniform Location
GLuint vertexPositionBuffer;

//Meshs(self)
Mesh create;
Mesh coin;
Mesh body;				                            // Mesh
Mesh wheelB;
Mesh wheelF;
Mesh turret;
Mesh shot;


//Screen size
int screenWidth(ISCREENW);
int screenHeight(ISCREENH);
float aspectRatio(0);

int state(0);
int doneGame(0);
int portSwitch(2);
int shotsDrawn(0);

//Array of key states
bool keyStates[256];

Vector3f cameraPos(cinitX, cinitY, cinitZ);

int map[MAPH][MAPW][MAPL];
int currentLevel(1);
char** levels = new char*[6];

float timeSet(0.0);
TimeStamp timeStamp;

Mouse globalMouse;
Shot* bulletArray = new Shot[MAXBULLETS];
Player* players = new Player[2];

Player& playerOne = players[0]; 
Player& playerTwo = players[1];


Create* creates = new Create[MAPH*MAPW*MAPL];

//! Main Program Entry
int main(int argc, char** argv){	
    //init OpenGL
    if(!initGL(argc, argv)){
	return -1;
    }

    //Init Key States to false;    
    for(int i = 0 ; i < 256; i++){
        keyStates[i] = false;
    }
     
    //Set up your program(self)
    initShader();

    create.loadOBJ("../models/cube.obj");    
    coin.loadOBJ("../models/coin.obj"); 
    body.loadOBJ("../models/chassis.obj");  
    wheelB.loadOBJ("../models/back_wheel.obj");
    wheelF.loadOBJ("../models/front_wheel.obj");
    turret.loadOBJ("../models/turret.obj");
    shot.loadOBJ("../models/ball.obj");

    initTexture("../models/Crate.bmp", textures[0]);   
    initTexture("../models/coin.bmp", textures[1]);  
    initTexture("../models/hamvee.bmp", textures[2]);    
    initTexture("../models/ball.bmp", textures[3]);

    levels[0] = "../levels/level_0.txt";
    levels[1] = "../levels/level_1.txt";
    levels[2] = "../levels/level_2.txt";
    levels[3] = "../levels/level_3.txt";
    levels[4] = "../levels/level_4.txt";
    levels[5] = "../levels/level_5.txt";

    cameraManip.setPanTiltRadius(INITPAN, INITTILT, INITRAD);
    loadMap(levels[currentLevel]);
    nullBullets();

    initGeometry();

    //Enter main loop
    glutMainLoop();

    //Delete shader program
    glDeleteProgram(shaderProgramID);

    return 0;
}

//! Function to Initlise OpenGL
bool initGL(int argc, char** argv){
    //StratGL
    glutInit(&argc, argv);//Init GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);//Set Display Mode
    glutInitWindowSize(screenWidth, screenHeight);//Set Window Size
    glutInitWindowPosition(200, 200);// Window Position
    glutCreateWindow("World WAR Tank!");//Create Window
    if (glewInit() != GLEW_OK) // Init GLEW
	{
	    std::cout << "Failed to initialize GLEW" << std::endl;
	    return false;
	}

    glutTimerFunc(1000/FPS - FADJ,Timer, 0);
	
    //Set Display function
    glutDisplayFunc(display);
	
    //Set Interaction Functions
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyUp); 
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);
    glutMotionFunc(motion);
	
    //self interaction
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);

    return true;
}




//! Display Loop
void display(void){

   //Start up
   screenWidth = glutGet(GLUT_WINDOW_WIDTH);
   screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
   aspectRatio = screenWidth/screenHeight;
   glUseProgram(shaderProgramID);
   handleKeys();//Handle keys
   glViewport(0,0,screenWidth, screenHeight);//Set Viewport
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );// Clear the screen 

   //Lighting	      
   glUniform3f(LightPositionUniformLocation, lightPosition.x,lightPosition.y,lightPosition.z);
   glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 1.0);
   glUniform4f(SpecularUniformLocation, specular.x, specular.y, specular.z, 1.0);
   glUniform1f(SpecularPowerUniformLocation, specularPower);

switch(state){

   case 0:
   {
      timeStamp.updatable = 0;
      playerOne.cameraMode = 2;
      playerOne.speed = Vector3f(0,0,0);

      ProjectionMatrix.perspective(90, aspectRatio, 0.0001, 150.0);
      glUniformMatrix4fv(	
	   ProjectionUniformLocation, 
	   1,							
	   false,						
	   ProjectionMatrix.getPtr());
      
      updatePlayerPositions(playerOne,MAPSIZE);

      drawTank(playerOne,playerOne.tankPos.x,playerOne.tankPos.y,playerOne.tankPos.z, 1);
      drawMaze(MAPSIZE, 2);

      glBindTexture(GL_TEXTURE_2D, 0);

      handleMenuMouse();
      drawMenu();
      glUseProgram(0);
      glClearColor(0.6,0.6,1.0,1.0);
   break;
   }

   case 1:
   {
   timeStamp.updatable = 1;
   ProjectionMatrix.perspective(90, aspectRatio, 0.0001, 150.0);
   glUniformMatrix4fv(	
	ProjectionUniformLocation, 
	1,							
	false,						
	ProjectionMatrix.getPtr());

   //Update 
   updatePlayerPositions(playerOne,MAPSIZE);
   handleCameraMouse(playerOne);

   //Draw Scene
   drawTank(playerOne,playerOne.tankPos.x,playerOne.tankPos.y,playerOne.tankPos.z, 1);
   drawMaze(MAPSIZE, 2);
   drawShot(playerOne, playerOne.tankPos.x, playerOne.tankPos.y, playerOne.tankPos.z, INITPAN, INITTILT, 0.5);
   updateShot(INITPAN, INITTILT, 0.5, 1);

   isFinished();
   glBindTexture(GL_TEXTURE_2D, 0);

   drawHealthBar(playerOne, 1);

   glUseProgram(0);

   float points = playerOne.score;
   std::stringstream s3;
   s3 << points;
   std::string Score = "Score: " +s3.str();

   std::stringstream s4;
   float digit;
   double sc = 0.01;
   digit = (int)(timeStamp.time/sc)*sc;
   s4 << digit;
   std::string timeSetS = "Timer: " +s4.str();

   float level = currentLevel;
   std::stringstream s5;
   s5 << level;
   std::string levelNum = "Level: " +s5.str();

   render2dText(Score, 1.0, 1.0, 1.0, -1.0, 0.8);
   render2dText(timeSetS, 1.0, 1.0, 1.0, -1.0, 0.7);
   render2dText(levelNum, 1.0, 1.0, 1.0, -1.0, 0.6);

   glClearColor(0.6,0.6,1.0,1.0);
   break;
   }

   case 2:
   {
   timeStamp.updatable = 0;
   timeStamp.time = 0;
   glUseProgram(0);
   render2dText("You Died", 1.0, 0.0, 0.0, -0.1, 0.4);
   render2dText("Press 'R' to restart", 1.0, 0.0, 0.0, -0.2, 0.3);
   glClearColor(0.0,0.0,0.0,1.0);

   currentLevel = 1;
   loadMap(levels[currentLevel]);

   break;
   }

   case 3:
   {
   timeStamp.updatable = 0;
   float finalTime;
   std::stringstream s5;
   double sc = 0.01;
   if(doneGame == 0){
      finalTime = (int)(timeStamp.time/sc)*sc;
      doneGame = 1;
   }
   s5 << finalTime;
   std::string timeFin = "You Finished In: " +s5.str();

   glUseProgram(0);
   render2dText("You Win", 1.0, 0.0, 0.0, -0.1, 0.4);
   render2dText(timeFin, 1.0, 0.0, 0.0, -0.25, 0.3);
   glClearColor(0.0,0.0,0.0,1.0);

   break;
   }

   case 4:
   {
      timeStamp.updatable = 0;
      glUseProgram(0);
      render2dText("Controls:", 1.0, 0.0, 0.0, -0.125, 0.5);
      render2dText("W,A,S,D: Movement", 1.0, 0.0, 0.0, -0.25, 0.4);
      render2dText("Spacebar: Shoot", 1.0, 0.0, 0.0, -0.2, 0.3);
      render2dText("E: Jump", 1.0, 0.0, 0.0, -0.1, 0.2);
      render2dText("Q: Change view point", 1.0, 0.0, 0.0, -0.25, 0.1);
      render2dText("R: Restart Level", 1.0, 0.0, 0.0, -0.2, 0.0);
      render2dText("Mouse: Aim", 1.0, 0.0, 0.0, -0.15, -0.1);
      render2dText("The goal of each level is to collect the 4 coins as fast as possable.", 1.0, 0.0, 0.0, -0.7, -0.4);
      glClearColor(0.0,0.0,0.0,1.0);
   break;
   }

   case 5:
   {
   timeStamp.updatable = 1;
   portSwitch = 2;
   while(portSwitch){
      if(portSwitch == 2){
         glViewport(0,0,screenWidth/2, screenHeight);
      }
      if(portSwitch == 1){
         glViewport(screenWidth/2,0,screenWidth/2, screenHeight);
      }
      ProjectionMatrix.perspective(90, aspectRatio/2, 0.0001, 150.0);
      glUniformMatrix4fv(	
   	   ProjectionUniformLocation, 
	   1,							
	   false,						
	   ProjectionMatrix.getPtr());

      if(portSwitch == 2){
         updatePlayerPositions(playerOne,MAPSIZE);
         drawShot(playerOne, playerOne.tankPos.x, playerOne.tankPos.y, playerOne.tankPos.z, INITPAN, INITTILT, 0.5);
      }
      if(portSwitch == 1){
         updatePlayerPositions(playerTwo,MAPSIZE);
         drawShot(playerTwo, playerTwo.tankPos.x, playerTwo.tankPos.y, playerTwo.tankPos.z, INITPAN, INITTILT, 0.5);
      }

      updateShot(INITPAN, INITTILT, 0.5, 0.5);

      handleCameraMouse(playerOne);

      drawMaze(MAPSIZE, 2);

      drawTank(playerOne,playerOne.tankPos.x,playerOne.tankPos.y,playerOne.tankPos.z, 1);
      ambient = Vector3f(0.0,0.0,0.5);
      glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 0.1);
      drawTank(playerTwo,playerTwo.tankPos.x,playerTwo.tankPos.y,playerTwo.tankPos.z, 1);
      ambient = Vector3f(0.1,0.1,0.1);
      glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 1.0);

      portSwitch--;
   }

   isTwoFinished();
   
   glBindTexture(GL_TEXTURE_2D, 0);

   glViewport(0,0,screenWidth, screenHeight);

   drawHealthBar(playerOne, 1);
   drawHealthBar(playerTwo, -1);

   glUseProgram(0);
   glClearColor(0.6,0.6,1.0,1.0);
   break;
   }

   case 6:
   {
   timeStamp.updatable = 0;
   glUseProgram(0);
   render2dText("Player One Wins!", 1.0, 0.0, 0.0, -0.1, 0.4);
   glClearColor(0.0,0.0,0.0,1.0);

   break;
   }

   case 7:
   {
   timeStamp.updatable = 0;
   glUseProgram(0);
   render2dText("Player Two Wins!", 1.0, 0.0, 0.0, -0.1, 0.4);
   glClearColor(0.0,0.0,0.0,1.0);

   break;
   }
}
   glutSwapBuffers();
}



//! Keyboard Interaction
void keyboard(unsigned char key, int x, int y){
   //Quits program when esc is pressed
   if (key == 27){	//esc key code
        if(state != 2 && state != 3){
           if(state ==5){
              exit(0);
           }
           if(state == 6 || state == 7){
              exit(0);
           }
	   state = 0;
        }
   }
    
   //Set key status
   keyStates[key] = true;
}

//! Handle key up situation
void keyUp(unsigned char key, int x, int y){
   keyStates[key] = false;
}


//! Handle Keys
void handleKeys(){
   //keys should be handled here
   if(keyStates['w']){
        if(state == 1 || state == 5){
           playerOne.movingX = ACCEL/FPS;
           playerOne.movingZ = ACCEL/FPS;

           playerCreateCollisionX(playerOne, FRONT, FORWARDS, 3);
           playerCreateCollisionX(playerOne, BACK, FORWARDS, 3);
           playerCreateCollisionZ(playerOne, FRONT, FORWARDS, 3);
           playerCreateCollisionZ(playerOne, BACK, FORWARDS, 3);
        }
   }
   if(keyStates['s']){
        if(state == 1 || state == 5){
           playerOne.movingX = -ACCEL/FPS;
           playerOne.movingZ = -ACCEL/FPS;

           playerCreateCollisionX(playerOne, FRONT, BACKWARDS, -2);
           playerCreateCollisionX(playerOne, BACK, BACKWARDS, -2);
           playerCreateCollisionZ(playerOne, FRONT, BACKWARDS,  -2);
           playerCreateCollisionZ(playerOne, BACK, BACKWARDS, -2);
        }
   }
   if(keyStates['d']){
        if(state == 1 || state == 5){
           playerOne.rotationBody += 0.01;
           playerOne.rotationTurret += 0.01;
        }
   }
   if(keyStates['a']){
        if(state == 1 || state == 5){
           playerOne.rotationBody -= 0.01;
           playerOne.rotationTurret -= 0.01;
        }
   }
   if((!keyStates['w']) && (!keyStates['s'])){

        if(state == 1 || state == 5){
           playerCreateCollisionX(playerOne, FRONT, BACKWARDS, -2);
           playerCreateCollisionX(playerOne, BACK, BACKWARDS, -2);
           playerCreateCollisionZ(playerOne, FRONT, BACKWARDS,  -2);
           playerCreateCollisionZ(playerOne, BACK, BACKWARDS, -2);
           playerCreateCollisionX(playerOne, FRONT, FORWARDS, 3);
           playerCreateCollisionX(playerOne, BACK, FORWARDS, 3);
           playerCreateCollisionZ(playerOne, FRONT, FORWARDS, 3);
           playerCreateCollisionZ(playerOne, BACK, FORWARDS, 3);

           if((absoluteFloat(playerOne.speed.x) >= (ACCEL/FPS)/10)){
              playerOne.movingX = (3*ACCEL/FPS)*(playerOne.speed.x < 0 ? 1:-1);
           }else{
              playerOne.movingX = 0;
           }
           if((absoluteFloat(playerOne.speed.z) >= (ACCEL/FPS)/10)){
              playerOne.movingZ = (3*ACCEL/FPS)*(playerOne.speed.z < 0 ? 1:-1);
           }else{
              playerOne.movingZ = 0;
           }
        }
   }
   if(keyStates[' ']){
        if(state == 1 || state == 5){
           playerOne.shooting = 1;
        }
   }
   if(!keyStates[' ']){
        if(state == 1 || state == 5){
           playerOne.shooting = 0;
        }
   }
   if(keyStates['r']){
        state = 1;
        loadMap(levels[currentLevel]);
        resetPlayer(playerOne);
        playerOne.health = 3;
        playerOne.score = 0;
   }
   if(keyStates['q']){
        if(state == 1 || state == 5){
           if(!playerOne.camTrigger){
              playerOne.cameraMode = playerOne.cameraMode == 0 ? 1:0;
              playerOne.camTrigger = 1;
           }
        }
   }
   if(!keyStates['q']){
        if(state == 1 || state == 5){
           playerOne.camTrigger = 0;
        }
   }
   if(keyStates['e']){
        if(state == 1 || state == 5){
           playerOne.jump = 1;
        }
   }
   if(!keyStates['e']){
        if(state == 1 || state == 5){
           playerOne.jump = 0;
        }
   }
   //Player 2
   if(keyStates['8']){
        if(state == 5){
           playerTwo.movingX = ACCEL/FPS;
           playerTwo.movingZ = ACCEL/FPS;

           playerCreateCollisionX(playerTwo, FRONT, FORWARDS, 3);
           playerCreateCollisionX(playerTwo, BACK, FORWARDS, 3);
           playerCreateCollisionZ(playerTwo, FRONT, FORWARDS, 3);
           playerCreateCollisionZ(playerTwo, BACK, FORWARDS, 3);
        }
   }
   if(keyStates['5']){
        if(state == 5){
           playerTwo.movingX = -ACCEL/FPS;
           playerTwo.movingZ = -ACCEL/FPS;

           playerCreateCollisionX(playerTwo, FRONT, BACKWARDS, -2);
           playerCreateCollisionX(playerTwo, BACK, BACKWARDS, -2);
           playerCreateCollisionZ(playerTwo, FRONT, BACKWARDS,  -2);
           playerCreateCollisionZ(playerTwo, BACK, BACKWARDS, -2);
        }
   }
   if(keyStates['6']){
        if(state == 5){
           playerTwo.rotationBody += 0.01;
           playerTwo.rotationTurret += 0.01;
        }
   }
   if(keyStates['4']){
        if(state == 5){
           playerTwo.rotationBody -= 0.01;
           playerTwo.rotationTurret -= 0.01;
        }
   }
   if((!keyStates['8']) && (!keyStates['5'])){

        if(state == 5){
           playerCreateCollisionX(playerTwo, FRONT, BACKWARDS, -2);
           playerCreateCollisionX(playerTwo, BACK, BACKWARDS, -2);
           playerCreateCollisionZ(playerTwo, FRONT, BACKWARDS,  -2);
           playerCreateCollisionZ(playerTwo, BACK, BACKWARDS, -2);
           playerCreateCollisionX(playerTwo, FRONT, FORWARDS, 3);
           playerCreateCollisionX(playerTwo, BACK, FORWARDS, 3);
           playerCreateCollisionZ(playerTwo, FRONT, FORWARDS, 3);
           playerCreateCollisionZ(playerTwo, BACK, FORWARDS, 3);

           if((absoluteFloat(playerTwo.speed.x) >= (ACCEL/FPS)/10)){
              playerTwo.movingX = (3*ACCEL/FPS)*(playerTwo.speed.x < 0 ? 1:-1);
           }else{
              playerTwo.movingX = 0;
           }
           if((absoluteFloat(playerTwo.speed.z) >= (ACCEL/FPS)/10)){
              playerTwo.movingZ = (3*ACCEL/FPS)*(playerTwo.speed.z < 0 ? 1:-1);
           }else{
              playerTwo.movingZ = 0;
           }
        }
   }
   if(keyStates['+']){
        if(state == 5){
           playerTwo.shooting = 1;
        }
   }
   if(!keyStates['+']){
        if(state == 5){
           playerTwo.shooting = 0;
        }
   }

   if(keyStates['0']){
        if(state == 5){
           playerTwo.jump = 1;
        }
   }
   if(!keyStates['0']){
        if(state == 5){
           playerTwo.jump = 0;
        }
   }
   if(!keyStates['7']){
        if(state == 5){
           playerTwo.rotationTurret += 0.02;
        }
   }
   if(!keyStates['9']){
        if(state == 5){
           playerTwo.rotationTurret -= 0.02;
        }
   }
}

//! Mouse Interaction
void mouse(int button, int state, int x, int y){
   globalMouse.button = button;
   globalMouse.state = state;
}

//! Motion
void motion(int x, int y){
   globalMouse.x = x;
   globalMouse.y = y;
}

//! Timer Function
void Timer(int value){
   timeSet += 0.01667;
   if(timeStamp.updatable){
      timeStamp.time += 0.01667;
   }

   if(playerOne.shotCooldown > 0){
      playerOne.shotCooldown -= 0.02;
      if(playerOne.shotCooldown < 0){
         playerOne.shotCooldown = 0;
      }
   }
   if(playerTwo.shotCooldown > 0){
      playerTwo.shotCooldown -= 0.02;
      if(playerTwo.shotCooldown < 0){
         playerTwo.shotCooldown = 0;
      }
   }
   glutPostRedisplay();
   glutTimerFunc(1000/FPS - FADJ,Timer, 0);
}


//Self Functions
int drawMesh(float x, float y, float z, Mesh mesh, GLuint texture, float rotationP, float rotationT, float scale){

   Matrix4x4 n;

   ModelViewMatrix.toIdentity();

   setCamera();

   setOriantation(rotationP, rotationT);

   ModelViewMatrix.translate(x, y, z);

   ModelViewMatrix.scale(scale, scale, scale);

   n = cameraManip.apply(ModelViewMatrix);

   glUniformMatrix4fv(	
	MVMatrixUniformLocation,  	
	1,					       
	false,				     
	n.getPtr());	      

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture);
   glUniform1i(TextureMapUniformLocation, 0);

        mesh.Draw(vertexPositionAttribute,vertexNormalAttribute,vertexTexcoordAttribute);

   return 1;
}

int drawMaze(float scale, int coinScale){
   
   for(int k = 0; k < MAPH; k++){
      for(int i = 0; i < MAPW; i++){
         for(int j = 0; j < MAPL; j++){
            
            switch(map[k][i][j]){
               case 2:
                  drawCoin(2*i*scale, ((2*k)-1)*scale, -2*j*scale, INITPAN, INITTILT, coinScale);
               break;
               case 1:
                  drawCreate(2*i*scale, ((2*k)-1)*scale, -2*j*scale, INITPAN, INITTILT, scale);
                  creates[k*MAPL*MAPW + i*MAPL + j].position = Vector3f(2*i*scale, ((2*k)-1)*scale, -2*j*scale);
                  creates[k*MAPL*MAPW + i*MAPL + j].applied = 1;
               break;
            }
         }
      }
   }
   return 1;
}

int drawTank(Player& player, float offX, float offY, float offZ, float scale){
	

   drawMesh(0+offX, 0+offY, 0+offZ, body, textures[2], player.rotationBody, INITTILT, scale);
   drawMesh(0+offX, 0+offY, 0+offZ, wheelB, textures[2], player.rotationBody, INITTILT, scale);
   drawMesh(0+offX, 0+offY, 0+offZ, wheelF, textures[2], player.rotationBody, INITTILT, scale);
   drawMesh(0+offX, 0+offY, 0+offZ, turret, textures[2], player.rotationTurret, INITTILT, scale);

   for(int i(0); i < MAXBULLETS; i++){
      Vector3f bullet = bulletArray[i].position - player.tankPos;
      float dist = (bullet.x)*(bullet.x) + (bullet.y)*(bullet.y) + (bullet.z)*(bullet.z);
      if(sqrt(dist) < 2 && bulletArray[i].applied){
         nullBullet(i);
         player.health--;
      }
   }

   return 1;
}

int setCamera(){

   Player player = playerOne;

   if(state == 5){
      if(portSwitch == 2){
         player = playerOne;
      }
      if(portSwitch == 1){
         player = playerTwo;
      }
   }
   
   Vector3f look;
   switch(player.cameraMode){
      case 0:
         look = player.tankPos;
      break;
      case 1:
         look = Vector3f(player.tankPos.x + 20*player.directionTurret.x,player.tankPos.y,player.tankPos.z + 20*player.directionTurret.z);
      break;
      case 2:
         look = Vector3f((MAPW)*MAPSIZE,0,(-MAPL)*MAPSIZE);
      break;
   }

   ModelViewMatrix.lookAt(cameraPos, look, Vector3f(0,1,0));

}

int updatePlayerPositions(Player& player, int floorScale){

   if(player.speed.y != 0){
      player.inAir = 1;
   }else{
      player.inAir = 0;
   }
   
   player.directionBody = Vector3f (sin(player.rotationBody),0,-cos(player.rotationBody));
   player.directionTurret = Vector3f (sin(player.rotationTurret),0,-cos(player.rotationTurret));

   float grav = determinGravPlayer(player, floorScale);
   if(grav == 0){
      player.speed.y = 0;
   }

   if(!player.inAir){
      player.speed.y += player.jump * JUMPHEIGHT;
   }

   int create;

   player.speed.x += player.movingX;
   if(player.speed.x > MAXSPEED){
      player.speed.x = MAXSPEED;
   }
   if(player.speed.x < -MAXSPEED){
      player.speed.x = -MAXSPEED;
   }

   player.speed.y += grav;

   player.speed.z += player.movingZ;
   if(player.speed.z > MAXSPEED){
      player.speed.z = MAXSPEED;
   }
   if(player.speed.z < -MAXSPEED){
      player.speed.z = -MAXSPEED;
   }

   player.tankPos.x = player.tankPos.x + (player.directionBody.x)*player.speed.x;
   player.tankPos.y = player.tankPos.y + player.speed.y;
   player.tankPos.z = player.tankPos.z + (player.directionBody.z)*player.speed.z;

   switch(player.cameraMode){
      case 0:
      cinitX = 10;
      cinitY = 6;
      cinitZ = 10;
      cameraPos.x = player.tankPos.x - cinitX*(player.directionTurret.x);
      cameraPos.y = player.tankPos.y + cinitY;
      cameraPos.z = player.tankPos.z - cinitZ*(player.directionTurret.z);
      break;
      case 1:
      cinitX = 0.5;
      cinitY = 4;
      cinitZ = 0.5;
      cameraPos.x = player.tankPos.x - cinitX*(player.directionTurret.x);
      cameraPos.y = player.tankPos.y + cinitY;
      cameraPos.z = player.tankPos.z - cinitZ*(player.directionTurret.z);
      break;
      case 2:
      cameraPos.x =70*sin(timeSet/2) + 40;
      cameraPos.y =20;
      cameraPos.z =-70*cos(timeSet/2) - 40;
      break;
   }

   return 1;
}


int drawCreate(float x, float y, float z, float rotationP, float rotationT, float scale){
   
   if((mapZ(playerOne.tankPos.z, scale) == mapZ(z, scale))&&(mapX(playerOne.tankPos.x, scale) == mapX(x, scale))&&(mapY(playerOne.tankPos.y, scale) == mapY(y, scale))){
   
      ambient = Vector3f(0.5,0.5,0.5);
      glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 0.1);
   }
   if((mapZ(playerTwo.tankPos.z, scale) == mapZ(z, scale))&&(mapX(playerTwo.tankPos.x, scale) == mapX(x, scale))&&(mapY(playerTwo.tankPos.y, scale) == mapY(y, scale))){
   
      ambient = Vector3f(0.5,0.5,0.5);
      glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 0.1);
   }
   drawMesh(x,y,z,create, textures[0],rotationP, rotationT, scale);

   ambient = Vector3f(0.1,0.1,0.1);
   glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 1.0);

   for(int i(0); i < MAXBULLETS; i++){
      Vector3f createPos = Vector3f(x, y, z);
      Vector3f bullet = bulletArray[i].position - createPos;
      float dist = (bullet.x)*(bullet.x) + (bullet.y)*(bullet.y) + (bullet.z)*(bullet.z);
      if(sqrt(dist) <= 5 && bulletArray[i].applied){
         nullBullet(i);
      }
   }
   return 1;
}

int loadMap(char* mapFile){
   
   std::ifstream inFile;
   char input = ' ';

   for(int k =0; k < MAPH; k++){
      for(int j =0; j < MAPL; j++){
         for(int i =0; i < MAPW; i++){
            map[k][i][j] = 0;
         }
      }
   }

   inFile.open(mapFile);
   if(!(inFile.good())){
      std::cerr << "Non-vaild file" << std::endl;
      exit(EXIT_FAILURE);
   }

   inFile >> std::ws >> input >> std::ws;

   if(input != '['){
      std::cerr << "Non-vaild map file" << std::endl;
      exit(EXIT_FAILURE);
   }

   for(int k(0); k < MAPH; k++){
      for(int i(0); i < MAPL; i++){
         for(int j =0; j < MAPW; j++){
            inFile >> std::ws >> input >> std::ws;
            map[k][j][i] = (int)input - (int)'0';
            if(map[k][j][i] == 3){
               playerOne.tankPos.x = playerOne.initalPos.x = 2*j*MAPSIZE;
               playerOne.tankPos.y = playerOne.initalPos.y = MAPSIZE*(2*k -1);
               playerOne.tankPos.z = playerOne.initalPos.z = -2*i*MAPSIZE;;
            }
            if(map[k][j][i] == 4){
               playerTwo.tankPos.x = playerTwo.initalPos.x = 2*j*MAPSIZE;
               playerTwo.tankPos.y = playerTwo.initalPos.y = MAPSIZE*(2*k -1);
               playerTwo.tankPos.z = playerTwo.initalPos.z = -2*i*MAPSIZE;
            }
         }
      }
      inFile >> std::ws >> input >> std::ws;
      if(input != '-'){
         break;
      }
   }
   
   inFile.close();

   return 1;
}

int setOriantation(float pan, float tilt){
   
   cameraManip.setPanTiltRadius(pan, tilt, INITRAD);

}

int handleCameraMouse(Player& player){
 
   float turretAdjust = PI*((globalMouse.x/screenWidth)-0.5);
   
   player.rotationTurret = turretAdjust + player.rotationBody;
   
}

float determinGravPlayer(Player player, int floorScale){

   int create = getCreate(player.tankPos.x,player.tankPos.y,player.tankPos.z);
   if(create == -1){
      return GRAVITY;
   }

   if(playerCollisionY(creates[create].position.y, player, floorScale, 0)&&(creates[create].applied)){
      return 0;
   }
   
   return GRAVITY;
}

int isFinished(){
   if(playerOne.tankPos.y < -100){
      resetPlayer(playerOne);
      playerOne.health--;
   }
   if(playerOne.score == 4){
      loadMap(nextLevel());
      playerOne.health = 3;
   }
   if(playerOne.health <= 0){
      state = 2;
   }
}

int isTwoFinished(){
   if(playerTwo.tankPos.y < -100){
      resetPlayer(playerTwo);
      playerTwo.health--;
   }
   if(playerOne.tankPos.y < -100){
      resetPlayer(playerOne);
      playerOne.health--;
   }
   if(playerTwo.health <= 0){
      state = 6;
   }
   if(playerOne.health <= 0){
      state = 7;
   }
}

int drawCoin(float x, float y, float z, float rotationP, float rotationT, float scale){

   float spin(2*PI*cos(timeSet)+rotationP);
   float bobY(y + 0.5*cos(timeSet));
   Vector3f coinPos = Vector3f(x, y, z);
   
   for(int i(0); i < MAXBULLETS; i++){
      Vector3f bullet = bulletArray[i].position - coinPos;
      float distBull = (bullet.x)*(bullet.x) + (bullet.y)*(bullet.y) + (bullet.z)*(bullet.z);
      if(sqrt(distBull) <= 4 && bulletArray[i].applied){
         map[mapY(y,MAPSIZE)][mapX(x,MAPSIZE)][mapZ(z,MAPSIZE)] = 0;
         nullBullet(i);
         playerOne.score += 1;
      }
   }

   Vector3f tank = playerOne.tankPos - coinPos;
   float dist = (tank.x)*(tank.x) + (tank.y)*(tank.y) + (tank.z)*(tank.z);
   if(sqrt(dist) <= 5.5){
      switch(state){
         case 1:
         {
         map[mapY(y,MAPSIZE)][mapX(x,MAPSIZE)][mapZ(z,MAPSIZE)] = 0;
         playerOne.score += 1;
         break;
         }

         case 5:
         {
         if(playerOne.health != 3){
            map[mapY(y,MAPSIZE)][mapX(x,MAPSIZE)][mapZ(z,MAPSIZE)] = 0;
            playerOne.health += 1;
         }
         break;
         }
      }
   }

   Vector3f tankTwo = playerTwo.tankPos - coinPos;
   dist = (tankTwo.x)*(tankTwo.x) + (tankTwo.y)*(tankTwo.y) + (tankTwo.z)*(tankTwo.z);
   if(sqrt(dist) <= 5.5){
      if(playerTwo.health != 3){
         map[mapY(y,MAPSIZE)][mapX(x,MAPSIZE)][mapZ(z,MAPSIZE)] = 0;
         playerTwo.health += 1;
      }
   }

   drawMesh(x,bobY,z,coin, textures[1],spin, rotationT, scale);

   return 1;
}

int drawShot(Player& player, float x, float y, float z, float rotationP, float rotationT, float scale){

   Vector3f directionTurret = player.directionTurret;
   
   for(int i(0); i < MAXBULLETS; i++){

      Shot& currentShot = bulletArray[i];
      
      if((player.shooting == 1) && (player.shotCooldown == 0) && (currentShot.applied == 0)){

         player.shotCooldown = 0.5;

         currentShot.position.x = x;
         currentShot.position.y = y + 2;
         currentShot.position.z = z;
         currentShot.timeInit = timeSet;
         currentShot.applied = 1;
         currentShot.direction.x = (directionTurret.x);
         currentShot.direction.y = 0;
         currentShot.direction.z = (directionTurret.z);
         currentShot.velocity.x = SHOTSPEED;
         currentShot.velocity.y = 0;
         currentShot.velocity.z = SHOTSPEED;

         drawMesh(currentShot.position.x,currentShot.position.y,currentShot.position.z,shot, textures[3],rotationP, rotationT, scale);
      }
   }

   return 1;
}

int updateShot(float rotationP, float rotationT, float scale, float velocityMultiplyer){
   
   for(int i(0); i < MAXBULLETS; i++){

      Shot& currentShot = bulletArray[i];
      
      if(currentShot.applied == 1){
         if(currentShot.persistance >= 5){

            nullBullet(i);

         }else{

            currentShot.velocity.y += GRAVITY;

            currentShot.position.x = currentShot.position.x + currentShot.direction.x*currentShot.velocity.x*velocityMultiplyer;
            currentShot.position.y = currentShot.position.y + currentShot.velocity.y*velocityMultiplyer;
            currentShot.position.z = currentShot.position.z + currentShot.direction.z*currentShot.velocity.z*velocityMultiplyer;
            currentShot.persistance = timeSet - currentShot.timeInit;
           drawMesh(currentShot.position.x,currentShot.position.y,currentShot.position.z,shot, textures[3],rotationP, rotationT, scale);
            
         }
      }

   }

   return 1;
}

int nullBullets(){

   for(int i(0); i < MAXBULLETS; i++){
      bulletArray[i].position.x = 0.0;
      bulletArray[i].position.y = 0.0;
      bulletArray[i].position.z = 0.0;
      bulletArray[i].applied = 0;
      bulletArray[i].timeInit = -1.0;
      bulletArray[i].persistance = 0.0;
   }

   return 1;
}

int nullBullet(int i){

   if((i >= 0)&&(i <= MAXBULLETS)){
      bulletArray[i].position.x = 0.0;
      bulletArray[i].position.y = 0.0;
      bulletArray[i].position.z = 0.0;
      bulletArray[i].applied = 0;
      bulletArray[i].timeInit = -1.0;
      bulletArray[i].persistance = 0.0;
   }

   return 1;
}

int mapX(float x, int floorScale){

   int change = 1;
   if(x < 0){
      change = -1;
   }

   float standX = ((x/floorScale))/2;
   int sideX = absoluteFloat(standX - (int)standX) > 0.5 ? standX+change : standX;

   return sideX;
}
int mapZ(float z, int floorScale){

   int change = 1;
   if(z > 0){
      change = -1;
   }

   float standZ = ((-z/floorScale))/2;
   int sideZ = absoluteFloat(standZ - (int)standZ) > 0.5 ? standZ+change : standZ;

   return sideZ;
}
int mapY(float y, int floorScale){

   int change = 1;
   if(y < 0){
      change = -1;
   }

   float standY = (((y+floorScale)/floorScale))/2;
   int sideY = absoluteFloat(standY - (int)standY) >= 0.5 ? standY+change : standY;
}

int playerCollisionX(float x, Player player, float padO, float padX){
   
   if((player.tankPos.x + padX < x)&&(player.tankPos.x + padX > x-padO)){
      return -1;//left
   }
   if((player.tankPos.x - padX > x)&&(player.tankPos.x - padX < x+padO)){
      return 1;//right
   }
   return 0;
}
int playerCollisionY(float y, Player player, float padO, float padY){
   
   if((player.tankPos.y + padY < y)&&(player.tankPos.y + padY > y-padO)){
      return -1;//bottom
   }
   if((player.tankPos.y - padY > y)&&(player.tankPos.y - padY < y+padO)){
      return 1;//top
   }
   return 0;
}
int playerCollisionZ(float z, Player player, float padO, float padZ){
   
   if((player.tankPos.z + padZ < z)&&(player.tankPos.z + padZ > z-padO)){
      return -1;//back
   }
   if((player.tankPos.z - padZ > z)&&(player.tankPos.z - padZ < z+padO)){
      return 1;//front
   }
   return 0;
}

float absoluteFloat(float in){

  float out;
  out = in <= 0 ? -in: in;
  return out;
}

int getCreate(float x, float y, float z){
   int createNum = MAPL*MAPW*(mapY(y, MAPSIZE))+MAPL*(mapX(x, MAPSIZE))+(mapZ(z, MAPSIZE));

   if((mapY(y, MAPSIZE) < 0)||(mapY(y, MAPSIZE) >= MAPH)){
      return -1;
   }
   if((mapX(x, MAPSIZE) < 0)||(mapX(x, MAPSIZE) >= MAPW)){
      return -1;
   }
   if((mapZ(z, MAPSIZE) < 0)||(mapZ(z, MAPSIZE) >= MAPL)){
      return -1;
   }

   return createNum;
}

int drawHealthBar(Player player, int screenSide){

   drawHUD(-0.75*screenSide, 0.9, 0, 0.6666*player.health, 0.2, 1.0, 0.0, 0.0);
   drawHUD(-0.75*screenSide, 0.9, 0, 2.2, 0.4, 0.0, 0.0, 0.0);

   return 1;
}

int drawMenu(){

   drawHUD(0,  0.1, 0, 3, 0.6, 0.0, 0.0, 0.0);
   drawHUD(0, -0.1, 0, 3, 0.6, 0.0, 0.0, 0.0);
   drawHUD(0, -0.3, 0, 3, 0.6, 0.0, 0.0, 0.0);
   drawHUD(0, -0.5, 0, 3, 0.6, 0.0, 0.0, 0.0);

   glUseProgram(0);
   render2dText("Start/Continue", 1.0, 0.0, 0.0, -0.15, 0.1);
   render2dText("2 Player", 1.0, 0.0, 0.0, -0.1, -0.1);
   render2dText("Options", 1.0, 0.0, 0.0, -0.1, -0.3);
   render2dText("Quit", 1.0, 0.0, 0.0, -0.05, -0.5);
   glClearColor(1.0,1.0,1.0,1.0);

   return 1;
}

int playerCreateCollisionZ(Player& player, int position, int movment, float hitBox){

   int create = getCreate(player.tankPos.x,player.tankPos.y+2,player.tankPos.z + hitBox*player.directionBody.z);
   if(create != -1){
      if(absoluteFloat(playerCollisionZ(creates[create].position.z, player, MAPSIZE, hitBox*position*player.directionBody.z))){
         player.movingZ = 0;
         player.speed.z = -movment*10*ACCEL/FPS;
      }
   }
}

int playerCreateCollisionX(Player& player, int position, int movment, float hitBox){

   int create = getCreate(player.tankPos.x + hitBox*player.directionBody.x,player.tankPos.y+2,player.tankPos.z);
   if(create != -1){
      if(absoluteFloat(playerCollisionX(creates[create].position.x, player, MAPSIZE, hitBox*position*player.directionBody.x))){
         player.movingX = 0;
         player.speed.x = -movment*10*ACCEL/FPS;
      }
   }
}

int drawHUD(float x, float y, float rotation, float scaleX,  float scaleY, float red, float green, float blue){

   ambient = Vector3f(red,green,blue);
   specularPower = 10000.0;
   glUniform1f(SpecularPowerUniformLocation, specularPower);
   glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 0.1);

   ProjectionMatrix.ortho(-1, 1, -1, 1, -0.0001, -2);
   glUniformMatrix4fv(	
	ProjectionUniformLocation, 
	1,							
	false,						
	ProjectionMatrix.getPtr());
   Matrix4x4 m(ModelViewMatrix);

   m.toIdentity();

   m.translate(x, y, 1.0);

   m.rotate(rotation, 0, 0, 1);

   m.scale(scaleX, scaleY, 1);

   glUniformMatrix4fv(	
	MVMatrixUniformLocation,  	
	1,					       
	false,				     
	m.getPtr());	      

   drawGeometry();

   ambient = Vector3f(0.1, 0.1, 0.1);
   specularPower = 10.0;
   glUniform1f(SpecularPowerUniformLocation, specularPower);
   glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 0.1);

}

int handleMenuMouse(){
   switch(state){
   case 0:
      if((globalMouse.x >  0.35*screenHeight)&&(globalMouse.x < 0.65*screenHeight)&&(globalMouse.state == 0)&&(globalMouse.button == 0)){
         if((globalMouse.y > 0.42*screenWidth)&&(globalMouse.y < 0.58*screenWidth)){
            state = 1;
            playerOne.cameraMode = 0;
         }
         if((globalMouse.y > 0.52*screenWidth)&&(globalMouse.y < 0.58*screenWidth)){
            resetPlayer(playerOne);
            loadMap(levels[0]);
            state = 5;
            playerOne.cameraMode = 0;
         }
         if((globalMouse.y > 0.62*screenWidth)&&(globalMouse.y < 0.68*screenWidth)){
            state = 4;
         }
         if((globalMouse.y > 0.72*screenWidth)&&(globalMouse.y < 0.78*screenWidth)){
            exit(0);
         }
      }
   break;
   }
}

char* nextLevel(){

   playerOne.score = 0;
   currentLevel++;
   if(currentLevel >= 6){
      std::cout << "Fin" << std::endl;
      state = 3;
      return levels[currentLevel -1];
   }

   return levels[currentLevel];
}

int resetPlayer(Player& player){
   player.tankPos = Vector3f(player.initalPos.x, player.initalPos.y, player.initalPos.z);
   player.speed = Vector3f(0,0,0);
}

//Init Shader(self)
void initShader(){
   //Create shader
   shaderProgramID = Shader::LoadFromFile("shader.vert","shader.frag");
    
   // Get a handle for our vertex position buffer
   vertexPositionAttribute = glGetAttribLocation(shaderProgramID,  "aVertexPosition");
   vertexNormalAttribute = glGetAttribLocation(shaderProgramID,    "aVertexNormal");
   vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID, "aVertexTexcoord");

    //!
   MVMatrixUniformLocation         = glGetUniformLocation(shaderProgramID, "MVMatrix_uniform"); 
   ProjectionUniformLocation       = glGetUniformLocation(shaderProgramID, "ProjMatrix_uniform"); 
   LightPositionUniformLocation    = glGetUniformLocation(shaderProgramID, "LightPosition_uniform"); 
   AmbientUniformLocation          = glGetUniformLocation(shaderProgramID, "Ambient_uniform"); 
   SpecularUniformLocation         = glGetUniformLocation(shaderProgramID, "Specular_uniform"); 
   SpecularPowerUniformLocation    = glGetUniformLocation(shaderProgramID, "SpecularPower_uniform");
   TextureMapUniformLocation       = glGetUniformLocation(shaderProgramID, "TextureMap_uniform"); 
}

void initTexture(std::string filename, GLuint & textureID){ // self
   //Generate texture and bind
   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_2D, textureID);
    
   //Set texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

   //Get texture Data
   int width, height;
   char* data;
   Texture::LoadBMP(filename, width, height, data);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   glBindTexture(GL_TEXTURE_2D, 0);

   //Cleanup data - copied to GPU
   delete[] data;
}

void render2dText(std::string text, float r, float g, float b, float x, float y){
   glColor3f(r,g,b);
   glRasterPos2f(x, y); // window coordinates
   for(unsigned int i = 0; i < text.size(); i++){
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
   }
}

void initGeometry()
{
	static const GLfloat vertexPositionData[] = 
	{ 
		-0.1f,  0.1f, 0.1f,
		-0.1f, -0.1f, 0.1f,
		 0.1f, -0.1f, 0.1f,

		 0.1f, -0.1f, 0.1f,
		 0.1f,  0.1f, 0.1f,
		-0.1f,  0.1f, 0.1f,
	};

	glGenBuffers(1, &vertexPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositionData), vertexPositionData, GL_STATIC_DRAW);
}

void drawGeometry()
{
	glEnableVertexAttribArray(vertexPositionAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
	glVertexAttribPointer(
		vertexPositionAttribute,
		3,                      
		GL_FLOAT,               
		GL_FALSE,               
		0,                      
		(void*)0                   
	);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(vertexPositionAttribute);
}
















