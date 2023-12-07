// CS370 Final Project
// Fall 2023

#define STB_IMAGE_IMPLEMENTATION
#include "../common/stb_image.h"	// Sean Barrett's image loader - http://nothings.org/
#include <stdio.h>
#include <vector>
#include "../common/vgl.h"
#include "../common/objloader.h"
#include "../common/utils.h"
#include "../common/vmath.h"
#include "lighting.h"
#include "house.h"
#define DEG2RAD (M_PI/180.0)

using namespace vmath;
using namespace std;

// Vertex array and buffer names
enum VAO_IDs {Cube, TableCube, ArtCube, DoorCube, WindowCube, Monkey,Fan, Mug, Coffee, Bowl, Apples, Elevator, Falcon, Background, NumVAOs};
enum ObjBuffer_IDs {PosBuffer, NormBuffer, TexBuffer, NumObjBuffers};
enum Color_Buffer_IDs {RedCube, GreenCube, BlueCube, BlackCube, NumColorBuffers};
enum LightBuffer_IDs {LightBuffer, NumLightBuffers};
enum MaterialBuffer_IDs {MaterialBuffer, NumMaterialBuffers};
enum MaterialNames {GreyPlastic, Copper, Dough, BlueAcrylic, GreenAcrylic, CoffeeBlack};
enum Textures {Blank, Carpet, Art, Door, Window, Table, BowlTex, AppleTex, Sky, ElevatorTex, ElevatorDoorTex, NumTextures};

// Vertex array and buffer objects
GLuint VAOs[NumVAOs];
GLuint ObjBuffers[NumVAOs][NumObjBuffers];
GLuint ColorBuffers[NumColorBuffers];
GLuint LightBuffers[NumLightBuffers];
GLuint MaterialBuffers[NumMaterialBuffers];
GLuint TextureIDs[NumTextures];

// Number of vertices in each object
GLint numVertices[NumVAOs];

// Number of component coordinates
GLint posCoords = 4;
GLint normCoords = 3;
GLint texCoords = 2;
GLint colCoords = 4;

// Model files
const char * cubeFile = "../models/unitcube.obj";
const char * tableFile = "../models/proper_table_set.obj";
const char * monkeyFile = "../models/monkey.obj";
const char * fanFile = "../models/fan.obj";
const char * mugFile = "../models/mug.obj";
const char * bowlFile = "../models/bowl.obj";
const char * applesFile = "../models/apples.obj";
const char * elevatorFile = "../models/elevator.obj";
const char * falconFile = "../models/falcon.obj";


// Texture files
const char * blankFile = "../textures/blank.png";
const char * carpetFile = "../textures/carpet.jpg";
const char * artFile = "../textures/art.png";
const char * doorFile = "../textures/scaledDoor.jpg";
const char * windowFile = "../textures/window.jpg";
const char * tableTexFile = "../textures/table.png";
const char * bowlTexFile = "../textures/bowl.png";
const char * appleTexFile = "../textures/apple.png";
const char * skyFile = "../textures/sky.png";
const char * elevatorTexFile = "../textures/elevator.png";
const char * elevatorDoorFile = "../textures/elevator_door.png";

// Camera
vec3 eye = {3.0f, 0.0f, 0.0f};
vec3 center = {0.0f, 0.0f, 0.0f};
vec3 up = {0.0f, 1.0f, 0.0f};
GLfloat azimuth = 0.0f;
//GLfloat daz = 2.0f;
GLfloat theta = 0.0f;
GLfloat look_delta = 0.2f;
GLfloat elevation = 90.0f;
GLfloat del = 2.0f;
GLfloat radius = 2.0f;
GLfloat dr = 0.1f;
GLfloat step = 1.0f;

//fan
GLfloat fan_ang = 0.0f;
GLdouble elTime = 0.0;
GLfloat fanSpeed = 60;
bool fanOn = false;

//Elevator
GLfloat elevator_height = WALL_HEIGHT;
GLfloat elevator_speed = 5.0f;
bool elevator_moving = false;
int elevator_dir = -1;
bool onElevator = false;
GLfloat eye_offset = 2.0f;

//Elevator Door
GLfloat door_pos = 0.0f;
GLfloat door_speed = 5.0f;
bool door_moving = true;
bool door_open = false;
bool door_closed = true;
int door_dir = 1.0f;
//Blinds
GLfloat blind_ang = 0.0f;
GLfloat blind_delta = 2.0f;

//Falcon
bool flightMode = false;
GLfloat turnSpeed = 57.0f;
GLfloat flightSpeed = 50.0f;
GLfloat look_mod = 5.0f;

// Shader variables
// Default (color) shader program references
GLuint default_program;
GLuint default_vPos;
GLuint default_vCol;
GLuint default_proj_mat_loc;
GLuint default_cam_mat_loc;
GLuint default_model_mat_loc;
const char *default_vertex_shader = "../default.vert";
const char *default_frag_shader = "../default.frag";

// Lighting shader program reference
GLuint lighting_program;
GLuint lighting_vPos;
GLuint lighting_vNorm;
GLuint lighting_camera_mat_loc;
GLuint lighting_model_mat_loc;
GLuint lighting_proj_mat_loc;
GLuint lighting_norm_mat_loc;
GLuint lighting_lights_block_idx;
GLuint lighting_materials_block_idx;
GLuint lighting_material_loc;
GLuint lighting_num_lights_loc;
GLuint lighting_light_on_loc;
GLuint lighting_eye_loc;
const char *lighting_vertex_shader = "../lighting.vert";
const char *lighting_frag_shader = "../lighting.frag";

// Texture shader program reference
GLuint texture_program;
GLuint texture_vPos;
GLuint texture_vTex;
GLuint texture_proj_mat_loc;
GLuint texture_camera_mat_loc;
GLuint texture_model_mat_loc;
const char *texture_vertex_shader = "../texture.vert";
const char *texture_frag_shader = "../texture.frag";

// Global state
mat4 proj_matrix;
mat4 camera_matrix;
mat4 normal_matrix;
mat4 model_matrix;

vector<LightProperties> Lights;
vector<MaterialProperties> Materials;
GLuint numLights = 0;
GLint lightOn[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// Global screen dimensions
GLint ww,hh;

//Custom variables
float cam_dist = 120.0f;

void display();
void render_scene();
void build_geometry();
void build_solid_color_buffer(GLuint num_vertices, vec4 color, GLuint buffer);
void build_materials( );
void build_lights( );
void build_textures();
void build_background(GLuint obj);
void draw_background();
void build_carpet(GLuint obj);
void build_art(GLuint obj);
void build_door(GLuint obj);
void build_window(GLuint obj);
void draw_blinds_panel(int offset);
void draw_blinds();
void load_model(const char * filename, GLuint obj);
void load_texture(const char * filename, GLuint texID, GLint magFilter, GLint minFilter, GLint sWrap, GLint tWrap, bool mipMap, bool invert);
void draw_color_obj(GLuint obj, GLuint color);
void draw_mat_object(GLuint obj, GLuint material);
void draw_tex_object(GLuint obj, GLuint texture);
void open_door();
void close_door();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, int button, int action, int mods);

int main(int argc, char**argv)
{
	// Create OpenGL window
	GLFWwindow* window = CreateWindow("Think Inside The Box");
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    } else {
        printf("OpenGL window successfully created\n");
    }

    // Store initial window size
    glfwGetFramebufferSize(window, &ww, &hh);

    // Register callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    // Load shaders and associate variables
    ShaderInfo default_shaders[] = { {GL_VERTEX_SHADER, default_vertex_shader},{GL_FRAGMENT_SHADER, default_frag_shader},{GL_NONE, NULL} };
    default_program = LoadShaders(default_shaders);
    default_vPos = glGetAttribLocation(default_program, "vPosition");
    default_vCol = glGetAttribLocation(default_program, "vColor");
    default_proj_mat_loc = glGetUniformLocation(default_program, "proj_matrix");
    default_cam_mat_loc = glGetUniformLocation(default_program, "camera_matrix");
    default_model_mat_loc = glGetUniformLocation(default_program, "model_matrix");

    // Load shaders
    // Load light shader
    ShaderInfo lighting_shaders[] = { {GL_VERTEX_SHADER, lighting_vertex_shader},{GL_FRAGMENT_SHADER, lighting_frag_shader},{GL_NONE, NULL} };
    lighting_program = LoadShaders(lighting_shaders);
    lighting_vPos = glGetAttribLocation(lighting_program, "vPosition");
    lighting_vNorm = glGetAttribLocation(lighting_program, "vNormal");
    lighting_proj_mat_loc = glGetUniformLocation(lighting_program, "proj_matrix");
    lighting_camera_mat_loc = glGetUniformLocation(lighting_program, "camera_matrix");
    lighting_norm_mat_loc = glGetUniformLocation(lighting_program, "normal_matrix");
    lighting_model_mat_loc = glGetUniformLocation(lighting_program, "model_matrix");
    lighting_lights_block_idx = glGetUniformBlockIndex(lighting_program, "LightBuffer");
    lighting_materials_block_idx = glGetUniformBlockIndex(lighting_program, "MaterialBuffer");
    lighting_material_loc = glGetUniformLocation(lighting_program, "Material");
    lighting_num_lights_loc = glGetUniformLocation(lighting_program, "NumLights");
    lighting_light_on_loc = glGetUniformLocation(lighting_program, "LightOn");
    lighting_eye_loc = glGetUniformLocation(lighting_program, "EyePosition");

    // Load texture shaders
    ShaderInfo texture_shaders[] = { {GL_VERTEX_SHADER, texture_vertex_shader},{GL_FRAGMENT_SHADER, texture_frag_shader},{GL_NONE, NULL} };
    texture_program = LoadShaders(texture_shaders);
    texture_vPos = glGetAttribLocation(texture_program, "vPosition");
    texture_vTex = glGetAttribLocation(texture_program, "vTexCoord");
    texture_proj_mat_loc = glGetUniformLocation(texture_program, "proj_matrix");
    texture_camera_mat_loc = glGetUniformLocation(texture_program, "camera_matrix");
    texture_model_mat_loc = glGetUniformLocation(texture_program, "model_matrix");

    // Create geometry buffers
    build_geometry();
    // Create material buffers
    build_materials();
    // Create light buffers
    build_lights();
    // Create textures
    build_textures();

    // Enable depth test
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Set Initial camera position
    GLfloat x, y, z;
    x = 0.0f;
    y = eye_offset;
    z = 0.0f;
    theta = 0.0f;
    eye = vec3(x, y, z);

    elTime = glfwGetTime();

    open_door();

    // Start loop
    while ( !glfwWindowShouldClose( window ) ) {

        GLdouble curTime = glfwGetTime();
        GLdouble dT = (curTime - elTime);
    	// Draw graphics
        display();

        if(onElevator){
            eye[1] = eye_offset + elevator_height;
        }
        // Update other events like input handling
        center = vec3(eye[0] + cos(theta), eye[1] + sin(azimuth), eye[2] + sin(theta));

        glfwPollEvents();

        if(flightMode){
            vec3 dir = vec3(center[0] - eye[0], 0.0f, center[2] - eye[2]);
            eye += dir*(flightSpeed * 100/60.0f) * dT;
        }

        if(fanOn){
            fan_ang -= dT * (fanSpeed * 360/60);
        }

        if(door_moving){
            door_pos += dT * door_dir * door_speed;

            if(door_pos > ELEVATOR_DOOR_Z + ELEVATOR_DOOR_LENGTH){
                door_pos = ELEVATOR_DOOR_Z + ELEVATOR_DOOR_LENGTH;
                door_open = true;
                door_moving = false;
            }
            else if (door_pos < ELEVATOR_DOOR_Z){
                door_pos = ELEVATOR_DOOR_Z;
                door_closed = true;
                door_moving = false;
            }
        }

        if(elevator_moving){
            if(door_closed){
                elevator_height += elevator_dir * dT * elevator_speed;
            }
        }

        if(elevator_height > WALL_HEIGHT){
            elevator_moving = false;
            onElevator = false;
            elevator_height = WALL_HEIGHT;
            elevator_dir *= -1;

            open_door();
        }
        else if(elevator_height < 0.0f){
            elevator_height = 0.0f;
            elevator_moving = false;
            onElevator = false;
            elevator_dir *= -1;

            open_door();
        }
        // Swap buffer onto screen
        glfwSwapBuffers( window );

        elTime = curTime;
    }

    // Close window
    glfwTerminate();
    return 0;

}

void display( )
{
    // Declare projection and camera matrices
    proj_matrix = mat4().identity();
    camera_matrix = mat4().identity();

	// Clear window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_background();

    // Compute anisotropic scaling
    GLfloat xratio = 1.0f;
    GLfloat yratio = 1.0f;
    // If taller than wide adjust y
    if (ww <= hh)
    {
        yratio = (GLfloat)hh / (GLfloat)ww;
    }
        // If wider than tall adjust x
    else if (hh <= ww)
    {
        xratio = (GLfloat)ww / (GLfloat)hh;
    }

    // DEFAULT ORTHOGRAPHIC PROJECTION
//    proj_matrix = ortho(-cam_dist*xratio, cam_dist*xratio, -cam_dist*yratio, cam_dist*yratio, -cam_dist, cam_dist);
    proj_matrix = frustum(-1.0f, 1.0f, -1.0f, 1.0f,1.0f, cam_dist);

    // Set camera matrix
    camera_matrix = lookat(eye, center, up);

    // Render objects
	render_scene();

	// Flush pipeline
	glFlush();
}

void render_scene( ) {
    // Declare transformation matrices
    model_matrix = mat4().identity();
    mat4 scale_matrix = mat4().identity();
    mat4 rot_matrix = mat4().identity();
    mat4 trans_matrix = mat4().identity();


//    normal_matrix = model_matrix.inverse().transpose();
//    draw_color_obj(Monkey, RedCube);
    // Set cube transformation matrix
    //Draw left wall
    trans_matrix = translate(-SIDE_WALL_X, WALL_Y, SIDE_WALL_Z);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(SIDE_WALL_WIDTH, WALL_HEIGHT, SIDE_WALL_LENGTH);
	model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw cube
    draw_color_obj(Cube, RedCube);

    //Draw right wall
    trans_matrix = translate(SIDE_WALL_X, WALL_Y, SIDE_WALL_Z);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(SIDE_WALL_WIDTH, WALL_HEIGHT, SIDE_WALL_LENGTH);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw cube
    draw_color_obj(Cube, RedCube);

    //Draw back wall
    trans_matrix = translate(FRONT_WALL_X, WALL_Y, -FRONT_WALL_Z);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(FRONT_WALL_WIDTH, WALL_HEIGHT, FRONT_WALL_LENGTH);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw cube
    draw_color_obj(Cube, GreenCube);

    //Draw front wall
    trans_matrix = translate(FRONT_WALL_X, WALL_Y, FRONT_WALL_Z);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(FRONT_WALL_WIDTH, WALL_HEIGHT, FRONT_WALL_LENGTH);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw cube
    draw_color_obj(Cube, GreenCube);

    //Draw floor
    trans_matrix = translate(0.0f, 0.0f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(FRONT_WALL_WIDTH, FLOOR_HEIGHT, SIDE_WALL_LENGTH);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw cube
    draw_tex_object(Cube, Carpet);

    //Draw floor
    trans_matrix = translate(0.0f, WALL_HEIGHT, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(FRONT_WALL_WIDTH, FLOOR_HEIGHT, SIDE_WALL_LENGTH);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw cube
    draw_color_obj(Cube, BlueCube);

    //Draw table
    trans_matrix = translate(TABLE_X, TABLE_Y, TABLE_Z);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(TABLE_WIDTH, TABLE_HEIGHT, TABLE_LENGTH);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(TableCube, Table);

//    Draw door
//    trans_matrix = translate(DOOR_X,DOOR_Y,DOOR_Z);
//    scale_matrix = scale(DOOR_WIDTH,DOOR_HEIGHT,DOOR_LENGTH);
//    model_matrix = trans_matrix*scale_matrix;
    model_matrix = mat4().identity();
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(DoorCube, Door);

    //Draw window
//    trans_matrix = translate(WINDOW_X,WINDOW_Y,WINDOW_Z);
//    scale_matrix = scale(WINDOW_WIDTH,WINDOW_HEIGHT,WINDOW_LENGTH);
//    model_matrix = trans_matrix*scale_matrix;
    model_matrix = mat4().identity();
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(WindowCube, Window);

    //Draw blinds
    draw_blinds();

    //Draw mirror
    trans_matrix = translate((MIRROR_X, MIRROR_Y, MIRROR_Z);
    scale_matrix = scale(MIRROR_WIDTH,MIRROR_HEIGHT,MIRROR_LENGTH);
    model_matrix = trans_matrix*scale_matrix;
    draw_color_obj(Cube, GreenCube);

    //Draw painting
//    trans_matrix = translate(PAINTING_X,PAINTING_Y,PAINTING_Z);
//    scale_matrix = scale(PAINTING_WIDTH,PAINTING_HEIGHT,PAINTING_LENGTH);
//    model_matrix = trans_matrix*scale_matrix;
    model_matrix = mat4().identity();
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(ArtCube, Art);

    //Fan
    rot_matrix = rotate(fan_ang, 0.0f,1.0f,0.0f);
    trans_matrix = translate(FAN_X,FAN_Y,FAN_Z);
    model_matrix = trans_matrix*rot_matrix;
    draw_tex_object(Fan, Table);

    //Draw fruit bowl
    rot_matrix = rotate(90.0f,1.0f,0.0f,0.0f);
    scale_matrix = scale(BOWL_WIDTH, BOWL_HEIGHT, BOWL_LENGTH);
    trans_matrix = translate(BOWL_X, BOWL_Y, BOWL_Z);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(Bowl, BowlTex);

    scale_matrix = scale(APPLE_SIZE,APPLE_SIZE,APPLE_SIZE);
    trans_matrix = translate(BOWL_X, BOWL_Y+0.05f, BOWL_Z);
    model_matrix = trans_matrix*scale_matrix;
    draw_tex_object(Apples, AppleTex);

    //Elevator
    rot_matrix = rotate(-90.0f,0.0f,1.0f,0.0f);
    scale_matrix = scale(ELEVATOR_WIDTH,ELEVATOR_HEIGHT,ELEVATOR_LENGTH);
    trans_matrix = translate(ELEVATOR_X, ELEVATOR_Y + elevator_height, ELEVATOR_Z);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    draw_tex_object(Elevator, ElevatorTex);

    //Elevator Door
    scale_matrix = scale(ELEVATOR_DOOR_WIDTH, ELEVATOR_DOOR_HEIGHT, ELEVATOR_DOOR_LENGTH);
    trans_matrix = translate(ELEVATOR_DOOR_X, ELEVATOR_DOOR_Y + elevator_height, ELEVATOR_DOOR_Z + door_pos);
    model_matrix = trans_matrix*scale_matrix;
    draw_tex_object(Cube, ElevatorDoorTex);

    //Draw Millenium Falcon
    scale_matrix = scale(FALCON_SIZE, FALCON_SIZE, FALCON_SIZE);
    rot_matrix = rotate(90.0f - theta*turnSpeed,0.0f, 1.0f, 0.0f);
    trans_matrix = translate(eye[0] + FALCON_OFFSET_X*(float)cos(theta), eye[1] + FALCON_OFFSET_Y, eye[2] + FALCON_OFFSET_Z*(float)sin(theta));
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    draw_tex_object(Falcon, ElevatorDoorTex);

    //Draw cup
    glDepthMask(GL_FALSE);
    scale_matrix = scale(CUP_WIDTH,CUP_HEIGHT,CUP_LENGTH);
    trans_matrix = translate(CUP_X,CUP_Y,CUP_Z);
    model_matrix = trans_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Mug,GreenAcrylic);

    // TODO: Render coffee
    scale_matrix = scale(CUP_WIDTH - 0.1f,CUP_HEIGHT/2 ,CUP_LENGTH - 0.1f);
    trans_matrix = translate(CUP_X,CUP_Y, CUP_Z);
    model_matrix = trans_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Coffee,CoffeeBlack);

    glDepthMask(GL_TRUE);
}

void build_geometry( )
{
    // Generate vertex arrays and buffers
    glGenVertexArrays(NumVAOs, VAOs);

    // Load models
    load_model(cubeFile, Cube);
    load_model(tableFile, TableCube);
    load_model(cubeFile, ArtCube);
    load_model(cubeFile, DoorCube);
    load_model(monkeyFile, Monkey);
    load_model(fanFile, Fan);
    load_model(mugFile, Mug);
    load_model(bowlFile, Bowl);
    load_model(applesFile, Apples);
    load_model(elevatorFile, Elevator);
    load_model(falconFile, Falcon);

    // Generate color buffers
    glGenBuffers(NumColorBuffers, ColorBuffers);

    // Build color buffers
    // Define cube vertex colors (red)
    build_solid_color_buffer(numVertices[Cube], vec4(1.0f, 0.0f, 0.0f, 1.0f), RedCube);
    build_solid_color_buffer(numVertices[Cube], vec4(0.0f, 1.0f, 0.0f, 1.0f), GreenCube);
    build_solid_color_buffer(numVertices[Cube], vec4(0.0f, 0.0f, 1.0f, 1.0f), BlueCube);
    build_solid_color_buffer(numVertices[Cube], vec4(0.0f, 0.0f, 0.0f, 1.0f), BlackCube);

    //Build textured objects
    build_art(ArtCube);
    build_door(DoorCube);
    build_window(WindowCube);

    build_background(Background);
}

void draw_background(){

    glDepthMask(GL_FALSE);
    // TODO: Draw background image
    // Set anisotropic scaling
    GLfloat xratio = 1.0f;
    GLfloat yratio = 1.0f;
    // If taller than wide adjust y
    if (ww <= hh)
    {
        yratio = (GLfloat)hh / (GLfloat)ww;
    }
        // If wider than tall adjust x
    else if (hh <= ww)
    {
        xratio = (GLfloat)ww / (GLfloat)hh;
    }
    // TODO: Set default orthographic projection
    proj_matrix = frustum(-1, 1, -1, 1, -1, cam_dist);
    // TODO: Set default camera matrix
    camera_matrix = mat4().identity();
    // TODO: Set default model matrix
    model_matrix = mat4().identity();
    // TODO: Draw background with depth buffer writes disabled
    draw_tex_object(Background, Sky);

    glDepthMask(GL_TRUE);
}

void build_materials( ) {
    // Add materials to Materials vector
    Materials.clear();
    // Create grey plastic material
    MaterialProperties greyPlastic = {
            vec4(0.1f, 0.1f, 0.1f, 1.0f), //ambient
            vec4(0.6f, 0.6f, 0.6f, 1.0f), //diffuse
            vec4(0.8f, 0.8f, 0.8f, 1.0f), //specular
            10.0f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad
    };
    Materials.push_back(greyPlastic);

    MaterialProperties copper = {
            vec4(0.19f,0.07f,0.02f, 1.0f),
            vec4(0.7f,0.27f,0.08f,1.0f),
            vec4(0.26f,0.14f,0.09f,1.0f),
            12.8f,
            {0.0f,0.0f,0.0f}
    };

    Materials.push_back(copper);

    MaterialProperties blueAcrylic = {
            vec4(0.2f, 0.2f, 0.3f, 0.8f), //ambient
            vec4(0.2f, 0.2f, 0.8f, 0.8f), //diffuse
            vec4(0.7f, 0.7f, 0.8f, 0.8f), //specular
            12.8f, //shininess
            {0.0f, 0.0f, 0.0f}
    };

    Materials.push_back(blueAcrylic);

    MaterialProperties greenAcrylic = {
            vec4(0.2f, 0.3f, 0.2f, 0.6f), //ambient
            vec4(0.2f, 0.8f, 0.2f, 0.6f), //diffuse
            vec4(0.7f, 0.8f, 0.7f, 0.6f), //specular
            12.8f, //shininess
            {0.0f, 0.0f, 0.0f}
    };

    Materials.push_back(greenAcrylic);

    MaterialProperties coffeeBlack = {
            vec4(0.1,0.1,0.1,0.8),
            vec4(0.1,0.1,0.1,0.8),
            vec4(0.1,0.1,0.1,0.8),
            12.0f,
            {0.0f,0.0f}
    };

    Materials.push_back(coffeeBlack);

    glGenBuffers(NumMaterialBuffers, MaterialBuffers);
    glBindBuffer(GL_UNIFORM_BUFFER, MaterialBuffers[MaterialBuffer]);
    glBufferData(GL_UNIFORM_BUFFER, Materials.size()*sizeof(MaterialProperties), Materials.data(), GL_STATIC_DRAW);
}

void build_lights( ) {
    // Add lights to Lights vector
    LightProperties fanLight = {
            POINT, //type
            {0.0f, 0.0f, 0.0f}, //pad
            vec4(1.0f, 1.0f, 1.0f, 1.0f), //ambient
            vec4(1.0f, 1.0f, 1.0f, 1.0f), //diffuse
            vec4(1.0f, 1.0f, 1.0f, 1.0f), //specular
            vec4(0.0f, 2.0f, 0.0f, 1.0f),  //position
            vec4(0.0f, 0.0f, 0.0f, 0.0f), //direction
            0.0f,   //cutoff
            0.0f,  //exponent
            {0.0f, 0.0f}  //pad2
    };

    Lights.push_back(fanLight);
    // Set numLights
    numLights = Lights.size();

    // Turn all lights on
    for (int i = 0; i < numLights; i++) {
        lightOn[i] = 1;
    }

    // Create uniform buffer for lights
    if (glIsBuffer(LightBuffers[LightBuffer])) {
        glDeleteBuffers(NumLightBuffers, LightBuffers);
    }

    // Create uniform buffer for lights
    glGenBuffers(NumLightBuffers, LightBuffers);
    glBindBuffer(GL_UNIFORM_BUFFER, LightBuffers[LightBuffer]);
    glBufferData(GL_UNIFORM_BUFFER, Lights.size()*sizeof(LightProperties), Lights.data(), GL_STATIC_DRAW);
}

void build_textures( ) {

    // Create textures and activate unit 0
    glGenTextures( NumTextures,  TextureIDs);
    glActiveTexture( GL_TEXTURE0 );

    load_texture(blankFile, Blank, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(carpetFile, Carpet, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, false);
    load_texture(artFile, Art, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, false);
    load_texture(doorFile, Door, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, false);
    load_texture(windowFile, Window, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, true);
    load_texture(tableTexFile,Table, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, true);
    load_texture(bowlTexFile,BowlTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, true);
    load_texture(appleTexFile,AppleTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, true);
    load_texture(skyFile,Sky, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, true);
    load_texture(elevatorTexFile,ElevatorTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, true);
    load_texture(elevatorDoorFile,ElevatorDoorTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT,true, true);
}

void build_background(GLuint obj) {
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;

    vertices = {
            vec4(1.0f, 1.0f, 0.0f, 1.0f),
            vec4(-1.0f, 1.0f, 0.0f, 1.0f),
            vec4(-1.0f, -1.0f, 0.0f, 1.0f),
            vec4(-1.0f, -1.0f, 0.0f, 1.0f),
            vec4(1.0f, -1.0f, 0.0f, 1.0f),
            vec4(1.0f, 1.0f, 0.0f, 1.0f),
    };

    normals = {
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.0f, 0.0f),
    };

    // TODO: Define texture coordinates for background
    uvCoords = {
            {0.0f, 0.0f},
            {-1.0f, 0.0f},
            {-1.0f, -1.0f},
            {-1.0f, -1.0f},
            {0.0f, -1.0f},
            {0.0f, 0.0f},
    };

    numVertices[obj] = vertices.size();

    glBindVertexArray(VAOs[obj]);
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
}

void build_carpet(GLuint obj){
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;


    // Define 3D vertices for cube
    vertices = {
            vec4(FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, SIDE_WALL_LENGTH/2, 1.0f),
            vec4(FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, -SIDE_WALL_LENGTH/2, 1.0f),
            vec4(-FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, -SIDE_WALL_LENGTH/2, 1.0f),
            vec4(-FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, SIDE_WALL_LENGTH/2, 1.0f),
    };

    normals = {
            vec3(0.0f,1.0f,0.0f),
            vec3(0.0f,1.0f,0.0f),
            vec3(0.0f,1.0f,0.0f),
            vec3(0.0f,1.0f,0.0f),
    };

    uvCoords = {
            {1.0f,1.0f},
            {1.0f,0.0f},
            {0.0f,0.0f},
            {0.0f,1.0f},
    };


    // Set number of vertices
    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void build_table(GLuint obj){
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;


    // Define 3D vertices for cube
    vertices = {
            vec4(FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, SIDE_WALL_LENGTH/2, 1.0f),
            vec4(FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, -SIDE_WALL_LENGTH/2, 1.0f),
            vec4(-FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, -SIDE_WALL_LENGTH/2, 1.0f),
            vec4(-FRONT_WALL_WIDTH/2, FLOOR_HEIGHT/2, SIDE_WALL_LENGTH/2, 1.0f),
    };

    normals = {
            vec3(0.0f,1.0f,0.0f),
            vec3(0.0f,1.0f,0.0f),
            vec3(0.0f,1.0f,0.0f),
            vec3(0.0f,1.0f,0.0f),
    };

    uvCoords = {
            {1.0f,1.0f},
            {1.0f,0.0f},
            {0.0f,0.0f},
            {0.0f,1.0f},
    };


    // Set number of vertices
    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void build_art(GLuint obj){
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;
    vector<ivec3> indices;

    // Define 3D vertices for cube
    vertices = {
            vec4(PAINTING_X + PAINTING_WIDTH/2, PAINTING_Y + PAINTING_HEIGHT/2, PAINTING_Z - PAINTING_LENGTH/2, 1.0f),
            vec4(PAINTING_X + PAINTING_WIDTH/2, PAINTING_Y - PAINTING_HEIGHT/2, PAINTING_Z - PAINTING_LENGTH/2, 1.0f),
            vec4(PAINTING_X - PAINTING_WIDTH/2, PAINTING_Y - PAINTING_HEIGHT/2, PAINTING_Z - PAINTING_LENGTH/2, 1.0f),
            vec4(PAINTING_X - PAINTING_WIDTH/2, PAINTING_Y + PAINTING_HEIGHT/2, PAINTING_Z - PAINTING_LENGTH/2, 1.0f),
    };

    uvCoords = {
            {0.0f,0.0f},
            {0.0f,1.0f},
            {1.0f,1.0f},
            {1.0f,0.0f},
    };

    // Define face indices (ensure proper orientation)
    indices = {
            {0, 1, 2},     // Top
            {2, 3, 0},
    };
    int numFaces = indices.size();

    // Create object vertices and colors from faces
    vector<vec4> obj_vertices;
    vector<vec3> obj_normals;
    vector<vec2> obj_uvs;
    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 3; j++) {
            obj_vertices.push_back(vertices[indices[i][j]]);
            obj_normals.push_back(vec3(0.0f, 1.0f, 0.0f));
            obj_uvs.push_back(uvCoords[indices[i][j]]);
        }
    }

    // Set numVertices as total number of INDICES
    numVertices[obj] = 3*numFaces;


    // Generate object buffers for obj
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);

    // Bind and load object buffers for obj
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], obj_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], obj_normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], obj_uvs.data(), GL_STATIC_DRAW);
}

void build_door(GLuint obj){
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;
    vector<ivec3> indices;

    // Define 3D vertices for cube
    vertices = {
            vec4(DOOR_X - DOOR_WIDTH/2, DOOR_Y + DOOR_HEIGHT/2, DOOR_Z + DOOR_LENGTH/2, 1.0f),
            vec4(DOOR_X - DOOR_WIDTH/2, DOOR_Y - DOOR_HEIGHT/2, DOOR_Z + DOOR_LENGTH/2, 1.0f),
            vec4(DOOR_X - DOOR_WIDTH/2, DOOR_Y - DOOR_HEIGHT/2, DOOR_Z - DOOR_LENGTH/2, 1.0f),
            vec4(DOOR_X - DOOR_WIDTH/2, DOOR_Y + DOOR_HEIGHT/2, DOOR_Z - DOOR_LENGTH/2, 1.0f),
    };

    uvCoords = {
            {0.8f,1.0f},
            {0.8f,0.0f},
            {0.2f,0.0f},
            {0.2f,1.0f},
    };

    // Define face indices (ensure proper orientation)
    indices = {
            {0, 3, 2},     // Top
            {2, 1, 0},
    };
    int numFaces = indices.size();

    // Create object vertices and colors from faces
    vector<vec4> obj_vertices;
    vector<vec3> obj_normals;
    vector<vec2> obj_uvs;
    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 3; j++) {
            obj_vertices.push_back(vertices[indices[i][j]]);
            obj_normals.push_back(vec3(0.0f, 1.0f, 0.0f));
            obj_uvs.push_back(uvCoords[indices[i][j]]);
        }
    }

    // Set numVertices as total number of INDICES
    numVertices[obj] = 3*numFaces;


    // Generate object buffers for obj
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);

    // Bind and load object buffers for obj
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], obj_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], obj_normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], obj_uvs.data(), GL_STATIC_DRAW);
}

void build_window(GLuint obj){
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;
    vector<ivec3> indices;

    // Define 3D vertices for cube
    vertices = {
            vec4(WINDOW_X + WINDOW_WIDTH/2, WINDOW_Y + WINDOW_HEIGHT/2, WINDOW_Z + WINDOW_LENGTH/2, 1.0f),
            vec4(WINDOW_X + WINDOW_WIDTH/2, WINDOW_Y - WINDOW_HEIGHT/2, WINDOW_Z + WINDOW_LENGTH/2, 1.0f),
            vec4(WINDOW_X - WINDOW_WIDTH/2, WINDOW_Y - WINDOW_HEIGHT/2, WINDOW_Z + WINDOW_LENGTH/2, 1.0f),
            vec4(WINDOW_X - WINDOW_WIDTH/2, WINDOW_Y + WINDOW_HEIGHT/2, WINDOW_Z + WINDOW_LENGTH/2, 1.0f),
    };

    uvCoords = {
            {1.0f,1.0f},
            {1.0f,0.0f},
            {0.0f,0.0f},
            {0.0f,1.0f},
    };

    // Define face indices (ensure proper orientation)
    indices = {
            {0, 3, 2},     // Top
            {2, 1, 0},
    };
    int numFaces = indices.size();

    // Create object vertices and colors from faces
    vector<vec4> obj_vertices;
    vector<vec3> obj_normals;
    vector<vec2> obj_uvs;
    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 3; j++) {
            obj_vertices.push_back(vertices[indices[i][j]]);
            obj_normals.push_back(vec3(0.0f, 1.0f, 0.0f));
            obj_uvs.push_back(uvCoords[indices[i][j]]);
        }
    }

    // Set numVertices as total number of INDICES
    numVertices[obj] = 3*numFaces;


    // Generate object buffers for obj
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);

    // Bind and load object buffers for obj
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], obj_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], obj_normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], obj_uvs.data(), GL_STATIC_DRAW);
}

void draw_blinds_panel(int offset){
    model_matrix = mat4().identity();
    mat4 scale_matrix = mat4().identity();
    mat4 rot_matrix = mat4().identity();
    mat4 trans_matrix = mat4().identity();

    scale_matrix = scale(BLIND_WIDTH,BLIND_HEIGHT,BLIND_LENGTH);
    rot_matrix = rotate(blind_ang, 0.0f,1.0f,0.0f);
    trans_matrix = translate(BLIND_X + (BLIND_OFFSET*offset), BLIND_Y, BLIND_Z);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    draw_color_obj(Cube, BlackCube);
}

void draw_blinds(){
    int num_blinds;
    if((NUM_BLINDS % 2) == 1){
        num_blinds = NUM_BLINDS/2 + 1;
    }
    else{
        num_blinds = NUM_BLINDS/2;
    }
    for(int i = -num_blinds + 1; i < num_blinds; i++ ){
        draw_blinds_panel(i);
    }
}

void open_door(){
    door_closed = false;
    door_moving = true;
    door_dir = 1.0f;
}

void close_door(){
    door_open = false;
    door_moving = true;
    door_dir = -1.0f;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // ESC to quit
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    // Adjust azimuth
    if (key == GLFW_KEY_A) {
        if(flightMode){
            theta -= look_delta/look_mod;
        }
        else{
            theta -= look_delta;
        }
    } else if (key == GLFW_KEY_D) {
        if(flightMode){
            theta += look_delta/look_mod;
        }
        else{
            theta += look_delta;
        }
    }

    // Adjust elevation angle
    if (key == GLFW_KEY_W)
    {
        vec3 eye_prediction;
        vec3 dir = vec3(center[0] - eye[0], 0.0f, center[2] - eye[2]);
        eye_prediction = eye + dir*step;

        if(!flightMode) {
            if (eye_prediction[2] < (SIDE_WALL_LENGTH / 2 - WALL_BUFFER) &&
                eye_prediction[2] > (-SIDE_WALL_LENGTH / 2 + WALL_BUFFER)) {
                if (eye_prediction[0] < (FRONT_WALL_WIDTH / 2 - WALL_BUFFER) &&
                    eye_prediction[0] > (-FRONT_WALL_WIDTH / 2 + WALL_BUFFER)) {
                    if (!onElevator) {
                        eye = eye_prediction;
                    }
                }
            }
        }
    }
    else if (key == GLFW_KEY_S)
    {
        vec3 eye_prediction;
        vec3 dir = -vec3(center[0] - eye[0], 0.0f, center[2] - eye[2]);
        eye_prediction = eye + dir*step;

        if(!flightMode)
        {
            if (eye_prediction[2] < SIDE_WALL_LENGTH / 2 - WALL_BUFFER &&
                eye_prediction[2] > -SIDE_WALL_LENGTH / 2 + WALL_BUFFER) {
                if (eye_prediction[0] < FRONT_WALL_WIDTH / 2 - WALL_BUFFER &&
                    eye_prediction[0] > -FRONT_WALL_WIDTH / 2 + WALL_BUFFER) {
                    if (!onElevator) {
                        eye = eye_prediction;
                    }
                }
            }
        }
    }
    else if(key == GLFW_KEY_Z){
        azimuth -= look_delta;
    }
    else if(key == GLFW_KEY_X){
        azimuth += look_delta;
    }
    else if(key == GLFW_KEY_O){
        blind_ang += blind_delta;
    }
    else if(key == GLFW_KEY_F && action == GLFW_PRESS){
        fanOn = !fanOn;
    }
    else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        elevator_moving = true;
        close_door();
        onElevator = false;
        if(eye[0] > ELEVATOR_X -ELEVATOR_BOUNDS && eye[0] < ELEVATOR_X + ELEVATOR_BOUNDS){
            if(eye[2] > ELEVATOR_Z -ELEVATOR_BOUNDS && eye[2] < ELEVATOR_X + ELEVATOR_BOUNDS){
                if(abs(eye[1] - eye_offset - elevator_height) < 0.2f){
                    onElevator = true;
                }
            }
        }
    }
    else if(key == GLFW_KEY_T && action == GLFW_PRESS){
        flightMode = true;
        eye = vec3(FLIGHT_POS_X, FLIGHT_POS_Y, FLIGHT_POS_Z);
    }
    else if(key == GLFW_KEY_I && action == GLFW_PRESS){
        flightMode = false;
        eye = vec3(HOUSE_X, HOUSE_Y, HOUSE_Z);
    }
}

void mouse_callback(GLFWwindow *window, int button, int action, int mods){

}

#include "utilfuncs.cpp"
