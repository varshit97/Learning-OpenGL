#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define PI M_PI
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
} Matrices;

GLuint programID;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
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

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
            0,                  // attribute 0. Vertices
            3,                  // size (x,y,z)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
            1,                  // attribute 1. Color
            3,                  // size (r,g,b)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

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

typedef pair< float, float > dub;
typedef pair< dub ,float > tup;
#define F first
#define S second
#define mp make_pair
#define pb push_back

vector< tup > centre[1000];
glm::vec3 trans[1000];
float rotat[1000];
VAO* objects[1000];
float velx[1000];
float vely[1000];
bool movable[1000];
float Timer[1000];
float Mass[1000];
float COR=0.6f;
float startX[1000];
float startY[1000];
float currentX[1000];
float currentY[1000];
float ADG=1.0f;
float tick=0.3f;

float sqr(float x)
{
    return x*x;
}
float dis(dub p,dub q)
{
    return sqr(p.F-q.F)+sqr(p.S-q.S);
}
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
            {
                //cout << currentX[0] << " " << currentY[0] << "\n";
                //cout << A.F.F << " " << A.F.S << " " << A.S << "\n";
                //cout << B.F.F << " " << B.F.S << " " << B.S << "\n";
                return true;
            }
        }
    }
    return false;
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

/*--------------------------------------------------------- CHANGEABLE FUNCTIONS ---------------------------------------------------------*/


float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;

void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_C:
                rectangle_rot_status = !rectangle_rot_status;
                break;
            case GLFW_KEY_P:
                triangle_rot_status = !triangle_rot_status;
                break;
            case GLFW_KEY_X:
                // do something ..
                break;
            default:
                break;
        }
    }
    else if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                quit(window);
                break;
            default:
                break;
        }
    }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
    switch (key) {
        case 'Q':
        case 'q':
            quit(window);
            break;
        default:
            break;
    }
}

float ux=30,uy=60;
double xmousePos=0,ymousePos=0;
float cur_angle=0;

int mouseState=0,buttonPressed=0;
float wheel1X=-330,wheel1Y=-240;
float cannonX=0,cannonY=0;
float constStartX=cannonX,constStartY=cannonY;
float timer=0;

void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_RELEASE)
            {
                //  triangle_rot_dir *= -1;
                buttonPressed=1;
            }
            if(action==GLFW_PRESS)
            {
                buttonPressed=1;
                mouseState=1;
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE) {
                rectangle_rot_dir *= -1;
            }
            break;
        default:
            break;
    }
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
       is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    GLfloat fov = 90.0f;

    // sets the viewport of openGL renderer
    glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

    // set the projection matrix as perspective
    /* glMatrixMode (GL_PROJECTION);
       glLoadIdentity ();
       gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
    // Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projection = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, 0.1f, 500.0f);
}

//Define shapes
VAO *triangle, *rectangle,*wheel1,*wheel2,*barrel,*firebarrel;
VAO *leftWall,*rightWall,*bottomWall,*topWall,*tankhead;

// Creates the triangle object used in this sample code
VAO* createSector(float R,int parts)
{
    float diff=360.0f/parts;
    float A1=formatAngle(-diff/2);
    float A2=formatAngle(diff/2);
    GLfloat vertex_buffer_data[]={0.0f,0.0f,0.0f,R*cos(D2R(A1)),R*sin(D2R(A1)),0.0f,R*cos(D2R(A2)),R*sin(D2R(A2)),0.0f};
    GLfloat color_buffer_data[]={1,0,0,1,0,0,1,0,0};
    return create3DObject(GL_TRIANGLES,3,vertex_buffer_data,color_buffer_data,GL_FILL);
}

// Creates the rectangle object used in this sample code
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
    // create3DObject creates and returns a handle to a VAO that can be used later
    return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;
int count=0,fall=0;

void drawobject(VAO* obj,glm::vec3 trans,float angle,glm::vec3 rotat)
{
    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 VP = Matrices.projection * Matrices.view;
    glm::mat4 MVP;	// MVP = Projection * View * Model
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translatemat = glm::translate(trans);
    glm::mat4 rotatemat = glm::rotate(D2R(formatAngle(angle)), rotat);
    Matrices.model *= (translatemat * rotatemat);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(obj);
}


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
/* Render the scene with openGL */
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
bool flag=true;
void applyCollisions()
{
    for(int i=0;i<3;i++)
    {
        for(int j=i+1;j<3;j++)
        {
            if( ((velx[i]!=0 || vely[i]!=0) || (velx[j]!=0 || vely[j]!=0) ) && (movable[i] || movable[j]) && checkCollision(i,j) )
            {
                if(!movable[i])
                {
                    if(i==1)
                    {
                        vely[j]=-COR*yvel(vely[j],0.3f,Mass[j],Timer[j],ADG);
                        startX[j]=trans[j][0];
                        startY[j]=trans[j][1];
                        Timer[j]=tick;
                    }
                }
                //else if(!movable[j])
                else if(!movable[j])
                {
                    if(j==1)
                    {
                        vely[i]=-COR*(vely[i]-ADG*Timer[i]);
                        startX[i]=trans[i][0];
                        startY[i]=trans[i][1];
                        cout << startX[i] << " " << startY[i] << "\n";
                        Timer[i]=0.0f;
                        while(abs(yvel(vely[i],0.3f,Mass[i],Timer[i],ADG))>0.01f && checkCollision(i,j))
                        {
                            Timer[i]+=0.05f;
                            trans[i][0]=currentX[i]=startX[i]+xdis(velx[i],0.3f,Mass[i],Timer[i]);
                            trans[i][1]=currentY[i]=startY[i]+ydis(vely[i],0.3f,Mass[i],Timer[i],ADG);
                        }
                    }
                    if(j==2)
                    {
                        velx[i]=-COR*xvel(velx[i],0.3f,Mass[i],Timer[i]);
                        startX[i]=currentX[i];
                        startY[i]=currentY[i];
                        Timer[i]=0.0f;
                        while(checkCollision(i,j))
                        {
                            Timer[i]+=0.05f;
                            trans[i][0]=currentX[i]=startX[i]+xdis(velx[i],0.3f,Mass[i],Timer[i]);
                            trans[i][1]=currentY[i]=startY[i]+ydis(vely[i],0.3f,Mass[i],Timer[i],ADG);
                        }
                        cout << trans[i][0] << " a " <<trans[i][1] << "\n";
                        cout << currentX[i] << " b " << currentY[i] << "\n";
                    }
                }
                else if(movable[i] && movable[j])
                {
                    conserveMomentum(i,j);
                    // Iterate over all objects to move time frame - TODO
                    startX[i]=currentX[i];
                    startY[i]=currentY[i];
                    startX[j]=currentX[j];
                    startY[j]=currentY[j];
                    Timer[i]=tick;
                    Timer[j]=tick;
                    while(checkCollision(i,j))
                    {
                        Timer[i]+=0.05f;
                        Timer[j]+=0.05f;
                        trans[i][0]=currentX[i]=startX[i]+xdis(velx[i],0.3f,Mass[i],Timer[i]);
                        trans[i][1]=currentY[i]=startY[i]+ydis(vely[i],0.3f,Mass[i],Timer[i],ADG);

                        trans[j][0]=currentX[j]=startX[j]+xdis(velx[j],0.3f,Mass[j],Timer[j]);
                        trans[j][1]=currentY[j]=startY[j]+ydis(vely[j],0.3f,Mass[j],Timer[j],ADG);
                    }
                    flag=false;
                }
            }
        }
    }
}

float rotateBarrel=0,prevBAngle;
float prevCannonX,prevCannonY;
float xpos,prevPX,prevPY;

void updatePositions()
{
    if(1-buttonPressed)
    {
        currentX[0]=-280.f+40*cos(D2R(rotateBarrel));
        currentY[0]=-210.0f+40*sin(D2R(rotateBarrel));
        startX[0]=currentX[0];
        startY[0]=currentY[0];
    }
    for(int i=1-buttonPressed;i<3;i++)
    {
        float veloy=yvel(vely[i],0.3f,Mass[i],Timer[i],ADG);
        float velox=xvel(velx[i],0.3f,Mass[i],Timer[i]);
        if(i==0)
        {
        }
        if(velx[i]!=0.0f || vely[i]!=0.0f)
        {
            currentX[i]=startX[i]+(velx[i]*Timer[i]);
            currentY[i]=startY[i]+(vely[i]*Timer[i]-0.5*ADG*sqr(Timer[i]));
            trans[i][0]=currentX[i];
            trans[i][1]=currentY[i];
            Timer[i]+=tick;
        }
    }

}

/* Render the scene with openGL */
/* Edit this function according to your assignment */

void draw ()
{
    if(buttonPressed==0)
    {
        rotateBarrel=atan2((530-ymousePos),(xmousePos-40))*(180/M_PI);
        cur_angle=atan2((530-ymousePos),(xmousePos-40))*(180/M_PI);
        startX[0]=constStartX=cannonX=-280+60*cos(rotateBarrel*(M_PI/180));
        startY[0]=constStartY=cannonY=-210+60*sin(rotateBarrel*(M_PI/180));
        trans[0][0]=cannonX;
        trans[0][1]=cannonY;
        xpos=xmousePos;
        float speed=xpos-constStartX;
        float u=20*(speed/400);
        ux=u*cos(cur_angle*(M_PI/180));
        uy=u*sin(cur_angle*(M_PI/180));
        velx[0]=ux;
        vely[0]=uy;
    }

    if(buttonPressed==1)
    {
        applyCollisions();
    }
    updatePositions();
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    //1st glm::vec3(xpos,ypos), 2nd glm::vec3(rotate about) 
    //projectile

    //Semi circle
    for(int k=0;k<180;k++)
    {
        drawobject(tankhead,glm::vec3(-280,-210,0),k,glm::vec3(0,0,1));
    }
    //Left Wall
    drawobject(leftWall,glm::vec3(-390,50,0),0,glm::vec3(0,0,1));
    //bottom Wall
    drawobject(bottomWall,glm::vec3(0,-290,0),0,glm::vec3(0,0,1));
    //Top wall
    drawobject(topWall,glm::vec3(0,290,0),0,glm::vec3(0,0,1));
    //Right wall
    drawobject(rightWall,glm::vec3(390,50,0),90,glm::vec3(0,0,1));
    //Left Wheel
    for(int i=0;i<360;i++)
    {
        drawobject(wheel1,glm::vec3(wheel1X,wheel1Y,0),i,glm::vec3(0,0,1));
    }
    //Right Wheel
    for(int j=0;j<360;j++)
    {
        drawobject(wheel2,glm::vec3(-235,-240,0),j,glm::vec3(0,0,1));
    }
    //Joining wheels
    drawobject(barrel,glm::vec3(-285,-240,0),0,glm::vec3(0,0,1));
    //Cannon Barrel
    if(rotateBarrel>=25.0052 && rotateBarrel<=100)
    {
        for(int i=0;i<360;i++)
        {
            drawobject(rectangle,glm::vec3(trans[0][0],trans[0][1],0),i,glm::vec3(0,0,1));
        }
        rotateBarrel=atan2((530-ymousePos),(xmousePos-40))*(180/M_PI);
        drawobject(firebarrel,glm::vec3(-280+40*cos(rotateBarrel*(M_PI/180)),-210+40*sin(rotateBarrel*(M_PI/180)),0),rotateBarrel,glm::vec3(0,0,1));
        prevBAngle=rotateBarrel;
        prevCannonX=trans[0][0];
        prevCannonY=trans[0][1];
    }
    else
    {
        for(int i=0;i<360;i++)
        {
            drawobject(rectangle,glm::vec3(prevCannonX,prevCannonY,0),i,glm::vec3(0,0,1));
        }
        rotateBarrel=atan2((530-ymousePos),(xmousePos-40))*(180/M_PI);
        drawobject(firebarrel,glm::vec3(-280+40*cos(prevBAngle*(M_PI/180)),-210+40*sin(prevBAngle*(M_PI/180)),0),prevBAngle,glm::vec3(0,0,1));
    }
    //cout << "cannon coordinates " << cannonX << " " << cannonY << endl;
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
       is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    // For the cannon base
    wheel1=createSector(30,18); // Generate the VAO, VBOs, vertices data & copy into the array buffer
    wheel2=createSector(30,18); // Generate the VAO, VBOs, vertices data & copy into the array buffer
    barrel=createRectangle(50,30);
    firebarrel=createRectangle(60,10);
    tankhead=createSector(30,25);
    //set sizes length,breadth
    rectangle=createSector(10,18);
    objects[0]=rectangle;
    centre[0].pb(mp(mp(0.0f,0.0f),12.0f));
    trans[0]=glm::vec3(0.0f,0.0f,0.0f);
    currentX[0]=0.0f;
    currentY[0]=0.0f;
    rotat[0]=0.0f;
    movable[0]=true;
    Mass[0]=200.0f;
    Timer[0]=0.0f;
    velx[0]=0.0f;
    vely[0]=0.0f;
    leftWall=createRectangle(10,500);
    bottomWall=createRectangle(400,10);
    objects[1]=bottomWall;
    divideRect(1,500.0f,10.0f);
    trans[1]=glm::vec3(0.0f,-290.0f,0.0f);
    rotat[1]=0.0f;
    movable[1]=false;
    topWall=createRectangle(500,10);
    rightWall=createRectangle(350.0f,10.0f);
    objects[2]=rightWall;
    divideRect(2,350.0f,10.0f);
    trans[2]=glm::vec3(640.0f,0.0f,0.0f);
    rotat[2]=90.0f;
    movable[2]=false;
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
    int width = 801;
    int height = 601;

    GLFWwindow* window = initGLFW(width, height);

    initGL (window, width, height);

    float last_update_time = glfwGetTime(), current_time;

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {

        // OpenGL Draw commands
        draw();

        // Swap Frame Buffer in float buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();
        glfwGetCursorPos(window,&xmousePos,&ymousePos);
        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= 0.5) { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..
            last_update_time = current_time;
        }
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
