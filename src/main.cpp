#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <cstdlib>

using namespace std;

#ifdef BROCCOLI
#include <broccoli.h>
#endif

#include <ncurses.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

//  Engine parts
#include "Shader.h"
#include "Render.h"
#include "Model.h"
#include "Display.h"
#include "Actor.h"
#include "Camera.h"
#include "Model_vbotorus.h"

enum class queue_events {
    STRAFE_LEFT,
    STRAFE_RIGHT,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    YAW_LEFT,
    YAW_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    PITCH_UP,
    PITCH_DOWN,
    COLOR_CHANGE,
    MODEL_CHANGE,
    RENDER_PRIMITIVE_CHANGE,
    APPLICATION_QUIT
};

queue<queue_events> gqueue;
shared_ptr<Display> display { new Display };
shared_ptr<Renderer> renderer;
shared_ptr<Camera> camera;
shared_ptr<Entity> selected;
vector<shared_ptr<Actor>> scene_graph;


//  Constants and Vars
//
Shader default_vertex_shader(GL_VERTEX_SHADER), default_fragment_shader(GL_FRAGMENT_SHADER);
shared_ptr<ShaderProgram> default_shader;
shared_ptr<ShaderProgram> global_shader;
glm::mat4 VP;
glm::mat4 camera_matrix;
glm::mat3 NormalMatrix;
glm::mat4 ModelViewMatrix;

//  Function and Declarations
void Init( );
void GlutIdle( );
void GlutReshape( int newWidth, int newHeight );
void GlutDisplay( );
void GlutKeyboard( unsigned char key, int x, int y );
void CleanupAndExit( );
void update_stats();
GLint UnmapRenderPrimitive ( int rp );
void GenerateModels( );
void GenerateEntities( );
void GenerateShaders( );

//  Globalized user vars
GLfloat strafe{ 1.0f }, height{ 0.0f }, depth{ -15.0f }, rotate{ 0.0f };
float dir = 1.0f;
float xpos = 2.0f;
float ypos = 0.0f;
GLint global_render_primitive = 0;
GLint max_primitives = 3;
GLint global_model_id = 0;





//  Global state vars
BroConn *bc;
int queue_length = 0;
int queue_length_max = 0;
uint64 event_count = 0;
int color = 0;

//  callback function signature
//  typedef void (*BroEventFunc) (BroConn *bc, void *user_data, ...);
//static void remote_conn_cb(BroConn *bc, void *user_data, BroRecord *conn);
static void remote_conn_cb(BroConn *bc, void *user_data, BroRecord *conn)
{
    printw("new_connection\n");
}
static void http_request_cb(BroConn *bc, void *user_data, BroRecord *conn)
{
    printw("http_request\n");
}

int gi = 0;
GLfloat gf = 10.0f;
static void connection_count_cb( BroConn *bc, void *user_data, uint64 *count)
{
  event_count = *count;
  GLfloat a = 0.0f;
  shared_ptr<Actor> actor = shared_ptr<Actor>{ new Actor(gf, 0.0f, /*a*/0.0f, a, 0.0f, 0.0f, 0 ) };
  actor->setShader(global_shader);
  scene_graph.push_back(actor);
  gf += 3.0f;

  BroEvent * rep = bro_event_new("count_updated");
  bro_event_send(bc, rep);
  //bro_event_free(rep);
}

int main(int argc, char * argv[])
{

    //  Initialize broccoli
    bro_init(NULL);

    //  Turn on debuging
    bro_debug_calltrace = 0;
    bro_debug_messages = 0;

    //  Where our server lives
    string host_str = "10.0.1.4:47758";

    // Setup the connection
    if (!(bc = bro_conn_new_str(host_str.c_str(), BRO_CFLAG_CACHE|BRO_CFLAG_RECONNECT | BRO_CFLAG_ALWAYS_QUEUE))) {
        cerr << "Requesting new bro connection failed" << endl;
        exit(EXIT_FAILURE);
    }

    //  Setup our callback for connection events
    //    bro_event_registry_add(bc, "http_request", (BroEventFunc)http_request_cb, NULL);
    //    bro_event_registry_add(bc, "new_connection", (BroEventFunc)remote_conn_cb, NULL);
    bro_event_registry_add(bc, "send_count", (BroEventFunc)connection_count_cb, NULL);

    //  Check the connection is valid
    if (!bro_conn_connect(bc)) {
        cerr << "Error connecting to remote bro instance: \"" << host_str << "\"" << endl;
        exit(EXIT_FAILURE);
    }

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
    glutInitWindowSize( display->getWidth(), display->getHeight() );

    glutCreateWindow( argv[0] );
    if ( glewInit( ) ) {
      cerr << "Unable to initialize GLEW ... exiting " << endl;
      exit( EXIT_FAILURE );
    }

    //  Initialize common systems

    //  Camera
    camera = shared_ptr<Camera>{ new Camera( strafe, height, depth, 0.0f, 0.0f,
                                             0.0f ) };
    renderer = display->getRenderer();
    display->setCamera (camera);

    //  Load our Application Items
    GenerateModels( );
    GenerateShaders( );

    //  This scene specific items
    GenerateEntities( );

    //  Boiler Plate
    glutIdleFunc( GlutIdle );
    glutReshapeFunc( GlutReshape );
    glutDisplayFunc( GlutDisplay );
    glutKeyboardFunc( GlutKeyboard );

    //  Go forth and loop
    glutMainLoop( );

    // //  ncurses section
    // initscr();
    // raw ();
    // nodelay(stdscr, TRUE);
    // bool quit = false;
    // do {
    //   mvprintw(1,4,"Hello broception!\n");
    //   if (bro_conn_process_input(bc))
    //     printw("input proccessed\n");
    //   else
    //     printw("no input\n");
    //   if (bro_conn_alive(bc))
    //     printw("connection alive\n");
    //   else printw("connection dead\n");
    //   update_stats();
    //   printw("bro queue length is: %d\n", queue_length);
    //   printw("bro queue length size is: %d\n ", queue_length_max);
    //   printw("event count: %d\n", event_count);
    //   refresh();
    //   char keycommand = getch ();
    //   if (keycommand == 'q')
    //     quit = true;
    //   else if (keycommand == 'c') {
    //       BroEvent *ev;
    //       if ( !( ev = bro_event_new("count_update")))
    //       {
    //           cerr << "Unable to create new event" << endl;
    //       }
    //       bro_event_send(bc, ev);
    //       bro_event_free(ev);
    //   }

    // } while (!sleep(1) && !quit);
    // endwin();
    // bro_conn_delete(bc);
    // return 0;
}


void update_stats()
{
    queue_length = bro_event_queue_length(bc);
    queue_length_max = bro_event_queue_length_max(bc);
}

void GenerateShaders( ) {
    //  Shaders
    try {
        default_vertex_shader.SourceFile("shaders/default.vert");
        default_fragment_shader.SourceFile ("shaders/default.frag");
        default_vertex_shader.Compile();
        default_fragment_shader.Compile();
        default_shader = shared_ptr<ShaderProgram> { new ShaderProgram };
        default_shader->addShader(default_vertex_shader);
        default_shader->addShader(default_fragment_shader);
        default_shader->link();
        default_shader->unuse();
        default_shader->printActiveUniforms();
    }
    catch (ShaderProgramException excp) {
        cerr << excp.what() << endl;
        exit (EXIT_FAILURE);
    }

    global_shader = default_shader;
}

void GlutReshape( int newWidth, int newHeight )
{
    display->Reshape(newWidth, newHeight);
    glViewport(0,0, display->getWidth(), display->getHeight());
}


void GlutDisplay( )
{
    glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 r_matrix =
            glm::rotate( glm::mat4 (), camera->getOrientation()[0], glm::vec3( 1.0f, 0.0f, 0.0f ) );
    r_matrix =
            glm::rotate( r_matrix, camera->getOrientation()[1], glm::vec3( 0.0f, 1.0f, 0.0f ) );
    r_matrix =
            glm::rotate( r_matrix, camera->getOrientation()[2], glm::vec3( 0.0f, 0.0f, 1.0f ) );
    glm::vec4 cr = r_matrix * glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f );
    camera_matrix = glm::lookAt(
                camera->getPosition(),
                camera->getPosition() + glm::vec3( cr.x, cr.y, cr.z ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
    glm::mat4 model = glm::mat4(1.0f);
    VP = display->getPerspective() * camera_matrix;

    global_shader->use();
    global_shader->setUniform("VP", VP);
    global_shader->setUniform("M", model);

    global_shader->setUniform("color", color);

    display->Render( scene_graph );
    global_shader->unuse();

    glFinish( );
    glutSwapBuffers( );
}

void GlutKeyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    default:
      break;
    case 27:
    case 'q':
    case 'Q':
        gqueue.push( queue_events::APPLICATION_QUIT );
        break;
    case 'h':
    case 'H':
        gqueue.push( queue_events::YAW_LEFT );
      break;
    case 'l':
    case 'L':
        gqueue.push( queue_events::YAW_RIGHT );
      break;
    case 'a':
    case 'A':
        gqueue.push( queue_events::STRAFE_LEFT );
      break;
    case 'd':
    case 'D':
        gqueue.push( queue_events::STRAFE_RIGHT );
      break;
    case 's':
    case 'S':
        gqueue.push( queue_events::MOVE_BACKWARD );
      break;
    case 'w':
    case 'W':
        gqueue.push( queue_events::MOVE_FORWARD );
      break;
    case 'k':
    case 'K':
        gqueue.push( queue_events::MOVE_UP );
      break;
    case '-':
        gqueue.push( queue_events::PITCH_UP );
      break;
    case '+':
        gqueue.push( queue_events::PITCH_DOWN );
      break;
    case 'j':
    case 'J':
        gqueue.push( queue_events::MOVE_DOWN );
      break;
    case 'c':
    case 'C':
        gqueue.push( queue_events::COLOR_CHANGE );
      break;
    case 'm':
    case 'M':
        gqueue.push( queue_events::MODEL_CHANGE );
      break;
    case 'r':
    case 'R':
        gqueue.push( queue_events::RENDER_PRIMITIVE_CHANGE );
      break;
    }
}

const glm::vec3 back_movement(0.0f, 0.0f, 1.0f);
const glm::vec3 forward_movement(0.0f, 0.0f, -1.0f);
const glm::vec3 left_movement( -0.3f, 0.0f, 0.0f);
const glm::vec3 right_movement( 0.3f, 0.0f, 0.0f);
const glm::vec3 up_movement(0.0f, 1.0f, 0.0f);
const glm::vec3 down_movement(0.0f, -1.0f, 0.0f);

void GlutIdle( )
{
    //  Pump the events loop
    while ( !gqueue.empty( ) ) {
        switch ( gqueue.front( ) ) {
        case queue_events::MOVE_FORWARD:
            selected->move (forward_movement);
            break;
        case queue_events::MOVE_BACKWARD:
            selected->move (back_movement);
            break;
        case queue_events::STRAFE_RIGHT:
            selected->move (right_movement);
            break;
        case queue_events::STRAFE_LEFT:
            selected->move(left_movement);
            break;
        case queue_events::YAW_RIGHT:
            selected->orient(up_movement);
            break;
        case queue_events::YAW_LEFT:
            selected->orient(down_movement);
            break;
        case queue_events::MOVE_UP:
            selected->move(up_movement);
            break;
        case queue_events::MOVE_DOWN:
            selected->move(down_movement);
            break;
        case queue_events::PITCH_UP:
            selected->orient(right_movement);
            break;
        case queue_events::PITCH_DOWN:
            selected->orient(left_movement);
            break;
        case queue_events::COLOR_CHANGE:
            color = ( color >= 4 ? 1 : color + 1 );
            break;
        case queue_events::RENDER_PRIMITIVE_CHANGE:
            global_render_primitive += 1;
            global_render_primitive %= max_primitives;
            for ( int i = 0; i < renderer->models.size(); i++)
              renderer->models[i]->renderPrimitive = UnmapRenderPrimitive(global_render_primitive);
          break;
        case queue_events::MODEL_CHANGE:
            global_model_id += 1;
            global_model_id %= renderer->models.size();
            for (int i = 0; i < scene_graph.size(); i++)
              scene_graph[i]->model_id = global_model_id;
          break;
        case queue_events::APPLICATION_QUIT:
            CleanupAndExit( );
        }
        gqueue.pop( );
    }

    glutPostRedisplay( );
    bro_conn_process_input(bc);
}

void CleanupAndExit( )
{
    exit( EXIT_SUCCESS );
}

void GenerateModels( ) {
    int ext = 0;
    shared_ptr<Simple_equation_model_t> tmp;
    shared_ptr<VBOTorus> tmpt;

    //  Generate Torus
    tmpt = shared_ptr<VBOTorus> { new VBOTorus (0.7f, 0.3f, 50, 50) };
    tmpt->renderPrimitive = UnmapRenderPrimitive(global_render_primitive);
    tmpt->name = "vbo_torus";
    renderer->add_model( tmpt );

    //  Generate Some equation model
    for ( auto power_to :
    { 1.0f, 1.2f, 1.4f, 1.6f, 1.8f, 2.1f, 2.2f, 2.3f, 3.5f, 4.0f } ) {
        tmp =
                shared_ptr<Simple_equation_model_t>{ new Simple_equation_model_t };
        float x = 0.0f;
        float z = 0.0f;
        tmp->numVertices = 600;
        tmp->vertices.resize( tmp->numVertices * 3 );
        for ( int i = 0; i < tmp->numVertices; i++, x += 0.1f, z += 0.05f ) {
            tmp->vertices[i * 3] = x;
            tmp->vertices[i * 3 + 1] = powf( x, power_to );
            tmp->vertices[i * 3 + 2] = 0.0f; // z;
            if ( z >= -1.0f )
                z = 0.0f;
        }
        tmp->name = "ex15_" + to_string( ext++ );
        tmp->renderPrimitive = UnmapRenderPrimitive(global_render_primitive);
        tmp->setup_render_model( );
        renderer->add_model( tmp );
    }

}

void GenerateEntities( ) {
   //  Actors
    GLfloat a = 0.0f;
    for ( int i = 0; i < 1; i++, a += 10.0f ) {
        shared_ptr<Actor> actor = shared_ptr<Actor>{ new Actor(a, 0.0f, /*a*/0.0f, a, 0.0f, 0.0f, 0 ) };
        actor->setShader(global_shader);
        scene_graph.push_back(actor);
        gi = i+1;
    }
    //  Selected Entity
    selected = camera;
}

int UnmapRenderPrimitive (int rp)
{
    switch (rp) {
    case 0:
        return GL_POINTS;
    case 1:
        return GL_LINES;
    case 2:
        return GL_TRIANGLES;
    }
    return GL_POINTS;
}
