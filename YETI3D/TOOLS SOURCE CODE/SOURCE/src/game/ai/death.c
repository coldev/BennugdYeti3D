void death_tick(yeti_t* yeti, entity_t* e)
{
  animation_tick(&e->animation);
  
  if (!e->animation.nloops)
  {
    e->radius = 0;
    e->ontick = NULL;
  }
}

void death_init(yeti_t* yeti, entity_t* e)
{
  entity_freeze(e);
  animation_mode(&e->animation, MD2_ANIM_DEATH1 + yeti->keyboard.tick % 3, 80, 1);
  e->ontick = death_tick;
}
