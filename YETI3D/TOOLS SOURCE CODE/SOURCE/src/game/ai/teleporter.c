void teleporter_tick(yeti_t* yeti, entity_t* e)
{
  pickup_tick(yeti, e, 128);
}

void teleporter_init(yeti_t* yeti, entity_t* e)
{
  e->tt = i2f(4);
  e->visual.data = model_teleporter;
  e->visual.ondraw = yeti_model_draw;
  e->visual.mode = DRAW_MODE_BLEND;
  e->ontick = teleporter_tick;
}
