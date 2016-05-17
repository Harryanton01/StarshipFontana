#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), score(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

  const int number_of_aliens = 3;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);       //alien placement
    auto pos   = Point2((i*number_of_aliens*18) +480, 100.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }
for(int i=0; i<number_of_aliens; i++) {
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((i*number_of_aliens*18)+canvas_w*0.05, 250.0f);         //alien placement
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w*0.85), 400);                           //coin placement
  coin->SetPosition(pos);
  coins.push_back(coin);


  auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
  const int number_of_walls = 10;
  for(int j=0; j<number_of_walls; j++){
	auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);    //wall placement
 	auto pos = Point2((number_of_walls+(45*j)),100.0f);
	wall->SetPosition(pos);
	walls.push_back(wall);
  }

  for(int j=0; j<number_of_walls; j++){
	auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
 	auto pos = Point2((canvas_w-number_of_walls-45*j),250.0f);    //wall placement
	wall->SetPosition(pos);
	walls.push_back(wall);
  }
}

SFApp::~SFApp() {
}
int SFApp::GetScore(){	
	return score;		//This function gets the score
}
void SFApp::AddToScore(int x){
score += x;			//This function adds the integer x to the score
}


/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    PlayerWest();
/* This detects collision between player and wall and it stops the player from going through by going the opposite direction*/
      for(auto w : walls){
	if(player->CollidesWith(w)){  
	  PlayerEast();
   }
  }
    break;
  case SFEVENT_PLAYER_RIGHT:
    PlayerEast();

      for(auto w : walls){
	if(player->CollidesWith(w)){ // if player collides with wall while this event is active go left
	 PlayerWest();
   }
  }
    break;
  case SFEVENT_PLAYER_UP:
    PlayerNorth();

       for(auto w : walls){
	if(player->CollidesWith(w)){ 
	 PlayerSouth();
   }
  }
    break;
  case SFEVENT_PLAYER_DOWN:
    PlayerSouth();

       for(auto w : walls){
	if(player->CollidesWith(w)){ 
	 PlayerNorth();
   }
  }
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::PlayerNorth(){
	player->GoNorth();   //This makes the player move up
}

void SFApp::PlayerSouth(){
	player->GoSouth();   //This makes the player move down
}

void SFApp::PlayerWest(){
	player->GoWest();    //This makes the player move left
}

void SFApp::PlayerEast(){
	player->GoEast();    //This makes the player move right
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
	AddToScore(100);
    }
  }
}

  for(auto w : walls){
    for(auto p : projectiles){	
	if(p-> CollidesWith(w)){ 	//Stops projectiles going through walls
	  p->SetNotAlive();
	  p->GoSouth();
    }
  }
}
  for(auto c : coins){
   if(c->CollidesWith(player)){
	c->SetNotAlive();		//Code to detect collision between player and coin
	AddToScore(200);
        int scores = GetScore();
        cout<<"Congratulations you won! Your score is: "<< scores << endl;   
	is_running=false;
}
}

 for(auto a : aliens){
   if(a->CollidesWith(player)){
	AddToScore(-100);
        int scores = GetScore();
        cout<<"Game Over, an alien just defeated you! Your score is: "<< scores << endl; // Code to detect collision between player and alien		
	is_running=false;
}
}


  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }
  
  for(auto w: walls){ //draw the walls
    w->OnRender();
}
  for(auto c: coins) {
    if(c->IsAlive()){
    c->OnRender();
}
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
