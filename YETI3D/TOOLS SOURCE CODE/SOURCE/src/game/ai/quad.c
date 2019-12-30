void quad_tick(yeti_t* yeti, entity_t* e)
{
  pickup_tick(yeti, e, 170);
}

void quad_init(yeti_t* yeti, entity_t* e)
{
  e->tt = i2f(8);
  e->visual.data = model_quad;
  e->visual.ondraw = yeti_model_draw;
  e->ontick = quad_tick;
  //e->onhit  = guard_hit;
}
