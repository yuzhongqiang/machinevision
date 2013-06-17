/*
* wnd_gtk.c
*
*/

#include "precomp.hpp"

#ifndef WIN32

#ifdef HAVE_GTK

#include "gtk/gtk.h"
#include "gdk/gdkkeysyms.h"
#include <stdio.h>

#ifdef HAVE_OPENGL
    #include <gtk/gtkgl.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

// TODO Fix the initial window size when flags=0.  Right now the initial window is by default
// 320x240 size.  A better default would be actual size of the image.  Problem
// is determining desired window size with trackbars while still allowing resizing.
//
// Gnome Totem source may be of use here, see bacon_video_widget_set_scale_ratio
// in totem/src/backend/bacon-video-widget-xine.c

////////////////////////////////////////////////////////////
// MvImageWidget GTK Widget Public API
////////////////////////////////////////////////////////////
typedef struct _MvImageWidget        MvImageWidget;
typedef struct _MvImageWidgetClass   MvImageWidgetClass;

struct _MvImageWidget {
    GtkWidget widget;
    MvMat * original_image;
    MvMat * scaled_image;
    int flags;
};

struct _MvImageWidgetClass
{
  GtkWidgetClass parent_class;
};


/** Allocate new image viewer widget */
GtkWidget*     mvImageWidgetNew      (int flags);

/** Set the image to display in the widget */
void           mvImageWidgetSetImage(MvImageWidget * widget, const MvArr *arr);

// standard GTK object macros
#define MV_IMAGE_WIDGET(obj)          GTK_CHECK_CAST (obj, mvImageWidget_get_type (), MvImageWidget)
#define MV_IMAGE_WIDGET_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, mvImageWidget_get_type (), MvImageWidgetClass)
#define MV_IS_IMAGE_WIDGET(obj)       GTK_CHECK_TYPE (obj, mvImageWidget_get_type ())

/////////////////////////////////////////////////////////////////////////////
// Private API ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
GtkType        mvImageWidget_get_type (void);

static GtkWidgetClass * parent_class = NULL;

// flag to help size initial window
#define MV_WINDOW_NO_IMAGE 2

void mvImageWidgetSetImage(MvImageWidget * widget, const MvArr *arr){
    MvMat * mat, stub;
    int origin=0;

    //printf("mvImageWidgetSetImage\n");

    if( MV_IS_IMAGE_HDR( arr ))
        origin = ((IplImage*)arr)->origin;

    mat = mvGetMat(arr, &stub);

    if(widget->original_image && !MV_ARE_SIZES_EQ(mat, widget->original_image)){
        mvReleaseMat( &widget->original_image );
    }
    if(!widget->original_image){
        widget->original_image = mvCreateMat( mat->rows, mat->cols, MV_8UC3 );
        gtk_widget_queue_resize( GTK_WIDGET( widget ) );
    }
    mvConvertImage( mat, widget->original_image,
                            (origin != 0 ? MV_MVTIMG_FLIP : 0) + MV_MVTIMG_SWAP_RB );
    if(widget->scaled_image){
        mvResize( widget->original_image, widget->scaled_image, MV_INTER_AREA );
    }

    // window does not refresh without this
    gtk_widget_queue_draw( GTK_WIDGET(widget) );
}

GtkWidget*
mvImageWidgetNew (int flags)
{
  MvImageWidget *image_widget;

  image_widget = MV_IMAGE_WIDGET( gtk_type_new (mvImageWidget_get_type ()) );
  image_widget->original_image = 0;
  image_widget->scaled_image = 0;
  image_widget->flags = flags | MV_WINDOW_NO_IMAGE;

  return GTK_WIDGET (image_widget);
}

static void
mvImageWidget_realize (GtkWidget *widget)
{
  GdkWindowAttr attributes;
  gint attributes_mask;

  //printf("mvImageWidget_realize\n");
  g_return_if_fail (widget != NULL);
  g_return_if_fail (MV_IS_IMAGE_WIDGET (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget) |
    GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK |
    GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);

  widget->style = gtk_style_attach (widget->style, widget->window);

  gdk_window_set_user_data (widget->window, widget);

  gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static MvSize mvImageWidget_calc_size( int im_width, int im_height, int max_width, int max_height ){
    float aspect = (float)im_width/(float)im_height;
    float max_aspect = (float)max_width/(float)max_height;
    if(aspect > max_aspect){
        return mvSize( max_width, mvRound(max_width/aspect) );
    }
    return mvSize( mvRound(max_height*aspect), max_height );
}

static void
mvImageWidget_size_request (GtkWidget      *widget,
                       GtkRequisition *requisition)
{
    MvImageWidget * image_widget = MV_IMAGE_WIDGET( widget );

    //printf("mvImageWidget_size_request ");
    // the case the first time mvShowImage called or when AUTOSIZE
    if( image_widget->original_image &&
        ((image_widget->flags & MV_WINDOW_AUTOSIZE) ||
         (image_widget->flags & MV_WINDOW_NO_IMAGE)))
    {
        //printf("original ");
        requisition->width = image_widget->original_image->cols;
        requisition->height = image_widget->original_image->rows;
    }
    // default case
    else if(image_widget->scaled_image){
        //printf("scaled ");
        requisition->width = image_widget->scaled_image->cols;
        requisition->height = image_widget->scaled_image->rows;
    }
    // the case before mvShowImage called
    else{
        //printf("default ");
        requisition->width = 320;
        requisition->height = 240;
    }
    //printf("%d %d\n",requisition->width, requisition->height);
}

static void mvImageWidget_set_size(GtkWidget * widget, int max_width, int max_height){
    MvImageWidget * image_widget = MV_IMAGE_WIDGET( widget );

    //printf("mvImageWidget_set_size %d %d\n", max_width, max_height);

    // don't allow to set the size
    if(image_widget->flags & MV_WINDOW_AUTOSIZE) return;
    if(!image_widget->original_image) return;

    MvSize scaled_image_size = mvImageWidget_calc_size( image_widget->original_image->cols,
            image_widget->original_image->rows, max_width, max_height );

    if( image_widget->scaled_image &&
            ( image_widget->scaled_image->cols != scaled_image_size.width ||
              image_widget->scaled_image->rows != scaled_image_size.height ))
    {
        mvReleaseMat( &image_widget->scaled_image );
    }
    if( !image_widget->scaled_image ){
        image_widget->scaled_image = mvCreateMat( scaled_image_size.height, scaled_image_size.width,        MV_8UC3 );


    }
    assert( image_widget->scaled_image );
}

static void
mvImageWidget_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation)
{
  MvImageWidget *image_widget;

  //printf("mvImageWidget_size_allocate\n");
  g_return_if_fail (widget != NULL);
  g_return_if_fail (MV_IS_IMAGE_WIDGET (widget));
  g_return_if_fail (allocation != NULL);

  widget->allocation = *allocation;
  image_widget = MV_IMAGE_WIDGET (widget);


  if( (image_widget->flags & MV_WINDOW_AUTOSIZE)==0 && image_widget->original_image ){
      // (re) allocated scaled image
      if( image_widget->flags & MV_WINDOW_NO_IMAGE ){
          mvImageWidget_set_size( widget, image_widget->original_image->cols,
                                          image_widget->original_image->rows);
      }
      else{
          mvImageWidget_set_size( widget, allocation->width, allocation->height );
      }
      mvResize( image_widget->original_image, image_widget->scaled_image, MV_INTER_AREA );
  }

  if (GTK_WIDGET_REALIZED (widget))
    {
      image_widget = MV_IMAGE_WIDGET (widget);

      if( image_widget->original_image &&
              ((image_widget->flags & MV_WINDOW_AUTOSIZE) ||
               (image_widget->flags & MV_WINDOW_NO_IMAGE)) )
      {
          widget->allocation.width = image_widget->original_image->cols;
          widget->allocation.height = image_widget->original_image->rows;
          gdk_window_move_resize( widget->window, allocation->x, allocation->y,
                  image_widget->original_image->cols, image_widget->original_image->rows );
          if(image_widget->flags & MV_WINDOW_NO_IMAGE){
              image_widget->flags &= ~MV_WINDOW_NO_IMAGE;
              gtk_widget_queue_resize( GTK_WIDGET(widget) );
          }
      }
      else{
          gdk_window_move_resize (widget->window,
                  allocation->x, allocation->y,
                  allocation->width, allocation->height );

      }
    }
}

static void
mvImageWidget_destroy (GtkObject *object)
{
  MvImageWidget *image_widget;

  g_return_if_fail (object != NULL);
  g_return_if_fail (MV_IS_IMAGE_WIDGET (object));

  image_widget = MV_IMAGE_WIDGET (object);

  mvReleaseMat( &image_widget->scaled_image );
  mvReleaseMat( &image_widget->original_image );

  if (GTK_OBJECT_CLASS (parent_class)->destroy)
    (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void mvImageWidget_class_init (MvImageWidgetClass * klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;

  parent_class = GTK_WIDGET_CLASS( gtk_type_class (gtk_widget_get_type ()) );

  object_class->destroy = mvImageWidget_destroy;

  widget_class->realize = mvImageWidget_realize;
  widget_class->size_request = mvImageWidget_size_request;
  widget_class->size_allocate = mvImageWidget_size_allocate;
  widget_class->button_press_event = NULL;
  widget_class->button_release_event = NULL;
  widget_class->motion_notify_event = NULL;
}

static void
mvImageWidget_init (MvImageWidget *image_widget)
{
    image_widget->original_image=0;
    image_widget->scaled_image=0;
    image_widget->flags=0;
}

GtkType mvImageWidget_get_type (void){
  static GtkType image_type = 0;

  if (!image_type)
    {
      static const GtkTypeInfo image_info =
      {
        (gchar*)"MvImageWidget",
        sizeof (MvImageWidget),
        sizeof (MvImageWidgetClass),
        (GtkClassInitFunc) mvImageWidget_class_init,
        (GtkObjectInitFunc) mvImageWidget_init,
        /* reserved_1 */ NULL,
        /* reserved_1 */ NULL,
        (GtkClassInitFunc) NULL
      };

      image_type = gtk_type_unique (GTK_TYPE_WIDGET, &image_info);
    }

  return image_type;
}
/////////////////////////////////////////////////////////////////////////////
// End MvImageWidget
/////////////////////////////////////////////////////////////////////////////


struct MvWindow;

typedef struct MvTrackbar
{
    int signature;
    GtkWidget* widget;
    char* name;
    MvTrackbar* next;
    MvWindow* parent;
    int* data;
    int pos;
    int maxval;
    MvTrackbarCallback notify;
    MvTrackbarCallback2 notify2;
    void* userdata;
}
MvTrackbar;


typedef struct MvWindow
{
    int signature;
    GtkWidget* widget;
    GtkWidget* frame;
    GtkWidget* paned;
    char* name;
    MvWindow* prev;
    MvWindow* next;

    int last_key;
    int flags;
    int status;//0 normal, 1 fullscreen (YV)

    MvMouseCallback on_mouse;
    void* on_mouse_param;

    struct
    {
        int pos;
        int rows;
        MvTrackbar* first;
    }
    toolbar;

#ifdef HAVE_OPENGL
    bool useGl;

    MvOpenGlDrawCallback glDrawCallback;
    void* glDrawData;
#endif
}
MvWindow;


static gboolean imvOnClose( GtkWidget* widget, GdkEvent* event, gpointer user_data );
static gboolean imvOnKeyPress( GtkWidget* widget, GdkEventKey* event, gpointer user_data );
static void imvOnTrackbar( GtkWidget* widget, gpointer user_data );
static gboolean imvOnMouse( GtkWidget *widget, GdkEvent *event, gpointer user_data );

#ifdef HAVE_GTHREAD
int thread_started=0;
static gpointer imvWindowThreadLoop();
GMutex*				   last_key_mutex;
GCond*				   cond_have_key;
GMutex*				   window_mutex;
GThread*			   window_thread;
GtkWidget*             mvTopLevelWidget = 0;
#endif

static int             last_key = -1;
static MvWindow* hg_windows = 0;

MV_IMPL int mvInitSystem( int argc, char** argv )
{
    static int wasInitialized = 0;

    // check initialization status
    if( !wasInitialized )
    {
        hg_windows = 0;

        gtk_disable_setlocale();
        gtk_init( &argc, &argv );

        #ifdef HAVE_OPENGL
            gtk_gl_init(&argc, &argv);
        #endif

        wasInitialized = 1;
    }

    return 0;
}

MV_IMPL int mvStartWindowThread(){
#ifdef HAVE_GTHREAD
    mvInitSystem(0,NULL);
    if (!thread_started) {
    if (!g_thread_supported ()) {
        /* the GThread system wasn't inited, so init it */
        g_thread_init(NULL);
    }

    // this mutex protects the window resources
    window_mutex = g_mutex_new();

    // protects the 'last key pressed' variable
    last_key_mutex = g_mutex_new();

    // conditional that indicates a key has been pressed
    cond_have_key = g_cond_new();

    // this is the window update thread
    window_thread = g_thread_create((GThreadFunc) imvWindowThreadLoop,
                    NULL, TRUE, NULL);
    }
    thread_started = window_thread!=NULL;
    return thread_started;
#else
    return 0;
#endif
}

#ifdef HAVE_GTHREAD
gpointer imvWindowThreadLoop(){
    while(1){
        g_mutex_lock(window_mutex);
        gtk_main_iteration_do(FALSE);
        g_mutex_unlock(window_mutex);

        // little sleep
        g_usleep(500);

        g_thread_yield();
    }
    return NULL;
}

#define MV_LOCK_MUTEX() \
if(thread_started && g_thread_self()!=window_thread){ g_mutex_lock( window_mutex ); } else { }

#define MV_UNLOCK_MUTEX() \
if(thread_started && g_thread_self()!=window_thread){ g_mutex_unlock( window_mutex); } else { }

#else
#define MV_LOCK_MUTEX()
#define MV_UNLOCK_MUTEX()
#endif

static MvWindow* imvFindWindowByName( const char* name )
{
    MvWindow* window = hg_windows;
    while( window != 0 && strcmp(name, window->name) != 0 )
        window = window->next;

    return window;
}

static MvWindow* imvWindowByWidget( GtkWidget* widget )
{
    MvWindow* window = hg_windows;

    while( window != 0 && window->widget != widget &&
           window->frame != widget && window->paned != widget )
        window = window->next;

    return window;
}

double mvGetModeWindow_GTK(const char* name)//YV
{
    double result = -1;

    MV_FUNCNAME( "mvGetModeWindow_GTK" );

    __BEGIN__;

    MvWindow* window;

    if (!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if (!window)
        MV_ERROR( MV_StsNullPtr, "NULL window" );

    MV_LOCK_MUTEX();
    result = window->status;
    MV_UNLOCK_MUTEX();

    __END__;
    return result;
}


void mvSetModeWindow_GTK( const char* name, double prop_value)//Yannick Verdie
{

    MV_FUNCNAME( "mvSetModeWindow_GTK" );

    __BEGIN__;

    MvWindow* window;

    if(!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if( !window )
        MV_ERROR( MV_StsNullPtr, "NULL window" );

    if(window->flags & MV_WINDOW_AUTOSIZE)//if the flag MV_WINDOW_AUTOSIZE is set
        EXIT;

    //so easy to do fullscreen here, Linux rocks !

    if (window->status==MV_WINDOW_FULLSCREEN && prop_value==MV_WINDOW_NORMAL)
    {
        MV_LOCK_MUTEX();
        gtk_window_unfullscreen(GTK_WINDOW(window->frame));
        window->status=MV_WINDOW_NORMAL;
        MV_UNLOCK_MUTEX();
        EXIT;
    }

    if (window->status==MV_WINDOW_NORMAL && prop_value==MV_WINDOW_FULLSCREEN)
    {
        MV_LOCK_MUTEX();
        gtk_window_fullscreen(GTK_WINDOW(window->frame));
        window->status=MV_WINDOW_FULLSCREEN;
        MV_UNLOCK_MUTEX();
        EXIT;
    }

    __END__;
}


double mvGetPropWindowAutoSize_GTK(const char* name)
{
    double result = -1;

    MV_FUNCNAME( "mvGetPropWindowAutoSize_GTK" );

    __BEGIN__;

    MvWindow* window;

    if (!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if (!window)
        EXIT; // keep silence here

    result = window->flags & MV_WINDOW_AUTOSIZE;

    __END__;

    return result;
}

double mvGetRatioWindow_GTK(const char* name)
{
    double result = -1;

    MV_FUNCNAME( "mvGetRatioWindow_GTK" );

    __BEGIN__;

    MvWindow* window;

    if (!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if (!window)
        EXIT; // keep silence here

    result = static_cast<double>(window->widget->allocation.width) / window->widget->allocation.height;

    __END__;

    return result;
}

double mvGetOpenGlProp_GTK(const char* name)
{
    double result = -1;

#ifdef HAVE_OPENGL
    MV_FUNCNAME( "mvGetOpenGlProp_GTK" );

    __BEGIN__;

    MvWindow* window;

    if (!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if (!window)
        EXIT; // keep silence here

    result = window->useGl;

    __END__;
#else
    (void)name;
#endif

    return result;
}


// OpenGL support

#ifdef HAVE_OPENGL

namespace
{
    void createGlContext(MvWindow* window)
    {
        GdkGLConfig* glconfig;

        MV_FUNCNAME( "createGlContext" );

        __BEGIN__;

        // Try double-buffered visual
        glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE));
        if (!glconfig)
            MV_ERROR( MV_OpenGlApiCallError, "Can't Create A GL Device Context" );

        // Set OpenGL-capability to the widget
        if (!gtk_widget_set_gl_capability(window->widget, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE))
            MV_ERROR( MV_OpenGlApiCallError, "Can't Create A GL Device Context" );

        window->useGl = true;

        __END__;
    }

    void drawGl(MvWindow* window)
    {
        MV_FUNCNAME( "drawGl" );

        __BEGIN__;

        GdkGLContext* glcontext = gtk_widget_get_gl_context(window->widget);
        GdkGLDrawable* gldrawable = gtk_widget_get_gl_drawable(window->widget);

        if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
            MV_ERROR( MV_OpenGlApiCallError, "Can't Activate The GL Rendering Context" );

        glViewport(0, 0, window->widget->allocation.width, window->widget->allocation.height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (window->glDrawCallback)
            window->glDrawCallback(window->glDrawData);

        if (gdk_gl_drawable_is_double_buffered (gldrawable))
            gdk_gl_drawable_swap_buffers(gldrawable);
        else
            glFlush();

        gdk_gl_drawable_gl_end(gldrawable);

        __END__;
    }
}

#endif // HAVE_OPENGL


static gboolean mvImageWidget_expose(GtkWidget* widget, GdkEventExpose* event, gpointer data)
{
#ifdef HAVE_OPENGL
    MvWindow* window = (MvWindow*)data;

    if (window->useGl)
    {
        drawGl(window);
        return TRUE;
    }
#else
    (void)data;
#endif

  MvImageWidget *image_widget;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (MV_IS_IMAGE_WIDGET (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (event->count > 0)
    return FALSE;

  image_widget = MV_IMAGE_WIDGET (widget);

  gdk_window_clear_area (widget->window,
                         0, 0,
                         widget->allocation.width,
                         widget->allocation.height);
  if( image_widget->scaled_image ){
      // center image in available region
      int x0 = (widget->allocation.width - image_widget->scaled_image->cols)/2;
      int y0 = (widget->allocation.height - image_widget->scaled_image->rows)/2;

      gdk_draw_rgb_image( widget->window, widget->style->fg_gc[GTK_STATE_NORMAL],
          x0, y0, MIN(image_widget->scaled_image->cols, widget->allocation.width),
          MIN(image_widget->scaled_image->rows, widget->allocation.height),
          GDK_RGB_DITHER_MAX, image_widget->scaled_image->data.ptr, image_widget->scaled_image->step );
  }
  else if( image_widget->original_image ){
      gdk_draw_rgb_image( widget->window, widget->style->fg_gc[GTK_STATE_NORMAL],
          0, 0,
          MIN(image_widget->original_image->cols, widget->allocation.width),
           MIN(image_widget->original_image->rows, widget->allocation.height),
          GDK_RGB_DITHER_MAX, image_widget->original_image->data.ptr, image_widget->original_image->step );
  }
  return TRUE;
}

MV_IMPL int mvNamedWindow( const char* name, int flags )
{
    int result = 0;
    MV_FUNCNAME( "mvNamedWindow" );

    __BEGIN__;

    MvWindow* window;
    int len;

    mvInitSystem(1,(char**)&name);
    if( !name )
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    // Check the name in the storage
    if( imvFindWindowByName( name ) != 0 )
    {
        result = 1;
        EXIT;
    }

    len = strlen(name);
    MV_CALL( window = (MvWindow*)mvAlloc(sizeof(MvWindow) + len + 1));
    memset( window, 0, sizeof(*window));
    window->name = (char*)(window + 1);
    memcpy( window->name, name, len + 1 );
    window->flags = flags;
    window->signature = MV_WINDOW_MAGIC_VAL;
    window->last_key = 0;
    window->on_mouse = 0;
    window->on_mouse_param = 0;
    memset( &window->toolbar, 0, sizeof(window->toolbar));
    window->next = hg_windows;
    window->prev = 0;
    window->status = MV_WINDOW_NORMAL;//YV

    MV_LOCK_MUTEX();

    window->frame = gtk_window_new( GTK_WINDOW_TOPLEVEL );

    window->paned = gtk_vbox_new( FALSE, 0 );
    window->widget = mvImageWidgetNew( flags );
    gtk_box_pack_end( GTK_BOX(window->paned), window->widget, TRUE, TRUE, 0 );
    gtk_widget_show( window->widget );
    gtk_container_add( GTK_CONTAINER(window->frame), window->paned );
    gtk_widget_show( window->paned );

#ifndef HAVE_OPENGL
    if (flags & MV_WINDOW_OPENGL)
        MV_ERROR( MV_OpenGlNotSupported, "Library was built without OpenGL support" );
#else
    if (flags & MV_WINDOW_OPENGL)
        createGlContext(window);

    window->glDrawCallback = 0;
    window->glDrawData = 0;
#endif

    //
    // configure event handlers
    // TODO -- move this to MvImageWidget ?
    gtk_signal_connect( GTK_OBJECT(window->frame), "key-press-event",
                        GTK_SIGNAL_FUNC(imvOnKeyPress), window );
    gtk_signal_connect( GTK_OBJECT(window->widget), "button-press-event",
                        GTK_SIGNAL_FUNC(imvOnMouse), window );
    gtk_signal_connect( GTK_OBJECT(window->widget), "button-release-event",
                        GTK_SIGNAL_FUNC(imvOnMouse), window );
    gtk_signal_connect( GTK_OBJECT(window->widget), "motion-notify-event",
                        GTK_SIGNAL_FUNC(imvOnMouse), window );
    gtk_signal_connect( GTK_OBJECT(window->frame), "delete-event",
                        GTK_SIGNAL_FUNC(imvOnClose), window );
    gtk_signal_connect( GTK_OBJECT(window->widget), "expose-event",
                        GTK_SIGNAL_FUNC(mvImageWidget_expose), window );

    gtk_widget_add_events (window->widget, GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK) ;

    gtk_widget_show( window->frame );
    gtk_window_set_title( GTK_WINDOW(window->frame), name );

    if( hg_windows )
        hg_windows->prev = window;
    hg_windows = window;

    gtk_window_set_resizable( GTK_WINDOW(window->frame), (flags & MV_WINDOW_AUTOSIZE) == 0 );


    // allow window to be resized
    if( (flags & MV_WINDOW_AUTOSIZE)==0 ){
        GdkGeometry geometry;
        geometry.min_width = 50;
        geometry.min_height = 50;
        gtk_window_set_geometry_hints( GTK_WINDOW( window->frame ), GTK_WIDGET( window->widget ),
            &geometry, (GdkWindowHints) (GDK_HINT_MIN_SIZE));
    }

    MV_UNLOCK_MUTEX();

#ifdef HAVE_OPENGL
    if (window->useGl)
        mvSetOpenGlContext(name);
#endif

    result = 1;
    __END__;

    return result;
}


#ifdef HAVE_OPENGL

MV_IMPL void mvSetOpenGlContext(const char* name)
{
    MvWindow* window;
    GdkGLContext* glcontext;
    GdkGLDrawable* gldrawable;

    MV_FUNCNAME( "mvSetOpenGlContext" );

    __BEGIN__;

    if(!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if (!window)
        MV_ERROR( MV_StsNullPtr, "NULL window" );

    if (!window->useGl)
        MV_ERROR( MV_OpenGlNotSupported, "Window doesn't support OpenGL" );

    glcontext = gtk_widget_get_gl_context(window->widget);
    gldrawable = gtk_widget_get_gl_drawable(window->widget);

    if (!gdk_gl_drawable_make_current(gldrawable, glcontext))
        MV_ERROR( MV_OpenGlApiCallError, "Can't Activate The GL Rendering Context" );

    __END__;
}

MV_IMPL void mvUpdateWindow(const char* name)
{
    MV_FUNCNAME( "mvUpdateWindow" );

    __BEGIN__;

    MvWindow* window;

    if (!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if (!window)
        EXIT;

    // window does not refresh without this
    gtk_widget_queue_draw( GTK_WIDGET(window->widget) );

    __END__;
}

MV_IMPL void mvSetOpenGlDrawCallback(const char* name, MvOpenGlDrawCallback callback, void* userdata)
{
    MvWindow* window;

    MV_FUNCNAME( "mvCreateOpenGLCallback" );

    __BEGIN__;

    if(!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if( !window )
        EXIT;

    if (!window->useGl)
        MV_ERROR( MV_OpenGlNotSupported, "Window was created without OpenGL context" );

    window->glDrawCallback = callback;
    window->glDrawData = userdata;

    __END__;
}

#endif // HAVE_OPENGL




static void imvDeleteWindow( MvWindow* window )
{
    MvTrackbar* trackbar;

    if( window->prev )
        window->prev->next = window->next;
    else
        hg_windows = window->next;

    if( window->next )
        window->next->prev = window->prev;

    window->prev = window->next = 0;

    gtk_widget_destroy( window->frame );

    for( trackbar = window->toolbar.first; trackbar != 0; )
    {
        MvTrackbar* next = trackbar->next;
        mvFree( &trackbar );
        trackbar = next;
    }

    mvFree( &window );
#ifdef HAVE_GTHREAD
    // if last window, send key press signal
    // to jump out of any waiting mvWaitKey's
    if(hg_windows==0 && thread_started){
        g_cond_broadcast(cond_have_key);
    }
#endif
}


MV_IMPL void mvDestroyWindow( const char* name )
{
    MV_FUNCNAME( "mvDestroyWindow" );

    __BEGIN__;

    MvWindow* window;

    if(!name)
        MV_ERROR( MV_StsNullPtr, "NULL name string" );

    window = imvFindWindowByName( name );
    if( !window )
        EXIT;

    // note that it is possible for the update thread to run this function
    // if there is a call to mvShowImage in a mouse callback
    // (this would produce a deadlock on window_mutex)
    MV_LOCK_MUTEX();

    imvDeleteWindow( window );

    MV_UNLOCK_MUTEX();

    __END__;
}


MV_IMPL void
mvDestroyAllWindows( void )
{
    MV_LOCK_MUTEX();

    while( hg_windows )
    {
        MvWindow* window = hg_windows;
        imvDeleteWindow( window );
    }
    MV_UNLOCK_MUTEX();
}

// MvSize imvCalcOptimalWindowSize( MvWindow * window, MvSize new_image_size){
//     MvSize window_size;
//     GtkWidget * toplevel = gtk_widget_get_toplevel( window->frame );
//     gdk_drawable_get_size( GDK_DRAWABLE(toplevel->window),
//             &window_size.width, &window_size.height );

//     window_size.width = window_size.width + new_image_size.width - window->widget->allocation.width;
//     window_size.height = window_size.height + new_image_size.height - window->widget->allocation.height;

//     return window_size;
// }

MV_IMPL void
mvShowImage( const char* name, const MvArr* arr )
{
    MV_FUNCNAME( "mvShowImage" );

    __BEGIN__;

    MvWindow* window;

    if( !name )
        MV_ERROR( MV_StsNullPtr, "NULL name" );

    MV_LOCK_MUTEX();

    window = imvFindWindowByName(name);
    if(!window)
    {
        mvNamedWindow(name, 1);
        window = imvFindWindowByName(name);
    }

    if( window && arr )
    {
    #ifdef HAVE_OPENGL
        if (window->useGl)
        {
            MvMat stub;
            MvMat* mat = mvGetMat(arr, &stub);
            mv::Mat im(mat);
            mv::imshow(name, im);
            return;
        }
    #endif

        MvImageWidget * image_widget = MV_IMAGE_WIDGET( window->widget );
        mvImageWidgetSetImage( image_widget, arr );
    }

    MV_UNLOCK_MUTEX();

    __END__;
}

MV_IMPL void mvResizeWindow(const char* name, int width, int height )
{
    MV_FUNCNAME( "mvResizeWindow" );

    __BEGIN__;

    MvWindow* window;
    MvImageWidget * image_widget;

    if( !name )
        MV_ERROR( MV_StsNullPtr, "NULL name" );

    window = imvFindWindowByName(name);
    if(!window)
        EXIT;

    image_widget = MV_IMAGE_WIDGET( window->widget );
    //if(image_widget->flags & MV_WINDOW_AUTOSIZE)
        //EXIT;

    MV_LOCK_MUTEX();

    gtk_window_set_resizable( GTK_WINDOW(window->frame), 1 );
    gtk_window_resize( GTK_WINDOW(window->frame), width, height );

    // disable initial resize since presumably user wants to keep
    // this window size
    image_widget->flags &= ~MV_WINDOW_NO_IMAGE;

    MV_UNLOCK_MUTEX();

    __END__;
}


MV_IMPL void mvMoveWindow( const char* name, int x, int y )
{
    MV_FUNCNAME( "mvMoveWindow" );

    __BEGIN__;

    MvWindow* window;

    if( !name )
        MV_ERROR( MV_StsNullPtr, "NULL name" );

    window = imvFindWindowByName(name);
    if(!window)
        EXIT;

    MV_LOCK_MUTEX();

    gtk_window_move( GTK_WINDOW(window->frame), x, y );

    MV_UNLOCK_MUTEX();

    __END__;
}


static MvTrackbar*
imvFindTrackbarByName( const MvWindow* window, const char* name )
{
    MvTrackbar* trackbar = window->toolbar.first;

    for( ; trackbar != 0 && strcmp( trackbar->name, name ) != 0; trackbar = trackbar->next )
        ;

    return trackbar;
}

static int
imvCreateTrackbar( const char* trackbar_name, const char* window_name,
                   int* val, int count, MvTrackbarCallback on_notify,
                   MvTrackbarCallback2 on_notify2, void* userdata )
{
    int result = 0;

    MV_FUNCNAME( "imvCreateTrackbar" );

    __BEGIN__;

    /*char slider_name[32];*/
    MvWindow* window = 0;
    MvTrackbar* trackbar = 0;

    if( !window_name || !trackbar_name )
        MV_ERROR( MV_StsNullPtr, "NULL window or trackbar name" );

    if( count <= 0 )
        MV_ERROR( MV_StsOutOfRange, "Bad trackbar maximal value" );

    window = imvFindWindowByName(window_name);
    if( !window )
        EXIT;

    trackbar = imvFindTrackbarByName(window,trackbar_name);

    MV_LOCK_MUTEX();

    if( !trackbar )
    {
        int len = strlen(trackbar_name);
        trackbar = (MvTrackbar*)mvAlloc(sizeof(MvTrackbar) + len + 1);
        memset( trackbar, 0, sizeof(*trackbar));
        trackbar->signature = MV_TRACKBAR_MAGIC_VAL;
        trackbar->name = (char*)(trackbar+1);
        memcpy( trackbar->name, trackbar_name, len + 1 );
        trackbar->parent = window;
        trackbar->next = window->toolbar.first;
        window->toolbar.first = trackbar;

        GtkWidget* hscale_box = gtk_hbox_new( FALSE, 10 );
        GtkWidget* hscale_label = gtk_label_new( trackbar_name );
        GtkWidget* hscale = gtk_hscale_new_with_range( 0, count, 1 );
        gtk_range_set_update_policy( GTK_RANGE(hscale), GTK_UPDATE_CONTINUOUS );
        gtk_scale_set_digits( GTK_SCALE(hscale), 0 );
        //gtk_scale_set_value_pos( hscale, GTK_POS_TOP );
        gtk_scale_set_draw_value( GTK_SCALE(hscale), TRUE );

        trackbar->widget = hscale;
        gtk_box_pack_start( GTK_BOX(hscale_box), hscale_label, FALSE, FALSE, 5 );
        gtk_widget_show( hscale_label );
        gtk_box_pack_start( GTK_BOX(hscale_box), hscale, TRUE, TRUE, 5 );
        gtk_widget_show( hscale );
        gtk_box_pack_start( GTK_BOX(window->paned), hscale_box, FALSE, FALSE, 5 );
        gtk_widget_show( hscale_box );

    }

    if( val )
    {
        int value = *val;
        if( value < 0 )
            value = 0;
        if( value > count )
            value = count;
        gtk_range_set_value( GTK_RANGE(trackbar->widget), value );
        trackbar->pos = value;
        trackbar->data = val;
    }

    trackbar->maxval = count;
    trackbar->notify = on_notify;
    trackbar->notify2 = on_notify2;
    trackbar->userdata = userdata;
    gtk_signal_connect( GTK_OBJECT(trackbar->widget), "value-changed",
                        GTK_SIGNAL_FUNC(imvOnTrackbar), trackbar );

    // queue a widget resize to trigger a window resize to
    // compensate for the addition of trackbars
    gtk_widget_queue_resize( GTK_WIDGET(window->widget) );


    MV_UNLOCK_MUTEX();

    result = 1;

    __END__;

    return result;
}


MV_IMPL int
mvCreateTrackbar( const char* trackbar_name, const char* window_name,
                  int* val, int count, MvTrackbarCallback on_notify )
{
    return imvCreateTrackbar(trackbar_name, window_name, val, count,
                             on_notify, 0, 0);
}


MV_IMPL int
mvCreateTrackbar2( const char* trackbar_name, const char* window_name,
                   int* val, int count, MvTrackbarCallback2 on_notify2,
                   void* userdata )
{
    return imvCreateTrackbar(trackbar_name, window_name, val, count,
                             0, on_notify2, userdata);
}


MV_IMPL void
mvSetMouseCallback( const char* window_name, MvMouseCallback on_mouse, void* param )
{
    MV_FUNCNAME( "mvSetMouseCallback" );

    __BEGIN__;

    MvWindow* window = 0;

    if( !window_name )
        MV_ERROR( MV_StsNullPtr, "NULL window name" );

    window = imvFindWindowByName(window_name);
    if( !window )
        EXIT;

    window->on_mouse = on_mouse;
    window->on_mouse_param = param;

    __END__;
}


MV_IMPL int mvGetTrackbarPos( const char* trackbar_name, const char* window_name )
{
    int pos = -1;

    MV_FUNCNAME( "mvGetTrackbarPos" );

    __BEGIN__;

    MvWindow* window;
    MvTrackbar* trackbar = 0;

    if( trackbar_name == 0 || window_name == 0 )
        MV_ERROR( MV_StsNullPtr, "NULL trackbar or window name" );

    window = imvFindWindowByName( window_name );
    if( window )
        trackbar = imvFindTrackbarByName( window, trackbar_name );

    if( trackbar )
        pos = trackbar->pos;

    __END__;

    return pos;
}


MV_IMPL void mvSetTrackbarPos( const char* trackbar_name, const char* window_name, int pos )
{
    MV_FUNCNAME( "mvSetTrackbarPos" );

    __BEGIN__;

    MvWindow* window;
    MvTrackbar* trackbar = 0;

    if( trackbar_name == 0 || window_name == 0 )
        MV_ERROR( MV_StsNullPtr, "NULL trackbar or window name" );

    window = imvFindWindowByName( window_name );
    if( window )
        trackbar = imvFindTrackbarByName( window, trackbar_name );

    if( trackbar )
    {
        if( pos < 0 )
            pos = 0;

        if( pos > trackbar->maxval )
            pos = trackbar->maxval;
    }

    MV_LOCK_MUTEX();

    gtk_range_set_value( GTK_RANGE(trackbar->widget), pos );

    MV_UNLOCK_MUTEX();

    __END__;
}


MV_IMPL void* mvGetWindowHandle( const char* window_name )
{
    void* widget = 0;

    MV_FUNCNAME( "mvGetWindowHandle" );

    __BEGIN__;

    MvWindow* window;

    if( window_name == 0 )
        MV_ERROR( MV_StsNullPtr, "NULL window name" );

    window = imvFindWindowByName( window_name );
    if( window )
        widget = (void*)window->widget;

    __END__;

    return widget;
}


MV_IMPL const char* mvGetWindowName( void* window_handle )
{
    const char* window_name = "";

    MV_FUNCNAME( "mvGetWindowName" );

    __BEGIN__;

    MvWindow* window;

    if( window_handle == 0 )
        MV_ERROR( MV_StsNullPtr, "NULL window" );

    window = imvWindowByWidget( (GtkWidget*)window_handle );
    if( window )
        window_name = window->name;

    __END__;

    return window_name;
}

static gboolean imvOnKeyPress( GtkWidget * /*widget*/,
                GdkEventKey* event, gpointer /*user_data*/ )
{
    int code = 0;

    switch( event->keyval )
    {
    case GDK_Escape:
        code = 27;
        break;
    case GDK_Return:
    case GDK_Linefeed:
        code = '\n';
        break;
    case GDK_Tab:
        code = '\t';
    break;
    default:
        code = event->keyval;
    }

    code |= event->state << 16;

#ifdef HAVE_GTHREAD
    if(thread_started) g_mutex_lock(last_key_mutex);
#endif

    last_key = code;

#ifdef HAVE_GTHREAD
    if(thread_started){
        // signal any waiting threads
        g_cond_broadcast(cond_have_key);
        g_mutex_unlock(last_key_mutex);
    }
#endif

    return FALSE;
}


static void imvOnTrackbar( GtkWidget* widget, gpointer user_data )
{
    int pos = mvRound( gtk_range_get_value(GTK_RANGE(widget)));
    MvTrackbar* trackbar = (MvTrackbar*)user_data;

    if( trackbar && trackbar->signature == MV_TRACKBAR_MAGIC_VAL &&
        trackbar->widget == widget )
    {
        trackbar->pos = pos;
        if( trackbar->data )
            *trackbar->data = pos;
        if( trackbar->notify2 )
            trackbar->notify2(pos, trackbar->userdata);
        else if( trackbar->notify )
            trackbar->notify(pos);
    }
}

static gboolean imvOnClose( GtkWidget* widget, GdkEvent* /*event*/, gpointer user_data )
{
    MvWindow* window = (MvWindow*)user_data;
    if( window->signature == MV_WINDOW_MAGIC_VAL &&
        window->frame == widget )
    {
        imvDeleteWindow(window);
    }
    return TRUE;
}


static gboolean imvOnMouse( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
    // TODO move this logic to MvImageWidget
    MvWindow* window = (MvWindow*)user_data;
    MvPoint2D32f pt32f = {-1., -1.};
    MvPoint pt = {-1,-1};
    int mv_event = -1, state = 0;
    MvImageWidget * image_widget = MV_IMAGE_WIDGET( widget );

    if( window->signature != MV_WINDOW_MAGIC_VAL ||
        window->widget != widget || !window->widget ||
        !window->on_mouse /*|| !image_widget->original_image*/)
        return FALSE;

    if( event->type == GDK_MOTION_NOTIFY )
    {
        GdkEventMotion* event_motion = (GdkEventMotion*)event;

        mv_event = MV_EVENT_MOUSEMOVE;
        pt32f.x = mvRound(event_motion->x);
        pt32f.y = mvRound(event_motion->y);
        state = event_motion->state;
    }
    else if( event->type == GDK_BUTTON_PRESS ||
             event->type == GDK_BUTTON_RELEASE ||
             event->type == GDK_2BUTTON_PRESS )
    {
        GdkEventButton* event_button = (GdkEventButton*)event;
        pt32f.x = mvRound(event_button->x);
        pt32f.y = mvRound(event_button->y);


        if( event_button->type == GDK_BUTTON_PRESS )
        {
            mv_event = event_button->button == 1 ? MV_EVENT_LBUTTONDOWN :
                       event_button->button == 2 ? MV_EVENT_MBUTTONDOWN :
                       event_button->button == 3 ? MV_EVENT_RBUTTONDOWN : 0;
        }
        else if( event_button->type == GDK_BUTTON_RELEASE )
        {
            mv_event = event_button->button == 1 ? MV_EVENT_LBUTTONUP :
                       event_button->button == 2 ? MV_EVENT_MBUTTONUP :
                       event_button->button == 3 ? MV_EVENT_RBUTTONUP : 0;
        }
        else if( event_button->type == GDK_2BUTTON_PRESS )
        {
            mv_event = event_button->button == 1 ? MV_EVENT_LBUTTONDBLCLK :
                       event_button->button == 2 ? MV_EVENT_MBUTTONDBLCLK :
                       event_button->button == 3 ? MV_EVENT_RBUTTONDBLCLK : 0;
        }
        state = event_button->state;
    }

    if( mv_event >= 0 ){
        // scale point if image is scaled
        if( (image_widget->flags & MV_WINDOW_AUTOSIZE)==0 &&
             image_widget->original_image &&
             image_widget->scaled_image ){
            // image origin is not necessarily at (0,0)
            int x0 = (widget->allocation.width - image_widget->scaled_image->cols)/2;
            int y0 = (widget->allocation.height - image_widget->scaled_image->rows)/2;
            pt.x = mvRound( ((pt32f.x-x0)*image_widget->original_image->cols)/
                                            image_widget->scaled_image->cols );
            pt.y = mvRound( ((pt32f.y-y0)*image_widget->original_image->rows)/
                                            image_widget->scaled_image->rows );
        }
        else{
            pt = mvPointFrom32f( pt32f );
        }

//        if((unsigned)pt.x < (unsigned)(image_widget->original_image->width) &&
//           (unsigned)pt.y < (unsigned)(image_widget->original_image->height) )
        {
            int flags = (state & GDK_SHIFT_MASK ? MV_EVENT_FLAG_SHIFTKEY : 0) |
                (state & GDK_CONTROL_MASK ? MV_EVENT_FLAG_CTRLKEY : 0) |
                (state & (GDK_MOD1_MASK|GDK_MOD2_MASK) ? MV_EVENT_FLAG_ALTKEY : 0) |
                (state & GDK_BUTTON1_MASK ? MV_EVENT_FLAG_LBUTTON : 0) |
                (state & GDK_BUTTON2_MASK ? MV_EVENT_FLAG_MBUTTON : 0) |
                (state & GDK_BUTTON3_MASK ? MV_EVENT_FLAG_RBUTTON : 0);
            window->on_mouse( mv_event, pt.x, pt.y, flags, window->on_mouse_param );
        }
    }

        return FALSE;
    }


static gboolean imvAlarm( gpointer user_data )
{
    *(int*)user_data = 1;
    return FALSE;
}


MV_IMPL int mvWaitKey( int delay )
{
#ifdef HAVE_GTHREAD
    if(thread_started && g_thread_self()!=window_thread){
        gboolean expired;
        int my_last_key;

        // wait for signal or timeout if delay > 0
        if(delay>0){
            GTimeVal timer;
            g_get_current_time(&timer);
            g_time_val_add(&timer, delay*1000);
            expired = !g_cond_timed_wait(cond_have_key, last_key_mutex, &timer);
        }
        else{
            g_cond_wait(cond_have_key, last_key_mutex);
            expired=false;
        }
        my_last_key = last_key;
        g_mutex_unlock(last_key_mutex);
        if(expired || hg_windows==0){
            return -1;
        }
        return my_last_key;
    }
    else{
#endif
        int expired = 0;
        guint timer = 0;
        if( delay > 0 )
            timer = g_timeout_add( delay, imvAlarm, &expired );
        last_key = -1;
        while( gtk_main_iteration_do(TRUE) && last_key < 0 && !expired && hg_windows != 0 )
            ;

        if( delay > 0 && !expired )
            g_source_remove(timer);
#ifdef HAVE_GTHREAD
    }
#endif
    return last_key;
}


#endif  // HAVE_GTK
#endif  // WIN32

/* End of file. */

