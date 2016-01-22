/****************************************************** DO NOT CHANGE THIS SECTION OF CODE **************************************************/
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
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

int width,height;
VAO* objects[MAX];
float Mass[MAX];
float velx[MAX];
float vely[MAX];
float Timer[MAX];
float startX[MAX];
float startY[MAX];
bool movable[MAX];
float COR=0.6f;
float ADG=1.0f;
float tick=0.6f;

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

/*********************************** Collision Detection **************************************************/


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

/***************************************************** KEYBOARD AND MOUSE FUNCTIONS  ********************************************************/

void keyboardDown(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'Q':
        case 'q':
        case 27: //ESC
            exit (0);
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

int buttonPressed=0;

void keyboardSpecialDown(int key, int x, int y)
{
}
void keyboardSpecialUp(int key, int x, int y)
{
}
void mouseClick(int button, int state, int x, int y)
{
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        buttonPressed=1;
        velx[9]=20*(cos(D2R(60)));
        vely[9]=20*(sin(D2R(60)));
        Timer[9]=0.0f;
    }
    cerr << x << y << "\n";
}
void mouseMotion(int x, int y)
{
    cerr << x << y << "\n";
}

float xmousepos,ymousepos;

void cursorPos(int x, int y)
{
    xmousepos=x;
    ymousepos=y;
    trans[9][0]=10*cos(D2R(60));
    trans[9][1]=10*sin(D2R(60));
}

void reshapeWindow(int width, int height)
{
    GLfloat fov=90.0f;
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    Matrices.projection=glm::ortho(-width/2.0f,width/2.0f,-height/2.0f,height/2.0f,0.1f, 500.0f);
}

VAO* createRectangle(float x,float y)
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
    return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
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

void trt(VAO* obj,double toX,double toY,double rot_angle,double width,double height)
{
    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 VP = Matrices.projection * Matrices.view;
    glm::mat4 MVP;  // MVP = Projection * View * Model
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translatepivot= glm::translate(glm::vec3(0,height/2,0));
    glm::mat4 translatemat = glm::translate(glm::vec3(toX,toY-(height/2),0));
    glm::mat4 rotatemat = glm::rotate(D2R(formatAngle(rot_angle)), glm::vec3(0,0,1));
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

double rotateBarrel=60;

void moveProjectile()
{
    if(buttonPressed==0)
    {
        startX[9]=trans[9][0]=-314+15*cos(D2R(60));
        startY[9]=trans[9][1]=-180+15*sin(D2R(60));
    }
    for(int i=1;i<10;i++)
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
            trans[i][1]=startY[i]+ydis(vely[i],0.3f,Mass[i],Timer[i],ADG);
            Timer[i]+=tick;
        }
    }
}

void draw ()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram (programID);
    //Drawing objects

    moveProjectile();

    //Walls
    //Floor
    drawobject(objects[0],trans[0],0,glm::vec3(0,0,1));   
    //Right Wall
    drawobject(objects[1],trans[1],0,glm::vec3(0,0,1));   
    //Top Wall
    drawobject(objects[2],trans[2],0,glm::vec3(0,0,1));   
    //Left Wall
    drawobject(objects[3],trans[3],0,glm::vec3(0,0,1));   

    //Cannon
    //Circle
    for(int i=1;i<360;i++)
    {
        drawobject(objects[4],trans[4],i,glm::vec3(0,0,1));   
    }
    //Rectangle
    drawobject(objects[5],trans[5],0,glm::vec3(0,0,1));   
    //Circle
    for(int i=1;i<360;i++)
    {
        drawobject(objects[6],trans[6],i,glm::vec3(0,0,1));   
    }
    //Tank Head
    for(int i=1;i<360;i++)
    {
        drawobject(objects[7],trans[7],i,glm::vec3(0,0,1));   
    }
    //Barrel
    drawobject(objects[8],trans[8],rotateBarrel,glm::vec3(0,0,1));
    //Projectile
    for(int i=1;i<360;i++)
    {
        drawobject(objects[9],trans[9],i,glm::vec3(0,0,1));   
    }
    //Pillar 1
    drawobject(objects[10],trans[10],0,glm::vec3(0,0,1));
    //Pillar 2
    drawobject(objects[11],trans[11],0,glm::vec3(0,0,1));
    //Lower block
    drawobject(objects[12],trans[12],0,glm::vec3(0,0,1));
    //Upper block
    drawobject(objects[13],trans[13],0,glm::vec3(0,0,1));
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

VAO* createSector(float R,int parts)
{
    float diff=360.0f/parts;
    float A1=formatAngle(-diff/2);
    float A2=formatAngle(diff/2);
    GLfloat vertex_buffer_data[]={0.0f,0.0f,0.0f,R*cos(D2R(A1)),R*sin(D2R(A1)),0.0f,R*cos(D2R(A2)),R*sin(D2R(A2)),0.0f};
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
    //Floor
    objects[0]=createRectangle(400.0f,10.0f);
    divideRect(0,400.0f,10.0f);
    trans[0]=glm::vec3(0.0f,-290.0f,0.0f);
    rotat[0]=0.0f;
    movable[0]=false;

    //Right Wall
    objects[1]=createRectangle(10.0f,300.0f);
    divideRect(1,10.0f,300.0f);
    trans[1]=glm::vec3(390.0f,0.0f,0.0f);
    rotat[1]=0.0f;
    movable[1]=false;
    //Top wall 
    objects[2]=createRectangle(400.0f,10.0f);
    divideRect(2,400.0f,10.0f);
    trans[2]=glm::vec3(0.0f,290.0f,0.0f);
    rotat[2]=0.0f;
    movable[2]=false;
    //Left Wall
    objects[3]=createRectangle(10.0f,300.0f);
    divideRect(3,10.0f,300.0f);
    trans[3]=glm::vec3(-390.0f,0.0f,0.0f);
    rotat[3]=0.0f;
    movable[3]=false;

    //Cannon
    //Circle
    objects[4]=createSector(20,18);
    trans[4]=glm::vec3(-360.0f,-260.0f,0.0f);
    rotat[4]=0.0f;
    movable[4]=false;

    //Rectangle
    objects[5]=createRectangle(35,30);
    trans[5]=glm::vec3(-317.0f,-212.0f,0.0f);
    rotat[5]=0.0f;
    movable[5]=false;

    //Circle
    objects[6]=createSector(20,18);
    trans[6]=glm::vec3(-273.0f,-260.0f,0.0f);
    rotat[6]=0.0f;
    movable[6]=false;

    //Tank Head
    objects[7]=createSector(20,18);
    trans[7]=glm::vec3(-314.0f,-180.0f,0.0f);
    rotat[7]=0.0f;
    movable[7]=false;

    //Barrel
    objects[8]=createRectangle(40,10);
    trans[8]=glm::vec3(-296.0f,-140.0f,0.0f);
    rotat[8]=0.0f;
    movable[8]=false;

    //Projectile
    objects[9]=createSector(10.0f,18);
    Mass[9]=250.0f;
    velx[9]=vely[9]=0.0f;
    trans[9]=glm::vec3(0.0f,0.0f,0.0f);
    rotat[9]=0.0f;
    movable[9]=true;

    //Pillar 1
    objects[10]=createRectangle(10,50);
    Mass[10]=250.0f;
    velx[10]=vely[10]=0.0f;
    trans[10]=glm::vec3(-50.0f,-230.0f,0.0f);
    rotat[10]=0.0f;
    movable[10]=false;

    //Pillar 2
    objects[11]=createRectangle(10,50);
    Mass[11]=250.0f;
    velx[11]=vely[11]=0.0f;
    trans[11]=glm::vec3(280.0f,-230.0f,0.0f);
    rotat[11]=0.0f;
    movable[11]=false;

    //Lower block
    objects[12]=createRectangle(40,30);
    trans[12]=glm::vec3(120.0f,-250.0f,0.0f);
    rotat[12]=0.0f;
    movable[12]=false;

    //Upper block
    objects[13]=createRectangle(20,20);
    trans[13]=glm::vec3(120.0f,-200.0f,0.0f);
    rotat[13]=0.0f;
    movable[13]=false;

    //Functionality
    programID=LoadShaders("Sample_GL.vert","Sample_GL.frag");
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
    reshapeWindow (width, height);
    glClearColor (0.3f, 0.3f, 0.3f, 0.0f);
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
    glutMainLoop ();
    return 0;
}
