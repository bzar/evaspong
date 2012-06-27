#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <string>

int const WINDOW_WIDTH = 800;
int const WINDOW_HEIGHT = 480;

double const BAT_WIDTH = WINDOW_WIDTH/30;
double const BAT_HEIGHT = WINDOW_HEIGHT/5;
double const BAT_MARGIN = WINDOW_WIDTH/40;
double const BAT_SPEED = 3.0;

double const BALL_WIDTH = WINDOW_WIDTH/30;
double const BALL_HEIGHT = WINDOW_WIDTH/30;
double const BALL_SPEED = WINDOW_WIDTH/150.0;

std::string const KEY_UP_LEFT = "a";
std::string const KEY_DOWN_LEFT = "z";
std::string const KEY_UP_RIGHT = "Up";
std::string const KEY_DOWN_RIGHT = "Down";

struct Vec2D
{
  double x;
  double y;
};

struct Bat
{
  Evas_Object* obj;
  Vec2D velocity;
};

struct Ball
{
  Evas_Object* obj;
  Vec2D velocity;
};

struct Score
{
  int left;
  int right;
  Evas_Object* leftText;
  Evas_Object* rightText;
};

struct Data
{
  double totalTime;
  Bat leftBat;
  Bat rightBat;
  Ball ball;
  Score score;
};

Eina_Bool tick(void *d);
void keyDown(void *data, Evas *e, Evas_Object *o, void *event_info);
void keyUp(void *data, Evas *e, Evas_Object *o, void *event_info);

int main(int argc, char *argv[])
{
  Ecore_Evas *window;
  Evas *canvas;

  ecore_evas_init();
  eina_log_level_set(EINA_LOG_LEVEL_INFO);
  window = ecore_evas_new(NULL, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
  
  if (!window)
  {
    EINA_LOG_CRIT("could not create window.");
    return -1;
  }

  canvas = ecore_evas_get(window);

  Evas_Object* bg = evas_object_rectangle_add(ecore_evas_get(window));
  evas_object_color_set(bg, 0, 0, 0, 255);
  evas_object_resize(bg, WINDOW_WIDTH, WINDOW_HEIGHT);
  evas_object_show(bg);
  evas_object_focus_set(bg, EINA_TRUE);
  ecore_evas_object_associate(window, bg, ECORE_EVAS_OBJECT_ASSOCIATE_BASE);
  
  Evas_Object* leftBat = evas_object_rectangle_add(ecore_evas_get(window));
  Evas_Object* rightBat = evas_object_rectangle_add(ecore_evas_get(window));
  evas_object_color_set(leftBat, 255, 255, 255, 255);
  evas_object_color_set(rightBat, 255, 255, 255, 255);
  evas_object_resize(leftBat, BAT_WIDTH, BAT_HEIGHT);
  evas_object_resize(rightBat, BAT_WIDTH, BAT_HEIGHT);
  evas_object_move(leftBat, BAT_MARGIN, WINDOW_HEIGHT/2 - BAT_HEIGHT/2);
  evas_object_move(rightBat, WINDOW_WIDTH - BAT_WIDTH - BAT_MARGIN, 
		   WINDOW_HEIGHT/2 - BAT_HEIGHT/2);
  evas_object_show(leftBat);
  evas_object_show(rightBat);
  
  Evas_Object* ball = evas_object_rectangle_add(ecore_evas_get(window));
  evas_object_color_set(ball, 255, 255, 255, 255);
  evas_object_resize(ball, BALL_WIDTH, BALL_HEIGHT);
  evas_object_move(ball, WINDOW_WIDTH/2 - BALL_WIDTH/2, WINDOW_HEIGHT/2 - BALL_HEIGHT/2);
  evas_object_show(ball);
  
  Evas_Object* leftText = evas_object_text_add(canvas);
  Evas_Object* rightText = evas_object_text_add(canvas);
  evas_object_color_set(leftText, 255, 255, 255, 255);
  evas_object_color_set(rightText, 255, 255, 255, 255);
  evas_object_resize(leftText, 200, 50);
  evas_object_resize(rightText, 200, 50);
  evas_object_move(leftText, 25, 25);
  evas_object_move(rightText, WINDOW_WIDTH - 45, 25);
  evas_object_text_font_set(leftText, "Sans", 20);
  evas_object_text_font_set(rightText, "Sans", 20);
  evas_object_text_text_set(leftText, "0");
  evas_object_text_text_set(rightText, "0");
  evas_object_show(leftText);
  evas_object_show(rightText);
  
  
  ecore_evas_show(window);
  
  Data data = { 0.0, 
    {leftBat, {0, 0}}, 
    {rightBat, {0, 0}}, 
    {ball, {BALL_SPEED, BALL_SPEED}},
    {0, 0, leftText, rightText}
  };
  
  ecore_animator_frametime_set(1.0/60);
  Ecore_Animator* timer = ecore_animator_add(tick, &data);
  
  evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_DOWN, keyDown, &data);
  evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_UP, keyUp, &data);
  
  ecore_main_loop_begin();

  if(timer)
    ecore_animator_del(timer);
  
  ecore_evas_free(window);

  ecore_evas_shutdown();

  return 0;
}

void bounceWalls(Ball& ball)
{
  Evas_Coord bx, by, bw, bh;
  evas_object_geometry_get(ball.obj, &bx, &by, &bw, &bh);
  
  double const MIN_Y = 0;
  double const MAX_Y = WINDOW_HEIGHT - BALL_HEIGHT;
  
  if(by < MIN_Y)
  {
    by = MIN_Y - by;
    if(ball.velocity.y < 0)
      ball.velocity.y = -ball.velocity.y;
  }
  else if(by > MAX_Y)
  {
    by = 2 * MAX_Y - by;
    if(ball.velocity.y > 0)
      ball.velocity.y = -ball.velocity.y;    
  }
  
  evas_object_move(ball.obj, bx, by);
}

void bounceBat(Ball& ball, Bat& bat)
{
  Evas_Coord x, y, w, h;
  evas_object_geometry_get(ball.obj, &x, &y, &w, &h);
  
  Evas_Coord bx, by, bw, bh;
  evas_object_geometry_get(bat.obj, &bx, &by, &bw, &bh);

  bx = ball.velocity.x > 0 ? bx - w : bx + bw;
  
  if((x - bx) * (x - ball.velocity.x - bx) <= 0 &&
    !(y > by + bh || by > y + h))
  {
    x = 2 *  bx - x + (ball.velocity.x > 0 ? -ball.velocity.x : ball.velocity.x);
    ball.velocity.x = -ball.velocity.x;
    ball.velocity.y = BALL_SPEED * ((y + h/2) - (by + bh/2)) / (bh / 2);
  }
}

void handleGoals(Ball& ball, Score& score)
{
  Evas_Coord bx, by, bw, bh;
  evas_object_geometry_get(ball.obj, &bx, &by, &bw, &bh);
  
  double const MIN_X = 0;
  double const MAX_X = WINDOW_WIDTH - BALL_WIDTH;
  
  if(bx + ball.velocity.x < MIN_X)
  {
    evas_object_move(ball.obj, WINDOW_WIDTH/2 - BALL_WIDTH/2, 
                     WINDOW_HEIGHT/2 - BALL_HEIGHT/2);
    score.right += 1;
    
    if(score.right < 10000) 
    {
      char str[4];
      eina_convert_itoa(score.right, str);
      evas_object_text_text_set(score.rightText, str);
    }
  }
  else if(bx + ball.velocity.x > MAX_X)
  {
    evas_object_move(ball.obj, WINDOW_WIDTH/2 - BALL_WIDTH/2, by);
    score.left += 1;
    
    if(score.left < 10000)
    {
      char str[4];
      eina_convert_itoa(score.left, str);
      evas_object_text_text_set(score.leftText, str);
    }
  }
}

Eina_Bool tick(void *d)
{
  Data* data = static_cast<Data*>(d);
  data->totalTime += ecore_animator_frametime_get();

  Evas_Coord x, y, w, h;
  
  evas_object_geometry_get(data->ball.obj, &x, &y, &w, &h);
  evas_object_move(data->ball.obj, x + data->ball.velocity.x, y + data->ball.velocity.y);
  
  evas_object_geometry_get(data->leftBat.obj, &x, &y, &w, &h);
  y = y + data->leftBat.velocity.y;
  y = y < 0 ? 0 : y + h > WINDOW_HEIGHT ? WINDOW_HEIGHT - h : y;
  evas_object_move(data->leftBat.obj, x, y);
  
  evas_object_geometry_get(data->rightBat.obj, &x, &y, &w, &h);
  y = y + data->rightBat.velocity.y;
  y = y < 0 ? 0 : y + h > WINDOW_HEIGHT ? WINDOW_HEIGHT - h : y;
  evas_object_move(data->rightBat.obj, x, y);
  
  bounceBat(data->ball, data->leftBat);
  bounceBat(data->ball, data->rightBat);
  bounceWalls(data->ball);
  handleGoals(data->ball, data->score);
  
  return EINA_TRUE;
}

void keyDown(void *d, Evas *e, Evas_Object *o, void *event_info)
{
  Evas_Event_Key_Down* event = static_cast<Evas_Event_Key_Down*>(event_info);
  Data* data = static_cast<Data*>(d);
  
  if(event->keyname == KEY_UP_RIGHT)
  {
    data->rightBat.velocity.y = -BAT_SPEED;
  }
  else if(event->keyname == KEY_DOWN_RIGHT)
  {
    data->rightBat.velocity.y = BAT_SPEED;
  }
  else if(event->keyname == KEY_UP_LEFT)
  {
    data->leftBat.velocity.y = -BAT_SPEED;
  }
  else if(event->keyname == KEY_DOWN_LEFT)
  {
    data->leftBat.velocity.y = BAT_SPEED;
  }
}

void keyUp(void *d, Evas *e, Evas_Object *o, void *event_info)
{
  Evas_Event_Key_Up* event = static_cast<Evas_Event_Key_Up*>(event_info);
  Data* data = static_cast<Data*>(d);

  if(event->keyname == KEY_UP_RIGHT || event->keyname == KEY_DOWN_RIGHT)
  {
    data->rightBat.velocity.y = 0;
  }
  else if(event->keyname == KEY_UP_LEFT || event->keyname == KEY_DOWN_LEFT)
  {
    data->leftBat.velocity.y = 0;
  }
}
