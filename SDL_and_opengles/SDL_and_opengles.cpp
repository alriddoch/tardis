/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <SDL.h>
// #include <GL/glew.h>
#include <SDL_opengl.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Input handler
void handleKeys(unsigned char key, int x, int y);

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//Shader loading utility programs
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

//Graphics program
GLuint gProgramID = 0;
GLint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

bool init()
{
  //Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  printf("SDL init ok\n");
  //Use OpenGL 3.1 core
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  //Create window
  gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if(gWindow == NULL)
  {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  printf("Create window ok\n");
  //Create context
  gContext = SDL_GL_CreateContext(gWindow);
  if(gContext == NULL)
  {
    printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  printf("Create context ok\n");

  //Use Vsync
  if(SDL_GL_SetSwapInterval(1) < 0)
  {
    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
  }

  //Initialize OpenGL
  if(!initGL())
  {
    printf("Unable to initialize OpenGL!\n");
    return false;
  }

  return true;
}

bool initGL()
{
  //Generate program
  gProgramID = glCreateProgram();

  //Create vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  //Get vertex source
  const GLchar* vertexShaderSource[] =
  {
#if 1
    "#version 100\n"
    "attribute vec2 LVertexPos2D;"
    "void main() {"
    "  gl_Position = vec4(LVertexPos2D.x, LVertexPos2D.y, 0, 1);"
    "}"
#else
    "attribute vec4 Position;"
    "attribute vec4 SourceColor;"
    "varying vec4 DestinationColor;"
    "void main(void) {"
    "    DestinationColor = SourceColor;"
    "    gl_Position = Position;"
    "}"
#endif
  };

  //Set vertex source
  glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

  //Compile vertex source
  glCompileShader(vertexShader);

  //Check vertex shader for errors
  GLint vShaderCompiled = GL_FALSE;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
  if(vShaderCompiled != GL_TRUE)
  {
    printf("Unable to compile vertex shader %d!\n", vertexShader);
    printShaderLog(vertexShader);
    return false;
  }

  //Attach vertex shader to program
  glAttachShader(gProgramID, vertexShader);


  //Create fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  //Get fragment source
  const GLchar* fragmentShaderSource[] =
  {
#if 0
    "out vec4 LFragment;"
    "void main() {"
    " LFragment = vec4(1.0, 1.0, 1.0, 1.0);"
    "}"
#else
    "#version 100\n"
    "void main(void) {"
    "  gl_FragColor = vec4(1.0, 0.0, 1.0, 0.0);"
    "}"
#endif
  };

  //Set fragment source
  glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

  //Compile fragment source
  glCompileShader(fragmentShader);

  //Check fragment shader for errors
  GLint fShaderCompiled = GL_FALSE;
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
  if(fShaderCompiled != GL_TRUE)
  {
    printf("Unable to compile fragment shader %d!\n", fragmentShader);
    printShaderLog(fragmentShader);
    return false;
  }

  //Attach fragment shader to program
  glAttachShader(gProgramID, fragmentShader);


  //Link program
  glLinkProgram(gProgramID);

  //Check for errors
  GLint programSuccess = GL_TRUE;
  glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
  if(programSuccess != GL_TRUE)
  {
    printf("Error linking program %d!\n", gProgramID);
    printProgramLog(gProgramID);
    return false;
  }

  //Get vertex attribute location
  gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
  if(gVertexPos2DLocation == -1)
  {
    printf("LVertexPos2D is not a valid glsl program variable!\n");
    return false;
  }

  //Initialize clear color
  glClearColor(0.f, 0.f, 0.f, 1.f);

  //VBO data
  GLfloat vertexData[] =
  {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
  };

  //IBO data
  GLuint indexData[] = { 0, 1, 2, 3 };

  //Create VBO
  glGenBuffers(1, &gVBO);
  glBindBuffer(GL_ARRAY_BUFFER, gVBO);
  glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

  //Create IBO
  glGenBuffers(1, &gIBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);
  
  return true;
}

void handleKeys(unsigned char key, int x, int y)
{
  //Toggle quad
  if(key == 'q')
  {
    gRenderQuad = !gRenderQuad;
  }
}

void update()
{
  //No per frame update needed
}

void render()
{
  //Clear color buffer
  glClear(GL_COLOR_BUFFER_BIT);
  
  //Render quad
  if(gRenderQuad)
  {
    //Bind program
    glUseProgram(gProgramID);

    //Enable vertex position
    glEnableVertexAttribArray(gVertexPos2DLocation);

    //Set vertex data
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glVertexAttribPointer(gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    //Set index data and render
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

    //Disable vertex position
    glDisableVertexAttribArray(gVertexPos2DLocation);

    //Unbind program
    glUseProgram(GL_ZERO);
  }
}

void close()
{
  //Deallocate program
  glDeleteProgram(gProgramID);

  //Destroy window  
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  //Quit SDL subsystems
  SDL_Quit();
}

void printProgramLog(GLuint program)
{
  //Make sure name is shader
  if(glIsProgram(program))
  {
    //Program log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    
    //Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    
    //Allocate string
    char* infoLog = new char[ maxLength ];
    
    //Get info log
    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if(infoLogLength > 0)
    {
      //Print Log
      printf("%s\n", infoLog);
    }
    
    //Deallocate string
    delete[] infoLog;
  }
  else
  {
    printf("Name %d is not a program\n", program);
  }
}

void printShaderLog(GLuint shader)
{
  //Make sure name is shader
  if(glIsShader(shader))
  {
    //Shader log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    
    //Get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    
    //Allocate string
    char* infoLog = new char[ maxLength ];
    
    //Get info log
    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if(infoLogLength > 0)
    {
      //Print Log
      printf("%s\n", infoLog);
    }

    //Deallocate string
    delete[] infoLog;
  }
  else
  {
    printf("Name %d is not a shader\n", shader);
  }
}

int main(int argc, char* args[])
{
  //Start up SDL and create window
  if(!init())
  {
    printf("Failed to initialize!\n");
  }
  else
  {
    printf("Init all ok\n");
    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;
    
    //Enable text input
    SDL_StartTextInput();

    //While application is running
    while(!quit)
    {
      //Handle events on queue
      while(SDL_PollEvent(&e) != 0)
      {
        //User requests quit
        if(e.type == SDL_QUIT)
        {
          quit = true;
        }
        //Handle keypress with current mouse position
        else if(e.type == SDL_TEXTINPUT)
        {
          int x = 0, y = 0;
          SDL_GetMouseState(&x, &y);
          handleKeys(e.text.text[ 0 ], x, y);
        }
      }

      //Render quad
      render();
      
      //Update screen
      SDL_GL_SwapWindow(gWindow);
    }
    
    //Disable text input
    SDL_StopTextInput();
  }

  //Free resources and close SDL
  close();

  return 0;
}
