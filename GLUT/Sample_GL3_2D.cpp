/****************************************************** DO NOT CHANGE THIS SECTION OF CODE **************************************************/
#include <iostream>
#include <cmath>
#include<cstring>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>
#include <sstream>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define PI M_PI

using namespace std;
typedef struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
}VAO;
struct GLMatrices {
    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
} Matrices;
GLuint programID;
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    GLint Result = GL_FALSE;
    int InfoLogLength;
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    return ProgramID;
}
VAO* create3DObject(GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;
    glGenVertexArrays(1, &(vao->VertexArrayID));
    glGenBuffers (1, &(vao->VertexBuffer));
    glGenBuffers (1, &(vao->ColorBuffer));
    glBindVertexArray (vao->VertexArrayID); 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer);
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    return vao;
}
void draw3DObject (struct VAO* vao)
{
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);
    glBindVertexArray (vao->VertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices);
}
/***************************************************** SELF DEFINED FUNCTIONS ***************************************************************/

#define F first
#define S second
#define mp make_pair
#define pb push_back
#define MAX 1000

typedef pair< float, float > dub;
typedef pair< dub ,float > tup;
map<char,int> value;
bitset<8> ok;

int width,height;
VAO* objects[MAX];
VAO* strokes[8];
float Mass[MAX];
float velx[MAX];
float vely[MAX];
float Timer[MAX];
float startX[MAX];
float startY[MAX];
bool movable[MAX];
int count[MAX]={0};
int score=0;
float COR=0.6f;
float ADG=1.0f;
float tick=0.6f;
float Lx=15.0f,Ly=20.0f;

vector< tup > centre[MAX];
glm::vec3 trans[MAX];
float rotat[MAX];

float formatAngle(float A)
{
    if(A<0.0f)
        return A+360.0f;
    if(A>=360.0f)
        return A-360.0f;
    return A;
}
float D2R(float A)
{
    return (A*PI)/180.0f;
}
float sqr(float x)
{
    return x*x;
}
float dis(dub p,dub q)
{
    return sqr(p.F-q.F)+sqr(p.S-q.S);
}

void *font = GLUT_BITMAP_TIMES_ROMAN_24;
void *fonts[] =
{
    GLUT_BITMAP_9_BY_15,
    GLUT_BITMAP_TIMES_ROMAN_10,
    GLUT_BITMAP_TIMES_ROMAN_24
};
char defaultMessage[] = "GLUT means OpenGL.";
char *message = defaultMessage;

void selectFont(int newfont)
{
    font = fonts[newfont];
    glutPostRedisplay();
}

void selectColor(int color)
{
    switch (color) {
        case 1:
            glColor3f(1.0, 0.0, 0.0);
            break;
        case 2:
            glColor3f(1.0, 0.0, 0.0);
            break;
        case 3:
            glColor3f(1.0, 0.0, 0.0);
            break;
    }
    glutPostRedisplay();
}

void output(int x, int y, char str[])
{
    int len, i;
    glRasterPos2f(x, y);
    len = (int) strlen(str);
    for (i = 0; i < len; i++) 
    {
        glutBitmapCharacter(font, str[i]);
    }
}

float toBits(int a)
{
    ok.reset();
    for(int i=0;i<8;i++)
    {
        if((a>>i)&1)
        {
            ok.set(i);
        }

    }
}

/********************************************************** AIR FRICTION ********************************************************************/

float EKMT(float K,float M,float T)
{
    return exp(-((K/M)*T));
}
float xvel(float V0,float K,float M,float T)
{
    return V0*EKMT(K,M,T);
}
float xdis(float V0,float K,float M,float T)
{
    return ((M/K)*V0)*(1.0f-EKMT(K,M,T));
}
float yvel(float V0,float K,float M,float T,float G)
{
    return ((V0+((M*G)/K))*EKMT(K,M,T))-((M*G)/K);
}
float ydis(float V0,float K,float M,float T,float G)
{
    return ((M/K)*(V0+((M*G)/K))*(1-EKMT(K,M,T)))-((M*G*T)/K);
}
float equilib(int i)
{
    return (Mass[i]/0.3f)*((vely[i]*0.3f+1.0f)/(Mass[i]*ADG));
}

/*********************************************************** DETECTING COLLISIONS ***********************************************************/


int collided(tup p,tup q)
{
    float cdis=dis(p.F,q.F);
    float rsum=sqr(p.S+q.S);
    if(cdis<=rsum)
        return 1;
    else
        return 0;
}

bool checkCollision(int i,int j)
{
    float R;
    tup A,B,V;
    for(int k=0;k<centre[i].size();k++)
    {
        for(int l=0;l<centre[j].size();l++)
        {
            V=centre[i][k];
            R=V.F.F;
            A.F.F=trans[i][0]+R*cos(D2R(formatAngle(rotat[i])));
            A.F.S=trans[i][1]+R*sin(D2R(formatAngle(rotat[i])));
            A.S=V.S;

            V=centre[j][l];
            R=V.F.F;
            B.F.F=trans[j][0]+R*cos(D2R(formatAngle(rotat[j])));
            B.F.S=trans[j][1]+R*sin(D2R(formatAngle(rotat[j])));
            B.S=V.S;
            if(collided(A,B))
                return true;
        }
    }
    return false;
}

/***************************************************** KEYBOARD AND MOUSE FUNCTIONS  *******************************************************/

float rotateBarrel;
float speed=0;
float xmousepos,ymousepos;
float zoomX=800,zoomY=600;
int buttonPressed=0;
bool visible=true;

void keyboardDown(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'Q':
            exit(0);
            break;
        case 'f':
            speed+=0.1;
            break;
        case 's':
            speed-=0.1;
            break;
        case ' ':
            speed=30*(xmousepos/400);
            buttonPressed=1;
            velx[9]=speed*(cos(rotateBarrel*(M_PI/180)));
            vely[9]=speed*(sin(rotateBarrel*(M_PI/180)));
            Timer[9]=0.0f;
            break;
        case 'q':
            exit(0);
            break;
        case 27: //ESC
            exit (0);
            break;
        default:
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 32:
            break;
        case 'b':
            break;
    }
}

float panX=0,panY=0;

void keyboardSpecialDown(int key, int x, int y)
{
    switch(key)
    {
        case 100:
            panX+=3.0f;
            break;
        case 102:
            panX-=3.0f;
            break;
        case 101:
            zoomY+=5.0f;
            zoomX+=5.0f;
            break;
        case 103:
            zoomY-=3.0f;
            zoomX-=3.0f;
            break;
    }
}

void keyboardSpecialUp(int key, int x, int y)
{
}

float radius=10.0f;
void mouseClick(int button, int state, int x, int y)
{
    speed=30*(xmousepos/400);
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        buttonPressed=1;
        velx[9]=speed*(cos(rotateBarrel*(M_PI/180)));
        vely[9]=speed*(sin(rotateBarrel*(M_PI/180)));
        Timer[9]=0.0f;
    }
    if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        buttonPressed=0;
        //radius=10.0f;
        rotateBarrel=0.0f;
        velx[9]=32.0f;
        vely[9]=0.0f;
        Timer[9]=0.0f;
    }
    if(button==4)
    {
        zoomX+=5.0f;
        zoomY+=5.0f;
    }
    else if(button==3)
    {
        zoomX-=5.0f;
        zoomY-=5.0f;
    }
    cerr << x << y << "\n";
}
void mouseMotion(int x, int y)
{
    cerr << x << y << "\n";
}

void cursorPos(int x, int y)
{
    xmousepos=x;
    ymousepos=y;
    trans[9][0]=speed*cos(D2R(rotateBarrel));
    trans[9][1]=speed*sin(D2R(rotateBarrel));
}

void reshapeWindow(int width, int height)
{
    GLfloat fov=90.0f;
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    Matrices.projection=glm::ortho(-zoomX/2.0f,zoomX/2.0f,-zoomY/2.0f,zoomY/2.0f,0.1f, 500.0f);
}

VAO* createRectangle(float x,float y, GLfloat colours[])
{
    GLfloat vertex_buffer_data [] = {
        -x,-y,0.0, // vertex 1
        x,-y,0.0, // vertex 2
        x, y,0.0, // vertex 3

        x, y,0.0, // vertex 3
        -x, y,0.0, // vertex 4
        -x,-y,0.0  // vertex 1
    };
    GLfloat color_buffer_data [] = {
        1,0,0, // color 1
        1,0,0, // color 2
        1,0,0, // color 3
        1,0,0, // color 3
        1,0,0, // color 4
        1,0,0  // color 1
    };
    return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, colours, GL_FILL);
}

void drawobject(VAO* obj,glm::vec3 trans,float angle,glm::vec3 rotat)
{
    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 VP = Matrices.projection * Matrices.view;
    glm::mat4 MVP;
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translatemat = glm::translate(trans);
    glm::mat4 rotatemat = glm::rotate(D2R(formatAngle(angle)), rotat);
    Matrices.model *= (translatemat * rotatemat);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(obj);
}

VAO* createLine(float X1,float Y1,float X2,float Y2)
{
    GLfloat vertex_buffer_data[]={X1,Y1,0.0f,X2,Y2,0.0f};
    GLfloat color_buffer_data[]={102.0/255.0,51.0/255.0,0,102.0/255.0,51.0/255.0,0};
    return create3DObject(GL_LINES,2,vertex_buffer_data,color_buffer_data,GL_LINE);
}

VAO* createSector(float R,int parts,GLfloat colours[])
{
    float diff=360.0f/parts;
    float A1=formatAngle(-diff/2);
    float A2=formatAngle(diff/2);
    GLfloat vertex_buffer_data[]={0.0f,0.0f,0.0f,R*cos(D2R(A1)),R*sin(D2R(A1)),0.0f,R*cos(D2R(A2)),R*sin(D2R(A2)),0.0f};
    GLfloat color_buffer_data[]={1,0,0,1,0,0,1,0,0};
    return create3DObject(GL_TRIANGLES,3,vertex_buffer_data,colours,GL_FILL);
}

void trt(VAO* obj,double toX,double toY,double rot_angle,double width,double height)
{
    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 VP = Matrices.projection * Matrices.view;
    glm::mat4 MVP;
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translatepivot= glm::translate(glm::vec3(width,height,0));
    //glm::mat4 revtranslatepivot= glm::translate(glm::vec3(-width,-4*height,0));
    glm::mat4 translatemat = glm::translate(glm::vec3(toX,toY,0));
    glm::mat4 rotatemat = glm::rotate(D2R(formatAngle(rot_angle)), glm::vec3(0,0,1));
    //Matrices.model *= (translatemat * revtranslatepivot * rotatemat * translatepivot);
    Matrices.model *= (translatemat * rotatemat * translatepivot);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(obj);
}

void conserveMomentum(int i,int j)
{
    float u1,u2,v1,v2;
    u1=xvel(velx[i],0.3f,Mass[i],Timer[i]);
    u2=xvel(velx[j],0.3f,Mass[j],Timer[j]);
    v1=(((Mass[i]-COR*Mass[j])*u1)/(Mass[i]+Mass[j]))+(((Mass[j]+COR*Mass[j])*u2)/(Mass[i]+Mass[j]));
    v2=u1*COR-u2*COR+v1;
    velx[i]=v1;
    velx[j]=v2;
    u1=yvel(vely[i],0.3f,Mass[i],Timer[i],ADG);
    u2=yvel(vely[j],0.3f,Mass[j],Timer[j],ADG);
    v1=(((Mass[i]-COR*Mass[j])*u1)/(Mass[i]+Mass[j]))+(((Mass[j]+COR*Mass[j])*u2)/(Mass[i]+Mass[j]));
    v2=u1*COR-u2*COR+v1;
    vely[i]=v1;
    vely[j]=v2;
}

void moveProjectile()
{
    if(buttonPressed==0)
    {
        startX[9]=trans[9][0]=-314+speed*cos(D2R(rotateBarrel));
        startY[9]=trans[9][1]=-190+speed*sin(D2R(rotateBarrel));
    }
    for(int i=1;i<16;i++)
    {
        if(i==9 && buttonPressed==0)
        {
            continue;
        }
        float veloy=yvel(vely[i],0.3f,Mass[i],Timer[i],ADG);
        float velox=xvel(velx[i],0.3f,Mass[i],Timer[i]);
        if(velx[i]!=0.0f || vely[i]!=0.0f)
        {
            trans[i][0]=startX[i]+xdis(velx[i],0.3f,Mass[i],Timer[i]);
            if(i!=13)
            {
                trans[i][1]=startY[i]+ydis(vely[i],0.3f,Mass[i],Timer[i],ADG);
            }
            Timer[i]+=tick;
        }
    }
}

bool vanish=false,vanish1=false,rod=false,rodscore=false;
bool temp=false,piggy=true;
int xpos=-320,flag1=0,flag=0;
float touch=20.0f;
float prevTransX,prevTransY;
GLfloat green[]={0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0};
GLfloat blueblack[]={0.0,0.0,51.0/255.0,0.0,0.0,51.0/255.0,0.0,0.0,51.0/255.0,0.0,0.0,51.0/255.0,0.0,0.0,51.0/255.0,0.0,0.0,51.0/255.0};

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram (programID);
    char str[10]="Varshit";
    output(0, 0, str);
    Matrices.projection=glm::ortho(-(zoomX/2.0f)+panX,(zoomX/2.0f)+panX,-(zoomY/2.0f)+panY,(zoomY/2.0f)+panY,0.1f, 500.0f);
    //output(100, 100, message);
    //output(50, 145, "(positioned in pixels with upper-left origin)");
    //Drawing objects
    moveProjectile();
    //Topple projectile
    if(checkCollision(9,10) && temp==false)
    {
        temp=true;
        velx[9]=-COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
        vely[9]=yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        startX[9]=trans[9][0];
        startY[9]=trans[9][1];
        Timer[9]=tick;
    }
    //Reflect from floor
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,0))
    {
        velx[9]=xvel(velx[9],0.3f,Mass[9],Timer[9]);
        vely[9]=-COR*yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        if((vely[9]<2.0f) && checkCollision(9,0))
        {
            trans[9][1]=-270.0f;
            velx[9]=0.0f;
            vely[9]=0.0f;
            Timer[9]=0.0f;
        }
        else
        {
            trans[9][1]=-270.0f;
            startX[9]=trans[9][0];
            startY[9]=trans[9][1];
            Timer[9]=tick;
        }
    }

    //Reflect from right wall
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,1))
    {
        velx[9]=-COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
        vely[9]=yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        trans[9][0]=360.0f;
        startX[9]=trans[9][0];
        startY[9]=trans[9][1];
        Timer[9]=tick;
    }

    //Reflect from left wall
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,3))
    {
        velx[9]=-COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
        vely[9]=yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        trans[9][0]=-370.0f;
        startX[9]=trans[9][0];
        startY[9]=trans[9][1];
        Timer[9]=tick;
    }

    //Move upper block
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,13))
    {
        count[13]+=1;
        score+=20;
        float prev=xvel(velx[9],0.3f,Mass[9],Timer[9]);
        if(prev>0.0f)
        {
            startX[9]=trans[13][0]-40.0f;
        }
        else
        {
            startX[9]=trans[13][0]+40.0f;
        }
        startY[9]=trans[9][1];
        startX[13]=trans[13][0];
        //velx[13]=velx[9]+prev*COR;
        if(checkCollision(9,13))
        {
            velx[9]=((Mass[9]-COR*Mass[13])/(Mass[9]+Mass[13]))*prev;
            velx[13]=COR*prev+velx[9];  
        }
        Timer[9]=tick;
        Timer[13]=tick;
    }
    if((velx[13]!=0.0f || vely[13]!=0.0f) && checkCollision(13,12))
    {
        if(trans[13][0]<120 && checkCollision(13,12))
        {
            velx[13]=-COR*xvel(velx[13],0.3f,Mass[13],Timer[13]);
        }
        while(trans[13][0]<120 && !checkCollision(13,12))
        {
            trans[13][1]-=0.1f;;
        }
        if(trans[13][0]>=120.0f && checkCollision(13,12))
        {
            velx[13]=COR*xvel(velx[13],0.3f,Mass[13],Timer[13]);
        }
        while(trans[13][0]>=120.0f && !checkCollision(13,12))
        {
            trans[13][1]-=1.0f;
        }
        startX[13]=trans[13][0];
        startY[13]=trans[13][1];
        Timer[13]=tick;
    }
    if((velx[13]!=0.0f || vely[13]!=0.0f) && checkCollision(13,1))
    {
        if(!checkCollision(13,12) && !checkCollision(13,0))
        {
            trans[13][1]-=1.0f;
            trans[13][0]-=1.0f;
        }
        //velx[13]=-COR*xvel(velx[13],0.3f,Mass[13],Timer[13]);
        //vely[13]=yvel(vely[13],0.3f,Mass[13],Timer[13],ADG);
        //trans[13][0]=360.0f;
        //startX[13]=trans[13][0];
        //startY[13]=trans[13][1];
        Timer[13]=tick;
    }

    //Lower block fixed
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,12))
    {
        if(trans[9][0]<120.0f && checkCollision(9,12))
        {
            velx[9]=-COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
            vely[9]=yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        }
        else if(trans[9][0]==120.0f)
        {
            trans[9][0]=120.0f;
            trans[9][1]=-220.0f;
        }
        else if(trans[9][0]>=160.0f && checkCollision(9,12))
        {
            velx[9]=-COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
            //velx[9]=-5.0f;
            vely[9]=10.0f;
            trans[9][0]=190.0f;
        }
        while(trans[9][1]>=-250.0f && checkCollision(9,12))
        {
            velx[9]=COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
            //velx[9]=3.0f;
            vely[9]=10.0f;
            trans[9][1]=-210.0f;
        }
        startX[9]=trans[9][0];
        startY[9]=trans[9][1];
        Timer[9]=tick;
    }

    //Pillar 2 fixed
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,11))
    {
        velx[9]=-COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
        vely[9]=yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        if(trans[9][0]<280)
        {
            trans[9][0]=280.0f-touch;
        }
        if(trans[9][0]==280)
        {
            trans[9][0]=280.0f;
        }
        if(trans[9][0]>280)
        {
            trans[9][0]=280.0f+touch;
        }
        startX[9]=trans[9][0];
        startY[9]=trans[9][1];
        Timer[9]=tick;
    }

    //Pillar 3 fixed
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,21))
    {
        velx[9]=-COR*xvel(velx[9],0.3f,Mass[9],Timer[9]);
        vely[9]=yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        if(trans[9][0]<150)
        {
            trans[9][0]=150.0f-touch;
        }
        if(trans[9][0]==150)
        {
            trans[9][0]=150.0f;
        }
        if(trans[9][0]>150)
        {
            trans[9][0]=150.0f+touch;
        }
        startX[9]=trans[9][0];
        startY[9]=trans[9][1];
        Timer[9]=tick;
    }

    //Pillar 4 fixed
    if((velx[9]!=0.0f || vely[9]!=0.0f) && checkCollision(9,22))
    {
        velx[9]=xvel(velx[9],0.3f,Mass[9],Timer[9]);
        vely[9]=-COR*yvel(vely[9],0.3f,Mass[9],Timer[9],ADG);
        if(trans[9][1]<trans[22][1] && checkCollision(9,22))
        {
            //cout << "bhajesh " << trans[9][1] << " " << trans[22][1] << endl;
            trans[9][1]=-touch;
            velx[9]=0.0f;
            vely[9]=0.0f;
            Timer[9]=0.0f;
        }
        else
        {
            trans[9][1]=touch;
            startX[9]=trans[9][0];
            startY[9]=trans[9][1];
            Timer[9]=tick;
        }
    }

    //power background
    drawobject(objects[23],trans[23],rotat[23],glm::vec3(0,0,1));   

    //Power
    int num=((int)xmousepos%800)/50;
    for(int j=0;j<num;j++)
    {
        drawobject(objects[14],glm::vec3(xpos+j*25,trans[14][1],0),rotat[14],glm::vec3(0,0,1));
    }

    //Walls
    //Floor
    drawobject(objects[0],trans[0],rotat[0],glm::vec3(0,0,1));   
    //Right Wall
    drawobject(objects[1],trans[1],rotat[1],glm::vec3(0,0,1));   
    //Top Wall
    drawobject(objects[2],trans[2],rotat[2],glm::vec3(0,0,1));   
    //Left Wall
    drawobject(objects[3],trans[3],rotat[3],glm::vec3(0,0,1));   
    //Sun
    for(int i=0;i<20;i+=2)
    {
        drawobject(objects[25],trans[25],i*20,glm::vec3(0,0,1));   
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[24],trans[24],i*20,glm::vec3(0,0,1));   
    }

    //Cannon
    //Circle
    for(int i=1;i<20;i++)
    {
        drawobject(objects[4],trans[4],i*20,glm::vec3(0,0,1));   
    }
    //Rectangle
    drawobject(objects[5],trans[5],rotat[5],glm::vec3(0,0,1));   
    //Circle
    for(int i=1;i<20;i++)
    {
        drawobject(objects[6],trans[6],i*20,glm::vec3(0,0,1));   
    }
    //Tank Head
    for(int i=1;i<20;i++)
    {
        drawobject(objects[7],trans[7],i*20,glm::vec3(0,0,1));   
    }
    //Barrel
    rotateBarrel=atan2((-ymousepos+300-trans[7][1]),(xmousepos-400-trans[7][0]))*(180/M_PI);
    trans[8][0]=trans[7][0]+50*cos(rotateBarrel*(M_PI/180));
    trans[8][1]=trans[7][1]+50*sin(rotateBarrel*(M_PI/180));
    drawobject(objects[8],trans[8],rotateBarrel,glm::vec3(0,0,1));
    //Projectile
    for(int i=1;i<360;i++)
    {
        drawobject(objects[9],trans[9],i,glm::vec3(0,0,1));   
    }
    for(int i=1;i<18;i++)
    {
        drawobject(objects[26],trans[26],i*10,glm::vec3(0,0,1));   
    }
    //Pillar 3
    drawobject(objects[21],trans[21],rotat[21],glm::vec3(0,0,1));
    //Pillar94
    drawobject(objects[22],trans[22],rotat[22],glm::vec3(0,0,1));
    //Power up
    if(!checkCollision(9,28) && !vanish)
    {
        for(int i=0;i<20;i++)
        {
            drawobject(objects[28],trans[28],i*20,glm::vec3(0,0,1));
        }
    }
    if(!checkCollision(9,29) && !vanish1)
    {
        for(int i=0;i<20;i++)
        {
            drawobject(objects[29],trans[29],i*20,glm::vec3(0,0,1));
        }
    }
    if(checkCollision(9,29) && radius==15)
    {
        vanish1=true;
        radius=10.0f;
    }
    if(checkCollision(9,28))
    {
        vanish=true;
        radius=15.0f;
    }
    if(vanish1)
    {
        objects[9]=createSector(radius,18,blueblack);
        trans[29]=glm::vec3(800.0f,500.0f,0.0f);
        centre[9].pb(mp(mp(0.0f,0.0f),radius));
        touch=20.0f;
        if(flag1==0)
        {
            score-=10;
        }
        flag1=1;
    }
    if(vanish)
    {
        objects[9]=createSector(radius,18,blueblack);
        if(flag==0)
        {
            score+=20;
        }
        trans[28]=glm::vec3(800.0f,500.0f,0.0f);
        centre[9].pb(mp(mp(0.0f,0.0f),radius));
        touch=40.0f;
        flag=1;
    }
    //Most of the drawing
    if(checkCollision(20,10) && piggy)
    {
        piggy=false;
        score+=40;
    }
    if(checkCollision(9,10))
    {
        count[10]+=1;
    }
    if(rodscore)
    {
        if(checkCollision(9,10))
        {
            score+=10;
            rodscore=false;
        }
    }
    for(int i=10;i<20;i++)
    {
        if(i==10 && temp && !rod)
        {
            rotat[i]-=1.0f;
            if(rotat[i]==0)
            {
                rod=true;
                trans[i][0]=5.0f;
                trans[i][1]=-270.0f;
                rodscore=true;
                temp=false;
                continue;
            }
            trans[i][0]=-45.0f;
            trans[i][1]=-280.0f;
            trt(objects[i],trans[i][0],trans[i][1],rotat[i],50.0f,10.0f);
        }
        else
        {
            if(count[i]<3 && i<15)
            {
                drawobject(objects[i],trans[i],rotat[i],glm::vec3(0,0,1));
            }
            if(count[i]>=3)
            {            
                trans[i]=glm::vec3(-400.0f,-300.0f,0.0f);
            }
            //Pigs
            if(piggy)
            {
                for(int i=1;i<20;i++)
                {
                    drawobject(objects[15],trans[15],i*20,glm::vec3(0,0,1));   
                }
                for(int i=1;i<20;i++)
                {
                    drawobject(objects[16],trans[16],i*20,glm::vec3(0,0,1));   
                }
                for(int i=1;i<20;i++)
                {
                    drawobject(objects[17],trans[17],i*20,glm::vec3(0,0,1));   
                }
                for(int i=1;i<20;i++)
                {
                    drawobject(objects[18],trans[18],i*20,glm::vec3(0,0,1));   
                }
                for(int i=1;i<20;i++)
                {
                    drawobject(objects[19],trans[19],i*20,glm::vec3(0,0,1));   
                }
                for(int i=1;i<20;i++)
                {
                    drawobject(objects[20],trans[20],i*20,glm::vec3(0,0,1));   
                }
            }
        }
    }
    //Inner Lower block
    drawobject(objects[27],trans[27],rotat[27],glm::vec3(0,0,1));
    //Inner Sun
    for(int i=0;i<20;i++)
    {
        drawobject(objects[30],trans[30],i*20,glm::vec3(0,0,1));
    }
    //Cloud
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-130,130,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-120,110,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-100,140,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-90,100,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-70,145,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-66,100,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-40,145,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-35,110,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-15,135,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-65,130,0),i*20,glm::vec3(0,0,1));
    }
    for(int i=0;i<20;i++)
    {
        drawobject(objects[31],glm::vec3(-85,130,0),i*20,glm::vec3(0,0,1));
    }
    //Inner Floor
    drawobject(objects[32],trans[32],rotat[32],glm::vec3(0,0,1));
    //Top rectangle
    drawobject(objects[33],trans[33],rotat[33],glm::vec3(0,0,1));
    //Text
    stringstream ss;
    ss << score;
    string text="score"+ss.str();
    for(int i=0;i<5;i++)
    {
        toBits(value[text[i]]);
        for(int j=0;j<8;j++)
        {
            if(ok[j])
            {
                drawobject(strokes[j],glm::vec3(100.0f+i*(2*Lx+5),235.0f,0),0,glm::vec3(0,0,1));
            }
        }
    }
    for(int i=5;i<text.length();i++)
    {
        toBits(value[text[i]]);
        for(int j=0;j<8;j++)
        {
            if(ok[j])
            {
                drawobject(strokes[j],glm::vec3(100.0f+i*(2*Lx+5),235.0f,0),0,glm::vec3(0,0,1));
            }
        }
    }
    glutSwapBuffers ();
}

void idle()
{
    draw ();
}

void initGLUT(int& argc, char** argv, int width, int height)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_CORE_PROFILE);
    glutInitWindowSize(width, height);
    glutCreateWindow("Assignment");
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "Error: Failed to initialise GLEW : "<< glewGetErrorString(err) << endl;
        exit (1);
    }
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(keyboardSpecialDown);
    glutSpecialUpFunc(keyboardSpecialUp);
    glutPassiveMotionFunc(cursorPos);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMotion);
    glutReshapeFunc(reshapeWindow);
    glutDisplayFunc(draw);
    glutIdleFunc(idle);
    //glutIgnoreKeyRepeat (true); // Ignore keys held down*/
}

VAO* createTriangle(float X1,float Y1,float X2,float Y2,float X3,float Y3)
{
    GLfloat vertex_buffer_data[]={X1,Y1,0.0f,X2,Y2,0.0f,X3,Y3,0.0f};
    GLfloat color_buffer_data[]={1,0,0,1,0,0,1,0,0};
    return create3DObject(GL_TRIANGLES,3,vertex_buffer_data,color_buffer_data,GL_FILL);
}

void divideRect(int i,float width,float height)
{
    for(int j=0;j<(int)(width/(2.0*height));j++)
    {
        centre[i].pb(mp(mp(-(2.0f*j+1.0f)*height,0.0f),height));
    }
    for(int j=0;j<(int)(width/(2.0*height));j++)
    {
        centre[i].pb(mp(mp((2.0f*j+1.0f)*height,0.0f),height));
    }
}

void initGL(int width, int height)
{
    //Colours

    float r,g,b;
    GLfloat blue[]={0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0};
    GLfloat redgreen[]={1.0,0.0,0.0,0.0,1.0,0.0,1.0,0.0,0.0,0.0,1.0,0.0,1.0,0.0,0.0,0.0,1.0,0.0};
    GLfloat bluegreen[]={0.0,0.0,1.0,0.0,1.0,0.0,0.0,0.0,1.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0};
    GLfloat yellow[]={1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0,0.0};
    GLfloat lightyellow[]={1.0,1.0,77.0/255.0,1.0,1.0,77.0/255.0,1.0,1.0,77.0/255.0,1.0,1.0,77.0/255.0,1.0,1.0,77.0/255.0,1.0,1.0,77.0/255.0};
    GLfloat lightblue[]={0.0,102.0/255.0,1.0,0.0,102.0/255.0,1.0,0.0,102.0/255.0,1.0,0.0,102.0/255.0,1.0,0.0,102.0/255.0,1.0,0.0,102.0/255.0,1.0};
    r=153.0/255.0;
    g=115.0/255.0;
    b=0.0/255.0;
    GLfloat lightbrown[]={r,g,b,r,g,b,r,g,b,r,g,b,r,g,b,r,g,b};
    r=77.0/255.0;
    g=58.0/255.0;
    b=0.0/255.0;
    GLfloat darkbrown[]={r,g,b,r,g,b,r,g,b,r,g,b,r,g,b,r,g,b};
    r=1;
    g=153.0/255.0;
    b=51.0/255.0;
    GLfloat lightorange[]={r,g,b,r,g,b,r,g,b,r,g,b,r,g,b,r,g,b};
    r=1;
    g=1;
    b=1;
    GLfloat white[]={r,g,b,r,g,b,r,g,b,r,g,b,r,g,b,r,g,b};
    r=153.0/255.0;
    g=1;
    b=221.0/255.0;
    GLfloat lighestblue[]={r,g,b,r,g,b,r,g,b,r,g,b,r,g,b,r,g,b};
    r=0;
    g=153.0/255.0;
    b=0;
    GLfloat lighestgreen[]={r,g,b,r,g,b,r,g,b,r,g,b,r,g,b,r,g,b};
    //Floor
    objects[0]=createRectangle(400.0f,10.0f,green);
    divideRect(0,400.0f,10.0f);
    trans[0]=glm::vec3(0.0f,-290.0f,0.0f);
    rotat[0]=0.0f;
    movable[0]=false;
    //Lower Floor
    objects[32]=createRectangle(380.0f,9.0f,lighestgreen);
    divideRect(32,400.0f,10.0f);
    trans[32]=glm::vec3(0.0f,-300.0f,0.0f);
    rotat[32]=0.0f;

    //Right Wall
    objects[1]=createRectangle(300.0f,10.0f,blue);
    divideRect(1,300.0f,10.0f);
    trans[1]=glm::vec3(390.0f,0.0f,0.0f);
    rotat[1]=90.0f;
    movable[1]=false;
    //Top wall 
    objects[2]=createRectangle(400.0f,10.0f,blue);
    divideRect(2,400.0f,10.0f);
    trans[2]=glm::vec3(0.0f,290.0f,0.0f);
    rotat[2]=0.0f;
    movable[2]=false;
    //Left Wall
    objects[3]=createRectangle(300.0f,10.0f,blue);
    divideRect(3,300.0f,10.0f);
    trans[3]=glm::vec3(-390.0f,0.0f,0.0f);
    rotat[3]=-90.0f;
    movable[3]=false;

    //Cannon
    //Circle
    objects[4]=createSector(20,18,blueblack);
    trans[4]=glm::vec3(-340.0f,-260.0f,0.0f);
    rotat[4]=0.0f;
    movable[4]=false;

    //Rectangle
    objects[5]=createRectangle(35,35,blueblack);
    trans[5]=glm::vec3(-317.0f,-230.0f,0.0f);
    rotat[5]=0.0f;
    movable[5]=false;
    //Top rectangle
    objects[33]=createRectangle(30,25,lightblue);
    trans[33]=glm::vec3(-317.0f,-220.0f,0.0f);
    rotat[33]=0.0f;
    movable[33]=false;

    //Circle
    objects[6]=createSector(20,18,blueblack);
    trans[6]=glm::vec3(-295.0f,-260.0f,0.0f);
    rotat[6]=0.0f;
    movable[6]=false;

    //Tank Head
    objects[7]=createSector(35,18,blueblack);
    trans[7]=glm::vec3(-317.0f,-190.0f,0.0f);
    rotat[7]=0.0f;
    movable[7]=false;

    //Barrel
    objects[8]=createRectangle(40,10,blueblack);
    trans[8]=glm::vec3(-296.0f,-140.0f,0.0f);
    rotat[8]=0.0f;
    movable[8]=false;

    //Projectile
    objects[9]=createSector(radius,18,blueblack);
    centre[9].pb(mp(mp(0.0f,0.0f),radius));
    Mass[9]=250.0f;
    velx[9]=vely[9]=0.0f;
    trans[9]=glm::vec3(0.0f,0.0f,0.0f);
    rotat[9]=0.0f;
    movable[9]=true;

    //Pillar 1
    objects[10]=createRectangle(50,10,darkbrown);
    divideRect(10,50.0f,10.0f);
    Mass[10]=450.0f;
    velx[10]=vely[10]=0.0f;
    trans[10]=glm::vec3(-50.0f,-230.0f,0.0f);
    rotat[10]=90.0f;
    movable[10]=true;

    //Pillar 2
    objects[11]=createRectangle(50,10,darkbrown);
    divideRect(11,50.0f,10.0f);
    Mass[11]=250.0f;
    velx[11]=vely[11]=0.0f;
    trans[11]=glm::vec3(280.0f,40.0f,0.0f);
    rotat[11]=90.0f;
    movable[11]=false;

    //Pillar 3
    objects[21]=createRectangle(50,10,darkbrown);
    divideRect(21,50.0f,10.0f);
    Mass[21]=250.0f;
    velx[21]=vely[21]=0.0f;
    trans[21]=glm::vec3(150.0f,40.0f,0.0f);
    rotat[21]=90.0f;
    movable[21]=false;

    //Pillar 4
    objects[22]=createRectangle(70,10,darkbrown);
    divideRect(22,70.0f,10.0f);
    Mass[22]=250.0f;
    velx[22]=vely[22]=0.0f;
    trans[22]=glm::vec3(220.0f,0.0f,0.0f);
    rotat[22]=0.0f;
    movable[22]=false;

    //Lower block
    objects[12]=createRectangle(60,30,lightbrown);
    divideRect(12,60.0f,30.0f);
    Mass[12]=450.0f;
    trans[12]=glm::vec3(120.0f,-250.0f,0.0f);
    velx[12]=vely[12]=0.0f;
    rotat[12]=0.0f;
    movable[12]=false;

    //Inner Lower Block
    objects[27]=createRectangle(55,25,darkbrown);
    divideRect(27,60.0f,30.0f);
    Mass[27]=450.0f;
    trans[27]=glm::vec3(120.0f,-250.0f,0.0f);
    velx[27]=vely[27]=0.0f;
    rotat[27]=0.0f;
    movable[27]=false;

    //Upper block
    objects[13]=createRectangle(40,20,lightbrown);
    divideRect(13,40.0f,20.0f);
    Mass[13]=450.0f;
    velx[13]=vely[13]=0.0f;
    trans[13]=glm::vec3(120.0f,-190.0f,0.0f);
    rotat[13]=90.0f;
    movable[13]=true;

    //Health Bar
    objects[14]=createRectangle(10,20,green);
    trans[14]=glm::vec3(-320.0f,240.0f,0.0f);
    rotat[14]=0.0f;
    movable[14]=false;

    //Power bar background
    objects[23]=createRectangle(200,30,blue);
    trans[23]=glm::vec3(-145.0f,240.0f,0.0f);
    rotat[23]=0.0f;
    movable[32]=false;
    //Pigs
    objects[15]=createSector(25.0f,18,green);
    centre[15].pb(mp(mp(0.0f,0.0f),25.0f));
    Mass[15]=250.0f;
    velx[15]=vely[15]=0.0f;
    trans[15]=glm::vec3(0.0f,-260.0f,0.0f);
    rotat[15]=0.0f;
    movable[15]=false;
    //Pig Eye1
    objects[16]=createSector(4.0f,18,blue);
    centre[16].pb(mp(mp(0.0f,0.0f),4.0f));
    Mass[16]=250.0f;
    velx[16]=vely[16]=0.0f;
    trans[16]=glm::vec3(-10.0f,-252.0f,0.0f);
    rotat[16]=0.0f;
    movable[16]=false;
    //Pig Eye2
    objects[17]=createSector(4.0f,18,blue);
    centre[17].pb(mp(mp(0.0f,0.0f),4.0f));
    Mass[17]=250.0f;
    velx[17]=vely[17]=0.0f;
    trans[17]=glm::vec3(10.0f,-252.0f,0.0f);
    rotat[17]=0.0f;
    movable[17]=false;
    //Pig nose
    objects[18]=createSector(8.0f,18,blue);
    centre[18].pb(mp(mp(0.0f,0.0f),8.0f));
    Mass[18]=250.0f;
    velx[18]=vely[18]=0.0f;
    trans[18]=glm::vec3(0.0f,-270.0f,0.0f);
    rotat[18]=0.0f;
    movable[18]=false;
    //Pig ear 1
    objects[19]=createSector(12.0f,18,green);
    centre[19].pb(mp(mp(0.0f,0.0f),12.0f));
    Mass[19]=250.0f;
    velx[19]=vely[19]=0.0f;
    trans[19]=glm::vec3(24.0f,-242.0f,0.0f);
    rotat[19]=0.0f;
    movable[19]=true;
    //Pig ear 2
    objects[20]=createSector(12.0f,18,green);
    centre[20].pb(mp(mp(0.0f,0.0f),12.0f));
    Mass[20]=250.0f;
    velx[20]=vely[20]=0.0f;
    trans[20]=glm::vec3(-24.0f,-242.0f,0.0f);
    rotat[20]=0.0f;
    movable[20]=false;

    //Sun
    objects[24]=createSector(40.0f,18,yellow);
    centre[24].pb(mp(mp(0.0f,0.0f),40.0f));
    Mass[24]=250.0f;
    velx[24]=vely[24]=0.0f;
    trans[24]=glm::vec3(-250.0f,100.0f,0.0f);
    rotat[24]=0.0f;
    movable[24]=false;
    //Inner Sun
    objects[30]=createSector(35.0f,18,lightorange);
    centre[30].pb(mp(mp(0.0f,0.0f),35.0f));
    Mass[30]=250.0f;
    velx[30]=vely[30]=0.0f;
    trans[30]=glm::vec3(-250.0f,100.0f,0.0f);
    rotat[30]=0.0f;
    movable[30]=false;
    //Sun shade
    objects[25]=createSector(60.0f,25,lightyellow);
    centre[25].pb(mp(mp(0.0f,0.0f),60.0f));
    Mass[25]=250.0f;
    velx[25]=vely[25]=0.0f;
    trans[25]=glm::vec3(-250.0f,100.0f,0.0f);
    rotat[25]=0.0f;
    movable[25]=false;
    //Inner tank head
    objects[26]=createSector(30.0f,18,lightblue);
    centre[26].pb(mp(mp(0.0f,0.0f),30.0f));
    Mass[26]=250.0f;
    velx[26]=vely[26]=0.0f;
    trans[26]=glm::vec3(-317.0f,-190.0f,0.0f);
    rotat[26]=0.0f;
    movable[26]=false;

    //Power up 
    objects[28]=createSector(30.0f,18,redgreen);
    centre[28].pb(mp(mp(0.0f,0.0f),0.0f));
    Mass[28]=250.0f;
    velx[28]=vely[28]=0.0f;
    trans[28]=glm::vec3(215.0f,40.0f,0.0f);
    rotat[28]=0.0f;
    movable[28]=false;

    //Power down
    objects[29]=createSector(30.0f,18,bluegreen);
    centre[29].pb(mp(mp(0.0f,0.0f),30.0f));
    Mass[29]=250.0f;
    velx[29]=vely[28]=0.0f;
    trans[29]=glm::vec3(280.0f,-250.0f,0.0f);
    rotat[29]=0.0f;
    movable[29]=false;

    //Text
    strokes[0]=createLine(-Lx,Ly,Lx,Ly);
    strokes[1]=createLine(-Lx,Ly,-Lx,0.0f);
    strokes[2]=createLine(-Lx,0.0f,-Lx,-Ly);
    strokes[3]=createLine(-Lx,-Ly,Lx,-Ly);
    strokes[4]=createLine(Lx,-Ly,Lx,0.0f);
    strokes[5]=createLine(Lx,0.0f,Lx,Ly);
    strokes[6]=createLine(-Lx,0.0f,Lx,0.0f);
    strokes[7]=createLine(0.0f,0.0f,Lx,-Ly);
    
    //Cloud 
    objects[31]=createSector(20.0f,18,lighestblue);
    centre[31].pb(mp(mp(0.0f,0.0f),20.0f));
    Mass[31]=250.0f;
    velx[31]=vely[31]=0.0f;
    trans[31]=glm::vec3(-100.0f,100.0f,0.0f);
    rotat[31]=0.0f;
    movable[31]=false;

    //Functionality
    programID=LoadShaders("Sample_GL.vert","Sample_GL.frag");
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
    reshapeWindow (width, height);
    GLfloat skyblue[]={0.0,1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0,0.0,1.0,1.0};
    glClearColor (0.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth (1.0f);
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
}

int main (int argc, char** argv)
{
    width = 800;
    height = 600;
    initGLUT (argc, argv, width, height);
    initGL(width, height);
    value['0']=63; 
    value['1']=48; 
    value['2']=109; 
    value['3']=121; 
    value['4']=114; 
    value['5']=91; 
    value['6']=95; 
    value['7']=49; 
    value['8']=127; 
    value['9']=123; 
    value['s']=91; 
    value['c']=15; 
    value['o']=63; 
    value['r']=231; 
    value['e']=79;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-mono")) {
            font = GLUT_BITMAP_9_BY_15;
        }
    }
    glutMainLoop ();
    return 0;
}
